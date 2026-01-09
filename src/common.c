/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "common.h"
#include <endian.h>

int
dpack_read_cnt8(struct dpack_decoder * __restrict decoder,
                size_t * __restrict               count)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(count);

	uint8_t cnt;
	int     err;

	err = dpack_decoder_read(decoder, &cnt, sizeof(cnt));
	if (!err) {
		*count = (size_t)cnt;
		return 0;
	}

	return err;
}

int
dpack_read_cnt16(struct dpack_decoder * __restrict decoder,
                 size_t * __restrict               count)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(count);

	uint16_t cnt;
	int      err;

	err = dpack_decoder_read(decoder, (uint8_t *)&cnt, sizeof(cnt));
	if (!err) {
		*count = (size_t)be16toh(cnt);
		return 0;
	}

	return err;
}

int
dpack_read_cnt32(struct dpack_decoder * __restrict decoder,
                 size_t * __restrict               count)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(count);

	uint32_t cnt;
	int      err;

	err = dpack_decoder_read(decoder, (uint8_t *)&cnt, sizeof(cnt));
	if (!err) {
		*count = (size_t)be32toh(cnt);
		return 0;
	}

	return err;
}

static __dpack_nonull(1) __warn_result
int
dpack_discard_fixcnt(struct dpack_decoder * __restrict decoder,
                     uint8_t                           tag,
                     unsigned int                      mask)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(tag);
	dpack_assert_intern(mask);

	return dpack_decoder_discard(decoder,
	                             (size_t)((unsigned int)tag & mask));
}

static __dpack_nonull(1) __warn_result
int
dpack_discard_cnt8(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_intern(decoder);

	size_t cnt;
	int    err;

	err = dpack_read_cnt8(decoder, &cnt);
	if (!err)
		return dpack_decoder_discard(decoder, cnt);

	return err;
}

static __dpack_nonull(1) __warn_result
int
dpack_discard_cnt16(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_intern(decoder);

	size_t cnt;
	int    err;

	err = dpack_read_cnt16(decoder, &cnt);
	if (!err)
		return dpack_decoder_discard(decoder, cnt);

	return err;
}

static __dpack_nonull(1) __warn_result
int
dpack_discard_cnt32(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_intern(decoder);

	size_t cnt;
	int    err;

	err = dpack_read_cnt32(decoder, &cnt);
	if (!err)
		return dpack_decoder_discard(decoder, cnt);

	return err;
}

static __dpack_nonull(1) __warn_result
int
dpack_discard_items(struct dpack_decoder * __restrict decoder,
                    unsigned int                      nr)
{
	dpack_decoder_assert_intern(decoder);

	while (nr--) {
		int err;

		err = dpack_decoder_discard_item(decoder);
		if (err)
			return err;
	}

	return 0;
}

#if defined(CONFIG_DPACK_ARRAY)

#include "dpack/array.h"

static __dpack_nonull(1) __warn_result
int
dpack_discard_fixarray(struct dpack_decoder * __restrict decoder,
                       uint8_t                           tag __unused)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(((unsigned int)tag & ~_DPACK_FIXARRAY_ELMNR_MAX) ==
	                    _DPACK_FIXARRAY_TAG);

	unsigned int nr = (unsigned int)(tag & _DPACK_FIXARRAY_ELMNR_MAX);

	return dpack_discard_items(decoder, nr);
}

#else  /* !defined(CONFIG_DPACK_ARRAY) */

static __dpack_nonull(1) __warn_result
int
dpack_discard_fixarray(struct dpack_decoder * __restrict decoder __unused,
                       uint8_t                           tag)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(((unsigned int)tag & ~_DPACK_FIXARRAY_ELMNR_MAX) ==
	                    _DPACK_FIXARRAY_TAG);

	return -ENOTSUP;
}

#endif /* defined(CONFIG_DPACK_ARRAY) */

#if defined(CONFIG_DPACK_ARRAY) && \
    (DPACK_ARRAY_ELMNR_MAX > _DPACK_FIXARRAY_ELMNR_MAX)

static __dpack_nonull(1) __warn_result
int
dpack_discard_array16(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_intern(decoder);

	size_t nr;
	int    err;

	err = dpack_read_cnt16(decoder, &nr);
	if (!err)
		return dpack_discard_items(decoder, (unsigned int)nr);

	return err;
}

