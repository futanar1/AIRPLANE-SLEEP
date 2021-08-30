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
#include <cstring>
#include <cstdint>
#include <cmath>
#include "base/scoped_holder.hpp"
#include "base/utf_helper.hpp"
#include "renderer/alphablend.hpp"
#include "renderer/canvas.hpp"
#include "renderer/text_renderer_freetype.hpp"
#include FT_STROKER_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H

namespace aribcaption {

TextRendererFreetype::TextRendererFreetype(Context& context, FontProvider& font_provider) :
      log_(GetContextLogger(context)), font_provider_(font_provider) {}

TextRendererFreetype::~TextRendererFreetype() = default;

bool TextRendererFreetype::Initialize() {
    FT_Library library;
    FT_Error error = FT_Init_FreeType(&library);
    if (error) {
        log_->e("Freetype: FT_Init_FreeType() failed");
        library_ = nullptr;
        return false;
    }

    library_ = ScopedHolder<FT_Library>(library, FT_Done_FreeType);
    return true;
}

void TextRendererFreetype::SetLanguage(uint32_t iso6392_language_code) {
    (void)iso6392_language_code;
    // No-OP
}

bool TextRendererFreetype::SetFontFamily(const std::vector<std::string>& font_family) {
    if (font_family.empty()) {
        return false;
    }

    if (!font