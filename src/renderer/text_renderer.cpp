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

#include <memory>
#include "aribcc_config.h"
#include "renderer/text_renderer.hpp"

#if defined(ARIBCC_USE_CORETEXT)
    #include "renderer/text_renderer_coretext.hpp"
#endif

#if defined(ARIBCC_USE_DIRECTWRITE)
    #include "renderer/text_renderer_directwrite.hpp"
#endif

#if defined(ARIBCC_USE_FREETYPE)
    #include "renderer/text_renderer_freetype.hpp"
#endif

namespace aribcaption {

std::unique_ptr<TextRenderer> TextRenderer::Create(TextRendererType type, Context& context, FontProvider& font_provider) {
    switch (type) {
#if defined(ARIBCC_USE_CORETEXT)
        case TextRendererType::kCoreText:
            return std::make_unique<TextRendererCoreText>(context, font_provider);
#endif

#if defined(ARIBCC_USE_DIRECTWRITE)
        case TextRendererType::kDirectWrite:
            return std::make_unique<TextRendererDirectWrite>(context, font_provider);
#endif

#if defined(ARIBCC_USE_FREETYPE)
        case TextRendererType::kFreetype:
            re