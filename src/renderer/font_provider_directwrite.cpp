
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

#include <windows.h>
#include <initguid.h>
#include "base/wchar_helper.hpp"
#include "renderer/font_provider_directwrite.hpp"

namespace aribcaption {

constexpr IID IID_IDWriteFactory = {0xb859ee5a, 0xd838, 0x4b5b, {0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48}};

FontProviderDirectWrite::FontProviderDirectWrite(Context& context) : log_(GetContextLogger(context)) {}

FontProviderType FontProviderDirectWrite::GetType() {
    return FontProviderType::kDirectWrite;
}

bool FontProviderDirectWrite::Initialize() {
    HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                     IID_IDWriteFactory,
                                     static_cast<IUnknown**>(&dwrite_factory_));
    if (FAILED(hr)) {
        log_->e("FontProviderDirectWrite: Failed to create IDWriteFactory");
        return false;
    }

    hr = dwrite_factory_->GetGdiInterop(&dwrite_gdi_interop_);
    if (FAILED(hr)) {
        log_->e("FontProviderDirectWrite: Failed to retrieve IDWriteGdiInterop");
        return false;
    }

    return true;
}

ComPtr<IDWriteFactory> FontProviderDirectWrite::GetDWriteFactory() {
    return dwrite_factory_;
}

void FontProviderDirectWrite::SetLanguage(uint32_t iso6392_language_code) {
    iso6392_language_code_ = iso6392_language_code;
}

static std::string ConvertFamilyName(const std::string& family_name, uint32_t iso6392_language_code) {
    std::string font_name = family_name;

    if (family_name == "sans-serif") {
        if (iso6392_language_code == ThreeCC("jpn")) {
            font_name = "MS Gothic";
        } else {
            font_name = "Verdana";
        }
    } else if (family_name == "serif") {
        if (iso6392_language_code == ThreeCC("jpn")) {
            font_name = "MS Mincho";
        } else {
            font_name = "Times New Roman";