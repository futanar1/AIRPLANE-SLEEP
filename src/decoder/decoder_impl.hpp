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

#ifndef ARIBCAPTION_DECODER_IMPL_HPP
#define ARIBCAPTION_DECODER_IMPL_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include "aribcaption/caption.hpp"
#include "aribcaption/context.hpp"
#include "aribcaption/decoder.hpp"
#include "base/logger.hpp"
#include "decoder/b24_codesets.hpp"

namespace aribcaption::internal {

class DecoderImpl {
public:
    explicit DecoderImpl(Context& context);
    ~DecoderImpl();
public:
    bool Initialize(EncodingScheme encoding_scheme = EncodingScheme::kAuto,
                    CaptionType type = CaptionType::kDefault,
                    Profile profile = Profile::kDefault,
                    LanguageId language_id = LanguageId::kDefault);
    void SetEncodingScheme(EncodingScheme encoding_scheme);
    void SetCaptionType(CaptionType type) { type_ = type; }
    void SetProfile(Profile profile);
    void SwitchLanguage(LanguageId language_id);
    void SetReplaceMSZFullWidthAlphanumeric(bool replace);
    [[nodiscard]]
    uint32_t QueryISO6392LanguageCode(LanguageId language_id) const;
    DecodeStatus Decode(const uint8_t* pes_data, size_t length, int64_t pts, DecodeResult& out_result);
    void Flush();
private:
    auto DetectEncodingScheme() -> EncodingScheme;
    void ResetGraphicSets();
    void ResetWritingFormat();
    void ResetInternalState();
    bool ParseCaptionManagementData(const uint8_t* data, size_t length);
    bool ParseCaptionStatementData(const uint8_t* data, size_t length);
    bool ParseDataUnit(const uint8_t* data, size_t length);
    bool ParseStatementBody(const uint8_t* data, size_t length);
    bool ParseDRCS(const uint8_t* data, size_t length, size_t byte_count);
    bool HandleC0(const uint8_t* data, size_t remain_bytes, size_t* bytes_processed);
    bool HandleESC(const uint8_t* data, size_t remain_bytes, size_t* bytes_processed);
    bool HandleC1(const uint8_t* data, size_t remain_bytes, size_t* bytes_processed);
    bool HandleCSI(const uint8_t* data, size_t remain_bytes, size_t* bytes_processed);
    bool Hand