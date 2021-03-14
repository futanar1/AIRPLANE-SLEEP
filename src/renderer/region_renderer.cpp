
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

#include <cassert>
#include "renderer/bitmap.hpp"
#include "renderer/canvas.hpp"
#include "renderer/region_renderer.hpp"

namespace aribcaption {

RegionRenderer::RegionRenderer(Context& context) : context_(context), log_(GetContextLogger(context)) {}

bool RegionRenderer::Initialize(FontProviderType font_provider_type, TextRendererType text_renderer_type) {
    font_provider_ = FontProvider::Create(font_provider_type, context_);
    if (!font_provider_->Initialize()) {
        return false;
    }

    text_renderer_ = TextRenderer::Create(text_renderer_type, context_, *font_provider_);
    if (!text_renderer_->Initialize()) {
        return false;
    }

    return true;
}

void RegionRenderer::SetFontLanguage(uint32_t iso6392_language_code) {
    assert(font_provider_ && text_renderer_);
    font_provider_->SetLanguage(iso6392_language_code);
    text_renderer_->SetLanguage(iso6392_language_code);
}

bool RegionRenderer::SetFontFamily(const std::vector<std::string>& font_family) {
    assert(text_renderer_);
    return text_renderer_->SetFontFamily(font_family);
}

void RegionRenderer::SetOriginalPlaneSize(int plane_width, int plane_height) {
    assert(plane_width > 0 && plane_height > 0);
    plane_width_ = plane_width;
    plane_height_ = plane_height;
    plane_inited_ = true;

    if (caption_area_inited_) {
        x_magnification_ = static_cast<float>(caption_area_width_) / static_cast<float>(plane_width);
        y_magnification_ = static_cast<float>(caption_area_height_) / static_cast<float>(plane_height);
    }
}