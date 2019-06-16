
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

#ifndef ARIBCAPTION_CAPTION_HPP
#define ARIBCAPTION_CAPTION_HPP

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <unordered_map>
#include "color.hpp"

namespace aribcaption {

/**
 * Constant for marking the PTS is undefined.
 */
constexpr int64_t PTS_NOPTS = ((int64_t)UINT64_C(0x8000000000000000));            // -1

/**
 * Constant for marking the duration is indefinite.
 *
 * Some ARIB captions have indefinite duration which means the caption's end time is undetermined.
 * Captions with indefinite duration should be presented until the next caption's PTS.
 */
constexpr int64_t DURATION_INDEFINITE = ((int64_t)UINT64_C(0x7FFFFFFFFFFFFFFF));  // int64_t max value

/**
 * Helper function for encoding ISO 639-2 3-char language code
 *
 * @param str 3-character string literal, e.g. "jpn"
 * @return e.g. "jpn" => 6A 70 6E => 0x006A706E
 */
template <size_t N>
inline constexpr uint32_t ThreeCC(const char (&str)[N]) {
    static_assert(N == 4, "ISO 639-2 literals must be exactly 3 characters long");
    return  (static_cast<uint32_t>(str[0]) << 16) |
            (static_cast<uint32_t>(str[1]) <<  8) |
            (static_cast<uint32_t>(str[2]) <<  0);
}

/**
 * Per-CaptionChar character styles
 */
enum CharStyle {
    kCharStyleDefault = 0,
    kCharStyleBold = 1u << 0,
    kCharStyleItalic = 1u << 1,