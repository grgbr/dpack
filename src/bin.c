/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/bin.h"
#include "dpack/codec.h"
#include "common.h"
#include <stdlib.h>

size_t
dpack_bin_size(size_t size)
{
	dpack_assert_api(size);
	dpack_assert_api(size <= DPACK_BINSZ_MAX);

	switch (size) {
	case 1 ... _DPACK_BIN8_SIZE_MAX:
		return DPACK_BIN8_SIZE(size);
#if DPACK_BINSZ_MAX > _DPACK_BIN8_SIZE_MAX
	case (_DPACK_BIN8_SIZE_MAX + 1) ... _DPACK_BIN16_SIZE_MAX:
		return DPACK_BIN16_SIZE(size);
#endif
#if DPACK_BINSZ_MAX > _DPACK_BIN16_SIZE_MAX
	case (_DPACK_BIN16_SIZE_MAX + 1) ... DPACK_BINSZ_MAX:
		return DPACK_BIN32_SIZE(size);
#endif
	default:
		dpack_assert_api(0);
	}

	unreachable();
}

int
dpack_encode_bin(struct dpack_encoder *  __restrict encoder,
                 const uint8_t * __restrict         value,
                 size_t                             size)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(value);
	dpack_assert_api(size);
	dpack_assert_api(size <= DPACK_BINSZ_MAX);

	int err;

	switch (size) {
	case 1 ... _DPACK_BIN8_SIZE_MAX:
		err = dpack_write_tag(encoder, DPACK_BIN8_TAG);
		if (!err) {
			uint8_t val = (uint8_t)size;

			err = dpack_encoder_write(encoder, &val, sizeof(val));
		}
		break;
#if DPACK_BINSZ_MAX > _DPACK_BIN8_SIZE_MAX
	case (_DPACK_BIN8_SIZE_MAX + 1) ... _DPACK_BIN16_SIZE_MAX:
		err = dpack_write_tag(encoder, DPACK_BIN16_TAG);
		if (!err) {
			uint16_t val = htobe16((uint16_t)size);

			err = dpack_encoder_write(encoder,
			                          (const uint8_t *)&val,
			                          sizeof(val));
		}
		break;
#endif
#if DPACK_BINSZ_MAX > _DPACK_BIN16_SIZE_MAX
	case (_DPACK_BIN16_SIZE_MAX + 1) ... DPACK_BINSZ_MAX:
		err = dpack_write_tag(encoder, DPACK_BIN32_TAG);
		if (!err) {
			uint32_t val = htobe32((uint32_t)size);

			err = dpack_encoder_write(encoder,
			                          (const uint8_t *)&val,
			                          sizeof(val));
		}
		break;
#endif
	default:
		dpack_assert_api(0);
		err = -ERANGE;
	}

	return (!err)
	       ? dpack_encoder_write(encoder, (const uint8_t *)value, size)
	       : err;
}

static __dpack_nonull(1) __warn_result
ssize_t
dpack_load_bin_tag(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_intern(decoder);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err) {
		switch (tag) {
		case DPACK_BIN8_TAG:
			{
				uint8_t sz;

				err = dpack_decoder_read(decoder,
				                         &sz,
				                         sizeof(sz));
				if (!err)
					return (ssize_t)sz;
				break;
			}
#if DPACK_BINSZ_MAX > _DPACK_BIN8_SIZE_MAX
		case DPACK_BIN16_TAG:
			{
				uint16_t sz;

				err = dpack_decoder_read(decoder,
				                         (uint8_t *)&sz,
				                         sizeof(sz));
				if (!err)
					return (ssize_t)(be16toh(sz));
				break;
			}
#endif
#if DPACK_BINSZ_MAX > _DPACK_BIN16_SIZE_MAX
		case DPACK_BIN32_TAG:
			{
				uint32_t sz;

				err = dpack_decoder_read(decoder,
				                         (uint8_t *)&sz,
				                         sizeof(sz));
				if (!err)
					return (ssize_t)(be32toh(sz));
				break;
			}
#endif
		default:
			err = dpack_maybe_discard(decoder, tag);
			if (!err)
				err = -ENOMSG;
		}
	}

	return err;
}

static __dpack_nonull(1) __warn_result
ssize_t
dpack_decode_bin_tag(struct dpack_decoder * __restrict decoder,
                     size_t                            min_sz,
                     size_t                            max_sz)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(min_sz);
	dpack_assert_intern(min_sz <= max_sz);
	dpack_assert_intern(max_sz <= DPACK_BINSZ_MAX);

	ssize_t sz;

	sz = dpack_load_bin_tag(decoder);
	if (sz > 0) {
		if (((size_t)sz >= min_sz) && ((size_t)sz <= max_sz))
			return sz;

		sz = (ssize_t)dpack_maybe_skip(decoder,
		                               (size_t)sz,
		                               DPACK_BINSZ_MAX);
		if (!sz)
			sz = -EMSGSIZE;
	}
	else if (!sz)
		sz = -EBADMSG;

	return sz;
}

static __dpack_nonull(1, 2) __warn_result
ssize_t
dpack_xtract_bindup(struct dpack_decoder * __restrict decoder,
                    uint8_t ** __restrict             value,
                    size_t                            size)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(value);
	dpack_assert_intern(size);
	dpack_assert_intern(size <= DPACK_BINSZ_MAX);

	uint8_t * bin;
	int       err;

	bin = malloc(size);
	if (!bin)
		return -errno;

	err = dpack_decoder_read(decoder, (uint8_t *)bin, size);
	if (!err) {
		*value = bin;
		return (ssize_t)size;
	}

	return err;
}

