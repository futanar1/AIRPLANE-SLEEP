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

#include "aribcaption/decoder.hpp"
#include "decoder/decoder_impl.hpp"

namespace aribcaption {

Decoder::Decoder(Context& context) : pimpl_(std::make_unique<internal::DecoderImpl>(context)) {}

Decoder::~Decoder() = default;

Decoder::Decoder(Decoder&&) noexcept = default;

Decoder& Decoder::operator=(Decoder&&) noexcept = default;

bool Decoder::Initialize(EncodingScheme encoding_scheme, CaptionType type, Profile profile, LanguageId language_id) {
    return pimpl_->Initialize(encoding_scheme, type, profile, language_id);
}

void Decoder::SetEncodingScheme(EncodingScheme encoding_scheme) {
    pimpl_->SetEncodingScheme(encoding_scheme);
}

void Decoder::