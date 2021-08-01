
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

#ifndef ARIBCAPTION_RENDERER_IMPL_HPP
#define ARIBCAPTION_RENDERER_IMPL_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <map>
#include "aribcaption/caption.hpp"
#include "aribcaption/renderer.hpp"
#include "base/logger.hpp"
#include "renderer/region_renderer.hpp"

namespace aribcaption::internal {

class RendererImpl {
public:
    explicit RendererImpl(Context& context);
    ~RendererImpl();
public:
    bool Initialize(CaptionType caption_type = CaptionType::kCaption,
                    FontProviderType font_provider_type = FontProviderType::kAuto,
                    TextRendererType text_renderer_type = TextRendererType::kAuto);

    void SetStrokeWidth(float dots);
    void SetReplaceDRCS(bool replace);
    void SetForceStrokeText(bool force_stroke);
    void SetForceNoRuby(bool force_no_ruby);
    void SetForceNoBackground(bool force_no_background);
    void SetMergeRegionImages(bool merge);

    bool SetDefaultFontFamily(const std::vector<std::string>& font_family, bool force_default);
    bool SetLanguageSpecificFontFamily(uint32_t language_code, const std::vector<std::string>& font_family);
    bool SetFrameSize(int frame_width, int frame_height);
    bool SetMargins(int top, int bottom, int left, int right);

    void SetStoragePolicy(CaptionStoragePolicy policy, std::optional<size_t> upper_limit = std::nullopt);

    bool AppendCaption(const Caption& caption);
    bool AppendCaption(Caption&& caption);

    RenderStatus TryRender(int64_t pts);
    RenderStatus Render(int64_t pts, RenderResult& out_result);
    void Flush();
private:
    void LoadDefaultFontFamilies();
    void CleanupCaptionsIfNecessary();
    void AdjustCaptionArea(int origin_plane_width, int origin_plane_height);
    void InvalidatePrevRenderedImages();
private:
    static Image MergeImages(std::vector<Image>& images);
public:
    RendererImpl(const RendererImpl&) = delete;
    RendererImpl& operator=(const RendererImpl&) = delete;
private:
    Context& context_;
    std::shared_ptr<Logger> log_;

    CaptionType expected_caption_type_ = CaptionType::kDefault;

    // iso639_language_code => FontFamily