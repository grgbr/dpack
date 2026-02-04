/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "common.h"
#if defined(CONFIG_DPACK_STRING)
#include "dpack/string.h"
#endif
#if defined(CONFIG_DPACK_BIN)
#include "dpack/bin.h"
#endif
#if defined(CONFIG_DPACK_ARRAY)
#include "dpack/array.h"
#endif
#include <endian.h>

static inline __dpack_nonull(1, 2) __warn_result
int
dpack_read_cnt8(struct dpack_decoder * __restrict decoder,
                unsigned int * __restrict         count)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(count);

	uint8_t cnt;
	int     err;

	err = dpack_decoder_read(decoder, &cnt, sizeof(cnt));
	if (!err) {
		*count = (unsigned int)cnt;
		return 0;
	}

	return err;
}

int
dpack_read_cnt16(struct dpack_decoder * __restrict decoder,
                 unsigned int * __restrict         count)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(count);

	uint16_t cnt;
	int      err;

	err = dpack_decoder_read(decoder, (uint8_t *)&cnt, sizeof(cnt));
	if (!err) {
		*count = (unsigned int)be16toh(cnt);
		return 0;
	}

	return err;
}

int
dpack_read_cnt32(struct dpack_decoder * __restrict decoder,
                 unsigned int * __restrict         count)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(count);

	uint32_t cnt;
	int      err;

	err = dpack_decoder_read(decoder, (uint8_t *)&cnt, sizeof(cnt));
	if (!err) {
		*count = (unsigned int)be32toh(cnt);
		return 0;
	}

	return err;
}

static __dpack_nonull(1) __warn_result
int
dpack_discard_fixcnt(struct dpack_decoder * __restrict decoder,
                     uint8_t                           tag,
                     unsigned int                      mask,
                     unsigned int                      max_cnt)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(tag);
	dpack_assert_intern(mask);
	dpack_assert_intern(max_cnt);

	size_t cnt = (size_t)((unsigned int)tag & mask);

	if (cnt <= max_cnt)
		return cnt ? dpack_decoder_skip(decoder, cnt) : 0;

	return -ENOTSUP;
}

#if (defined(CONFIG_DPACK_STRING) && \
     (DPACK_STRLEN_MAX > _DPACK_FIXSTR_LEN_MAX)) || \
    defined(CONFIG_DPACK_BIN)

static __dpack_nonull(1) __warn_result
int
dpack_discard_cnt8(struct dpack_decoder * __restrict decoder,
                   unsigned int                      max_cnt)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(max_cnt);

	unsigned int cnt;
	int          err;

	err = dpack_read_cnt8(decoder, &cnt);
	if (!err) {
		if (cnt <= max_cnt)
			return cnt ? dpack_decoder_skip(decoder, cnt) : 0;
		return -ENOTSUP;
	}

	return err;
}

#endif

#if (defined(CONFIG_DPACK_STRING) && \
     (DPACK_STRLEN_MAX > _DPACK_STR8_LEN_MAX)) || \
    (defined(CONFIG_DPACK_BIN) && (DPACK_BINSZ_MAX > _DPACK_BIN8_SIZE_MAX))

static __dpack_nonull(1) __warn_result
int
dpack_discard_cnt16(struct dpack_decoder * __restrict decoder,
                    unsigned int                      max_cnt)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(max_cnt);

	unsigned int cnt;
	int          err;

	err = dpack_read_cnt16(decoder, &cnt);
	if (!err) {
		if (cnt <= max_cnt)
			return cnt ? dpack_decoder_skip(decoder, cnt) : 0;
		return -ENOTSUP;
	}

	return err;
}

#endif

#if (defined(CONFIG_DPACK_STRING) && \
     (DPACK_STRLEN_MAX > _DPACK_STR16_LEN_MAX)) || \
    (defined(CONFIG_DPACK_BIN) && (DPACK_BINSZ_MAX > _DPACK_BIN16_SIZE_MAX))

