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

#include <cmath>
#include <algorithm>
#include <iterator>
#include "aribcaption/context.hpp"
#include "renderer/bitmap.hpp"
#include "renderer/canvas.hpp"
#include "renderer/renderer_impl.hpp"

namespace aribcaption::internal {

RendererImpl::RendererImpl(Context& context)
    : context_(context), log_(GetContextLogger(context)), region_renderer_(context) {}

RendererImpl::~RendererImpl() = default;

bool RendererImpl::Initialize(CaptionType caption_type,
                              FontProviderType font_provider_type,
                              TextRendererType text_renderer_type) {
    expected_caption_type_ = caption_type;
    LoadDefaultFontFamilies();
    return region_renderer_.Initialize(font_provider_type, text_renderer_type);
}

void RendererImpl::LoadDefaultFontFamilies() {
    // Font face for default language (0)
    language_font_family_[0] = { "sans-serif" };

    // Default fonts for Japanese (jpn)
    std::vector<std::string> jpn_default_font_family;
#if defined(_WIN32)
    jpn_default_font_family = {
        "Windows TV MaruGothic",
        "MS Gothic",
        "sans-serif",
    };
#elif defined(__APPLE__)
    jpn_default_font_family = {
        "Hiragino Maru Gothic ProN",
        "Hiragino Sans",
        "sans-serif",
    };
#elif defined(__ANDROID__)
    jpn_default_font_family = {
        "sans-serif"
    };
#else  // Linux, or other platforms
    jpn_default_font_family = {
        "Noto Sans CJK JP",
        "Noto Sans CJK",
        "Source Han Sans JP",
        "sans-serif",
    };
#endif
    language_font_family_[ThreeCC("jpn")] = std::move(jpn_default_font_family);

    // Default fonts for 