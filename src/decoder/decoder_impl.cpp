
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