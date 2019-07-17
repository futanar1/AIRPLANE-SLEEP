
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

#ifndef ARIBCAPTION_RENDERER_H
#define ARIBCAPTION_RENDERER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "aribcc_config.h"
#include "aribcc_export.h"
#include "context.h"
#include "caption.h"
#include "image.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enums for FontProvider indication
 */
typedef enum aribcc_fontprovider_type_t {
    /**
     * Detect and select FontProvider automatically. Should be used in most cases.
     */
    ARIBCC_FONTPROVIDER_TYPE_AUTO = 0,

#if defined(ARIBCC_USE_CORETEXT)
    /**
     * FontProvider relies on Apple CoreText API. Available on macOS and iOS.
     */
    ARIBCC_FONTPROVIDER_TYPE_CORETEXT = 1,
#endif

#if defined(ARIBCC_USE_DIRECTWRITE)
    /**
     * FontProvider relies on DirectWrite API. Available on Windows 7+.
     */
    ARIBCC_FONTPROVIDER_TYPE_DIRECTWRITE = 2,
#endif

#if defined(ARIBCC_USE_FONTCONFIG)
    /**
     * FontProvider using libfontconfig. Usually been used on Linux platforms.
     */
    ARIBCC_FONTPROVIDER_TYPE_FONTCONFIG = 3,
#endif

#if defined(ARIBCC_IS_ANDROID)
    /**
     * FontProvider for Android. Available on Android 2.x+.
     */
    ARIBCC_FONTPROVIDER_TYPE_ANDROID = 4,
#endif

#if defined(ARIBCC_USE_GDI_FONT)
    /**
     * FontProvder based on Win32 GDI API. Available on Windows 2000+.
     */
    ARIBCC_FONTPROVIDER_TYPE_GDI = 5
#endif
} aribcc_fontprovider_type_t;

/**
 * Enums for TextRenderer indication
 */
typedef enum aribcc_textrenderer_type_t {
    /**
     * Detect and select TextRenderer automatically. Should be used in most cases.
     */
    ARIBCC_TEXTRENDERER_TYPE_AUTO = 0,

#if defined(ARIBCC_USE_CORETEXT)
    /**
     * Apple CoreText API based TextRenderer. Available on macOS and iOS.
     */
    ARIBCC_TEXTRENDERER_TYPE_CORETEXT = 1,
#endif

#if defined(ARIBCC_USE_DIRECTWRITE)
    /**
     * DirectWrite API based TextRenderer. Available on Windows 7+.
     */
    ARIBCC_TEXTRENDERER_TYPE_DIRECTWRITE = 2,
#endif

#if defined(ARIBCC_USE_FREETYPE)
    /**
     * Freetype based TextRenderer. Available on all platforms.
     */
    ARIBCC_TEXTRENDERER_TYPE_FREETYPE = 3,
#endif
} aribcc_textrenderer_type_t;

/**
 * Enums for Renderer's caption storage policy indication
 */
typedef enum aribcc_caption_storage_policy_t {
    /**
     * The renderer will only keep minimal amount of captions for subsequent rendering if necessary.
     * This is the default behavior.
     */
    ARIBCC_CAPTION_STORAGE_POLICY_MINIMUM = 0,

    /**
     * The renderer will never evict appended captions unless you call the @aribcc_renderer_flush().
     * This may result in memory waste. Use at your own risk.
     */
    ARIBCC_CAPTION_STORAGE_POLICY_UNLIMITED = 1,

    /**
     * The renderer will keep appended captions at an upper limit of count.
     */
    ARIBCC_CAPTION_STORAGE_POLICY_UPPER_LIMIT_COUNT = 2,

    /**
     * The renderer will keep appended captions at an upper limit of duration, in milliseconds.
     */
    ARIBCC_CAPTION_STORAGE_POLICY_UPPER_LIMIT_DURATION = 3,
} aribcc_caption_storage_policy_t;

/**
 * Enums for reporting rendering status
 *