static __dpack_nonull(1) __warn_result
int
dpack_discard_cnt32(struct dpack_decoder * __restrict decoder,
                    unsigned int                      max_cnt)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(max_cnt);

	unsigned int cnt;
	int          err;

	err = dpack_read_cnt32(decoder, &cnt);
	if (!err) {
		if (cnt <= max_cnt)
			return cnt ? dpack_decoder_skip(decoder, cnt) : 0;
		return -ENOTSUP;
	}

	return err;
}

#endif

static __dpack_nonull(1) __warn_result
int
dpack_discard_items(struct dpack_decoder * __restrict decoder,
                    unsigned int                      nr)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(nr);

	do {
		int err;

		err = dpack_decoder_discard(decoder);
		if (err)
			return err;
	} while (--nr);

	return 0;
}

#if defined(CONFIG_DPACK_ARRAY)

static __dpack_nonull(1) __warn_result
int
dpack_discard_fixarray(struct dpack_decoder * __restrict decoder,
                       uint8_t                           tag __unused)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(((unsigned int)tag & ~_DPACK_FIXARRAY_ELMNR_MAX) ==
	                    _DPACK_FIXARRAY_TAG);

	unsigned int nr = (unsigned int)(tag & _DPACK_FIXARRAY_ELMNR_MAX);

	if (nr) {
		if (nr <= DPACK_ARRAY_ELMNR_MAX)
			return dpack_discard_items(decoder, nr);
		return -ENOTSUP;
	}

	return 0;
}

#else  /* !defined(CONFIG_DPACK_ARRAY) */

static __dpack_nonull(1) __warn_result
int
dpack_discard_fixarray(struct dpack_decoder * __restrict decoder __unused,
                       uint8_t                           tag __unused)
{
	dpack_decoder_assert_intern(decoder);

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

	unsigned nr;
	int      err;

	err = dpack_read_cnt16(decoder, &nr);
	if (!err) {
		if (nr <= DPACK_ARRAY_ELMNR_MAX)
			return nr ? dpack_discard_items(decoder, nr) : 0;
		err = -ENOTSUP;
	}

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

	unsigned int nr;
	int          err;

	err = dpack_read_cnt32(decoder, &nr);
	if (!err) {
		if (nr <= DPACK_ARRAY_ELMNR_MAX)
			return nr ? dpack_discard_items(decoder, nr) : 0;
		err = -ENOTSUP;
	}

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

	if (nr) {
		if (nr <= DPACK_MAP_FLDNR_MAX)
			return dpack_discard_items(decoder, 2 * nr);
		return -ENOTSUP;
	}

	return 0;
}

#else  /* !defined(CONFIG_DPACK_MAP) */

static __dpack_nonull(1) __warn_result
int
dpack_discard_fixmap(struct dpack_decoder * __restrict decoder __unused,
                     uint8_t                           tag __unused)
{
	dpack_decoder_assert_intern(decoder);

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

	unsigned int nr;
	int          err;

	err = dpack_read_cnt16(decoder, &nr);
	if (!err) {
		if (nr <= DPACK_MAP_FLDNR_MAX)
			return nr ? dpack_discard_items(decoder, 2 * nr) : 0;
		err = -ENOTSUP;
	}

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

	unsigned int nr;
	int          err;

	err = dpack_read_cnt32(decoder, &nr);
	if (!err) {
		if (nr <= DPACK_MAP_FLDNR_MAX)
			return nr ? dpack_discard_items(decoder, 2 * nr) : 0;
		err = -ENOTSUP;
	}

	return err;
}

#else  /* !(defined(CONFIG_DPACK_MAP) && \
            (DPACK_MAP_FLDNR_MAX > _DPACK_MAP16_FLDNR_MAX)) */

static __dpack_nonull(1) __warn_result
int
dpack_discard_map32(struct dpack_decoder * __restrict decoder __unused)
{
	dpack_decoder_assert_intern(decoder);

	return -ENOTSUP;
}

#endif /* defined(CONFIG_DPACK_MAP) && \
          (DPACK_MAP_FLDNR_MAX > _DPACK_MAP16_FLDNR_MAX) */