#else  /* !(defined(CONFIG_DPACK_ARRAY) && \
            (DPACK_ARRAY_ELMNR_MAX > _DPACK_FIXARRAY_ELMNR_MAX)) */

static __dpack_nonull(1) __warn_result
int
dpack_discard_array16(struct dpack_decoder * __restrict decoder __unused)
{
	dpack_decoder_assert_intern(decoder);

	return -ENOTSUP;
}

#endif /* defined(CONFIG_DPACK_ARRAY) && \
          (DPACK_ARRAY_ELMNR_MAX > _DPACK_FIXARRAY_ELMNR_MAX) */

#if defined(CONFIG_DPACK_ARRAY) && \
    (DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY16_ELMNR_MAX)

static __dpack_nonull(1) __warn_result
int
dpack_discard_array32(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_intern(decoder);

	size_t nr;
	int    err;

	err = dpack_read_cnt32(decoder, &nr);
	if (!err)
		return dpack_discard_items(decoder, (unsigned int)nr);

	return err;
}

#else  /* !(defined(CONFIG_DPACK_ARRAY) && \
            (DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY16_ELMNR_MAX)) */

static __dpack_nonull(1) __warn_result
int
dpack_discard_array32(struct dpack_decoder * __restrict decoder __unused)
{
	dpack_decoder_assert_intern(decoder);

	return -ENOTSUP;
}

#endif /* defined(CONFIG_DPACK_ARRAY) && \
          (DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY16_ELMNR_MAX) */

#if defined(CONFIG_DPACK_MAP)

#include "dpack/map.h"

static __dpack_nonull(1) __warn_result
int
dpack_discard_fixmap(struct dpack_decoder * __restrict decoder,
                     uint8_t                           tag)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(((unsigned int)tag & ~_DPACK_FIXMAP_FLDNR_MAX) ==
	                    _DPACK_FIXMAP_TAG);

	unsigned int nr = (unsigned int)(tag & _DPACK_FIXMAP_FLDNR_MAX);

	return dpack_discard_items(decoder, 2 * nr);
}

#else  /* !defined(CONFIG_DPACK_MAP) */

static __dpack_nonull(1) __warn_result
int
dpack_discard_fixmap(struct dpack_decoder * __restrict decoder __unused,
                     uint8_t                           tag __unused)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(((unsigned int)tag & ~_DPACK_FIXMAP_FLDNR_MAX) ==
	                    _DPACK_FIXMAP_TAG);

	return -ENOTSUP;
}

#endif /* defined(CONFIG_DPACK_MAP) */

#if defined(CONFIG_DPACK_MAP) && \
    (DPACK_MAP_FLDNR_MAX > _DPACK_FIXMAP_FLDNR_MAX)

static __dpack_nonull(1) __warn_result
int
dpack_discard_map16(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_intern(decoder);

	size_t nr;
	int    err;

	err = dpack_read_cnt16(decoder, &nr);
	if (!err)
		return dpack_discard_items(decoder, 2 * (unsigned int)nr);

	return err;
}

#else  /* !(defined(CONFIG_DPACK_MAP) && \
            (DPACK_MAP_FLDNR_MAX > _DPACK_FIXMAP_FLDNR_MAX)) */

static __dpack_nonull(1) __warn_result
int
dpack_discard_map16(struct dpack_decoder * __restrict decoder __unused)
{
	dpack_decoder_assert_intern(decoder);

	return -ENOTSUP;
}

#endif /* defined(CONFIG_DPACK_MAP) && \
          (DPACK_MAP_FLDNR_MAX > _DPACK_FIXMAP_FLDNR_MAX) */

#if defined(CONFIG_DPACK_MAP) && \
    (DPACK_MAP_FLDNR_MAX > _DPACK_MAP16_FLDNR_MAX)

static __dpack_nonull(1) __warn_result
int
dpack_discard_map32(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_intern(decoder);

	size_t nr;
	int    err;

	err = dpack_read_cnt32(decoder, &nr);
	if (!err)
		return dpack_discard_items(decoder, 2 * (unsigned int)nr);

	return err;
}

#else  /* !(defined(CONFIG_DPACK_MAP) && \
            (DPACK_MAP_FLDNR_MAX > _DPACK_MAP16_FLDNR_MAX)) */

static __dpack_nonull(1) __warn_result
int
dpack_discard_map32(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_intern(decoder);

	return -ENOTSUP;
}

