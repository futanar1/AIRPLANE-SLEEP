
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
#include <cmath>
#include <functional>
#include "base/utf_helper.hpp"
#include "base/wchar_helper.hpp"
#include "renderer/alphablend.hpp"
#include "renderer/canvas.hpp"
#include "renderer/font_provider_directwrite.hpp"
#include "renderer/text_renderer_directwrite.hpp"

namespace aribcaption {

class OutlineTextRenderer : public IDWriteTextRenderer {
public:
    using UnderlineDrawCB = std::function<void(const DWRITE_UNDERLINE* underline)>;
public:
    OutlineTextRenderer(float horizontal_scale,
                        float vertical_scale,
                        bool draw_outline_stroke,
                        float stroke_width,
                        ComPtr<ID2D1StrokeStyle> stroke_style,
                        ComPtr<ID2D1Factory> d2d_factory,
                        ComPtr<ID2D1RenderTarget> render_target,
                        ComPtr<ID2D1SolidColorBrush> fill_brush,
                        ComPtr<ID2D1SolidColorBrush> outline_brush,
                        UnderlineDrawCB underline_draw_cb)
        : horizontal_scale_(horizontal_scale),
          vertical_scale_(vertical_scale),
          draw_outline_stroke_(draw_outline_stroke),
          stroke_width_(stroke_width),
          stroke_style_(std::move(stroke_style)),
          d2d_factory_(std::move(d2d_factory)),
          render_target_(std::move(render_target)),
          fill_brush_(std::move(fill_brush)),
          outline_brush_(std::move(outline_brush)),
          underline_draw_cb_(std::move(underline_draw_cb)) {}

    virtual ~OutlineTextRenderer() = default;

    IFACEMETHOD(IsPixelSnappingDisabled)(void* client_drawing_context, BOOL* is_disabled) {
        *is_disabled = FALSE;
        return S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(void* client_drawing_context, DWRITE_MATRIX* transform) {
        render_target_->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
        return S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(void* client_drawing_context, FLOAT* pixels_pre_dip) {
        float dpi_x = 0.0f;
        float dpi_y = 0.0f;
        render_target_->GetDpi(&dpi_x, &dpi_y);
        *pixels_pre_dip = dpi_x / 96;
        return S_OK;
    }

    IFACEMETHOD(DrawGlyphRun)(void* client_drawing_context,
                              float baseline_origin_x,
                              float baseline_origin_y,
                              DWRITE_MEASURING_MODE measuring_mode,
                              DWRITE_GLYPH_RUN const* glyph_run,
                              DWRITE_GLYPH_RUN_DESCRIPTION const* glyph_run_description,
                              IUnknown* client_drawing_effect) {
        ComPtr<ID2D1PathGeometry> path_geometry;
        HRESULT hr = d2d_factory_->CreatePathGeometry(&path_geometry);
        if (FAILED(hr))
            return hr;

        ComPtr<ID2D1GeometrySink> sink;
        hr = path_geometry->Open(&sink);
        if (FAILED(hr))
            return hr;

        hr = glyph_run->fontFace->GetGlyphRunOutline(
            glyph_run->fontEmSize,
            glyph_run->glyphIndices,
            glyph_run->glyphAdvances,
            glyph_run->glyphOffsets,
            glyph_run->glyphCount,
            glyph_run->isSideways,
            glyph_run->bidiLevel % 2,
            sink.Get()
        );
        if (FAILED(hr))
            return hr;

        hr = sink->Close();
        if (FAILED(hr))
            return hr;

        const D2D1::Matrix3x2F matrix = D2D1::Matrix3x2F(
            horizontal_scale_, 0.0f,
            0.0f, vertical_scale_,
            baseline_origin_x, baseline_origin_y
        );

        ComPtr<ID2D1TransformedGeometry> transformed_geometry;
        hr = d2d_factory_->CreateTransformedGeometry(path_geometry.Get(), matrix, &transformed_geometry);
        if (FAILED(hr))
            return hr;

        if (draw_outline_stroke_) {
            render_target_->DrawGeometry(transformed_geometry.Get(),
                                         outline_brush_.Get(),
                                         stroke_width_,
                                         stroke_style_.Get());
        }
        render_target_->FillGeometry(transformed_geometry.Get(), fill_brush_.Get());

        return S_OK;
    }

    IFACEMETHOD(DrawUnderline)(void* client_drawing_context,
                               float baseline_origin_x,
                               float baseline_origin_y,
                               DWRITE_UNDERLINE const* underline,
                               IUnknown* client_drawing_effect) {
        underline_draw_cb_(underline);
        return S_OK;
    }

    IFACEMETHOD(DrawStrikethrough)(void* client_drawing_context,
                                   float baseline_origin_x,
                                   float baseline_origin_y,
                                   DWRITE_STRIKETHROUGH const* strike_through,
                                   IUnknown* client_drawing_effect) {
        return E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(void* client_drawing_context,
                                  float origin_x,
                                  float origin_y,
                                  IDWriteInlineObject* inline_object,
                                  BOOL is_sideways,
                                  BOOL is_right_to_left,
                                  IUnknown* client_drawing_effect) {
        return E_NOTIMPL;
    }
public:
    IFACEMETHOD_(unsigned long, AddRef)() {
        return InterlockedIncrement(&ref_count_);
    }

    IFACEMETHOD_(unsigned long, Release)() {
        unsigned long result = InterlockedDecrement(&ref_count_);
        if (result == 0) {
            delete this;
            return 0;
        }