ssize_t
dpack_decode_bindup(struct dpack_decoder * __restrict decoder,
                    uint8_t ** __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	ssize_t sz;

	sz = dpack_decode_bin_tag(decoder, 1, DPACK_BINSZ_MAX);
	dpack_assert_intern(sz);

	return (sz > 0) ? dpack_xtract_bindup(decoder, value, (size_t)sz)
	                : sz;
}

static __dpack_nonull(1) __warn_result
int
dpack_xtract_bin_equ(struct dpack_decoder * __restrict decoder, size_t size)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(size);
	dpack_assert_intern(size <= DPACK_BINSZ_MAX);

	ssize_t sz;

	sz = dpack_load_bin_tag(decoder);
	if (sz > 0) {
		if ((size_t)sz == size)
			return 0;

		sz = (ssize_t)dpack_maybe_skip(decoder,
		                               (size_t)sz,
		                               DPACK_BINSZ_MAX);
		if (!sz)
			sz = -EMSGSIZE;
	}
	else if (!sz)
		sz = -EBADMSG;

	return (int)sz;
}

ssize_t
dpack_decode_bindup_equ(struct dpack_decoder * __restrict decoder,
                        size_t                            size,
                        uint8_t ** __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(size);
	dpack_assert_api(size <= DPACK_BINSZ_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_xtract_bin_equ(decoder, size);

	return (!err) ? dpack_xtract_bindup(decoder, value, size) : err;
}

static __dpack_nonull(1) __warn_result
ssize_t
dpack_xtract_bin_max(struct dpack_decoder * __restrict decoder,
                     size_t                            max_sz)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(max_sz > 1);
	dpack_assert_intern(max_sz <= DPACK_BINSZ_MAX);

	ssize_t sz;

	sz = dpack_load_bin_tag(decoder);
	if (sz > 0) {
		if ((size_t)sz <= max_sz)
			return sz;

		sz = (ssize_t)dpack_maybe_skip(decoder,
		                               (size_t)sz,
		                               DPACK_BINSZ_MAX);
		if (!sz)
			sz = -EMSGSIZE;
	}
	else if (!sz)
		sz = -EBADMSG;

	return sz;
}

ssize_t
dpack_decode_bindup_max(struct dpack_decoder * __restrict decoder,
                        size_t                            max_sz,
                        uint8_t ** __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(max_sz > 1);
	dpack_assert_api(max_sz <= DPACK_BINSZ_MAX);
	dpack_assert_api(value);

	ssize_t sz;

	sz = dpack_xtract_bin_max(decoder, max_sz);
	dpack_assert_intern(sz);

	return (sz > 0) ? dpack_xtract_bindup(decoder, value, (uint32_t)sz)
	                : sz;
}

ssize_t
dpack_decode_bindup_range(struct dpack_decoder * __restrict decoder,
                          size_t                            min_sz,
                          size_t                            max_sz,
                          uint8_t ** __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(min_sz);
	dpack_assert_api(min_sz < max_sz);
	dpack_assert_api(max_sz <= DPACK_BINSZ_MAX);
	dpack_assert_api(value);

	ssize_t sz;

	sz = dpack_decode_bin_tag(decoder, min_sz, max_sz);
	dpack_assert_intern(sz);

	return (sz > 0) ? dpack_xtract_bindup(decoder, value, (size_t)sz)
	                : sz;
}

static __dpack_nonull(1, 2) __warn_result
ssize_t
dpack_xtract_bincpy(struct dpack_decoder * __restrict decoder,
                    uint8_t * __restrict              value,
                    size_t                            size)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(value);
	dpack_assert_intern(size);
	dpack_assert_intern(size <= DPACK_BINSZ_MAX);

	int err;

	err = dpack_decoder_read(decoder, value, size);

	return (!err) ? (ssize_t)size : err;
}

ssize_t
dpack_decode_bincpy(struct dpack_decoder * __restrict decoder,
                    size_t                            size,
                    uint8_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(size);
	dpack_assert_api(size <= DPACK_BINSZ_MAX);
	dpack_assert_api(value);

	ssize_t sz;

	sz = dpack_decode_bin_tag(decoder, 1, size);
	dpack_assert_intern(sz);

	return (sz > 0) ? dpack_xtract_bincpy(decoder, value, (size_t)sz)
	                : sz;
}

ssize_t
dpack_decode_bincpy_equ(struct dpack_decoder * __restrict decoder,
                        size_t                            size,
                        uint8_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(size);
	dpack_assert_api(size <= DPACK_BINSZ_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_xtract_bin_equ(decoder, size);

	return (!err) ? dpack_xtract_bincpy(decoder, value, size)
	              : err;
}

ssize_t
dpack_decode_bincpy_range(struct dpack_decoder * __restrict decoder,
                          size_t                            min_sz,
                          size_t                            max_sz,
                          uint8_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(min_sz);
	dpack_assert_api(min_sz < max_sz);
	dpack_assert_api(max_sz <= DPACK_BINSZ_MAX);
	dpack_assert_api(value);

	ssize_t sz;

	sz = dpack_decode_bin_tag(decoder, min_sz, max_sz);
	dpack_assert_intern(sz);

	return (sz > 0) ? dpack_xtract_bincpy(decoder, value, (size_t)sz)
	                : sz;
}