#endif /* defined(CONFIG_DPACK_MAP) && \
          (DPACK_MAP_FLDNR_MAX > _DPACK_MAP16_FLDNR_MAX) */

int
dpack_discard_body(struct dpack_decoder * __restrict decoder, uint8_t tag)
{
	dpack_decoder_assert_intern(decoder);

	switch (tag) {
#if defined(CONFIG_DPACK_SCALAR)
	case DPACK_FIXUINT_TAG:
	case DPACK_FIXINT_TAG:
	case DPACK_FALSE_TAG:
	case DPACK_TRUE_TAG:
	case DPACK_NIL_TAG:
		return 0;

	case DPACK_UINT8_TAG:
	case DPACK_INT8_TAG:
		return dpack_decoder_discard(decoder, sizeof(uint8_t));

	case DPACK_UINT16_TAG:
	case DPACK_INT16_TAG:
		return dpack_decoder_discard(decoder, sizeof(uint16_t));

	case DPACK_UINT32_TAG:
	case DPACK_INT32_TAG:
		return dpack_decoder_discard(decoder, sizeof(uint32_t));

	case DPACK_UINT64_TAG:
	case DPACK_INT64_TAG:
		return dpack_decoder_discard(decoder, sizeof(uint64_t));

#if defined(CONFIG_DPACK_FLOAT)
	case DPACK_FLOAT32_TAG:
		return dpack_decoder_discard(decoder, sizeof(float));
#endif /* defined(CONFIG_DPACK_FLOAT) */
#if defined(CONFIG_DPACK_DOUBLE)
	case DPACK_FLOAT64_TAG:
		return dpack_decoder_discard(decoder, sizeof(double));
#endif /* defined(CONFIG_DPACK_DOUBLE) */
#endif /* defined(CONFIG_DPACK_SCALAR) */

#if defined(CONFIG_DPACK_STRING)
	case DPACK_FIXSTR_TAG:
		return dpack_discard_fixcnt(decoder,
		                            tag,
		                            _DPACK_FIXSTR_LEN_MAX);
#if DPACK_STRLEN_MAX > _DPACK_FIXSTR_LEN_MAX
	case DPACK_STR8_TAG:
		return dpack_discard_cnt8(decoder);
#endif
#if DPACK_STRLEN_MAX > _DPACK_STR8_LEN_MAX
	case DPACK_STR16_TAG:
		return dpack_discard_cnt16(decoder);
#endif
#if DPACK_STRLEN_MAX > _DPACK_STR16_LEN_MAX
	case DPACK_STR32_TAG:
		return dpack_discard_cnt32(decoder);
#endif
#endif /* defined(CONFIG_DPACK_STRING) */

#if defined(CONFIG_DPACK_BIN)
	case DPACK_BIN8_TAG:
		return dpack_discard_cnt8(decoder);
#if DPACK_BINSZ_MAX > _DPACK_BIN8_SIZE_MAX
	case DPACK_BIN16_TAG:
		return dpack_discard_cnt16(decoder);
#endif
#if DPACK_BINSZ_MAX > _DPACK_BIN16_SIZE_MAX
	case DPACK_BIN32_TAG:
		return dpack_discard_cnt32(decoder);
#endif
#endif /* defined(CONFIG_DPACK_BIN) */

	case DPACK_FIXARRAY_TAG:
		return dpack_discard_fixarray(decoder, tag);
	case DPACK_ARRAY16_TAG:
		return dpack_discard_array16(decoder);
	case DPACK_ARRAY32_TAG:
		return dpack_discard_array32(decoder);

	case DPACK_FIXMAP_TAG:
		return dpack_discard_fixmap(decoder, tag);
	case DPACK_MAP16_TAG:
		return dpack_discard_map16(decoder);
	case DPACK_MAP32_TAG:
		return dpack_discard_map32(decoder);

	case DPACK_UNUSED_TAG:
		return 0;

	case DPACK_FIXEXT1_TAG:
	case DPACK_FIXEXT2_TAG:
	case DPACK_FIXEXT4_TAG:
	case DPACK_FIXEXT8_TAG:
	case DPACK_FIXEXT16_TAG:
	case DPACK_EXT8_TAG:
	case DPACK_EXT16_TAG:
	case DPACK_EXT32_TAG:
	default:
		break;
	}

	return -ENOTSUP;
}

int
dpack_decoder_discard_item(struct dpack_decoder * __restrict decoder)
{
	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err)
		return dpack_discard_body(decoder, tag);

	return err;
}
