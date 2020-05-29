
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

#ifndef ARIBCAPTION_ALPHABLEND_X86_HPP
#define ARIBCAPTION_ALPHABLEND_X86_HPP

#include <xmmintrin.h>  // SSE
#include <emmintrin.h>  // SSE2
#include <algorithm>
#include "renderer/alphablend_generic.hpp"

// Workaround Windows.h (minwindef.h) max/min macro definitions
#ifdef max
    #undef max
#endif

#ifdef min
    #undef min
#endif

namespace aribcaption::alphablend::internal {

namespace x86 {

#if defined(__SSE2__) || defined(_MSC_VER)

union alignas(16) Vec128i {
    uint8_t u8[16];
    __m128i m128i;
};

ALWAYS_INLINE void FillLine_SSE2(ColorRGBA* __restrict dest, ColorRGBA color, size_t width) {
    if (uint32_t unaligned_prefix_pixels = (reinterpret_cast<uintptr_t>(dest) % 16) / 4) {
        uint32_t unaligned_pixels = std::min(static_cast<uint32_t>(width), 4 - unaligned_prefix_pixels);
        FillLine_Generic(dest, color, unaligned_pixels);
        dest += unaligned_pixels;
        width -= unaligned_pixels;
    }

    uint32_t trailing_remain_pixels = 0;
    if ((trailing_remain_pixels = width % 4) != 0) {
        width -= trailing_remain_pixels;
    }

    __m128i color4 = _mm_set1_epi32(static_cast<int>(color.u32));

    for (size_t i = 0; i < width; i += 4, dest += 4) {
        _mm_store_si128(reinterpret_cast<__m128i*>(dest), color4);
    }

    if (trailing_remain_pixels) {
        FillLine_Generic(dest, color, trailing_remain_pixels);
    }
}

ALWAYS_INLINE void FillLineWithAlphas_SSE2(ColorRGBA* __restrict dest,
                                           const uint8_t* __restrict src, ColorRGBA color, size_t width) {
    //            RGBA_0xAABBGGRR
    const __m128i mask_0xffffffff = _mm_cmpeq_epi8(_mm_setzero_si128(), _mm_setzero_si128());
    const __m128i mask_0xff000000 = _mm_slli_epi32(mask_0xffffffff, 24);
    const __m128i mask_0x00ffffff = _mm_srli_epi32(mask_0xffffffff, 8);
