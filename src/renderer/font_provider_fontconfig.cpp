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
#include "base/language_code.hpp"
#include "base/scoped_holder.hpp"
#include "renderer/font_provider_fontconfig.hpp"

namespace aribcaption {

FontProviderFontconfig::FontProviderFontconfig(Context& context) :
      log_(GetContextLogger(context)) {}

FontProviderFontconfig::~FontProviderFontconfig() = default;

FontProviderType FontProviderFontconfig::GetType() {
    return FontProviderType::kFontconfig;
}

bool FontProviderFontconfig::Initialize() {
    FcConfig* config = nullptr;
    if (!(config = FcInitLoadConfigAndFonts())) {
        log_->e("Fontconfig: FcInitLoadConfigAndFonts() failed");
        return false;
    }
    config_ = ScopedHolder<FcConfig*>(config, FcConfigDestroy);
    return true;
}

void FontProviderFontconfig::SetLanguage(uint32_t iso6392_language_code) {
    iso6392_language_code_ = iso6392_language_code;
}

auto FontProviderFontconfig::GetFontFace(const std::string& font_name,
                                         std::optional<uint32_t> ucs4) -> Result<FontfaceInfo, FontProviderError> {
    assert(config_);

    ScopedHolder<FcPattern*> pattern(
        FcNameParse(reinterpret_cast<const FcChar8*>(font_name.c_str())),
        FcPatternDestroy
    );
    if (!pattern) {
        log_->e("Fontconfig: Cannot parse font pattern string");
        return Err(FontProviderError::kFontNotFound);
    }

    FcPatternAddString(pattern, FC_FAMILY, reinterpret_cast<const FcChar8*>(font_name.c_str()));
    FcPatternAddBool(pattern, FC_OUTLINE, FcTrue);

    if (FcTrue != FcConfigSubstitute(config_, pattern, FcMatchPattern)) {
        log_->e("Fontconfig: Substitution cannot be performed");
        return Err(FontProviderError::kOtherError);
    }
    FcDefaultSubstitute(pattern);

    FcPatternDel(pattern, FC_LANG);
    if (iso6392_language_code_) {
        ScopedHolder<FcLangSet*> fc_langset(FcLangSetCreate(), FcLangSetDestroy);
        FcLangSetAdd(fc_langset,
                     reinterpret_cast<const FcChar8*>(ISO6392ToISO6391LanguageString(iso6392_language_code_)));
        FcPatternAddLangSet(pattern, FC_LANG, fc_langset);
    }

    FcResult result = FcResultMatch;
    FcPattern* matched = FcFontMatch(config_, pattern, &result);
    if (!matched || result != FcResultMatch) {
        log_->w("Fontconfig: Cannot find a suitable font for %s", font_name.c_str());
        return Err(FontProviderError::kFontNotFound);
    }

    ScopedHolder<FcPattern*> best(matched, FcPatternDestroy);

    FcChar8* filename = nullptr;
    if (FcResultMatch != FcPatternGetString(best, FC_FILE, 0, &filename)) {
        log_->e("Fontconfig: Retrieve font filename fa