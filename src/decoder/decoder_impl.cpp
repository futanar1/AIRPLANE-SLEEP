
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

#include <cassert>
#include <cstring>
#include <cmath>
#include "base/logger.hpp"
#include "base/md5_helper.hpp"
#include "base/utf_helper.hpp"
#include "decoder/b24_codesets.hpp"
#include "decoder/b24_colors.hpp"
#include "decoder/b24_controlsets.hpp"
#include "decoder/b24_conv_tables.hpp"
#include "decoder/b24_drcs_conv.hpp"
#include "decoder/b24_gaiji_table.hpp"
#include "decoder/b24_macros.hpp"
#include "decoder/decoder_impl.hpp"

namespace aribcaption::internal {

DecoderImpl::DecoderImpl(Context& context) : log_(GetContextLogger(context)) {}

DecoderImpl::~DecoderImpl() = default;

bool DecoderImpl::Initialize(EncodingScheme encoding_scheme,
                             CaptionType type, Profile profile, LanguageId language_id) {
    request_encoding_ = encoding_scheme;
    if (request_encoding_ != EncodingScheme::kAuto) {
        active_encoding_ = encoding_scheme;
    }
    type_ = type;
    profile_ = profile;
    language_id_ = language_id;
    ResetInternalState();
    return true;
}

void DecoderImpl::SetEncodingScheme(EncodingScheme encoding_scheme) {
    request_encoding_ = encoding_scheme;

    if (encoding_scheme == EncodingScheme::kAuto) {
        EncodingScheme detected_encoding = DetectEncodingScheme();
        if (active_encoding_ != detected_encoding) {
            active_encoding_ = detected_encoding;
            ResetInternalState();
        }
    } else {  // encoding_scheme != kAuto
        if (active_encoding_ != encoding_scheme) {
            active_encoding_ = encoding_scheme;
            ResetInternalState();
        }
    }
};

void DecoderImpl::SetProfile(Profile profile) {
    profile_ = profile;
    ResetWritingFormat();
}

void DecoderImpl::SwitchLanguage(LanguageId language_id) {
    if (language_id_ != language_id) {
        language_id_ = language_id;
        current_iso6392_language_code_ = QueryISO6392LanguageCode(language_id);
    }
}

void DecoderImpl::SetReplaceMSZFullWidthAlphanumeric(bool replace) {
    replace_msz_fullwidth_ascii_ = replace;
}

uint32_t DecoderImpl::QueryISO6392LanguageCode(LanguageId language_id) const {
    if (language_infos_.empty()) {
        return current_iso6392_language_code_;
    }

    size_t index = static_cast<size_t>(language_id) - 1;
    if (index >= language_infos_.size()) {
        // Language not exist
        return 0;
    }

    const LanguageInfo& info = language_infos_[index];
    return info.iso6392_language_code;
}

DecodeStatus DecoderImpl::Decode(const uint8_t* pes_data, size_t length, int64_t pts, DecodeResult& out_result) {
    if (pes_data == nullptr) {
        log_->e("DecoderImpl: pes_data is nullptr");
        assert(pes_data != nullptr);
        return DecodeStatus::kError;
    }

    if (length < 3) {
        log_->e("DecoderImpl: pes_data size < 3, cannot parse");
        return DecodeStatus::kError;
    }

    out_result.caption.reset();
    pts_ = pts;
    const uint8_t* data = pes_data;

    uint8_t data_identifier = data[0];
    uint8_t private_stream_id = data[1];
    size_t PES_data_packet_header_length = data[2] & 0x0F;

    if (data_identifier != 0x80 && data_identifier != 0x81) {
        log_->e("DecoderImpl: Invalid data_identifier: 0x%02X", data_identifier);
        return DecodeStatus::kError;
    } else if (data_identifier != static_cast<uint8_t>(type_)) {
        log_->e("DecoderImpl: data_identifier mismatch, found: 0x%02X, expected: 0x%02X",
                data_identifier,
                static_cast<uint8_t>(type_));
        return DecodeStatus::kError;
    }

    if (private_stream_id != 0xFF) {
        log_->e("DecoderImpl: Invalid private_stream_id: 0x%02X", private_stream_id);
        return DecodeStatus::kError;
    }

    size_t data_group_begin = 3 + PES_data_packet_header_length;
    if (data_group_begin + 5 > length) {
        log_->e("DecoderImpl: pes_data length does not enough for a whole data_group");
        return DecodeStatus::kError;
    }

    uint8_t data_group_id = (data[data_group_begin] & 0b11111100) >> 2;
    size_t data_group_size = ((size_t)data[data_group_begin + 3] << 8) |
                             ((size_t)data[data_group_begin + 4] << 0);
    if (data_group_begin + 5 + data_group_size > length) {
        log_->e("DecoderImpl: pes_data length does not enough for a whole data_group");
        return DecodeStatus::kError;
    }

    if (data_group_size == 0) {
        return DecodeStatus::kNoCaption;
    }

    uint8_t dgi_id = data_group_id & 0x0F;
    int dgi_group = (data_group_id & 0xF0) >> 8;

    bool ret = false;

    caption_ = std::make_unique<Caption>();

    if (dgi_id == 0) {
        // Caption management data
        if (dgi_group == prev_dgi_group_) {
            /* According to ARIB TR-B14 4.2.4
             * For caption management data, if data_group_id group equals to previous management data's group
             * This packet could be considered as retransmission, ignore it
             */
            return DecodeStatus::kNoCaption;
        } else {
            // Handle caption management data
            prev_dgi_group_ = dgi_group;
            ret = ParseCaptionManagementData(data + data_group_begin + 5, data_group_size);
        }
    } else {
        // Caption statement data
        if (dgi_id != static_cast<uint8_t>(language_id_)) {
            // Non-expected language id, ignore it
            return DecodeStatus::kNoCaption;
        } else {
            // Handle caption statement data
            ret = ParseCaptionStatementData(data + data_group_begin + 5, data_group_size);
        }
    }

    if (!ret) {
        caption_.reset();
        return DecodeStatus::kError;
    }

    if (!caption_->regions.empty() || caption_->flags) {
        caption_->type = static_cast<CaptionType>(type_);
        caption_->iso6392_language_code = current_iso6392_language_code_;
        caption_->plane_width = caption_plane_width_;
        caption_->plane_height = caption_plane_height_;
        caption_->has_builtin_sound = has_builtin_sound_;
        caption_->builtin_sound_id = builtin_sound_id_;

        caption_->pts = pts_;

        if (caption_->wait_duration == 0) {
            caption_->wait_duration = DURATION_INDEFINITE;
        }

        out_result.caption = std::move(caption_);
        return DecodeStatus::kGotCaption;
    }

    return DecodeStatus::kNoCaption;
}

void DecoderImpl::Flush() {
    ResetInternalState();
}

auto DecoderImpl::DetectEncodingScheme() -> EncodingScheme {
    EncodingScheme encoding_scheme = EncodingScheme::kARIB_STD_B24_JIS;
    bool has_ucs = false, has_jpn = false, has_latin = false, has_eng = false, has_tgl = false;

    for (const auto& info : language_infos_) {
        if (info.TCS == 1) {
            has_ucs = true;
        } else if (info.iso6392_language_code == ThreeCC("jpn")) {
            has_jpn = true;
        } else if (info.iso6392_language_code == ThreeCC("por") || info.iso6392_language_code == ThreeCC("spa")) {
            has_latin = true;
        } else if (info.iso6392_language_code == ThreeCC("eng")) {
            has_eng = true;
        } else if (info.iso6392_language_code == ThreeCC("tgl")) {
            has_tgl = true;
        }
    }

    if (has_ucs) {
        encoding_scheme = EncodingScheme::kARIB_STD_B24_UTF8;
    } else if (has_jpn) {
        encoding_scheme = EncodingScheme::kARIB_STD_B24_JIS;
    } else if (has_latin) {
        encoding_scheme = EncodingScheme::kABNT_NBR_15606_1_Latin;
    } else if (has_eng || has_tgl) {
        encoding_scheme = EncodingScheme::kARIB_STD_B24_UTF8;  // Same as kISDB_T_Philippines_UTF8
    }

    return encoding_scheme;
}

void DecoderImpl::ResetGraphicSets() {
    // Set default G1~G4 codesets
    if (active_encoding_ == EncodingScheme::kABNT_NBR_15606_1_Latin) {
        // Latin language, defined in ABNT NBR 15606-1
        GX_[0] = kAlphanumericEntry;
        GX_[1] = kAlphanumericEntry;
        GX_[2] = kLatinExtensionEntry;
        GX_[3] = kLatinSpecialEntry;
    } else if (profile_ == Profile::kProfileA) {
        // full-seg, Profile A
        GX_[0] = kKanjiEntry;
        GX_[1] = kAlphanumericEntry;
        GX_[2] = kHiraganaEntry;
        GX_[3] = kMacroEntry;
    } else if (profile_ == Profile::kProfileC) {
        // one-seg, Profile C
        GX_[0] = kDRCS1Entry;
        GX_[1] = kAlphanumericEntry;
        GX_[2] = kKanjiEntry;
        GX_[3] = kMacroEntry;
    }
    GL_ = &GX_[0];
    GR_ = &GX_[2];
}