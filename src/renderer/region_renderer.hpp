
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

#ifndef ARIBCAPTION_REGION_RENDERER_HPP
#define ARIBCAPTION_REGION_RENDERER_HPP

#include <cmath>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "aribcaption/caption.hpp"
#include "aribcaption/context.hpp"
#include "aribcaption/image.hpp"
#include "base/logger.hpp"
#include "base/result.hpp"
#include "renderer/drcs_renderer.hpp"
#include "renderer/font_provider.hpp"
#include "renderer/rect.hpp"
#include "renderer/text_renderer.hpp"

namespace aribcaption {

enum class RegionRenderError {
    kFontNotFound,
    kCodePointNotFound,
    kImageTooSmall,
    kOtherError,
};

class RegionRenderer {
public:
    explicit RegionRenderer(Context& context);
    ~RegionRenderer() = default;
public:
    bool Initialize(FontProviderType font_provider_type = FontProviderType::kAuto,
                    TextRendererType text_renderer_type = TextRendererType::kAuto);
    void SetFontLanguage(uint32_t iso6392_language_code);
    bool SetFontFamily(const std::vector<std::string>& font_family);
    void SetOriginalPlaneSize(int plane_width, int plane_height);
    void SetTargetCaptionAreaRect(const Rect& rect);