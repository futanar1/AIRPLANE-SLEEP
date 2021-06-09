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

#include <cmath>
#include <algorithm>
#include <iterator>
#include "aribcaption/context.hpp"
#include "renderer/bitmap.hpp"
#include "renderer/canvas.hpp"
#include "renderer/renderer_impl.hpp"

namespace aribcaption::internal {

RendererImpl::RendererImpl(Context& context)
    : context_(context), log_(GetContextLogger(context)), region_renderer_(context) {}

RendererImpl::~RendererImpl() = default;

bool RendererImpl::Initialize(CaptionType caption_type,
                              FontProviderType font_provider_type,
                              TextRendererType text_renderer_type) {
    expected_caption_type_ = caption_type;
    LoadDefaultFontFamilies();
    return region_renderer_.Initialize(font_provider_type, text_renderer_type);
}

void RendererImpl::LoadDefaultFontFamilies() {
    // Font face for default language (0)
    language_font_family_[0] = { "sans-serif" };

    // Default fonts for Japanese (jpn)
    std::vector<std::string> jpn_default_font_family;
#if defined(_WIN32)
    jpn_default_font_family = {
        "Windows TV MaruGothic",
        "MS Gothic",
        "sans-serif",
    };
#elif defined(__APPLE__)
    jpn_default_font_family = {
        "Hiragino Maru Gothic ProN",
        "Hiragino Sans",
        "sans-serif",
    };
#elif defined(__ANDROID__)
    jpn_default_font_family = {
        "sans-serif"
    };
#else  // Linux, or other platforms
    jpn_default_font_family = {
        "Noto Sans CJK JP",
        "Noto Sans CJK",
        "Source Han Sans JP",
        "sans-serif",
    };
#endif
    language_font_family_[ThreeCC("jpn")] = std::move(jpn_default_font_family);

    // Default fonts for latin languages (Portuguese / Spanish)
    std::vector<std::string> latin_default_font_family = { "sans-serif" };
    language_font_family_[ThreeCC("por")] = latin_default_font_family;  // Portuguese
    language_font_family_[ThreeCC("spa")] = latin_default_font_family;  // Spanish
}

void RendererImpl::SetStrokeWidth(float dots) {
    region_renderer_.SetStrokeWidth(dots);
    InvalidatePrevRenderedImages();
}

void RendererImpl::SetReplaceDRCS(bool replace) {
    region_renderer_.SetReplaceDRCS(replace);
    InvalidatePrevRenderedImages();
}

void RendererImpl::SetForceStrokeText(bool force_stroke) {
    region_renderer_.SetForceStrokeText(force_stroke);
    InvalidatePrevRenderedImages();
}

void RendererImpl::SetForceNoRuby(bool force_no_ruby) {
    force_no_ruby_ = force_no_ruby;
    InvalidatePrevRenderedImages();
}

void RendererImpl::SetForceNoBackground(bool force_no_background) {
    region_renderer_.SetForceNoBackground(force_no_background);
    InvalidatePrevRenderedImages();
}

void RendererImpl::SetMergeRegionImages(bool merge) {
    bool prev = merge_region_images_;
    merge_region_images_ = merge;
    if (prev != merge) {
        InvalidatePrevRenderedImages();
    }
}

bool RendererImpl::SetDefaultFontFamily(const std::vector<std::string>& font_family, bool force_default) {
    force_default_font_family_ = force_default;
    return SetLanguageSpecificFontFamily(0, font_family);
}

bool RendererImpl::SetLanguageSpecificFontFamily(uint32_t language_code, const std::vector<std::string>& font_family) {
    if (font_family.empty()) {
        return false;
    }

    language_font_family_[language_code] = font_family;

    InvalidatePrevRenderedImages();
    return true;
}

bool RendererImpl::SetFrameSize(int frame_width, int frame_height) {
    if (frame_width < 0 || frame_height < 0) {
        assert(frame_width >= 0 && frame_height >= 0 && "Frame width/height must >= 0");
        return false;
    }

    if (frame_width_ != frame_width || frame_height_ != frame_height) {
        InvalidatePrevRenderedImages();
    }

    frame_width_ = frame_width;
    frame_height_ = frame_height;
    frame_size_inited_ = true;

    SetMargins(margin_top_, margin_bottom_, margin_left_, margin_right_);

    return true;
}

bool RendererImpl::SetMargins(int top, int bottom, int left, int right) {
    if (!frame_size_inited_) {
        assert(frame_size_inited_ && "Frame size is not indicated, call SetFrameSize() first");
        return false;
    }

    int video_width = frame_width_ - left - right;
    int video_height = frame_height_ - top - bottom;

    if (video_width < 0 || video_height < 0) {
        log_->e("RendererImpl: Invalid margins, video area size attempts to be < 0");
        return false;
    }

    if (margin_top_ != top || margin_bottom_ != bottom || margin_left_ != left || margin_right_ != right) {
        InvalidatePrevRenderedImages();
    }

    video_area_width_ = video_width;
    video_area_height_ = video_height;
    video_area_start_x_ = left;
    video_area_start_y_ = top;
    video_area_size_inited_ = true;

    margin_top_ = top;
    margin_bottom_ = bottom;
    margin_left_ = left;
    margin_right_ = right;
    margins_inited_ = true;

    return true;
}

void RendererImpl::SetStoragePolicy(CaptionStoragePolicy policy, std::optional<size_t> upper_limit) {
    storage_policy_ = policy;

    if (policy == CaptionStoragePolicy::kUpperLimitCount) {
        assert(upper_limit.has_value());
        upper_limit_count_ = upper_limit.value();
    } else if (policy == CaptionStoragePolicy::kUpperLimitDuration) {
        assert(upper_limit.has_value());
        upper_limit_duration_ = upper_limit.value();
    }
}

bool RendererImpl::AppendCaption(const Caption& caption) {
    assert(caption.pts != PTS_NOPTS && "Caption without PTS is not supported");
    assert(caption.plane_width > 0 && caption.plane_height > 0);

    if (caption.pts == PTS_NOPTS || caption.plane_width <= 0 || caption.plane_height <= 0) {
        return false;
    }

    int64_t pts = caption.pts;

    if (captions_.empty()) {
        captions_.emplace(pts, caption);
    } else {
        auto prev = captions_.lower_bound(pts - 1);
        if (prev == captions_.end() || (prev != captions_.begin() && prev->first > pts - 1)) {
            --prev;
        }

        // Correct previous caption's duration
        if (prev->first < pts && prev->second.wait_duration == DURATION_INDEFINITE) {
            Caption& prev_caption = prev->second;
            prev_caption.wait_duration = pts - prev_caption.pts;
        }

        captions_.insert_or_assign(std::next(prev), pts, caption);
    }

    if (pts <= prev_rendered_caption_pts_) {
        InvalidatePrevRenderedImages();
    }

    CleanupCaptionsIfNecessary();
    return true;
}

bool RendererImpl::AppendCaption(Caption&& caption) {
    assert(caption.pts != PTS_NOPTS && "Caption without PTS is not supported");
    assert(caption.plane_width > 0 && caption.plane_height > 0);

    if (caption.pts == PTS_NOPTS || caption.plane_width <= 0 || caption.plane_height <= 0) {
        return false;
    }

    int64_t pts = caption.pts;

    if (captions_.empty()) {
        captions_.emplace(pts, std::move(caption));
    } else {
        auto prev = captions_.lower_bound(pts - 1);
        if (prev == captions_.end() || (prev != captions_.begin() && prev->first > pts - 1)) {
            --prev;
        }

        // Correct previous caption's duration
        if (prev->first < pts && prev->second.wait_duration == DURATION_INDEFINITE) {
            Caption& prev_caption = prev->second;
            prev_caption.wait_duration = pts - prev_caption.pts;
        }

        captions_.insert_or_assign(std::next(prev), pts, std::move(caption));
    }

    if (pts <= prev_rendered_caption_pts_) {
        InvalidatePrevRenderedImages();
    }

    CleanupCaptionsIfNecessary();
    return true;
}

void RendererImpl::CleanupCaptionsIfNecessary() {
    if (storage_policy_ == CaptionStoragePolicy::kUnlimited) {
        return;
    } else if (storage_policy_ == CaptionStoragePolicy::kMinimum) {
        if (prev_rendered_caption_pts_ == PTS_NOPTS) {
            return;
        }
        auto prev_rendered_caption_iter = captions_.find(prev_rendered_caption_pts_);
        if (prev_rendered_caption_iter != captions_.end()) {
            captions_.erase(captions_.begin(), prev_rendered_caption_iter);
        }
    } else if (storage_policy_ == CaptionStoragePolicy::kUpperLimitCount) {
        if (captions_.size() <= upper_limit_count_) {
            return;
        }
        auto erase_end = std::prev(captions_.end(), static_cast<ptrdiff_t>(upper_limit_count_));
        if (erase_end != captions_.begin()) {
            captions_.erase(captions_.begin(), erase_end);
        }
    } else if (storage_policy_ == CaptionStoragePolicy::kUpperLimitDuration) {
        if (captions_.empty()) {
            return;
        }
        int64_t last_caption_pts = captions_.rbegin()->first;
        int64_t e