/*
 * Copyright (C) 2022 magicxqq <xqq@xqq.im>. All rights reserved.
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

#ifndef ARIBCAPTION_CAPTION_H
#define ARIBCAPTION_CAPTION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "aribcc_export.h"
#include "color.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Helper macro for encoding ISO 639-2 3-char language code
 *
 * @param a,b,c char
 * @code
 * uint32_t language_code = ARIBCC_MAKE_LANG('j', 'p', 'n')
 * @endcode
 */
#define ARIBCC_MAKE_LANG(a,b,c) ((((a) & 0xff) << 16) | (((b) & 0xff) << 8) | ((c) & 0xff))

/**
 * Constant for marking the PTS is undefined.
 */
#define ARIBCC_PTS_NOPTS ((int64_t)UINT64_C(0x8000000000000000))

/**
 * Constant for marking the duration is indefinite.
 *
 * Some ARIB captions have indefinite duration which means the caption's end time is undetermined.
 * Captions with indefinite duration should be presented until the next caption's PTS.
 */
#define ARIBCC_DURATION_INDEFINITE ((int64_t)UINT64_C(0x7FFFFFFFFFFFFFFF))

/**
 * Per-CaptionChar character styles
 */
typedef enum aribcc_charstyle_t {
    ARIBCC_CHARSTYLE_DEFAULT = 0,
    ARIBCC_CHARSTYLE_BOLD = 1u << 0,
    ARIBCC_CHARSTYLE_ITALIC = 1u << 1,
    ARIBCC_CHARSTYLE_UNDERLINE = 1u << 2,
    ARIBCC_CHARSTYLE_STROKE = 1u << 3
} aribcc_charstyle_t;

/**
 * Per-CaptionChar enclosure styles
 */
typedef enum aribcc_enclosurestyle_t {
    ARIBCC_ENCLOSURESTYLE_NONE = 0,
    ARIBCC_ENCLOSURESTYLE_BOTTOM = 1u << 0,
    ARIBCC_ENCLOSURESTYLE_RIGHT = 1u << 1,
    ARIBCC_ENCLOSURESTYLE_TOP = 1u << 2,
    ARIBCC_ENCLOSURESTYLE_LEFT = 1u << 3,
    ARIBCC_ENCLOSURESTYLE_DEFAULT = ARIBCC_ENCLOSURESTYLE_NONE
} aribcc_enclosurestyle_t;

/**
 * Enums for caption type indication. Usually, kCaption should be used.
 *
 * kSuperimpose should be indicated if you are handling ARIB superimpose (a kind of emergency caption).
 */
typedef enum aribcc_captiontype_t {
    ARIBCC_CAPTIONTYPE_CAPTION = 0x80,
    ARIBCC_CAPTIONTYPE_SUPERIMPOSE = 0x81,
    ARIBCC_CAPTIONTYPE_DEFAULT = ARIBCC_CAPTIONTYPE_CAPTION
} aribcc_captiontype_t;

/**
 * Represents the type of CaptionChar.
 *
 * The type will be kDRCS or kDRCSReplaced if it's a DRCS character.
 */
typedef enum aribcc_chartype_t {
    ARIBCC_CHARTYPE_TEXT = 0,
    ARIBCC_CHARTYPE_DRCS = 1,
    ARIBCC_CHARTYPE_DRCS_REPLACED = 2,
    ARIBCC_CHARTYPE_DEFAULT = ARIBCC_CHARTYPE_TEXT
} aribcc_chartype_t;

/**
 * Represents a caption character.
 */
typedef struct aribcc_caption_char_t {
    aribcc_chartype_t type;

    /**
     * Character's Unicode codepoint (UCS4). This field will be 0 if CaptionCharType is kDRCS.
     */
    uint32_t codepoint;

    /**
     * Character's PUA(Private Use Area) codepoint (UCS4)
     *
     * This field will be non-zero only if PUA code is defined for the character.
     *
     * Some ARIB additional symbols (i.e. Gaiji) are mapped into Private Use Area in Unicode BMP.
     * Though most of them could be mapped into characters introduced in Unicode 5.2,
     * Some fonts don't support Unicode 5.2 but support ARIB additional symbols through Private Use Area codes.
     */
    uint32_t pua_codepoint;

    /**
     * DRCS character's code. Only appears if if CaptionCharType is kDRCS or kDRCSReplaced.
     * It is a private code which is unrelated to Unicode.
     *
     * DRCS data could be retrieved from Caption::drcs_map using drcs_code.
     */
    uint32_t drcs_code;

    int x;
    int y;
    int char_width;
    int char_height;
    int char_horizontal_spacing;
    int char_vertical_spacing;
    float char_horizontal_scale;
    float char_vertical_scale;

    aribcc_color_t text_color;    ///< Color of the text (foreground)
    aribcc_color_t back_color;    ///< Color of the background
    aribcc_color_t stroke_color;  ///< Color of the storke text

    aribcc_charstyle_t style;
    aribcc_enclosurestyle_t enclosure_style;

    /**
     * String representation of character encoded in UTF-8. This string is Null-terminated.
     * Will be empty string if CaptionCharType is kDRCS because alte