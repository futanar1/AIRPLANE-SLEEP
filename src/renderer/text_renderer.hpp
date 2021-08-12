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

#ifndef ARIBCAPTION_TEXT_RENDERER_HPP
#define ARIBCAPTION_TEXT_RENDERER_HPP

#include <memory>
#include <optional>
#include "aribcaption/caption.hpp"
#include "aribcaption/context.hpp"
#include "aribcaption/renderer.hpp"
#include "base/result.hpp"
#include "renderer/bitmap.hpp"
#include "renderer/font_provider.hpp"

namespace aribcaption {

struct UnderlineInfo {
    int start_x = 0;
    int width = 0;
};

enum class TextRenderStatus {
    kOK,
    kFontNotFound,
    kCodePointNotFound,
    kOtherError
};

enum class TextRenderFallbackPolicy {
    kAutoFallback,
    kFailOnCodePointNotFound
};

class TextRen