
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

#ifndef ARIBCAPTION_DECODER_H
#define ARIBCAPTION_DECODER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "aribcc_export.h"
#include "caption.h"
#include "context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Component tag IDs of ARIB caption profiles
 *
 * Defined in ARIB STD-B10, part 2, Annex J
 */
typedef enum aribcc_profile_t {
    /**
     * ARIB-Subtitle & teletext coding (A-Profile)
     */
    ARIBCC_PROFILE_A = 0x0008,

    /**
     * Subtitle coding for digital terrestrial broadcasting (C profile)
     * Used in 1seg broadcasting.
     */
    ARIBCC_PROFILE_C = 0x0012,
    ARIBCC_PROFILE_DEFAULT = ARIBCC_PROFILE_A
} aribcc_profile_t;

/**
 * Enums for language indication.
 *
 * ARIB caption supports multiple languages, with up to 8 languages per 1 ES (STD-B24, Chapter 4, Table 4-1)
 * But the operating rules have limited the maximum number of languages transmitted in 1 ES to be 2 languages
 * according to ARIB TR-B14, Fascicle 1 2/2, 4.2.1 (6) and 4.2.2.
 */
typedef enum aribcc_languageid_t {
    ARIBCC_LANGUAGEID_FIRST = 1,
    ARIBCC_LANGUAGEID_SECOND = 2,
    ARIBCC_LANGUAGEID_DEFAULT = ARIBCC_LANGUAGEID_FIRST,
    ARIBCC_LANGUAGEID_MAX = ARIBCC_LANGUAGEID_SECOND
} aribcc_languageid_t;

/**
 * Enums for encoding scheme indication.
 *
 * Indicate @ARIBCC_ENCODING_SCHEME_AUTO for automatic detecting.
 *
 * For handling Japan (Japanese) ISDB captions, indicate @ARIBCC_ENCODING_SCHEME_ARIB_STD_B24_JIS
 * which uses 8-char JIS encoding.
 *
 * For handling SBTVD / ISDB-Tb captions used in South America, indicate @ARIBCC_ENCODING_SCHEME_ABNT_NBR_15606_1_LATIN
 * which is modified from @ARIBCC_ENCODING_SCHEME_ARIB_STD_B24_JIS for encoding Latin characters.
 *
 * For handling Philippines ISDB-T captions which is modified to use UTF-8 encoding, indicate
 * @ARIBCC_ENCODING_SCHEME_ISDB_T_PHILIPPINES_UTF8 which is currently identical to
 * @ARIBCC_ENCODING_SCHEME_ARIB_STD_B24_UTF8.
 */
typedef enum aribcc_encoding_scheme_t {
    ARIBCC_ENCODING_SCHEME_AUTO = 0,
    ARIBCC_ENCODING_SCHEME_ARIB_STD_B24_JIS = 1,
    ARIBCC_ENCODING_SCHEME_ARIB_STD_B24_UTF8 = 2,
    ARIBCC_ENCODING_SCHEME_ABNT_NBR_15606_1_LATIN = 3,
    ARIBCC_ENCODING_SCHEME_ISDB_T_PHILIPPINES_UTF8 = ARIBCC_ENCODING_SCHEME_ARIB_STD_B24_UTF8  // alias
} aribcc_encoding_scheme_t;

/**
 * Enums for reporting decoding status
 *
 * See @aribcc_decoder_decode()
 */
typedef enum aribcc_decode_status_t {
    ARIBCC_DECODE_STATUS_ERROR = 0,
    ARIBCC_DECODE_STATUS_NO_CAPTION = 1,
    ARIBCC_DECODE_STATUS_GOT_CAPTION = 2
} aribcc_decode_status_t;

/**
 * ARIB STD-B24 caption decoder
 *
 * Opaque type
 */
typedef struct aribcc_decoder_t aribcc_decoder_t;

/**
 * A context is needed for allocating the Decoder.
 *
 * The context shouldn't be freed before any other object constructed from the context has been freed.
 */
ARIBCC_API aribcc_decoder_t* aribcc_decoder_alloc(aribcc_context_t* context);

/**
 * Free the decoder and all related resources
 */
ARIBCC_API void aribcc_decoder_free(aribcc_decoder_t* decoder);

/**
 * @aribcc_decoder_initialize() must be called before calling any other aribcc_decoder_xxx() functions.
 *
 * @param decoder         @aribcc_decoder_t
 * @param encoding_scheme Indicate encoding scheme, see @aribcc_encoding_scheme_t
 * @param type            Indicate caption type, see @aribcc_captiontype_t
 * @param profile         Indicate caption profile, see @aribcc_profile_t
 * @param language_id     Indicate caption language, see @aribcc_languageid_t
 * @return true on success
 */
ARIBCC_API bool aribcc_decoder_initialize(aribcc_decoder_t* decoder,
                                          aribcc_encoding_scheme_t encoding_scheme,
                                          aribcc_captiontype_t type,
                                          aribcc_profile_t profile,
                                          aribcc_languageid_t language_id);

/**
 * Indicate encoding scheme
 * @param decoder          @aribcc_decoder_t
 * @param encoding_scheme  @aribcc_encoding_scheme_t
 */
ARIBCC_API void aribcc_decoder_set_encoding_scheme(aribcc_decoder_t* decoder,
                                                   aribcc_encoding_scheme_t encoding_scheme);

/**
 * Indicate caption type
 * @param decoder  @aribcc_decoder_t
 * @param type     @aribcc_captiontype_t
 */