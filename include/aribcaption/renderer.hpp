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

#ifndef ARIBCAPTION_RENDERER_HPP
#define ARIBCAPTION_RENDERER_HPP

#include <memory>
#include <optional>
#include "aribcc_config.h"
#include "aribcc_export.h"
#include "context.hpp"
#include "caption.hpp"
#include "image.hpp"

namespace aribcaption {

/**
 * Enums for FontProvider indication
 */
enum class FontProviderType {
    /**
     * Detect and select FontProvider automatically. Should be used in most cases.
     */
    kAuto = 0,

#if defined(ARIBCC_USE_CORETEXT)
    /**
     * FontProvider relies on Apple CoreText API. Available on macOS and iOS.
     */
    kCoreText = 1,
#endif

#if defined(ARIBCC_USE_DIRECTWRITE)
    /**
     * FontProvider relies on DirectWrite API. Available on Windows 7+.
     */
    kDirectWrite = 2,
#endif

#if defined(ARIBCC_USE_FONTCONFIG)
    /**
     * FontProvider using libfontconfig. Usually been used on Linux platforms.
     */
    kFontconfig = 3,
#endif

#if defined(ARIBCC_IS_ANDROID)
    /**
     * FontProvider for Android. Available on Android 2.x+.
     */
    kAndroid = 4,
#endif

#if defined(ARIBCC_USE_GDI_FONT)
    /**
     * FontProvder based on Win32 GDI API. Available on Windows 2000+.
     */
    kGDI = 5,
#endif
};

/**
 * Enums for TextRenderer indication
 */
enum class TextRendererType {
    /**
     * Detect and select TextRenderer automatically. Should be used in most cases.
     */
    kAuto = 0,

#if defined(ARIBCC_USE_CORETEXT)
    /**
     * Apple CoreText API based TextRenderer. Available on macOS and iOS.
     */
    kCoreText = 1,
#endif

#if defined(ARIBCC_USE_DIRECTWRITE)
    /**
     * DirectWrite API based TextRenderer. Available on Windows 7+.
     */
    kDirectWrite = 2,
#endif

#if defined(ARIBCC_USE_FREETYPE)
    /**
     * Freetype based TextRenderer. Available on all platforms.
     */
    kFreetype = 3,
#endif
};

namespace internal { class RendererImpl; }

/**
 * Enums for Renderer's caption storage policy indication
 */
enum class CaptionStoragePolicy {
    /**
     * The renderer will only keep minimal amount of captions for subsequent rendering if necessary.
     * This is the default behavior.
     */
    kMinimum = 0,

    /**
     * The renderer will never evict appended captions unless you call the @Renderer::Flush().
     * This may result in memory waste. Use at your own risk.
     */
    kUnlimited = 1,

    /**
     * The renderer will keep appended captions at an upper limit of count.
     */
    kUpperLimitCount = 2,

    /**
     * The renderer will keep appended captions at an upper limit of duration, in milliseconds.
     */
    kUpperLimitDuration = 3,
};

/**
 * Enums for reporting rendering status
 *
 * See @Renderer::Render()
 */
enum class RenderStatus {
    kError = 0,
    kNoImage = 1,
    kGotImage = 2,
    kGotImageUnchanged = 3,
};

/**
 * Structure for holding rendered caption images
 */
struct RenderResult {
    int64_t pts = 0;             ///< PTS of rendered caption
    int64_t duration = 0;        ///< duration of rendered caption, may be DURATION_INDEFINITE
    std::vector<Image> images;
};

/**
 * ARIB STD-B24 caption renderer
 */
class Renderer {
public:
    /**
     * A context is needed for constructing the Renderer.
     *
     * The context shouldn't be destructed before any other object constructed from the context has been destructed.
     */