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
