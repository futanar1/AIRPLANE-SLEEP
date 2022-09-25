/*
 * Copyright (C) 2021 magicxqq <xqq@xqq.im>. All rights reserved.
 *
 * This file is part of libaribcaption.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef _WIN32
    #include <windows.h>
#endif

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
}

#include <cinttypes>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <memory>
#include "aribcaption/aribcaption.hpp"
#include "renderer/bitmap.hpp"
#include "renderer/canvas.hpp"
#include "png_writer.hpp"
#include "stopwatch.hpp"

using namespace aribcaption;

constexpr int frame_area_width = 1920;
constexpr int frame_area_height = 1080;
constexpr int margin_left = 0;
constexpr int margin_top = 0;
constexpr int margin_right = 0;
constexpr int margin_bottom = 0;

#ifdef _WIN32
class UTF8CodePage {
public:
    UTF8CodePage() : old_codepage_(GetConsoleOutputCP()) {
        SetConsoleOutputCP(CP_UTF8);
    }
    ~UTF8CodePage() {
        SetConsoleOutputCP(old_codepage_);
    }
private:
    UINT old_codepage_;
};
#endif

class CaptionDecodeRendererFFmpeg {
public:
    explicit CaptionDecodeRendererFFmpeg()
        : aribcc_decoder_(aribcc_context_), aribcc_renderer_(aribcc_context_) {}

    ~CaptionDecodeRendererFFmpeg() {
        if (format_context_) {
            avformat_close_input(&format_context_);
        }
    }
public:
    bool Open(const char* input_filename) {
        InitCaptionDecoderRenderer();

        format_context_ = avformat_alloc_context();

        int ret = 0;

        if ((ret = avformat_open_input(&format_context_, input_filename, nullptr, nullptr)) < 0) {
            fprintf(stderr, "avformat_open_input failed\n");
            return false;
        }

        if ((ret = avformat_find_stream_info(format_context_, nullptr)) < 0) {
            fprintf(stderr, "avformat_find_stream_info failed\n");
            return false;
        }

        for (size_t i = 0; i < format_context_->nb_streams; i++) {
            AVStream* stream = format_context_->streams[i];
            AVCodecParameters* codec_params = stream->codecpar;

            if (codec_params->codec_type == AVMEDIA_TYPE_SUBTITLE && codec_params->codec_id == AV_CODEC_ID_ARIB_CAPTION) {
                // if (!(stream->disposition & AV_DISPOSITION_URGENT)) {
                arib_caption_index_ = stream->index;
                break;
                // }
            }
        }

        if (arib_caption_index_ == -1) {
            fprintf(stderr, "ARIB caption stream not found\n");
            avformat_close_input(&format_context_);
            return false;
        }

        return true;
    }

    void RunLoop() {
        int ret = 0;
        AVPacket packet{};

        while ((ret = av_read_frame(format_context_, &packet) == 0)) {
            if (packet.stream_index == arib_caption_index_) {
                AVStream* stream = format_context_->streams[arib_caption_index_];
                av_packet_rescale_ts(&packet, stream->time_base, AVRational{1, 1000});
                // packet.pts = superimpose_pts_++;
                DecodeRenderAndSave(&packet);
            }
            av_packet_unref(&packet);
        }
    }
private:
    void InitCaptionDecoderRenderer() {
        stop_watch_ = StopWatch::Create();

        aribcc_context_.SetLogcatCallback([](LogLevel level, const char* message) {
            if (level == LogLevel::kError || level == LogLevel::kWarning) {
                fprintf(stderr, "%s\n", message);
            } else {
                printf("%s\n", message);
            }
        });

        aribcc_decoder_.Initialize(EncodingScheme::kAuto, CaptionType::kCaption);

        aribcc_renderer_.Initialize();
        // aribcc_renderer_.SetForceStrokeText(true);
        aribcc_renderer_.SetFrameSize(frame_area_width, frame_area_height);
        aribcc_renderer_.SetMargins(margin_top, margin_bottom, margin_left, margin_right);
        // aribcc_renderer_.SetLanguageSpecificFontFamily(ThreeCC("jpn"), {"Rounded M+ 1m for ARIB"});
    }

    bool DecodeRenderAndSave(AVPacket* packet) {
        DecodeResult decode_result;

        auto status = aribcc_decoder_.Decode(packet->data, packet->size, packet->pts, decode_result);

        if (status == DecodeStatus::kGotCaption) {
            std::unique_ptr<Caption> caption = std::move(decode_result.caption);
            if (caption->wait_duration == DURATION_INDEFINITE) {
                printf("[%.3lfs][INDEFINITE] %s\n",
                       (double)caption->pts / 1000.0f,
                       caption->text.c_str());
            } else {
                printf("[%.3lfs][%.7lfs] %s\n",
                       (double)caption->pts / 1000.0f,
                       (double)caption->wait_duration / 1000.0f,
                       caption->text.c_str());
            }
            fflush(stdout);
            if (caption->iso6392_language_code == 0) {
                caption->iso6392_language_code = ThreeCC("jpn");
            }
            aribcc_renderer_.AppendCaption(std::move(*caption));
        } else if (status == DecodeStatus::kNoCaption) {
            return true;
        } else if (status == DecodeStatus::kError) {
            fprintf(stderr, "Decoder::Decode() returned error\n");
            return false;
        }

    