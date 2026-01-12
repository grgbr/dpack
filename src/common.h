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
	dpack_assert_intern((_ops)->left); \
	dpack_assert_intern((_ops)->used); \
	dpack_assert_intern((_ops)->write); \
	dpack_assert_intern((_ops)->fini); \

#define dpack_encoder_assert_intern(_encoder) \
	dpack_assert_intern(_encoder); \
	dpack_encoder_assert_ops_intern((_encoder)->ops)

static inline __dpack_nonull(1, 2) __warn_result
int
dpack_encoder_write(struct dpack_encoder * __restrict encoder,
                    const uint8_t * __restrict        data,
                    size_t                            size)
{
	dpack_encoder_assert_intern(encoder);
	dpack_assert_intern(data);
	dpack_assert_intern(size);

	return encoder->ops->write(encoder, data, size);
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
	dpack_assert_intern((_ops)->skip); \
	dpack_assert_intern((_ops)->fini)

#define dpack_decoder_assert_intern(_decoder) \
	dpack_assert_intern(_decoder); \
	dpack_decoder_assert_ops_intern((_decoder)->ops)

static inline __dpack_nonull(1, 2) __warn_result
int
dpack_decoder_read(struct dpack_decoder * __restrict decoder,
                   uint8_t * __restrict              data,
                   size_t                            size)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(data);
	dpack_assert_intern(size);

	return decoder->ops->read(decoder, data, size);
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

static inline __dpack_nothrow
void
dpack_fixcnt(uint8_t tag, unsigned int mask, unsigned int * __restrict count)
{
	dpack_assert_intern(tag);
	dpack_assert_intern(mask);
	dpack_assert_intern(count);

	*count = (unsigned int)tag & mask;
}

extern int
dpack_read_cnt16(struct dpack_decoder * __restrict decoder,
                 unsigned int * __restrict         count)
	__dpack_nonull(1, 2) __warn_result __export_intern;

extern int
dpack_read_cnt32(struct dpack_decoder * __restrict decoder,
                 unsigned int * __restrict         count)
	__dpack_nonull(1, 2) __warn_result __export_intern;

extern int
dpack_maybe_skip(struct dpack_decoder * __restrict decoder,
                 size_t                            size,
                 size_t                            max_sz)
	__dpack_nonull(1) __warn_result;

extern int
dpack_maybe_discard_items(struct dpack_decoder * __restrict decoder,
                          unsigned int                      count,
                          unsigned int                      max_cnt)
	__dpack_nonull(1) __warn_result __export_intern;

extern int
dpack_maybe_discard(struct dpack_decoder * __restrict decoder,
                    uint8_t                           tag)
	__dpack_nonull(1) __warn_result __export_intern;

#endif /* _DPACK_COMMON_H */
