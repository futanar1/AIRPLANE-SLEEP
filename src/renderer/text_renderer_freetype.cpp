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

    if (!font_family_.empty() && font_family_ != font_family) {
        // Reset Freetype faces
        main_face_.Reset();
        fallback_face_.Reset();
        main_face_data_.clear();
        fallback_face_data_.clear();
        main_face_index_ = 0;
    }

    font_family_ = font_family;
    return true;
}

auto TextRendererFreetype::BeginDraw(Bitmap& target_bmp) -> TextRenderContext {
    return TextRenderContext(target_bmp);
}

void TextRendererFreetype::EndDraw(TextRenderContext& context) {
    (void)context;
    // No-op
}

auto TextRendererFreetype::DrawChar(TextRenderContext& render_ctx, int target_x, int target_y,
                                    uint32_t ucs4, CharStyle style, ColorRGBA color, ColorRGBA stroke_color,
                                    float stroke_width, int char_width, int char_height,
                                    std::optional<UnderlineInfo> underline_info,
                                    TextRenderFallbackPolicy fallback_policy) -> TextRenderStatus {
    assert(char_height > 0);
    if (stroke_width < 0.0f) {
        stroke_width = 0.0f;
    }

    // Handle space characters
    if (ucs4 == 0x0009 || ucs4 == 0x0020 || ucs4 == 0x00A0 || ucs4 == 0x1680 ||
        ucs4 == 0x3000 || ucs4 == 0x202F || ucs4 == 0x205F || (ucs4 >= 0x2000 && ucs4 <= 0x200A)) {
        return TextRenderStatus::kOK;
    }

    if (!main_face_) {
        // If main FT_Face is not yet loaded, try load FT_Face from font_family_
        // We don't care about the codepoint (ucs4) now
        auto result = LoadFontFace(false);
        if (result.is_err()) {
            log_->e("Freetype: Cannot find valid font");
            return FontProviderErrorToStatus(result.error());
        }
        std::pair<FT_Face, size_t>& pair = result.value();
        main_face_ = ScopedHolder<FT_Face>(pair.first, FT_Done_Face);
        main_face_index_ = pair.second;
    }

    FT_Face face = main_face_;
    FT_UInt glyph_index = FT_Get_Char_Index(face, ucs4);

    if (glyph_index == 0) {
        log_->w("Freetype: Main font %s doesn't contain U+%04X", face->family_name, ucs4);

        if (fallback_policy == TextRenderFallbackPolicy::kFailOnCodePointNotFound) {
            return TextRenderStatus::kCodePointNotFound;
        }

        // Missing glyph, check fallback face
        if (fallback_face_ && (glyph_index = FT_Get_Char_Index(fallback_face_, ucs4))) {
            face = fallback_face_;
        } else if (main_face_index_ + 1 >= font_family_.size()) {
            // Fallback fonts not available
            return TextRenderStatus::kCodePointNotFound;
        } else {
            // Fallback fontface not loaded, or fallback fontface doesn't contain required codepoint
            // Load next fallback font face by specific codepoint
            auto result 