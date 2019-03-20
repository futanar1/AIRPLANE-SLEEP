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

#ifndef ARIBCAPTION_ALIGNED_ALLOC_HPP
#define ARIBCAPTION_ALIGNED_ALLOC_HPP

#include <cstddef>
#include <new>
#include <type_traits>
#include "aribcc_export.h"

namespace aribcaption {

ARIBCC_API void* AlignedAlloc(size_t size, size_t alignment);

ARIBCC_API void AlignedFree(void* ptr)