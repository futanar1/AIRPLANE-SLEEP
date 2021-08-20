
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

#include <CoreFoundation/CoreFoundation.h>
#include <cmath>
#include <utility>
#include "base/cfstr_helper.hpp"
#include "base/utf_helper.hpp"
#include "renderer/font_provider_coretext.hpp"
#include "renderer/text_renderer_coretext.hpp"

namespace aribcaption {

TextRendererCoreText::TextRendererCoreText(Context& context, FontProvider& font_provider)
    : log_(GetContextLogger(context)), font_provider_(font_provider) {}

TextRendererCoreText::~TextRendererCoreText() = default;

bool TextRendererCoreText::Initialize() {
    return true;
}

void TextRendererCoreText::SetLanguage(uint32_t iso6392_language_code) {
    (void)iso6392_language_code;
    // No-OP
}

bool TextRendererCoreText::SetFontFamily(const std::vector<std::string>& font_family) {
    if (font_family.empty()) {
        return false;
    }

    if (!font_family_.empty() && font_family_ != font_family) {
        // Reset CoreText fonts
        main_ctfont_pixel_height_ = 0;
        fallback_ctfont_pixel_height_ = 0;
        main_face_index_ = 0;
        main_ctfont_sized_.reset();
        main_ctfont_.reset();
        fallback_ctfont_sized_.reset();
        fallback_ctfont_.reset();
    }

    font_family_ = font_family;
    return true;
}

struct TextRenderContextPrivateCoreText : public TextRenderContext::ContextPrivate {
public:
    TextRenderContextPrivateCoreText() = default;
    ~TextRenderContextPrivateCoreText() override = default;
public:
    ScopedCFRef<CGContextRef> cg_context;
};

auto TextRendererCoreText::BeginDraw(Bitmap& target_bmp) -> TextRenderContext {
    ScopedCFRef<CGContextRef> ctx = CreateBitmapTargetCGContext(target_bmp);

    CGContextSetShouldAntialias(ctx.get(), true);
    CGContextSetShouldSmoothFonts(ctx.get(), true);

    CGContextSetAllowsFontSubpixelPositioning(ctx.get(), true);
    CGContextSetShouldSubpixelPositionFonts(ctx.get(), true);

    CGContextSetAllowsFontSubpixelQuantization(ctx.get(), true);
    CGContextSetShouldSubpixelQuantizeFonts(ctx.get(), true);

    auto priv = std::make_unique<TextRenderContextPrivateCoreText>();
    priv->cg_context = std::move(ctx);

    return TextRenderContext{target_bmp, std::move(priv)};
}

void TextRendererCoreText::EndDraw(TextRenderContext& context) {
    auto priv = static_cast<TextRenderContextPrivateCoreText*>(context.GetPrivate());
    // Release CGContext
    priv->cg_context.reset();
}

auto TextRendererCoreText::DrawChar(TextRenderContext& render_ctx, int target_x, int target_y,
                                    uint32_t ucs4, CharStyle style, ColorRGBA color, ColorRGBA stroke_color,
                                    float stroke_width, int char_width, int char_height,
                                    std::optional<UnderlineInfo> underline_info,
                                    TextRenderFallbackPolicy fallback_policy) -> TextRenderStatus {
    if (!render_ctx.GetPrivate()) {
        log_->e("TextRendererCoreText: Invalid TextRenderContext, BeginDraw() failed or not called");
        return TextRenderStatus::kOtherError;
    }

    assert(char_height > 0);
    if (stroke_width < 0.0f) {
        stroke_width = 0.0f;
    }

    // Handle space characters
    if (ucs4 == 0x0009 || ucs4 == 0x0020 || ucs4 == 0x00A0 || ucs4 == 0x1680 ||
        ucs4 == 0x3000 || ucs4 == 0x202F || ucs4 == 0x205F || (ucs4 >= 0x2000 && ucs4 <= 0x200A)) {
        return TextRenderStatus::kOK;
    }

    // Load main CTFont if not loaded
    if (!main_ctfont_) {
        auto result = LoadCTFont();
        if (result.is_err()) {
            log_->e("TextRendererCoreText: Cannot find valid font");
            return FontProviderErrorToStatus(result.error());
        }
        std::pair<ScopedCFRef<CTFontRef>, size_t>& pair = result.value();
        main_ctfont_ = std::move(pair.first);
        main_face_index_ = pair.second;
    }

    // Build size-specified main CTFont
    if (!main_ctfont_sized_ || char_height != main_ctfont_pixel_height_) {
        main_ctfont_pixel_height_ = char_height;
        main_ctfont_sized_ = CreateSizedCTFont(main_ctfont_.get(), char_height);
        if (!main_ctfont_sized_) {
            log_->e("TextRendererCoreText: Create sized CTFont failed");
            return TextRenderStatus::kOtherError;
        }
    }

    CTFontRef ctfont = main_ctfont_sized_.get();

    std::u16string utf16;
    size_t codeunit_count = utf::UTF16AppendCodePoint(utf16, ucs4);
    CGGlyph glyphs[2] = {0};
    bool has_glyph = CTFontGetGlyphsForCharacters(ctfont,
                                                  reinterpret_cast<UniChar*>(utf16.data()),
                                                  glyphs,
                                                  static_cast<CFIndex>(codeunit_count));