static __dpack_nonull(1) __warn_result
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
		return dpack_decoder_skip(decoder, sizeof(uint8_t));

	case DPACK_UINT16_TAG:
	case DPACK_INT16_TAG:
		return dpack_decoder_skip(decoder, sizeof(uint16_t));

	case DPACK_UINT32_TAG:
	case DPACK_INT32_TAG:
		return dpack_decoder_skip(decoder, sizeof(uint32_t));

	case DPACK_UINT64_TAG:
	case DPACK_INT64_TAG:
		return dpack_decoder_skip(decoder, sizeof(uint64_t));

#if defined(CONFIG_DPACK_FLOAT)
	case DPACK_FLOAT32_TAG:
		return dpack_decoder_skip(decoder, sizeof(float));
#endif /* defined(CONFIG_DPACK_FLOAT) */
#if defined(CONFIG_DPACK_DOUBLE)
	case DPACK_FLOAT64_TAG:
		return dpack_decoder_skip(decoder, sizeof(double));
#endif /* defined(CONFIG_DPACK_DOUBLE) */
#endif /* defined(CONFIG_DPACK_SCALAR) */

#if defined(CONFIG_DPACK_STRING)
	case DPACK_FIXSTR_TAG:
		return dpack_discard_fixcnt(decoder,
		                            tag,
		                            _DPACK_FIXSTR_LEN_MAX,
		                            DPACK_STRLEN_MAX);
#if DPACK_STRLEN_MAX > _DPACK_FIXSTR_LEN_MAX
	case DPACK_STR8_TAG:
		return dpack_discard_cnt8(decoder, DPACK_STRLEN_MAX);
#endif
#if DPACK_STRLEN_MAX > _DPACK_STR8_LEN_MAX
	case DPACK_STR16_TAG:
		return dpack_discard_cnt16(decoder, DPACK_STRLEN_MAX);
#endif
#if DPACK_STRLEN_MAX > _DPACK_STR16_LEN_MAX
	case DPACK_STR32_TAG:
		return dpack_discard_cnt32(decoder, DPACK_STRLEN_MAX);
#endif
#endif /* defined(CONFIG_DPACK_STRING) */

#if defined(CONFIG_DPACK_BIN)
	case DPACK_BIN8_TAG:
		return dpack_discard_cnt8(decoder, DPACK_BINSZ_MAX);
#if DPACK_BINSZ_MAX > _DPACK_BIN8_SIZE_MAX
	case DPACK_BIN16_TAG:
		return dpack_discard_cnt16(decoder, DPACK_BINSZ_MAX);
#endif
#if DPACK_BINSZ_MAX > _DPACK_BIN16_SIZE_MAX
	case DPACK_BIN32_TAG:
		return dpack_discard_cnt32(decoder, DPACK_BINSZ_MAX);
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
dpack_maybe_skip(struct dpack_decoder * __restrict decoder,
                 size_t                            size,
                 size_t                            max_sz)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_api(size);
	dpack_assert_api(max_sz);

	if (size <= max_sz) {
		if (!decoder->disc)
			return 0;
		return decoder->ops->skip(decoder, size);
	}

	return -ENOTSUP;
}

int
dpack_maybe_discard_items(struct dpack_decoder * __restrict decoder,
                          unsigned int                      count,
                          unsigned int                      max_cnt)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(count);
	dpack_assert_intern(max_cnt);

	if (count <= max_cnt) {
		if (!decoder->disc)
			return 0;
		return dpack_discard_items(decoder, count);
	}

	return -ENOTSUP;
}

int
dpack_maybe_discard(struct dpack_decoder * __restrict decoder, uint8_t tag)
{
	dpack_decoder_assert_intern(decoder);

	return (!decoder->disc) ? 0 : dpack_discard_body(decoder, tag);
}

int
dpack_decoder_discard(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_api(decoder);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err)
		return dpack_discard_body(decoder, tag);

	return err;
}
