
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

#ifndef ARIBCAPTION_B24_DECODER_HPP
#define ARIBCAPTION_B24_DECODER_HPP

#include <cstdint>
#include <memory>
#include "aribcc_export.h"
#include "caption.hpp"
#include "context.hpp"

namespace aribcaption {

namespace internal { class DecoderImpl; }

/**
 * Component tag IDs of ARIB caption profiles
 *
 * Defined in ARIB STD-B10, part 2, Annex J
 */
enum class Profile : uint8_t {
    /**
     * ARIB-Subtitle & teletext coding (A-Profile)
     */
    kProfileA = 0x0008,

    /**
     * Subtitle coding for digital terrestrial broadcasting (C profile)
     * Used in 1seg broadcasting.
     */
    kProfileC = 0x0012,
    kDefault = kProfileA
};

/**
 * Enums for language indication.
 *
 * ARIB captions supports multiple languages, with up to 8 languages per 1 ES (STD-B24, Chapter 4, Table 4-1)
 * But the operating rules have limited the maximum number of languages transmitted in 1 ES to be 2 languages
 * according to ARIB TR-B14, Fascicle 1 2/2, 4.2.1 (6) and 4.2.2.
 */
enum class LanguageId : uint8_t {