/*
 * Copyright (C) 2022 magicxqq <xqq@xqq.im>. All rights reserved.
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

#ifndef ARIBCAPTION_SAMPLE_DATA_H
#define ARIBCAPTION_SAMPLE_DATA_H

#ifdef __cplusplus
    #include <cstdint>
    #define SAMPLE_DATA_DECLEAR inline constexpr uint8_t
#else
    #include <stdint.h>
    #define SAMPLE_DATA_DECLEAR static const uint8_t
#endif

SAMPLE_DATA_DECLEAR sample_data_1[] = {
    0x80, 0xff, 0xf0, 0x04, 0x00, 0x00, 0x00, 0x4e, 0x3f, 0x00, 0x00, 0x4a, 0x1f, 0x20, 0x00, 0x00, 0x01, 0x0c,
    0x1f, 0x20, 0x00, 0x00, 0x3f, 0x9b, 0x37, 0x20, 0x53, 0x9b, 0x31, 0x37, 0x30, 0x3b, 0x33, 0x30, 0x20, 0x5f,
    0x9b, 0x36, 0x32, 0x30,