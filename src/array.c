/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/array.h"
#include "dpack/codec.h"
#include "common.h"

size_t
dpack_array_mixed_size(unsigned int elm_nr, size_t data_size)
{
	dpack_assert_api(elm_nr);
	dpack_assert_api(elm_nr <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_api(data_size);
	dpack_assert_api(data_size <= DPACK_ARRAY_DATA_SIZE_MAX);

	size_t head;

	switch (elm_nr) {
	case 1 ... _DPACK_FIXARRAY_ELMNR_MAX:
		head = DPACK_FIXARRAY_TAG_SIZE;
		break;
#if DPACK_ARRAY_ELMNR_MAX > _DPACK_FIXARRAY_ELMNR_MAX
	case (_DPACK_FIXARRAY_ELMNR_MAX + 1) ... _DPACK_ARRAY16_ELMNR_MAX:
		head = DPACK_ARRAY16_TAG_SIZE;
		break;
#endif
#if DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY16_ELMNR_MAX
	case (_DPACK_ARRAY16_ELMNR_MAX + 1) ... _DPACK_ARRAY32_ELMNR_MAX:
		head = DPACK_ARRAY32_TAG_SIZE;
		break;
#endif
	default:
		dpack_assert_api(0);
		unreachable();
	}

	return head + data_size;
}

/******************************************************************************
 * Array encoding
 ******************************************************************************/

int
dpack_array_begin_encode(struct dpack_encoder * __restrict encoder,
                         unsigned int                      nr)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(nr);
	dpack_assert_api(nr <= DPACK_ARRAY_ELMNR_MAX);

	int err;

	switch (nr) {
	case 1 ... _DPACK_FIXARRAY_ELMNR_MAX:
		err = dpack_write_tag(encoder,
		                      _DPACK_FIXARRAY_TAG | (uint8_t)nr);
		break;
#if DPACK_ARRAY_ELMNR_MAX > _DPACK_FIXARRAY_ELMNR_MAX
	case (_DPACK_FIXARRAY_ELMNR_MAX + 1) ... _DPACK_ARRAY16_ELMNR_MAX:
		err = dpack_write_tag(encoder, DPACK_ARRAY16_TAG);
		if (!err) {
			uint16_t val = htobe16((uint16_t)nr);

			err = dpack_encoder_write(encoder,
			                          (const uint8_t *)&val,
			                          sizeof(val));
		}
		break;
#endif
#if DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY16_ELMNR_MAX
	case (_DPACK_ARRAY16_ELMNR_MAX + 1) ... _DPACK_ARRAY32_ELMNR_MAX:
		err = dpack_write_tag(encoder, DPACK_ARRAY32_TAG);
		if (!err) {
			uint32_t val = htobe32((uint32_t)nr);

			err = dpack_encoder_write(encoder,
			                          (const uint8_t *)&val,
			                          sizeof(val));
		}
		break;
#endif
	default:
		dpack_assert_api(0);
	}

	return err;
}

/******************************************************************************
 * Basic array decoding
 ******************************************************************************/

static __dpack_nonull(1, 2) __warn_result
int
dpack_load_array_tag(struct dpack_decoder * __restrict decoder,
                     unsigned int * __restrict         nr)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(nr);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err) {
		switch (tag) {
		case DPACK_FIXARRAY_TAG:
			dpack_fixcnt(tag, _DPACK_FIXARRAY_ELMNR_MAX, nr);
			return 0;
#if DPACK_ARRAY_ELMNR_MAX > _DPACK_FIXARRAY_ELMNR_MAX
		case DPACK_ARRAY16_TAG:
			return dpack_read_cnt16(decoder, nr);
#endif
#if DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY16_ELMNR_MAX
		case DPACK_ARRAY32_TAG:
			return dpack_read_cnt32(decoder, nr);
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
int
dpack_array_maybe_discard_left(struct dpack_decoder * __restrict decoder,
                               unsigned int                      left)
{
	dpack_decoder_assert_intern(decoder);

	int err;

	if (left) {
		err = dpack_maybe_discard_items(decoder,
		                                left,
		                                DPACK_ARRAY_ELMNR_MAX);
		if (!err)
			err = -EMSGSIZE;
	}
	else
		err = -EBADMSG;

	return err;
}

int
dpack_array_decode_count(struct dpack_decoder * __restrict decoder,
                         unsigned int * __restrict         count)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(count);

	int err;

	err = dpack_load_array_tag(decoder, count);
	if (!err)
		return (*count) ? 0 : -EBADMSG;

	return err;
}

int
dpack_array_decode_count_equ(struct dpack_decoder * __restrict decoder,
                             unsigned int                      count)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(count);
	dpack_assert_api(count <= DPACK_ARRAY_ELMNR_MAX);

	unsigned cnt;
	int      err;

	err = dpack_load_array_tag(decoder, &cnt);
	if (!err) {
		if (cnt == count)
			return 0;
		err = dpack_array_maybe_discard_left(decoder, count);
	}

	return err;
}

int
dpack_array_decode_count_min(struct dpack_decoder * __restrict decoder,
                             unsigned int                      min_cnt,
                             unsigned int * __restrict         count)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(min_cnt);
	dpack_assert_api(min_cnt < DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_api(count);

	int err;

	err = dpack_load_array_tag(decoder, count);
	if (!err) {
		if ((*count >= min_cnt) && (*count <= DPACK_ARRAY_ELMNR_MAX))
			return 0;
		err = dpack_array_maybe_discard_left(decoder, *count);
	}

	return err;
}

