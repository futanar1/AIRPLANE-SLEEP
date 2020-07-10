
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
#include "renderer/alphablend.hpp"
#include "renderer/bitmap.hpp"
#include "renderer/canvas.hpp"
#include "renderer/drcs_renderer.hpp"

namespace aribcaption {

bool DRCSRenderer::DrawDRCS(const DRCS& drcs, CharStyle style, ColorRGBA color, ColorRGBA stroke_color,
                            int stroke_width, int target_width, int target_height,
                            Bitmap& target_bmp, int target_x, int target_y) {
    if (drcs.width == 0 || drcs.height == 0 || drcs.pixels.empty()) {
        return false;
    }

    Canvas canvas(target_bmp);

    // Draw stroke (border) if needed
    if (style & CharStyle::kCharStyleStroke) {
        Bitmap stroke_bitmap = DRCSToColoredBitmap(drcs, target_width, target_height, stroke_color);

        canvas.DrawBitmap(stroke_bitmap, target_x - stroke_width, target_y);
        canvas.DrawBitmap(stroke_bitmap, target_x + stroke_width, target_y);
        canvas.DrawBitmap(stroke_bitmap, target_x, target_y - stroke_width);
        canvas.DrawBitmap(stroke_bitmap, target_x, target_y + stroke_width);
    }

    // Draw DRCS with text color
    Bitmap text_bitmap = DRCSToColoredBitmap(drcs, target_width, target_height, color);
    canvas.DrawBitmap(text_bitmap, target_x, target_y);

    return true;
}

Bitmap DRCSRenderer::DRCSToColoredBitmap(const DRCS& drcs, int target_width, int target_height, ColorRGBA color) {
    Bitmap bitmap(target_width, target_height, PixelFormat::kRGBA8888);

    float x_fraction = static_cast<float>(drcs.width) / static_cast<float>(target_width);
    float y_fraction = static_cast<float>(drcs.height) / static_cast<float>(target_height);