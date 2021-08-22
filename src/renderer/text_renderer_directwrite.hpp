
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

#ifndef ARIBCAPTION_TEXT_RENDERER_DIRECTWRITE_HPP
#define ARIBCAPTION_TEXT_RENDERER_DIRECTWRITE_HPP

#include <wrl/client.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <optional>
#include "aribcaption/context.hpp"
#include "base/scoped_com_initializer.hpp"
#include "renderer/font_provider.hpp"
#include "renderer/text_renderer.hpp"

using Microsoft::WRL::ComPtr;

namespace aribcaption {

class TextRendererDirectWrite : public TextRenderer {
public:
    TextRendererDirectWrite(Context& context, FontProvider& font_provider);
    ~TextRendererDirectWrite() override;
public:
    bool Initialize() override;
    void SetLanguage(uint32_t iso6392_language_code) override;
    bool SetFontFamily(const std::vector<std::string>& font_family) override;
    auto BeginDraw(Bitmap& target_bmp) -> TextRenderContext override;
    void EndDraw(TextRenderContext& context) override;
    auto DrawChar(TextRenderContext& render_ctx, int x, int y,
                  uint32_t ucs4, CharStyle style, ColorRGBA color, ColorRGBA stroke_color,
                  float stroke_width, int char_width, int char_height,
                  std::optional<UnderlineInfo> underline_info,
                  TextRenderFallbackPolicy fallback_policy) -> TextRenderStatus override;