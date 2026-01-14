/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/codec.h"
#include "common.h"
#include <string.h>

/******************************************************************************
 * Buffer based encoder / packer
 ******************************************************************************/

#define dpack_encoder_assert_buffer_api(_encoder) \
	dpack_assert_api(_encoder); \
	dpack_encoder_assert_api(&(_encoder)->base); \
	dpack_assert_api((_encoder)->capa); \
	dpack_assert_api((_encoder)->tail <= (_encoder)->capa); \
	dpack_assert_api((_encoder)->buff)

static __dpack_nonull(1) __dpack_pure __dpack_nothrow __warn_result
size_t
dpack_encoder_buffer_left(const struct dpack_encoder * __restrict encoder)
{
	dpack_encoder_assert_buffer_api((const struct dpack_encoder_buffer *)
	                                encoder);

	const struct dpack_encoder_buffer * enc =
		(const struct dpack_encoder_buffer *)encoder;

	return enc->capa - enc->tail;
}

static __dpack_nonull(1) __dpack_pure __dpack_nothrow __warn_result
size_t
dpack_encoder_buffer_used(const struct dpack_encoder * __restrict encoder)
{
	dpack_encoder_assert_buffer_api((const struct dpack_encoder_buffer *)
	                                encoder);

	const struct dpack_encoder_buffer * enc =
		(const struct dpack_encoder_buffer *)encoder;

	return enc->tail;
}

static __dpack_nonull(1, 2) __dpack_nothrow __warn_result
int
dpack_encoder_buffer_write(struct dpack_encoder * __restrict encoder,
                           const uint8_t * __restrict        data,
                           size_t                            size)
{
	dpack_encoder_assert_buffer_api((const struct dpack_encoder_buffer *)
	                                encoder);
	dpack_assert_api(data);
	dpack_assert_api(size);

	struct dpack_encoder_buffer * enc = (struct dpack_encoder_buffer *)
	                                    encoder;
	size_t                        tail;

	if (!__builtin_add_overflow(enc->tail, size, &tail) &&
	    (tail <= enc->capa)) {
		memcpy(&enc->buff[enc->tail], data, size);
		enc->tail = tail;
		return 0;
	}

	return -EMSGSIZE;
}

static __dpack_nonull(1) __dpack_pure __dpack_nothrow __warn_result
int
dpack_encoder_buffer_fini(struct dpack_encoder * __restrict encoder __unused)
{
	dpack_encoder_assert_buffer_api((const struct dpack_encoder_buffer *)
	                                encoder);

	return 0;
}

const struct dpack_encoder_ops dpack_encoder_buffer_ops = {
	.left  = dpack_encoder_buffer_left,
	.used  = dpack_encoder_buffer_used,
	.write = dpack_encoder_buffer_write,
	.fini  = dpack_encoder_buffer_fini
};

void
dpack_encoder_init_buffer(struct dpack_encoder_buffer * __restrict encoder,
                          uint8_t * __restrict                     buffer,
                          size_t                                   size)
{
	dpack_assert_api(encoder);
	dpack_assert_api(buffer);
	dpack_assert_api(size);

	dpack_encoder_init(&encoder->base, &dpack_encoder_buffer_ops);
	encoder->tail = 0;
	encoder->capa = size;
	encoder->buff = buffer;
}

/******************************************************************************
 * Buffer based decoder / unpacker
 ******************************************************************************/

#define dpack_decoder_assert_buffer_api(_decoder) \
	dpack_assert_api(_decoder); \
	dpack_decoder_assert_api(&(_decoder)->base); \
	dpack_assert_api((_decoder)->capa); \
	dpack_assert_api((_decoder)->head <= (_decoder)->capa); \
	dpack_assert_api((_decoder)->buff)

static __dpack_nonull(1) __dpack_pure __dpack_nothrow __warn_result
size_t
dpack_decoder_buffer_left(const struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_buffer_api((const struct dpack_decoder_buffer *)
	                                decoder);

	const struct dpack_decoder_buffer * dec =
		(const struct dpack_decoder_buffer *)decoder;

	return dec->capa - dec->head;
}

static __dpack_nonull(1, 2) __dpack_nothrow __warn_result
int
dpack_decoder_buffer_read(struct dpack_decoder * __restrict decoder,
                          uint8_t * __restrict              data,
                          size_t                            size)
{
	dpack_decoder_assert_buffer_api((const struct dpack_decoder_buffer *)
	                                decoder);
	dpack_assert_intern(data);
	dpack_assert_intern(size);

	struct dpack_decoder_buffer * dec = (struct dpack_decoder_buffer *)
	                                    decoder;
	size_t                        head;

	if (!__builtin_add_overflow(dec->head, size, &head) &&
	    (head <= dec->capa)) {
		memcpy(data, &dec->buff[dec->head], size);
		dec->head = head;
		return 0;
	}

	return -ENODATA;
}

static __dpack_nonull(1) __dpack_nothrow __warn_result
int
dpack_decoder_buffer_skip(struct dpack_decoder * __restrict decoder,
                          size_t                            size)
{
	dpack_decoder_assert_buffer_api((const struct dpack_decoder_buffer *)
	                                decoder);
	dpack_assert_intern(size);

	struct dpack_decoder_buffer * dec = (struct dpack_decoder_buffer *)
	                                    decoder;
	size_t                        head;

	if (!__builtin_add_overflow(dec->head, size, &head) &&
	    (head <= dec->capa)) {
		dec->head = head;
		return 0;
	}

	return -ENODATA;
}

static __dpack_nonull(1) __dpack_nothrow __dpack_const __warn_result
int
dpack_decoder_buffer_fini(struct dpack_decoder * __restrict decoder __unused)
{
	dpack_decoder_assert_buffer_api((const struct dpack_decoder_buffer *)
	                                decoder);

	return 0;
}

const struct dpack_decoder_ops dpack_decoder_buffer_ops = {
	.left = dpack_decoder_buffer_left,
	.read = dpack_decoder_buffer_read,
	.skip = dpack_decoder_buffer_skip,
	.fini = dpack_decoder_buffer_fini
};

void
_dpack_decoder_init_buffer(struct dpack_decoder_buffer * __restrict decoder,
                           const uint8_t * __restrict               buffer,
                           size_t                                   size,
                           bool                                     discard)
{
	dpack_assert_api(decoder);
	dpack_assert_api(buffer);
	dpack_assert_api(size);

	dpack_decoder_init(&decoder->base,
	                   &dpack_decoder_buffer_ops,
	                   discard);
	decoder->head = 0;
	decoder->capa = size;
	decoder->buff = buffer;
}