int
dpack_array_decode_count_max(struct dpack_decoder * __restrict decoder,
                             unsigned int                      max_cnt,
                             unsigned int * __restrict         count)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(max_cnt);
	dpack_assert_api(max_cnt <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_api(count);

	int err;

	err = dpack_load_array_tag(decoder, count);
	if (!err) {
		if ((*count >= 1) && (*count <= max_cnt))
			return 0;
		err = dpack_array_maybe_discard_left(decoder, *count);
	}

	return err;
}

int
dpack_array_decode_count_range(struct dpack_decoder * __restrict decoder,
                               unsigned int                      min_cnt,
                               unsigned int                      max_cnt,
                               unsigned int * __restrict         count)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(min_cnt);
	dpack_assert_api(min_cnt < max_cnt);
	dpack_assert_api(max_cnt <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_api(count);

	int err;

	err = dpack_load_array_tag(decoder, count);
	if (!err) {
		if ((*count >= min_cnt) && (*count <= max_cnt))
			return 0;
		err = dpack_array_maybe_discard_left(decoder, *count);
	}

	return err;
}

int
dpack_array_decode_count_end(struct dpack_decoder * __restrict decoder,
                             unsigned int                      left)
{
	dpack_decoder_assert_api(decoder);

	return (!left) ? 0 : dpack_maybe_discard_items(decoder,
	                                               left,
	                                               DPACK_ARRAY_ELMNR_MAX);
}

/******************************************************************************
 * Array decoding helpers
 ******************************************************************************/

static __dpack_nonull(1, 2) __warn_result
int
dpack_array_decode_elems(struct dpack_decoder * __restrict decoder,
                         dpack_decode_item_fn *            decode,
                         void * __restrict                 data,
                         unsigned int                      nr)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(decode);
	dpack_assert_intern(nr);
	dpack_assert_intern(nr <= DPACK_ARRAY_ELMNR_MAX);

	unsigned int idx = 0;
	int          err;

	do {
		err = decode(decoder, idx, data);
		idx++;
		if (err)
			goto discard;
	} while (idx < nr);

	return 0;

discard:
	{
		if (nr) {
			int ret;

			ret = dpack_maybe_discard_items(decoder,
			                                nr,
			                                DPACK_ARRAY_ELMNR_MAX);
			if (ret)
				err = ret;
		}

		return err;
	}
}

static __dpack_nonull(1, 4) __warn_result
int
dpack_array_xtract_range(struct dpack_decoder * decoder,
                         unsigned int           min_nr,
                         unsigned int           max_nr,
                         dpack_decode_item_fn * decode,
                         void * __restrict      data)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(min_nr);
	dpack_assert_intern(min_nr < max_nr);
	dpack_assert_intern(max_nr <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_intern(decode);

	unsigned int nr;
	int          err;

	err = dpack_load_array_tag(decoder, &nr);
	if (!err) {
		if ((nr >= min_nr) && (nr <= max_nr))
			return dpack_array_decode_elems(decoder,
			                                decode,
			                                data,
			                                nr);
		err = dpack_array_maybe_discard_left(decoder, nr);
	}

	return err;
}

int
dpack_array_decode(struct dpack_decoder * __restrict decoder,
                   dpack_decode_item_fn *            decode,
                   void * __restrict                 data)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(decode);

	return dpack_array_xtract_range(decoder,
	                                1,
	                                DPACK_ARRAY_ELMNR_MAX,
	                                decode,
	                                data);
}

int
dpack_array_decode_equ(struct dpack_decoder * __restrict decoder,
                       unsigned int                      nr,
                       dpack_decode_item_fn *            decode,
                       void * __restrict                 data)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(nr);
	dpack_assert_api(nr <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_api(decode);

	unsigned int cnt;
	int          err;

	err = dpack_load_array_tag(decoder, &cnt);
	if (!err) {
		if (cnt == nr)
		       return dpack_array_decode_elems(decoder,
		                                       decode,
		                                       data,
		                                       nr);
		err = dpack_array_maybe_discard_left(decoder, cnt);
	}

	return err;
}

int
dpack_array_decode_min(struct dpack_decoder * __restrict decoder,
                     unsigned int                        min_nr,
                     dpack_decode_item_fn *              decode,
                     void * __restrict                   data)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(min_nr);
	dpack_assert_api(min_nr < DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_api(decode);

	return dpack_array_xtract_range(decoder,
	                                min_nr,
	                                DPACK_ARRAY_ELMNR_MAX,
	                                decode,
	                                data);
}

int
dpack_array_decode_max(struct dpack_decoder * __restrict decoder,
                       unsigned int                      max_nr,
                       dpack_decode_item_fn *            decode,
                       void * __restrict                 data)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(max_nr);
	dpack_assert_api(max_nr <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_api(decode);

	return dpack_array_xtract_range(decoder, 1, max_nr, decode, data);
}

int
dpack_array_decode_range(struct dpack_decoder * __restrict decoder,
                         unsigned int                      min_nr,
                         unsigned int                      max_nr,
                         dpack_decode_item_fn *            decode,
                         void * __restrict                 data)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(min_nr);
	dpack_assert_api(min_nr < max_nr);
	dpack_assert_api(max_nr <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_api(decode);

	return dpack_array_xtract_range(decoder, min_nr, max_nr, decode, data);
}
