/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _DPACK_COMMON_H
#define _DPACK_COMMON_H

#include "dpack/codec.h"
#include <errno.h>

#if defined(CONFIG_DPACK_ASSERT_INTERN)

#define dpack_assert_intern(_cond) \
	stroll_assert("dpack", _cond)

#else  /* !defined(CONFIG_DPACK_ASSERT_INTERN) */

#define dpack_assert_intern(_cond)

#endif /* defined(CONFIG_DPACK_ASSERT_INTERN) */

#define DPACK_FIXUINT_TAG   0x00U ... 0x7fU
#define _DPACK_FIXMAP_TAG   0x80U
#define DPACK_FIXMAP_TAG    _DPACK_FIXMAP_TAG ... 0x8fU
#define _DPACK_FIXARRAY_TAG 0x90U
#define DPACK_FIXARRAY_TAG  _DPACK_FIXARRAY_TAG ... 0x9fU
#define _DPACK_FIXSTR_TAG   0xa0U
#define DPACK_FIXSTR_TAG    _DPACK_FIXSTR_TAG ... 0xbfU
#define DPACK_NIL_TAG       0xc0U
#define DPACK_UNUSED_TAG    0xc1U
#define DPACK_FALSE_TAG     0xc2U
#define DPACK_TRUE_TAG      0xc3U
#define DPACK_BIN8_TAG      0xc4U
#define DPACK_BIN16_TAG     0xc5U
#define DPACK_BIN32_TAG     0xc6U
#define DPACK_EXT8_TAG      0xc7U
#define DPACK_EXT16_TAG     0xc8U
#define DPACK_EXT32_TAG     0xc9U
#define DPACK_FLOAT32_TAG   0xcaU
#define DPACK_FLOAT64_TAG   0xcbU
#define DPACK_UINT8_TAG     0xccU
#define DPACK_UINT16_TAG    0xcdU
#define DPACK_UINT32_TAG    0xceU
#define DPACK_UINT64_TAG    0xcfU
#define DPACK_INT8_TAG      0xd0U
#define DPACK_INT16_TAG     0xd1U
#define DPACK_INT32_TAG     0xd2U
#define DPACK_INT64_TAG     0xd3U
#define DPACK_FIXEXT1_TAG   0xd4U
#define DPACK_FIXEXT2_TAG   0xd5U
#define DPACK_FIXEXT4_TAG   0xd6U
#define DPACK_FIXEXT8_TAG   0xd7U
#define DPACK_FIXEXT16_TAG  0xd8U
#define DPACK_STR8_TAG      0xd9U
#define DPACK_STR16_TAG     0xdaU
#define DPACK_STR32_TAG     0xdbU
#define DPACK_ARRAY16_TAG   0xdcU
#define DPACK_ARRAY32_TAG   0xddU
#define DPACK_MAP16_TAG     0xdeU
#define DPACK_MAP32_TAG     0xdfU
#define DPACK_FIXINT_TAG    0xe0U ... 0xffU

/******************************************************************************
 * Encoding / packing
 ******************************************************************************/

#define dpack_encoder_assert_ops_intern(_ops) \
	dpack_assert_intern(_ops); \
	dpack_assert_intern((_ops)->write)

#define dpack_encoder_assert_intern(_encoder) \
	dpack_assert_intern(_encoder); \
	dpack_encoder_assert_ops_intern((_encoder)->ops)

static inline __dpack_nonull(1, 2)
void
_dpack_encoder_write(struct dpack_encoder * __restrict encoder,
                     const uint8_t * __restrict        data,
                     size_t                            size)
{
	dpack_encoder_assert_intern(encoder);
	dpack_assert_intern(data);
	dpack_assert_intern(size);
	dpack_assert_intern(size <= dpack_encoder_space_left(encoder));

	encoder->ops->write(encoder, data, size);
}

static inline __dpack_nonull(1, 2) __warn_result
int
dpack_encoder_write(struct dpack_encoder * __restrict encoder,
                    const uint8_t * __restrict        data,
                    size_t                            size)
{
	dpack_encoder_assert_intern(encoder);
	dpack_assert_intern(data);
	dpack_assert_intern(size);

	if (size <= dpack_encoder_space_left(encoder)) {
		_dpack_encoder_write(encoder, data, size);
		return 0;
	}

	return -EMSGSIZE;
}

static inline __dpack_nonull(1) __warn_result
int
dpack_write_tag(struct dpack_encoder * __restrict encoder,
                uint8_t                           tag)
{
	dpack_encoder_assert_intern(encoder);

	return dpack_encoder_write(encoder, &tag, sizeof(tag));
}

/******************************************************************************
 * Decoding / unpacking
 ******************************************************************************/

#define dpack_decoder_assert_ops_intern(_ops) \
	dpack_assert_intern(_ops); \
	dpack_assert_intern((_ops)->left); \
	dpack_assert_intern((_ops)->read); \
	dpack_assert_intern((_ops)->disc); \
	dpack_assert_intern((_ops)->fini)

#define dpack_decoder_assert_intern(_decoder) \
	dpack_assert_intern(_decoder); \
	dpack_decoder_assert_ops_intern((_decoder)->ops)

static inline __dpack_nonull(1)
void
_dpack_decoder_discard(struct dpack_decoder * __restrict decoder,
                       size_t                            size)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(size);
	dpack_assert_api(size <= dpack_decoder_data_left(decoder));

	decoder->ops->disc(decoder, size);
}

static inline __dpack_nonull(1) __warn_result
int
dpack_decoder_discard(struct dpack_decoder * __restrict decoder,
                      size_t                            size)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(size);

	if (size <= dpack_decoder_data_left(decoder)) {
		_dpack_decoder_discard(decoder, size);
		return 0;
	}

	return -ENODATA;
}

static inline __dpack_nonull(1, 2)
void
_dpack_decoder_read(struct dpack_decoder * __restrict decoder,
                    uint8_t * __restrict              data,
                    size_t                            size)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(data);
	dpack_assert_intern(size);
	dpack_assert_intern(size <= dpack_decoder_data_left(decoder));

	decoder->ops->read(decoder, data, size);
}

static inline __dpack_nonull(1, 2) __warn_result
int
dpack_decoder_read(struct dpack_decoder * __restrict decoder,
                   uint8_t * __restrict              data,
                   size_t                            size)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(data);
	dpack_assert_intern(size);

	if (size <= dpack_decoder_data_left(decoder)) {
		_dpack_decoder_read(decoder, data, size);
		return 0;
	}

	return -ENODATA;
}

static inline __dpack_nonull(1, 2) __warn_result
int
dpack_read_tag(struct dpack_decoder * __restrict decoder,
               uint8_t * __restrict              tag)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(tag);

	return dpack_decoder_read(decoder, tag, sizeof(*tag));
}

#endif /* _DPACK_COMMON_H */
