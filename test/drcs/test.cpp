
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

#include <cstdint>
#include <cstdio>
#include <memory>
#include "aribcaption/decoder.hpp"
#include "renderer/canvas.hpp"
#include "renderer/drcs_renderer.hpp"
#include "renderer/font_provider.hpp"
#include "renderer/font_provider_fontconfig.hpp"
#include "renderer/text_renderer.hpp"
#include "renderer/text_renderer_freetype.hpp"
#include "png_writer.hpp"
#include "sample_data.h"

constexpr bool draw_layout_box = false;
constexpr int scale_factor = 2;
constexpr bool force_stroke_text = true;
constexpr int stroke_width = 3;

using namespace aribcaption;

int main(int argc, const char* argv[]) {
    Context context;
    context.SetLogcatCallback([](LogLevel level, const char* message) {
        if (level == LogLevel::kError) {
            fprintf(stderr, "%s\n", message);
        } else {
            printf("%s\n", message);
        }
    });

    FontProviderFontconfig font_provider(context);
    font_provider.Initialize();

    TextRendererFreetype text_renderer(context, font_provider);
    text_renderer.Initialize();
    text_renderer.SetFontFamily({"Hiragino Maru Gothic ProN"});
    // text_renderer.SetFontFamily({"Rounded M+ 1m for ARIB"});

    DRCSRenderer drcs_renderer;

    Decoder decoder(context);
    decoder.Initialize();

    DecodeResult result;

    auto status = decoder.Decode(sample_data_drcs_1, sizeof(sample_data_drcs_1), 0, result);
