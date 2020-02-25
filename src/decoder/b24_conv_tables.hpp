
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

#ifndef ARIBCAPTION_B24_CONV_TABLES_HPP
#define ARIBCAPTION_B24_CONV_TABLES_HPP

#include <cstdint>
#include <unordered_map>

namespace aribcaption {

// Unicode codepoints (UCS4)

inline constexpr uint32_t kAlphanumericTable_Halfwidth[] = {
    0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028,
    0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f, 0x0030,
    0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038,
    0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f, 0x0040,
    0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048,
    0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0050,
    0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058,
    0x0059, 0x005a, 0x005b, 0x00a5, 0x005d, 0x005e, 0x005f, 0x0060,
    0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068,
    0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f, 0x0070,
    0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078,
    0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x203e
};

inline constexpr uint32_t kAlphanumericTable_Fullwidth[] = {
    0xff01, 0xff02, 0xff03, 0xff04, 0xff05, 0xff06, 0xff07, 0xff08,
    0xff09, 0xff0a, 0xff0b, 0xff0c, 0xff0d, 0xff0e, 0xff0f, 0xff10,
    0xff11, 0xff12, 0xff13, 0xff14, 0xff15, 0xff16, 0xff17, 0xff18,
    0xff19, 0xff1a, 0xff1b, 0xff1c, 0xff1d, 0xff1e, 0xff1f, 0xff20,
    0xff21, 0xff22, 0xff23, 0xff24, 0xff25, 0xff26, 0xff27, 0xff28,
    0xff29, 0xff2a, 0xff2b, 0xff2c, 0xff2d, 0xff2e, 0xff2f, 0xff30,
    0xff31, 0xff32, 0xff33, 0xff34, 0xff35, 0xff36, 0xff37, 0xff38,
    0xff39, 0xff3a, 0xff3b, 0xffe5, 0xff3d, 0xff3e, 0xff3f, 0xff40,
    0xff41, 0xff42, 0xff43, 0xff44, 0xff45, 0xff46, 0xff47, 0xff48,
    0xff49, 0xff4a, 0xff4b, 0xff4c, 0xff4d, 0xff4e, 0xff4f, 0xff50,
    0xff51, 0xff52, 0xff53, 0xff54, 0xff55, 0xff56, 0xff57, 0xff58,
    0xff59, 0xff5a, 0xff5b, 0xff5c, 0xff5d, 0xffe3
};

inline constexpr uint32_t kAlphanumericTable_Latin[] = {
    0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028,
    0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f, 0x0030,
    0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038,
    0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f, 0x0040,
    0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048,
    0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0050,
    0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058,
    0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f, 0x0060,
    0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068,
    0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f, 0x0070,
    0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078,
    0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e
};

inline constexpr uint32_t kLatinExtensionTable[] = {
    0x00a1, 0x00a2, 0x00a3, 0x20ac, 0x00a5, 0x0160, 0x00a7, 0x0161,
    0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ff, 0x00ae, 0x00af, 0x00b0,
    0x00b1, 0x00b2, 0x00b3, 0x017d, 0x00b5, 0x00b6, 0x00b7, 0x017e,
    0x00b9, 0x00ba, 0x00bb, 0x0152, 0x0153, 0x0178, 0x00bf, 0x00c0,
    0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, 0x00c8,
    0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, 0x00d0,
    0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, 0x00d8,
    0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, 0x00e0,
    0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, 0x00e8,
    0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, 0x00f0,
    0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, 0x00f8,
    0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe
};

inline constexpr uint32_t kLatinSpecialTable[] = {
    0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021,
    0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021,
    0x266a, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021,
    0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x00a4,
    0x00a6, 0x00a8, 0x00b4, 0x00b8, 0x00bc, 0x00bd, 0x00be, 0x0021,
    0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x2026,
    0x2588, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2122, 0x215b,
    0x215c, 0x215d, 0x215e, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021,
    0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021,
    0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021,
    0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021,
    0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021
};

inline constexpr uint32_t kHiraganaTable[] = {
    0x3041, 0x3042, 0x3043, 0x3044, 0x3045, 0x3046, 0x3047, 0x3048,
    0x3049, 0x304a, 0x304b, 0x304c, 0x304d, 0x304e, 0x304f, 0x3050,
    0x3051, 0x3052, 0x3053, 0x3054, 0x3055, 0x3056, 0x3057, 0x3058,
    0x3059, 0x305a, 0x305b, 0x305c, 0x305d, 0x305e, 0x305f, 0x3060,
    0x3061, 0x3062, 0x3063, 0x3064, 0x3065, 0x3066, 0x3067, 0x3068,
    0x3069, 0x306a, 0x306b, 0x306c, 0x306d, 0x306e, 0x306f, 0x3070,
    0x3071, 0x3072, 0x3073, 0x3074, 0x3075, 0x3076, 0x3077, 0x3078,
    0x3079, 0x307a, 0x307b, 0x307c, 0x307d, 0x307e, 0x307f, 0x3080,
    0x3081, 0x3082, 0x3083, 0x3084, 0x3085, 0x3086, 0x3087, 0x3088,
    0x3089, 0x308a, 0x308b, 0x308c, 0x308d, 0x308e, 0x308f, 0x3090,
    0x3091, 0x3092, 0x3093, 0x3000, 0x3000, 0x3000, 0x309d, 0x309e,
    0x30fc, 0x3002, 0x300c, 0x300d, 0x3001, 0x30fb
};

inline constexpr uint32_t kKatakanaTable[] = {
    0x30a1, 0x30a2, 0x30a3, 0x30a4, 0x30a5, 0x30a6, 0x30a7, 0x30a8,
    0x30a9, 0x30aa, 0x30ab, 0x30ac, 0x30ad, 0x30ae, 0x30af, 0x30b0,
    0x30b1, 0x30b2, 0x30b3, 0x30b4, 0x30b5, 0x30b6, 0x30b7, 0x30b8,
    0x30b9, 0x30ba, 0x30bb, 0x30bc, 0x30bd, 0x30be, 0x30bf, 0x30c0,
    0x30c1, 0x30c2, 0x30c3, 0x30c4, 0x30c5, 0x30c6, 0x30c7, 0x30c8,
    0x30c9, 0x30ca, 0x30cb, 0x30cc, 0x30cd, 0x30ce, 0x30cf, 0x30d0,
    0x30d1, 0x30d2, 0x30d3, 0x30d4, 0x30d5, 0x30d6, 0x30d7, 0x30d8,
    0x30d9, 0x30da, 0x30db, 0x30dc, 0x30dd, 0x30de, 0x30df, 0x30e0,
    0x30e1, 0x30e2, 0x30e3, 0x30e4, 0x30e5, 0x30e6, 0x30e7, 0x30e8,
    0x30e9, 0x30ea, 0x30eb, 0x30ec, 0x30ed, 0x30ee, 0x30ef, 0x30f0,
    0x30f1, 0x30f2, 0x30f3, 0x30f4, 0x30f5, 0x30f6, 0x30fd, 0x30fe,
    0x30fc, 0x3002, 0x300c, 0x300d, 0x3001, 0x30fb
};

inline constexpr uint32_t kJISX0201KatakanaTable[] = {
    0x3002, 0x300c, 0x300d, 0x3001, 0x30fb, 0x30f2, 0x30a1, 0x30a3,
    0x30a5, 0x30a7, 0x30a9, 0x30e3, 0x30e5, 0x30e7, 0x30c3, 0x30fc,
    0x30a2, 0x30a4, 0x30a6, 0x30a8, 0x30aa, 0x30ab, 0x30ad, 0x30af,
    0x30b1, 0x30b3, 0x30b5, 0x30b7, 0x30b9, 0x30bb, 0x30bd, 0x30bf,
    0x30c1, 0x30c4, 0x30c6, 0x30c8, 0x30ca, 0x30cb, 0x30cc, 0x30cd,
    0x30ce, 0x30cf, 0x30d2, 0x30d5, 0x30d8, 0x30db, 0x30de, 0x30df,
    0x30e0, 0x30e1, 0x30e2, 0x30e4, 0x30e6, 0x30e8, 0x30e9, 0x30ea,
    0x30eb, 0x30ec, 0x30ed, 0x30ef, 0x30f3, 0x309b, 0x309c, 0xfffd,
    0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
    0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
    0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
    0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
};

inline constexpr uint32_t kKanjiTable[] = {
    0x3000, 0x3001, 0x3002, 0xff0c, 0xff0e, 0x30fb, 0xff1a, 0xff1b,
    0xff1f, 0xff01, 0x309b, 0x309c, 0x00b4, 0xff40, 0x00a8, 0xff3e,
    0xffe3, 0xff3f, 0x30fd, 0x30fe, 0x309d, 0x309e, 0x3003, 0x4edd,
    0x3005, 0x3006, 0x3007, 0x30fc, 0x2015, 0x2010, 0xff0f, 0xff3c,
    0x301c, 0x2016, 0xff5c, 0x2026, 0x2025, 0x2018, 0x2019, 0x201c,
    0x201d, 0xff08, 0xff09, 0x3014, 0x3015, 0xff3b, 0xff3d, 0xff5b,
    0xff5d, 0x3008, 0x3009, 0x300a, 0x300b, 0x300c, 0x300d, 0x300e,
    0x300f, 0x3010, 0x3011, 0xff0b, 0x2212, 0x00b1, 0x00d7, 0x00f7,
    0xff1d, 0x2260, 0xff1c, 0xff1e, 0x2266, 0x2267, 0x221e, 0x2234,
    0x2642, 0x2640, 0x00b0, 0x2032, 0x2033, 0x2103, 0xffe5, 0xff04,
    0x00a2, 0x00a3, 0xff05, 0xff03, 0xff06, 0xff0a, 0xff20, 0x00a7,
    0x2606, 0x2605, 0x25cb, 0x25cf, 0x25ce, 0x25c7, 0x25c6, 0x25a1,
    0x25a0, 0x25b3, 0x25b2, 0x25bd, 0x25bc, 0x203b, 0x3012, 0x2192,
    0x2190, 0x2191, 0x2193, 0x3013, 0xff07, 0xff02, 0xff0d, 0xff5e,
    0x3033, 0x3034, 0x3035, 0x303b, 0x303c, 0x30ff, 0x309f, 0x2208,
    0x220b, 0x2286, 0x2287, 0x2282, 0x2283, 0x222a, 0x2229, 0x2284,
    0x2285, 0x228a, 0x228b, 0x2209, 0x2205, 0x2305, 0x2306, 0x2227,
    0x2228, 0x00ac, 0x21d2, 0x21d4, 0x2200, 0x2203, 0x2295, 0x2296,
    0x2297, 0x2225, 0x2226, 0x2985, 0x2986, 0x3018, 0x3019, 0x3016,
    0x3017, 0x2220, 0x22a5, 0x2312, 0x2202, 0x2207, 0x2261, 0x2252,
    0x226a, 0x226b, 0x221a, 0x223d, 0x221d, 0x2235, 0x222b, 0x222c,
    0x2262, 0x2243, 0x2245, 0x2248, 0x2276, 0x2277, 0x2194, 0x212b,
    0x2030, 0x266f, 0x266d, 0x266a, 0x2020, 0x2021, 0x00b6, 0x266e,
    0x266b, 0x266c, 0x2669, 0x25ef, 0x25b7, 0x25b6, 0x25c1, 0x25c0,
    0x2197, 0x2198, 0x2196, 0x2199, 0x21c4, 0x21e8, 0x21e6, 0x21e7,
    0x21e9, 0x2934, 0x2935, 0xff10, 0xff11, 0xff12, 0xff13, 0xff14,
    0xff15, 0xff16, 0xff17, 0xff18, 0xff19, 0x29bf, 0x25c9, 0x303d,
    0xfe46, 0xfe45, 0x25e6, 0x2022, 0xff21, 0xff22, 0xff23, 0xff24,
    0xff25, 0xff26, 0xff27, 0xff28, 0xff29, 0xff2a, 0xff2b, 0xff2c,
    0xff2d, 0xff2e, 0xff2f, 0xff30, 0xff31, 0xff32, 0xff33, 0xff34,
    0xff35, 0xff36, 0xff37, 0xff38, 0xff39, 0xff3a, 0x2213, 0x2135,
    0x210f, 0x33cb, 0x2113, 0x2127, 0xff41, 0xff42, 0xff43, 0xff44,
    0xff45, 0xff46, 0xff47, 0xff48, 0xff49, 0xff4a, 0xff4b, 0xff4c,
    0xff4d, 0xff4e, 0xff4f, 0xff50, 0xff51, 0xff52, 0xff53, 0xff54,
    0xff55, 0xff56, 0xff57, 0xff58, 0xff59, 0xff5a, 0x30a0, 0x2013,
    0x29fa, 0x29fb, 0x3041, 0x3042, 0x3043, 0x3044, 0x3045, 0x3046,
    0x3047, 0x3048, 0x3049, 0x304a, 0x304b, 0x304c, 0x304d, 0x304e,
    0x304f, 0x3050, 0x3051, 0x3052, 0x3053, 0x3054, 0x3055, 0x3056,
    0x3057, 0x3058, 0x3059, 0x305a, 0x305b, 0x305c, 0x305d, 0x305e,
    0x305f, 0x3060, 0x3061, 0x3062, 0x3063, 0x3064, 0x3065, 0x3066,
    0x3067, 0x3068, 0x3069, 0x306a, 0x306b, 0x306c, 0x306d, 0x306e,
    0x306f, 0x3070, 0x3071, 0x3072, 0x3073, 0x3074, 0x3075, 0x3076,
    0x3077, 0x3078, 0x3079, 0x307a, 0x307b, 0x307c, 0x307d, 0x307e,
    0x307f, 0x3080, 0x3081, 0x3082, 0x3083, 0x3084, 0x3085, 0x3086,
    0x3087, 0x3088, 0x3089, 0x308a, 0x308b, 0x308c, 0x308d, 0x308e,
    0x308f, 0x3090, 0x3091, 0x3092, 0x3093, 0x3094, 0x3095, 0x3096,
    0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
    0x30a1, 0x30a2, 0x30a3, 0x30a4, 0x30a5, 0x30a6, 0x30a7, 0x30a8,
    0x30a9, 0x30aa, 0x30ab, 0x30ac, 0x30ad, 0x30ae, 0x30af, 0x30b0,
    0x30b1, 0x30b2, 0x30b3, 0x30b4, 0x30b5, 0x30b6, 0x30b7, 0x30b8,
    0x30b9, 0x30ba, 0x30bb, 0x30bc, 0x30bd, 0x30be, 0x30bf, 0x30c0,
    0x30c1, 0x30c2, 0x30c3, 0x30c4, 0x30c5, 0x30c6, 0x30c7, 0x30c8,
    0x30c9, 0x30ca, 0x30cb, 0x30cc, 0x30cd, 0x30ce, 0x30cf, 0x30d0,
    0x30d1, 0x30d2, 0x30d3, 0x30d4, 0x30d5, 0x30d6, 0x30d7, 0x30d8,
    0x30d9, 0x30da, 0x30db, 0x30dc, 0x30dd, 0x30de, 0x30df, 0x30e0,
    0x30e1, 0x30e2, 0x30e3, 0x30e4, 0x30e5, 0x30e6, 0x30e7, 0x30e8,
    0x30e9, 0x30ea, 0x30eb, 0x30ec, 0x30ed, 0x30ee, 0x30ef, 0x30f0,
    0x30f1, 0x30f2, 0x30f3, 0x30f4, 0x30f5, 0x30f6, 0xfffd, 0xfffd,
    0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0x0391, 0x0392,
    0x0393, 0x0394, 0x0395, 0x0396, 0x0397, 0x0398, 0x0399, 0x039a,
    0x039b, 0x039c, 0x039d, 0x039e, 0x039f, 0x03a0, 0x03a1, 0x03a3,
    0x03a4, 0x03a5, 0x03a6, 0x03a7, 0x03a8, 0x03a9, 0x2664, 0x2660,
    0x2662, 0x2666, 0x2661, 0x2665, 0x2667, 0x2663, 0x03b1, 0x03b2,
    0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7, 0x03b8, 0x03b9, 0x03ba,
    0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf, 0x03c0, 0x03c1, 0x03c3,
    0x03c4, 0x03c5, 0x03c6, 0x03c7, 0x03c8, 0x03c9, 0x03c2, 0x24f5,
    0x24f6, 0x24f7, 0x24f8, 0x24f9, 0x24fa, 0x24fb, 0x24fc, 0x24fd,
    0x24fe, 0x2616, 0x2617, 0x3020, 0x260e, 0x2600, 0x2601, 0x2602,
    0x2603, 0x2668, 0x25b1, 0x31f0, 0x31f1, 0x31f2, 0x31f3, 0x31f4,
    0x31f5, 0x31f6, 0x31f7, 0x31f8, 0x31f9, 0xfffd, 0x31fa, 0x31fb,
    0x31fc, 0x31fd, 0x31fe, 0x31ff, 0x0410, 0x0411, 0x0412, 0x0413,
    0x0414, 0x0415, 0x0401, 0x0416, 0x0417, 0x0418, 0x0419, 0x041a,
    0x041b, 0x041c, 0x041d, 0x041e, 0x041f, 0x0420, 0x0421, 0x0422,
    0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042a,
    0x042b, 0x042c, 0x042d, 0x042e, 0x042f, 0x23be, 0x23bf, 0x23c0,
    0x23c1, 0x23c2, 0x23c3, 0x23c4, 0x23c5, 0x23c6, 0x23c7, 0x23c8,
    0x23c9, 0x23ca, 0x23cb, 0x23cc, 0x0430, 0x0431, 0x0432, 0x0433,
    0x0434, 0x0435, 0x0451, 0x0436, 0x0437, 0x0438, 0x0439, 0x043a,
    0x043b, 0x043c, 0x043d, 0x043e, 0x043f, 0x0440, 0x0441, 0x0442,
    0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044a,
    0x044b, 0x044c, 0x044d, 0x044e, 0x044f, 0x30f7, 0x30f8, 0x30f9,
    0x30fa, 0x22da, 0x22db, 0x2153, 0x2154, 0x2155, 0x2713, 0x2318,
    0x2423, 0x23ce, 0x2500, 0x2502, 0x250c, 0x2510, 0x2518, 0x2514,
    0x251c, 0x252c, 0x2524, 0x2534, 0x253c, 0x2501, 0x2503, 0x250f,
    0x2513, 0x251b, 0x2517, 0x2523, 0x2533, 0x252b, 0x253b, 0x254b,
    0x2520, 0x252f, 0x2528, 0x2537, 0x253f, 0x251d, 0x2530, 0x2525,
    0x2538, 0x2542, 0x3251, 0x3252, 0x3253, 0x3254, 0x3255, 0x3256,
    0x3257, 0x3258, 0x3259, 0x325a, 0x325b, 0x325c, 0x325d, 0x325e,
    0x325f, 0x32b1, 0x32b2, 0x32b3, 0x32b4, 0x32b5, 0x32b6, 0x32b7,
    0x32b8, 0x32b9, 0x32ba, 0x32bb, 0x32bc, 0x32bd, 0x32be, 0x32bf,
    0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
    0x25d0, 0x25d1, 0x25d2, 0x25d3, 0x203c, 0x2047, 0x2048, 0x2049,
    0x01cd, 0x01ce, 0x01d0, 0x1e3e, 0x1e3f, 0x01f8, 0x01f9, 0x01d1,
    0x01d2, 0x01d4, 0x01d6, 0x01d8, 0x01da, 0x01dc, 0xfffd, 0xfffd,
    0x20ac, 0x00a0, 0x00a1, 0x00a4, 0x00a6, 0x00a9, 0x00aa, 0x00ab,
    0x00ad, 0x00ae, 0x00af, 0x00b2, 0x00b3, 0x00b7, 0x00b8, 0x00b9,
    0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, 0x00c0, 0x00c1,
    0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, 0x00c8, 0x00c9,
    0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, 0x00d0, 0x00d1,
    0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d8, 0x00d9, 0x00da,
    0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, 0x00e0, 0x00e1, 0x00e2,
    0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, 0x00e8, 0x00e9, 0x00ea,
    0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, 0x00f0, 0x00f1, 0x00f2,
    0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f8, 0x00f9, 0x00fa, 0x00fb,
    0x00fc, 0x00fd, 0x00fe, 0x00ff, 0x0100, 0x012a, 0x016a, 0x0112,
    0x014c, 0x0101, 0x012b, 0x016b, 0x0113, 0x014d, 0x0104, 0x02d8,
    0x0141, 0x013d, 0x015a, 0x0160, 0x015e, 0x0164, 0x0179, 0x017d,
    0x017b, 0x0105, 0x02db, 0x0142, 0x013e, 0x015b, 0x02c7, 0x0161,
    0x015f, 0x0165, 0x017a, 0x02dd, 0x017e, 0x017c, 0x0154, 0x0102,
    0x0139, 0x0106, 0x010c, 0x0118, 0x011a, 0x010e, 0x0143, 0x0147,
    0x0150, 0x0158, 0x016e, 0x0170, 0x0162, 0x0155, 0x0103, 0x013a,
    0x0107, 0x010d, 0x0119, 0x011b, 0x010f, 0x0111, 0x0144, 0x0148,
    0x0151, 0x0159, 0x016f, 0x0171, 0x0163, 0x02d9, 0x0108, 0x011c,
    0x0124, 0x0134, 0x015c, 0x016c, 0x0109, 0x011d, 0x0125, 0x0135,
    0x015d, 0x016d, 0x0271, 0x028b, 0x027e, 0x0283, 0x0292, 0x026c,
    0x026e, 0x0279, 0x0288, 0x0256, 0x0273, 0x027d, 0x0282, 0x0290,
    0x027b, 0x026d, 0x025f, 0x0272, 0x029d, 0x028e, 0x0261, 0x014b,
    0x0270, 0x0281, 0x0127, 0x0295, 0x0294, 0x0266, 0x0298, 0x01c2,
    0x0253, 0x0257, 0x0284, 0x0260, 0x0193, 0x0153, 0x0152, 0x0268,
    0x0289, 0x0258, 0x0275, 0x0259, 0x025c, 0x025e, 0x0250, 0x026f,
    0x028a, 0x0264, 0x028c, 0x0254, 0x0251, 0x0252, 0x028d, 0x0265,
    0x02a2, 0x02a1, 0x0255, 0x0291, 0x027a, 0x0267, 0x025a, 0xfffd,
    0x01fd, 0x1f70, 0x1f71, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd,
    0xfffd, 0xfffd, 0xfffd, 0x1f72, 0x1f73, 0x0361, 0x02c8, 0x02cc,
    0x02d0, 0x02d1, 0x0306, 0x203f, 0x030b, 0x0301, 0x0304, 0x0300,
    0x030f, 0x030c, 0x0302, 0x02e5, 0x02e6, 0x02e7, 0x02e8, 0x02e9,
    0xfffd, 0xfffd, 0x0325, 0x032c, 0x0339, 0x031c, 0x031f, 0x0320,
    0x0308, 0x033d, 0x0329, 0x032f, 0x02de, 0x0324, 0x0330, 0x033c,
    0x0334, 0x031d, 0x031e, 0x0318, 0x0319, 0x032a, 0x033a, 0x033b,
    0x0303, 0x031a, 0x2776, 0x2777, 0x2778, 0x2779, 0x277a, 0x277b,
    0x277c, 0x277d, 0x277e, 0x277f, 0x24eb, 0x24ec, 0x24ed, 0x24ee,
    0x24ef, 0x24f0, 0x24f1, 0x24f2, 0x24f3, 0x24f4, 0x2170, 0x2171,
    0x2172, 0x2173, 0x2174, 0x2175, 0x2176, 0x2177, 0x2178, 0x2179,
    0x217a, 0x217b, 0x24d0, 0x24d1, 0x24d2, 0x24d3, 0x24d4, 0x24d5,
    0x24d6, 0x24d7, 0x24d8, 0x24d9, 0x24da, 0x24db, 0x24dc, 0x24dd,
    0x24de, 0x24df, 0x24e0, 0x24e1, 0x24e2, 0x24e3, 0x24e4, 0x24e5,
    0x24e6, 0x24e7, 0x24e8, 0x24e9, 0x32d0, 0x32d1, 0x32d2, 0x32d3,
    0x32d4, 0x32d5, 0x32d6, 0x32d7, 0x32d8, 0x32d9, 0x32da, 0x32db,
    0x32dc, 0x32dd, 0x32de, 0x32df, 0x32e0, 0x32e1, 0x32e2, 0x32e3,
    0x32fa, 0x32e9, 0x32e5, 0x32ed, 0x32ec, 0xfffd, 0xfffd, 0xfffd,
    0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0x2051, 0x2042,
    0x2460, 0x2461, 0x2462, 0x2463, 0x2464, 0x2465, 0x2466, 0x2467,
    0x2468, 0x2469, 0x246a, 0x246b, 0x246c, 0x246d, 0x246e, 0x246f,
    0x2470, 0x2471, 0x2472, 0x2473, 0x2160, 0x2161, 0x2162, 0x2163,
    0x2164, 0x2165, 0x2166, 0x2167, 0x2168, 0x2169, 0x216a, 0x3349,
    0x3314, 0x3322, 0x334d, 0x3318, 0x3327, 0x3303, 0x3336, 0x3351,
    0x3357, 0x330d, 0x3326, 0x3323, 0x332b, 0x334a, 0x333b, 0x339c,
    0x339d, 0x339e, 0x338e, 0x338f, 0x33c4, 0x33a1, 0x216b, 0xfffd,
    0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0x337b, 0x301d,
    0x301f, 0x2116, 0x33cd, 0x2121, 0x32a4, 0x32a5, 0x32a6, 0x32a7,
    0x32a8, 0x3231, 0x3232, 0x3239, 0x337e, 0x337d, 0x337c, 0xfffd,
    0xfffd, 0xfffd, 0x222e, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0x221f,
    0x22bf, 0xfffd, 0xfffd, 0xfffd, 0x2756, 0x261e, 0x4ff1, 0x2000b,
    0x3402, 0x4e28, 0x4e2f, 0x4e30, 0x4e8d, 0x4ee1, 0x4efd, 0x4eff,
    0x4f03, 0x4f0b, 0x4f60, 0x4f48, 0x4f49, 0x4f56, 0x4f5f, 0x4f6a,
    0x4f6c, 0x4f7e, 0x4f8a, 0x4f94, 0x4f97, 0xfa30, 0x4fc9, 0x4fe0,
    0x5001, 0x5002, 0x500e, 0x5018, 0x5027, 0x502e, 0x5040, 0x503b,
    0x5041, 0x5094, 0x50cc, 0x50f2, 0x50d0, 0x50e6, 0xfa31, 0x5106,
    0x5103, 0x510b, 0x511e, 0x5135, 0x514a, 0xfa32, 0x5155, 0x5157,
    0x34b5, 0x519d, 0x51c3, 0x51ca, 0x51de, 0x51e2, 0x51ee, 0x5201,
    0x34db, 0x5213, 0x5215, 0x5249, 0x5257, 0x5261, 0x5293, 0x52c8,
    0xfa33, 0x52cc, 0x52d0, 0x52d6, 0x52db, 0xfa34, 0x52f0, 0x52fb,
    0x5300, 0x5307, 0x531c, 0xfa35, 0x5361, 0x5363, 0x537d, 0x5393,
    0x539d, 0x53b2, 0x5412, 0x5427, 0x544d, 0x549c, 0x546b, 0x5474,
    0x547f, 0x5488, 0x5496, 0x54a1, 0x54a9, 0x54c6, 0x54ff, 0x550e,
    0x552b, 0x5535, 0x5550, 0x555e, 0x5581, 0x5586, 0x558e, 0xfa36,
    0x55ad, 0x55ce, 0xfa37, 0x5608, 0x560e, 0x563b, 0x5649, 0x5676,
    0x5666, 0xfa38, 0x566f, 0x5671, 0x5672, 0x5699, 0x569e, 0x56a9,
    0x56ac, 0x56b3, 0x56c9, 0x56ca, 0x570a, 0x2123d, 0x5721, 0x572f,
    0x5733, 0x5734, 0x5770, 0x5777, 0x577c, 0x579c, 0xfa0f, 0x2131b,
    0x57b8, 0x57c7, 0x57c8, 0x57cf, 0x57e4, 0x57ed, 0x57f5, 0x57f6,
    0x57ff, 0x5809, 0xfa10, 0x5861, 0x5864, 0xfa39, 0x587c, 0x5889,
    0x589e, 0xfa3a, 0x58a9, 0x2146e, 0x58d2, 0x58ce, 0x58d4, 0x58da,
    0x58e0, 0x58e9, 0x590c, 0x8641, 0x595d, 0x596d, 0x598b, 0x5992,
    0x59a4, 0x59c3, 0x59d2, 0x59dd, 0x5a13, 0x5a23, 0x5a67, 0x5a6d,
    0x5a77, 0x5a7e, 0x5a84, 0x5a9e, 0x5aa7, 0x5ac4, 0x218bd, 0x5b19,
    0x5b25, 0x525d, 0x4e9c, 0x5516, 0x5a03, 0x963f, 0x54c0, 0x611b,
    0x6328, 0x59f6, 0x9022, 0x8475, 0x831c, 0x7a50, 0x60aa, 0x63e1,
    0x6e25, 0x65ed, 0x8466, 0x82a6, 0x9bf5, 0x6893, 0x5727, 0x65a1,
    0x6271, 0x5b9b, 0x59d0, 0x867b, 0x98f4, 0x7d62, 0x7dbe, 0x9b8e,
    0x6216, 0x7c9f, 0x88b7, 0x5b89, 0x5eb5, 0x6309, 0x6697, 0x6848,
    0x95c7, 0x978d, 0x674f, 0x4ee5, 0x4f0a, 0x4f4d, 0x4f9d, 0x5049,
    0x56f2, 0x5937, 0x59d4, 0x5a01, 0x5c09, 0x60df, 0x610f, 0x6170,
    0x6613, 0x6905, 0x70ba, 0x754f, 0x7570, 0x79fb, 0x7dad, 0x7def,
    0x80c3, 0x840e, 0x8863, 0x8b02, 0x9055, 0x907a, 0x533b, 0x4e95,
    0x4ea5, 0x57df, 0x80b2, 0x90c1, 0x78ef, 0x4e00, 0x58f1, 0x6ea2,
    0x9038, 0x7a32, 0x8328, 0x828b, 0x9c2f, 0x5141, 0x5370, 0x54bd,
    0x54e1, 0x56e0, 0x59fb, 0x5f15, 0x98f2, 0x6deb, 0x80e4, 0x852d,
    0x9662, 0x9670, 0x96a0, 0x97fb, 0x540b, 0x53f3, 0x5b87, 0x70cf,
    0x7fbd, 0x8fc2, 0x96e8, 0x536f, 0x9d5c, 0x7aba, 0x4e11, 0x7893,
    0x81fc, 0x6e26, 0x5618, 0x5504, 0x6b1d, 0x851a, 0x9c3b, 0x59e5,
    0x53a9, 0x6d66, 0x74dc, 0x958f, 0x5642, 0x4e91, 0x904b, 0x96f2,
    0x834f, 0x990c, 0x53e1, 0x55b6, 0x5b30, 0x5f71, 0x6620, 0x66f3,
    0x6804, 0x6c38, 0x6cf3, 0x6d29, 0x745b, 0x76c8, 0x7a4e, 0x9834,
    0x82f1, 0x885b, 0x8a60, 0x92ed, 0x6db2, 0x75ab, 0x76ca, 0x99c5,
    0x60a6, 0x8b01, 0x8d8a, 0x95b2, 0x698e, 0x53ad, 0x5186, 0x5712,
    0x5830, 0x5944, 0x5bb4, 0x5ef6, 0x6028, 0x63a9, 0x63f4, 0x6cbf,
    0x6f14, 0x708e, 0x7114, 0x7159, 0x71d5, 0x733f, 0x7e01, 0x8276,
    0x82d1, 0x8597, 0x9060, 0x925b, 0x9d1b, 0x5869, 0x65bc, 0x6c5a,
    0x7525, 0x51f9, 0x592e, 0x5965, 0x5f80, 0x5fdc, 0x62bc, 0x65fa,
    0x6a2a, 0x6b27, 0x6bb4, 0x738b, 0x7fc1, 0x8956, 0x9d2c, 0x9d0e,
    0x9ec4, 0x5ca1, 0x6c96, 0x837b, 0x5104, 0x5c4b, 0x61b6, 0x81c6,
    0x6876, 0x7261, 0x4e59, 0x4ffa, 0x5378, 0x6069, 0x6e29, 0x7a4f,
    0x97f3, 0x4e0b, 0x5316, 0x4eee, 0x4f55, 0x4f3d, 0x4fa1, 0x4f73,
    0x52a0, 0x53ef, 0x5609, 0x590f, 0x5ac1, 0x5bb6, 0x5be1, 0x79d1,
    0x6687, 0x679c, 0x67b6, 0x6b4c, 0x6cb3, 0x706b, 0x73c2, 0x798d,
    0x79be, 0x7a3c, 0x7b87, 0x82b1, 0x82db, 0x8304, 0x8377, 0x83ef,
    0x83d3, 0x8766, 0x8ab2, 0x5629, 0x8ca8, 0x8fe6, 0x904e, 0x971e,
    0x868a, 0x4fc4, 0x5ce8, 0x6211, 0x7259, 0x753b, 0x81e5, 0x82bd,
    0x86fe, 0x8cc0, 0x96c5, 0x9913, 0x99d5, 0x4ecb, 0x4f1a, 0x89e3,
    0x56de, 0x584a, 0x58ca, 0x5efb, 0x5feb, 0x602a, 0x6094, 0x6062,
    0x61d0, 0x6212, 0x62d0, 0x6539, 0x9b41, 0x6666, 0x68b0, 0x6d77,
    0x7070, 0x754c, 0x7686, 0x7d75, 0x82a5, 0x87f9, 0x958b, 0x968e,
    0x8c9d, 0x51f1, 0x52be, 0x5916, 0x54b3, 0x5bb3, 0x5d16, 0x6168,
    0x6982, 0x6daf, 0x788d, 0x84cb, 0x8857, 0x8a72, 0x93a7, 0x9ab8,
    0x6d6c, 0x99a8, 0x86d9, 0x57a3, 0x67ff, 0x86ce, 0x920e, 0x5283,
    0x5687, 0x5404, 0x5ed3, 0x62e1, 0x64b9, 0x683c, 0x6838, 0x6bbb,
    0x7372, 0x78ba, 0x7a6b, 0x899a, 0x89d2, 0x8d6b, 0x8f03, 0x90ed,
    0x95a3, 0x9694, 0x9769, 0x5b66, 0x5cb3, 0x697d, 0x984d, 0x984e,
    0x639b, 0x7b20, 0x6a2b, 0x6a7f, 0x68b6, 0x9c0d, 0x6f5f, 0x5272,
    0x559d, 0x6070, 0x62ec, 0x6d3b, 0x6e07, 0x6ed1, 0x845b, 0x8910,
    0x8f44, 0x4e14, 0x9c39, 0x53f6, 0x691b, 0x6a3a, 0x9784, 0x682a,
    0x515c, 0x7ac3, 0x84b2, 0x91dc, 0x938c, 0x565b, 0x9d28, 0x6822,
    0x8305, 0x8431, 0x7ca5, 0x5208, 0x82c5, 0x74e6, 0x4e7e, 0x4f83,
    0x51a0, 0x5bd2, 0x520a, 0x52d8, 0x52e7, 0x5dfb, 0x559a, 0x582a,
    0x59e6, 0x5b8c, 0x5b98, 0x5bdb, 0x5e72, 0x5e79, 0x60a3, 0x611f,
    0x6163, 0x61be, 0x63db, 0x6562, 0x67d1, 0x6853, 0x68fa, 0x6b3e,
    0x6b53, 0x6c57, 0x6f22, 0x6f97, 0x6f45, 0x74b0, 0x7518, 0x76e3,
    0x770b, 0x7aff, 0x7ba1, 0x7c21, 0x7de9, 0x7f36, 0x7ff0, 0x809d,
    0x8266, 0x839e, 0x89b3, 0x8acc, 0x8cab, 0x9084, 0x9451, 0x9593,
    0x9591, 0x95a2, 0x9665, 0x97d3, 0x9928, 0x8218, 0x4e38, 0x542b,
    0x5cb8, 0x5dcc, 0x73a9, 0x764c, 0x773c, 0x5ca9, 0x7feb, 0x8d0b,
    0x96c1, 0x9811, 0x9854, 0x9858, 0x4f01, 0x4f0e, 0x5371, 0x559c,
    0x5668, 0x57fa, 0x5947, 0x5b09, 0x5bc4, 0x5c90, 0x5e0c, 0x5e7e,
    0x5fcc, 0x63ee, 0x673a, 0x65d7, 0x65e2, 0x671f, 0x68cb, 0x68c4,
    0x6a5f, 0x5e30, 0x6bc5, 0x6c17, 0x6c7d, 0x757f, 0x7948, 0x5b63,
    0x7a00, 0x7d00, 0x5fbd, 0x898f, 0x8a18, 0x8cb4, 0x8d77, 0x8ecc,
    0x8f1d, 0x98e2, 0x9a0e, 0x9b3c, 0x4e80, 0x507d, 0x5100, 0x5993,
    0x5b9c, 0x622f, 0x6280, 0x64ec, 0x6b3a, 0x72a0, 0x7591, 0x7947,
    0x7fa9, 0x87fb, 0x8abc, 0x8b70, 0x63ac, 0x83ca, 0x97a0, 0x5409,
    0x5403, 0x55ab, 0x6854, 0x6a58, 0x8a70, 0x7827, 0x6775, 0x9ecd,
    0x5374, 0x5ba2, 0x811a, 0x8650, 0x9006, 0x4e18, 0x4e45, 0x4ec7,
    0x4f11, 0x53ca, 0x5438, 0x5bae, 0x5f13, 0x6025, 0x6551, 0x673d,
    0x6c42, 0x6c72, 0x6ce3, 0x7078, 0x7403, 0x7a76, 0x7aae, 0x7b08,
    0x7d1a, 0x7cfe, 0x7d66, 0x65e7, 0x725b, 0x53bb, 0x5c45, 0x5de8,
    0x62d2, 0x62e0, 0x6319, 0x6e20, 0x865a, 0x8a31, 0x8ddd, 0x92f8,
    0x6f01, 0x79a6, 0x9b5a, 0x4ea8, 0x4eab, 0x4eac, 0x4f9b, 0x4fa0,
    0x50d1, 0x5147, 0x7af6, 0x5171, 0x51f6, 0x5354, 0x5321, 0x537f,
    0x53eb, 0x55ac, 0x5883, 0x5ce1, 0x5f37, 0x5f4a, 0x602f, 0x6050,
    0x606d, 0x631f, 0x6559, 0x6a4b, 0x6cc1, 0x72c2, 0x72ed, 0x77ef,
    0x80f8, 0x8105, 0x8208, 0x854e, 0x90f7, 0x93e1, 0x97ff, 0x9957,
    0x9a5a, 0x4ef0, 0x51dd, 0x5c2d, 0x6681, 0x696d, 0x5c40, 0x66f2,
    0x6975, 0x7389, 0x6850, 0x7c81, 0x50c5, 0x52e4, 0x5747, 0x5dfe,
    0x9326, 0x65a4, 0x6b23, 0x6b3d, 0x7434, 0x7981, 0x79bd, 0x7b4b,
    0x7dca, 0x82b9, 0x83cc, 0x887f, 0x895f, 0x8b39, 0x8fd1, 0x91d1,
    0x541f, 0x9280, 0x4e5d, 0x5036, 0x53e5, 0x533a, 0x72d7, 0x7396,
    0x77e9, 0x82e6, 0x8eaf, 0x99c6, 0x99c8, 0x99d2, 0x5177, 0x611a,
    0x865e, 0x55b0, 0x7a7a, 0x5076, 0x5bd3, 0x9047, 0x9685, 0x4e32,
    0x6adb, 0x91e7, 0x5c51, 0x5c48, 0x6398, 0x7a9f, 0x6c93, 0x9774,
    0x8f61, 0x7aaa, 0x718a, 0x9688, 0x7c82, 0x6817, 0x7e70, 0x6851,
    0x936c, 0x52f2, 0x541b, 0x85ab, 0x8a13, 0x7fa4, 0x8ecd, 0x90e1,
    0x5366, 0x8888, 0x7941, 0x4fc2, 0x50be, 0x5211, 0x5144, 0x5553,
    0x572d, 0x73ea, 0x578b, 0x5951, 0x5f62, 0x5f84, 0x6075, 0x6176,
    0x6167, 0x61a9, 0x63b2, 0x643a, 0x656c, 0x666f, 0x6842, 0x6e13,
    0x7566, 0x7a3d, 0x7cfb, 0x7d4c, 0x7d99, 0x7e4b, 0x7f6b, 0x830e,
    0x834a, 0x86cd, 0x8a08, 0x8a63, 0x8b66, 0x8efd, 0x981a, 0x9d8f,
    0x82b8, 0x8fce, 0x9be8, 0x5287, 0x621f, 0x6483, 0x6fc0, 0x9699,
    0x6841, 0x5091, 0x6b20, 0x6c7a, 0x6f54, 0x7a74, 0x7d50, 0x8840,
    0x8a23, 0x6708, 0x4ef6, 0x5039, 0x5026, 0x5065, 0x517c, 0x5238,
    0x5263, 0x55a7, 0x570f, 0x5805, 0x5acc, 0x5efa, 0x61b2, 0x61f8,
    0x62f3, 0x6372, 0x691c, 0x6a29, 0x727d, 0x72ac, 0x732e, 0x7814,
    0x786f, 0x7d79, 0x770c, 0x80a9, 0x898b, 0x8b19, 0x8ce2, 0x8ed2,
    0x9063, 0x9375, 0x967a, 0x9855, 0x9a13, 0x9e78, 0x5143, 0x539f,
    0x53b3, 0x5e7b, 0x5f26, 0x6e1b, 0x6e90, 0x7384, 0x73fe, 0x7d43,
    0x8237, 0x8a00, 0x8afa, 0x9650, 0x4e4e, 0x500b, 0x53e4, 0x547c,
    0x56fa, 0x59d1, 0x5b64, 0x5df1, 0x5eab, 0x5f27, 0x6238, 0x6545,
    0x67af, 0x6e56, 0x72d0, 0x7cca, 0x88b4, 0x80a1, 0x80e1, 0x83f0,
    0x864e, 0x8a87, 0x8de8, 0x9237, 0x96c7, 0x9867, 0x9f13, 0x4e94,
    0x4e92, 0x4f0d, 0x5348, 0x5449, 0x543e, 0x5a2f, 0x5f8c, 0x5fa1,
    0x609f, 0x68a7, 0x6a8e, 0x745a, 0x7881, 0x8a9e, 0x8aa4, 0x8b77,
    0x9190, 0x4e5e, 0x9bc9, 0x4ea4, 0x4f7c, 0x4faf, 0x5019, 0x5016,
    0x5149, 0x516c, 0x529f, 0x52b9, 0x52fe, 0x539a, 0x53e3, 0x5411,
    0x540e, 0x5589, 0x5751, 0x57a2, 0x597d, 0x5b54, 0x5b5d, 0x5b8f,
    0x5de5, 0x5de7, 0x5df7, 0x5e78, 0x5e83, 0x5e9a, 0x5eb7, 0x5f18,
    0x6052, 0x614c, 0x6297, 0x62d8, 0x63a7, 0x653b, 0x6602, 0x6643,
    0x66f4, 0x676d, 0x6821, 0x6897, 0x69cb, 0x6c5f, 0x6d2a, 0x6d69,
    0x6e2f, 0x6e9d, 0x7532, 0x7687, 0x786c, 0x7a3f, 0x7ce0, 0x7d05,
    0x7d18, 0x7d5e, 0x7db1, 0x8015, 0x8003, 0x80af, 0x80b1, 0x8154,
    0x818f, 0x822a, 0x8352, 0x884c, 0x8861, 0x8b1b, 0x8ca2, 0x8cfc,
    0x90ca, 0x9175, 0x9271, 0x783f, 0x92fc, 0x95a4, 0x964d, 0x9805,
    0x9999, 0x9ad8, 0x9d3b, 0x525b, 0x52ab, 0x53f7, 0x5408, 0x58d5,
    0x62f7, 0x6fe0, 0x8c6a, 0x8f5f, 0x9eb9, 0x514b, 0x523b, 0x544a,
    0x56fd, 0x7a40, 0x9177, 0x9d60, 0x9ed2, 0x7344, 0x6f09, 0x8170,
    0x7511, 0x5ffd, 0x60da, 0x9aa8, 0x72db, 0x8fbc, 0x6b64, 0x9803,
    0x4eca, 0x56f0, 0x5764, 0x58be, 0x5a5a, 0x6068, 0x61c7, 0x660f,
    0x6606, 0x6839, 0x68b1, 0x6df7, 0x75d5, 0x7d3a, 0x826e, 0x9b42,
    0x4e9b, 0x4f50, 0x53c9, 0x5506, 0x5d6f, 0x5de6, 0x5dee, 0x67fb,
    0x6c99, 0x7473, 0x7802, 0x8a50, 0x9396, 0x88df, 0x5750, 0x5ea7,
    0x632b, 0x50b5, 0x50ac, 0x518d, 0x6700, 0x54c9, 0x585e, 0x59bb,
    0x5bb0, 0x5f69, 0x624d, 0x63a1, 0x683d, 0x6b73, 0x6e08, 0x707d,
    0x91c7, 0x7280, 0x7815, 0x7826, 0x796d, 0x658e, 0x7d30, 0x83dc,
    0x88c1, 0x8f09, 0x969b, 0x5264, 0x5728, 0x6750, 0x7f6a, 0x8ca1,
    0x51b4, 0x5742, 0x962a, 0x583a, 0x698a, 0x80b4, 0x54b2, 0x5d0e,
    0x57fc, 0x7895, 0x9dfa, 0x4f5c, 0x524a, 0x548b, 0x643e, 0x6628,
    0x6714, 0x67f5, 0x7a84, 0x7b56, 0x7d22, 0x932f, 0x685c, 0x9bad,
    0x7b39, 0x5319, 0x518a, 0x5237, 0x5bdf, 0x62f6, 0x64ae, 0x64e6,
    0x672d, 0x6bba, 0x85a9, 0x96d1, 0x7690, 0x9bd6, 0x634c, 0x9306,
    0x9bab, 0x76bf, 0x6652, 0x4e09, 0x5098, 0x53c2, 0x5c71, 0x60e8,
    0x6492, 0x6563, 0x685f, 0x71e6, 0x73ca, 0x7523, 0x7b97, 0x7e82,
    0x8695, 0x8b83, 0x8cdb, 0x9178, 0x9910, 0x65ac, 0x66ab, 0x6b8b,
    0x4ed5, 0x4ed4, 0x4f3a, 0x4f7f, 0x523a, 0x53f8, 0x53f2, 0x55e3,
    0x56db, 0x58eb, 0x59cb, 0x59c9, 0x59ff, 0x5b50, 0x5c4d, 0x5e02,
    0x5e2b, 0x5fd7, 0x601d, 0x6307, 0x652f, 0x5b5c, 0x65af, 0x65bd,
    0x65e8, 0x679d, 0x6b62, 0x6b7b, 0x6c0f, 0x7345, 0x7949, 0x79c1,
    0x7cf8, 0x7d19, 0x7d2b, 0x80a2, 0x8102, 0x81f3, 0x8996, 0x8a5e,
    0x8a69, 0x8a66, 0x8a8c, 0x8aee, 0x8cc7, 0x8cdc, 0x96cc, 0x98fc,
    0x6b6f, 0x4e8b, 0x4f3c, 0x4f8d, 0x5150, 0x5b57, 0x5bfa, 0x6148,
    0x6301, 0x6642, 0x6b21, 0x6ecb, 0x6cbb, 0x723e, 0x74bd, 0x75d4,
    0x78c1, 0x793a, 0x800c, 0x8033, 0x81ea, 0x8494, 0x8f9e, 0x6c50,
    0x9e7f, 0x5f0f, 0x8b58, 0x9d2b, 0x7afa, 0x8ef8, 0x5b8d, 0x96eb,
    0x4e03, 0x53f1, 0x57f7, 0x5931, 0x5ac9, 0x5ba4, 0x6089, 0x6e7f,
    0x6f06, 0x75be, 0x8cea, 0x5b9f, 0x8500, 0x7be0, 0x5072, 0x67f4,
    0x829d, 0x5c61, 0x854a, 0x7e1e, 0x820e, 0x5199, 0x5c04, 0x6368,
    0x8d66, 0x659c, 0x716e, 0x793e, 0x7d17, 0x8005, 0x8b1d, 0x8eca,
    0x906e, 0x86c7, 0x90aa, 0x501f, 0x52fa, 0x5c3a, 0x6753, 0x707c,
    0x7235, 0x914c, 0x91c8, 0x932b, 0x82e5, 0x5bc2, 0x5f31, 0x60f9,
    0x4e3b, 0x53d6, 0x5b88, 0x624b, 0x6731, 0x6b8a, 0x72e9, 0x73e0,
    0x7a2e, 0x816b, 0x8da3, 0x9152, 0x9996, 0x5112, 0x53d7, 0x546a,
    0x5bff, 0x6388, 0x6a39, 0x7dac, 0x9700, 0x56da, 0x53ce, 0x5468,
    0x5b97, 0x5c31, 0x5dde, 0x4fee, 0x6101, 0x62fe, 0x6d32, 0x79c0,
    0x79cb, 0x7d42, 0x7e4d, 0x7fd2, 0x81ed, 0x821f, 0x8490, 0x8846,
    0x8972, 0x8b90, 0x8e74, 0x8f2f, 0x9031, 0x914b, 0x916c, 0x96c6,
    0x919c, 0x4ec0, 0x4f4f, 0x5145, 0x5341, 0x5f93, 0x620e, 0x67d4,
    0x6c41, 0x6e0b, 0x7363, 0x7e26, 0x91cd, 0x9283, 0x53d4, 0x5919,
    0x5bbf, 0x6dd1, 0x795d, 0x7e2e, 0x7c9b, 0x587e, 0x719f, 0x51fa,
    0x8853, 0x8ff0, 0x4fca, 0x5cfb, 0x6625, 0x77ac, 0x7ae3, 0x821c,
    0x99ff, 0x51c6, 0x5faa, 0x65ec, 0x696f, 0x6b89, 0x6df3, 0x6e96,
    0x6f64, 0x76fe, 0x7d14, 0x5de1, 0x9075, 0x9187, 0x9806, 0x51e6,
    0x521d, 0x6240, 0x6691, 0x66d9, 0x6e1a, 0x5eb6, 0x7dd2, 0x7f72,
    0x66f8, 0x85af, 0x85f7, 0x8af8, 0x52a9, 0x53d9, 0x5973, 0x5e8f,
    0x5f90, 0x6055, 0x92e4, 0x9664, 0x50b7, 0x511f, 0x52dd, 0x5320,
    0x5347, 0x53ec, 0x54e8, 0x5546, 0x5531, 0x5617, 0x5968, 0x59be,
    0x5a3c, 0x5bb5, 0x5c06, 0x5c0f, 0x5c11, 0x5c1a, 0x5e84, 0x5e8a,
    0x5ee0, 0x5f70, 0x627f, 0x6284, 0x62db, 0x638c, 0x6377, 0x6607,
    0x660c, 0x662d, 0x6676, 0x677e, 0x68a2, 0x6a1f, 0x6a35, 0x6cbc,
    0x6d88, 0x6e09, 0x6e58, 0x713c, 0x7126, 0x7167, 0x75c7, 0x7701,
    0x785d, 0x7901, 0x7965, 0x79f0, 0x7ae0, 0x7b11, 0x7ca7, 0x7d39,
    0x8096, 0x83d6, 0x848b, 0x8549, 0x885d, 0x88f3, 0x8a1f, 0x8a3c,
    0x8a54, 0x8a73, 0x8c61, 0x8cde, 0x91a4, 0x9266, 0x937e, 0x9418,
    0x969c, 0x9798, 0x4e0a, 0x4e08, 0x4e1e, 0x4e57, 0x5197, 0x5270,
    0x57ce, 0x5834, 0x58cc, 0x5b22, 0x5e38, 0x60c5, 0x64fe, 0x6761,
    0x6756, 0x6d44, 0x72b6, 0x7573, 0x7a63, 0x84b8, 0x8b72, 0x91b8,
    0x9320, 0x5631, 0x57f4, 0x98fe, 0x62ed, 0x690d, 0x6b96, 0x71ed,
    0x7e54, 0x8077, 0x8272, 0x89e6, 0x98df, 0x8755, 0x8fb1, 0x5c3b,
    0x4f38, 0x4fe1, 0x4fb5, 0x5507, 0x5a20, 0x5bdd, 0x5be9, 0x5fc3,
    0x614e, 0x632f, 0x65b0, 0x664b, 0x68ee, 0x699b, 0x6d78, 0x6df1,
    0x7533, 0x75b9, 0x771f, 0x795e, 0x79e6, 0x7d33, 0x81e3, 0x82af,
    0x85aa, 0x89aa, 0x8a3a, 0x8eab, 0x8f9b, 0x9032, 0x91dd, 0x9707,
    0x4eba, 0x4ec1, 0x5203, 0x5875, 0x58ec, 0x5c0b, 0x751a, 0x5c3d,
    0x814e, 0x8a0a, 0x8fc5, 0x9663, 0x976d, 0x7b25, 0x8acf, 0x9808,
    0x9162, 0x56f3, 0x53a8, 0x9017, 0x5439, 0x5782, 0x5e25, 0x63a8,
    0x6c34, 0x708a, 0x7761, 0x7c8b, 0x7fe0, 0x8870, 0x9042, 0x9154,
    0x9310, 0x9318, 0x968f, 0x745e, 0x9ac4, 0x5d07, 0x5d69, 0x6570,
    0x67a2, 0x8da8, 0x96db, 0x636e, 0x6749, 0x6919, 0x83c5, 0x9817,
    0x96c0, 0x88fe, 0x6f84, 0x647a, 0x5bf8, 0x4e16, 0x702c, 0x755d,
    0x662f, 0x51c4, 0x5236, 0x52e2, 0x59d3, 0x5f81, 0x6027, 0x6210,
    0x653f, 0x6574, 0x661f, 0x6674, 0x68f2, 0x6816, 0x6b63, 0x6e05,
    0x7272, 0x751f, 0x76db, 0x7cbe, 0x8056, 0x58f0, 0x88fd, 0x897f,
    0x8aa0, 0x8a93, 0x8acb, 0x901d, 0x9192, 0x9752, 0x9759, 0x6589,
    0x7a0e, 0x8106, 0x96bb, 0x5e2d, 0x60dc, 0x621a, 0x65a5, 0x6614,
    0x6790, 0x77f3, 0x7a4d, 0x7c4d, 0x7e3e, 0x810a, 0x8cac, 0x8d64,
    0x8de1, 0x8e5f, 0x78a9, 0x5207, 0x62d9, 0x63a5, 0x6442, 0x6298,
    0x8a2d, 0x7a83, 0x7bc0, 0x8aac, 0x96ea, 0x7d76, 0x820c, 0x8749,
    0x4ed9, 0x5148, 0x5343, 0x5360, 0x5ba3, 0x5c02, 0x5c16, 0x5ddd,
    0x6226, 0x6247, 0x64b0, 0x6813, 0x6834, 0x6cc9, 0x6d45, 0x6d17,
    0x67d3, 0x6f5c, 0x714e, 0x717d, 0x65cb, 0x7a7f, 0x7bad, 0x7dda,
    0x7e4a, 0x7fa8, 0x817a, 0x821b, 0x8239, 0x85a6, 0x8a6e, 0x8cce,
    0x8df5, 0x9078, 0x9077, 0x92ad, 0x9291, 0x9583, 0x9bae, 0x524d,
    0x5584, 0x6f38, 0x7136, 0x5168, 0x7985, 0x7e55, 0x81b3, 0x7cce,
    0x564c, 0x5851, 0x5ca8, 0x63aa, 0x66fe, 0x66fd, 0x695a, 0x72d9,
    0x758f, 0x758e, 0x790e, 0x7956, 0x79df, 0x7c97, 0x7d20, 0x7d44,
    0x8607, 0x8a34, 0x963b, 0x9061, 0x9f20, 0x50e7, 0x5275, 0x53cc,
    0x53e2, 0x5009, 0x55aa, 0x58ee, 0x594f, 0x723d, 0x5b8b, 0x5c64,
    0x531d, 0x60e3, 0x60f3, 0x635c, 0x6383, 0x633f, 0x63bb, 0x64cd,
    0x65e9, 0x66f9, 0x5de3, 0x69cd, 0x69fd, 0x6f15, 0x71e5, 0x4e89,
    0x75e9, 0x76f8, 0x7a93, 0x7cdf, 0x7dcf, 0x7d9c, 0x8061, 0x8349,
    0x8358, 0x846c, 0x84bc, 0x85fb, 0x88c5, 0x8d70, 0x9001, 0x906d,
    0x9397, 0x971c, 0x9a12, 0x50cf, 0x5897, 0x618e, 0x81d3, 0x8535,
    0x8d08, 0x9020, 0x4fc3, 0x5074, 0x5247, 0x5373, 0x606f, 0x6349,
    0x675f, 0x6e2c, 0x8db3, 0x901f, 0x4fd7, 0x5c5e, 0x8cca, 0x65cf,
    0x7d9a, 0x5352, 0x8896, 0x5176, 0x63c3, 0x5b58, 0x5b6b, 0x5c0a,
    0x640d, 0x6751, 0x905c, 0x4ed6, 0x591a, 0x592a, 0x6c70, 0x8a51,
    0x553e, 0x5815, 0x59a5, 0x60f0, 0x6253, 0x67c1, 0x8235, 0x6955,
    0x9640, 0x99c4, 0x9a28, 0x4f53, 0x5806, 0x5bfe, 0x8010, 0x5cb1,
    0x5e2f, 0x5f85, 0x6020, 0x614b, 0x6234, 0x66ff, 0x6cf0, 0x6ede,
    0x80ce, 0x817f, 0x82d4, 0x888b, 0x8cb8, 0x9000, 0x902e, 0x968a,
    0x9edb, 0x9bdb, 0x4ee3, 0x53f0, 0x5927, 0x7b2c, 0x918d, 0x984c,
    0x9df9, 0x6edd, 0x7027, 0x5353, 0x5544, 0x5b85, 0x6258, 0x629e,
    0x62d3, 0x6ca2, 0x6fef, 0x7422, 0x8a17, 0x9438, 0x6fc1, 0x8afe,
    0x8338, 0x51e7, 0x86f8, 0x53ea, 0x53e9, 0x4f46, 0x9054, 0x8fb0,
    0x596a, 0x8131, 0x5dfd, 0x7aea, 0x8fbf, 0x68da, 0x8c37, 0x72f8,
    0x9c48, 0x6a3d, 0x8ab0, 0x4e39, 0x5358, 0x5606, 0x5766, 0x62c5,
    0x63a2, 0x65e6, 0x6b4e, 0x6de1, 0x6e5b, 0x70ad, 0x77ed, 0x7aef,
    0x7baa, 0x7dbb, 0x803d, 0x80c6, 0x86cb, 0x8a95, 0x935b, 0x56e3,
    0x58c7, 0x5f3e, 0x65ad, 0x6696, 0x6a80, 0x6bb5, 0x7537, 0x8ac7,
    0x5024, 0x77e5, 0x5730, 0x5f1b, 0x6065, 0x667a, 0x6c60, 0x75f4,
    0x7a1a, 0x7f6e, 0x81f4, 0x8718, 0x9045, 0x99b3, 0x7bc9, 0x755c,
    0x7af9, 0x7b51, 0x84c4, 0x9010, 0x79e9, 0x7a92, 0x8336, 0x5ae1,
    0x7740, 0x4e2d, 0x4ef2, 0x5b99, 0x5fe0, 0x62bd, 0x663c, 0x67f1,
    0x6ce8, 0x866b, 0x8877, 0x8a3b, 0x914e, 0x92f3, 0x99d0, 0x6a17,
    0x7026, 0x732a, 0x82e7, 0x8457, 0x8caf, 0x4e01, 0x5146, 0x51cb,
    0x558b, 0x5bf5, 0x5e16, 0x5e33, 0x5e81, 0x5f14, 0x5f35, 0x5f6b,
    0x5fb4, 0x61f2, 0x6311, 0x66a2, 0x671d, 0x6f6e, 0x7252, 0x753a,
    0x773a, 0x8074, 0x8139, 0x8178, 0x8776, 0x8abf, 0x8adc, 0x8d85,
    0x8df3, 0x929a, 0x9577, 0x9802, 0x9ce5, 0x52c5, 0x6357, 0x76f4,
    0x6715, 0x6c88, 0x73cd, 0x8cc3, 0x93ae, 0x9673, 0x6d25, 0x589c,
    0x690e, 0x69cc, 0x8ffd, 0x939a, 0x75db, 0x901a, 0x585a, 0x6802,
    0x63b4, 0x69fb, 0x4f43, 0x6f2c, 0x67d8, 0x8fbb, 0x8526, 0x7db4,
    0x9354, 0x693f, 0x6f70, 0x576a, 0x58f7, 0x5b2c, 0x7d2c, 0x722a,
    0x540a, 0x91e3, 0x9db4, 0x4ead, 0x4f4e, 0x505c, 0x5075, 0x5243,
    0x8c9e, 0x5448, 0x5824, 0x5b9a, 0x5e1d, 0x5e95, 0x5ead, 0x5ef7,
    0x5f1f, 0x608c, 0x62b5, 0x633a, 0x63d0, 0x68af, 0x6c40, 0x7887,
    0x798e, 0x7a0b, 0x7de0, 0x8247, 0x8a02, 0x8ae6, 0x8e44, 0x9013,
    0x90b8, 0x912d, 0x91d8, 0x9f0e, 0x6ce5, 0x6458, 0x64e2, 0x6575,
    0x6ef4, 0x7684, 0x7b1b, 0x9069, 0x93d1, 0x6eba, 0x54f2, 0x5fb9,