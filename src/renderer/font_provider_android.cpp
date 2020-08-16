
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

#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include "renderer/font_provider_android.hpp"

using namespace tinyxml2;
using namespace aribcaption::internal;

inline constexpr const char* kAndroidFontsXML_LMP = "/system/etc/fonts.xml";
inline constexpr const char* kAndroidFontsXML_OLD_System = "/system/etc/system_fonts.xml";
inline constexpr const char* kAndroidFontsXML_OLD_Fallback = "/system/etc/fallback_fonts.xml";
inline constexpr const char* kAndroidFontsXML_OLD_Fallback_JA = "/system/etc/fallback_fonts-ja.xml";
inline constexpr const char* kAndroidFontsXML_OLD_Vendor = "/vendor/etc/fallback_fonts.xml";
inline constexpr const char* kAndroidFontsXML_OLD_Vendor_JA = "/vendor/etc/fallback_fonts-ja.xml";

namespace aribcaption {

FontProviderAndroid::FontProviderAndroid(Context& context) : log_(GetContextLogger(context)) {}

FontProviderType FontProviderAndroid::GetType() {
    return FontProviderType::kAndroid;
}

bool FontProviderAndroid::Initialize() {
    base_font_path_ = getenv("ANDROID_ROOT");
    base_font_path_.append("/fonts/");

    bool ret = ParseAndroidSystemFonts();
    return ret;
}

void FontProviderAndroid::SetLanguage(uint32_t iso6392_language_code) {
    iso6392_language_code_ = iso6392_language_code;
}

auto FontProviderAndroid::GetFontFace(const std::string &font_name,
                                      std::optional<uint32_t> ucs4) -> Result<FontfaceInfo, FontProviderError> {
    FontFile* font_file = nullptr;

    if (iso6392_language_code_ == ThreeCC("jpn") || iso6392_language_code_ == 0) {
        FontFamily* family = FindFallbackFamilyByLanguageAndFallbackFor("ja", font_name.c_str());
        if (!family) {
            family = FindFallbackFamilyByLanguageAndFallbackFor("ja", "sans-serif");
        }
        if (!family) {
            return Err(FontProviderError::kFontNotFound);
        }
        for (auto& font : family->fonts) {
            if (font.weight == 400 && !font.is_italic) {
                font_file = &font;
                break;
            }
        }
    } else {
        FontFamily* family = FindFamilyByName(font_name.c_str());
        if (!family) {
            return Err(FontProviderError::kFontNotFound);
        }
        for (auto& font : family->fonts) {
            if (font.weight == 400 && !font.is_italic) {
                font_file = &font;
                break;
            }
        }
    }

    FontfaceInfo info;
    info.family_name = font_name;
    info.postscript_name = font_file->postscript_name;
    info.filename = base_font_path_ + font_file->filename;
    info.face_index = font_file->collection_index;
    info.provider_type = FontProviderType::kAndroid;

    return Ok(std::move(info));
}

FontFamily* FontProviderAndroid::FindFamilyByName(const char *search_name) {
    for (auto& family : font_families_) {
        for (auto& name : family.names) {
            if (name == search_name) {
                return &family;
            }
        }
    }

    return nullptr;
}

FontFamily* FontProviderAndroid::FindFallbackFamilyByLanguageAndFallbackFor(const char* lang,
                                                                            const char *fallback_for) {
    for (auto& family : font_families_) {
        if (!family.is_fallback) {
            continue;
        }
        if (family.fallback_for != fallback_for) {
            continue;
        }
        for (auto& language : family.languages) {
            if (language == lang) {
                return &family;
            }
        }
    }

    return nullptr;
}

bool FontProviderAndroid::ParseAndroidSystemFonts() {
    // First, try parse the fonts.xml with new format (Lollipop+)
    // /system/etc/fonts.xml
    if (!ParseFontsXML(kAndroidFontsXML_LMP)) {
        log_->w("FontProviderAndroid: Load Lollipop+ config %s failed", kAndroidFontsXML_LMP);

        bool legacy_config_load_succeed = false;

        // If failed, fallback to parse the old system_fonts.xml & fallback_fonts.xml
        // /system/etc/system_fonts.xml
        if (!ParseFontsXML(kAndroidFontsXML_OLD_System)) {
            log_->w("FontProviderAndroid: Load legacy config %s failed", kAndroidFontsXML_OLD_System);
        } else {
            legacy_config_load_succeed = true;
        }

        // /system/etc/fallback_fonts-ja.xml
        if (!ParseFontsXML(kAndroidFontsXML_OLD_Fallback_JA)) {
            // /system/etc/fallback_fonts.xml
            if (!ParseFontsXML(kAndroidFontsXML_OLD_Fallback)) {
                log_->w("FontProviderAndroid: Load legacy fallback config %s failed", kAndroidFontsXML_OLD_Fallback);
            }
        }

        // /vendor/etc/fallback_fonts-ja.xml
        if (!ParseFontsXML(kAndroidFontsXML_OLD_Vendor_JA)) {
            // /vendor/etc/fallback_fonts.xml
            if (!ParseFontsXML(kAndroidFontsXML_OLD_Vendor)) {
                log_->v("FontProviderAndroid: Cannot load legacy vendor config %s", kAndroidFontsXML_OLD_Vendor);
            }
        }

        // If failed, consider Android version is below 4.x (e.g. 2.x, 3.x)
        if (!legacy_config_load_succeed) {
            if (!PrepareFontsForGingerbread()) {
                log_->w("FontProviderAndroid: Search fonts for Android 2.x (Gingerbread) failed");
                return false;
            }
        }

        AnnotateLanguageForOldFamilySets();
    }

    return true;
}

static bool CheckFileExists(const char* filename) {
    return access(filename, R_OK) == 0;
}

bool FontProviderAndroid::CheckFileAndAppendFontFamily(const char* family_name, const char* filename, bool is_fallback) {
    std::string full_filename = base_font_path_ + filename;
    if (!CheckFileExists(full_filename.c_str())) {
        return false;
    }

    FontFamily& family = font_families_.emplace_back();
    if (family_name) {
        family.names.emplace_back(family_name);
    }
    if (is_fallback) {
        family.is_fallback = true;
        family.fallback_for = "sans-serif";
    }