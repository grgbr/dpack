/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
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
	dpack_assert_api(data_size <= _DPACK_ARRAY_DATA_SIZE_MAX);

	size_t head;

	switch (elm_nr) {
	case 1 ... _DPACK_FIXARRAY_ELMNR_MAX:
		head = MPACK_TAG_SIZE_FIXARRAY;
		break;

#if DPACK_ARRAY_ELMNR_MAX > _DPACK_FIXARRAY_ELMNR_MAX
	case (_DPACK_FIXARRAY_ELMNR_MAX + 1) ... _DPACK_ARRAY16_ELMNR_MAX:
		head = MPACK_TAG_SIZE_ARRAY16;
		break;
#endif

#if DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY16_ELMNR_MAX
	case (_DPACK_ARRAY16_ELMNR_MAX + 1) ... _DPACK_ARRAY32_ELMNR_MAX:
		head = MPACK_TAG_SIZE_ARRAY32;
		break;
#endif
	default:
		dpack_assert_api(0);
	}

	return head + data_size;
}

/******************************************************************************
 * Array encoding
 ******************************************************************************/

/*
 * Watch out !!
 *
 * This function is marked as __leaf for now. However, it calls
 * mpack_writer_flag_error() which in turn may call a function from the current
 * compilation unit thanks to the writer error_fn() function pointer of mpack.
 *
 * If modifying dpack_encoder_init_buffer() and / or registering an error
 * function (thanks to mpack_writer_set_error_handler()) is required for
 * internal DPack purposes, MAKE SURE you return to current compilation unit
 * only by return or by exception handling.
 *
 * See Stroll's __leaf documentation for more infos.
 */
int
dpack_array_begin_encode(struct dpack_encoder * encoder, unsigned int nr)
{
	dpack_assert_api_encoder(encoder);
	dpack_assert_api(nr);
	dpack_assert_api(nr <= DPACK_ARRAY_ELMNR_MAX);

	mpack_start_array(&encoder->mpack, nr);

	return dpack_encoder_error_state(&encoder->mpack);
}

/*
 * Watch out !!
 *
 * This function is marked as __leaf for now. However, when built with mpack's
 * write tracking support, it calls mpack_writer_flag_error() which in turn may
 * call a function from the current compilation unit thanks to the writer
 * error_fn() function pointer of mpack.
 *
 * If modifying dpack_encoder_init_buffer() and / or registering an error
 * function (thanks to mpack_writer_set_error_handler()) is required for
 * internal DPack purposes, MAKE SURE you return to current compilation unit
 * only by return or by exception handling.
 *
 * See Stroll's __leaf documentation for more infos.
 */
void
dpack_array_end_encode(struct dpack_encoder * encoder)
{
	dpack_assert_api_encoder(encoder);

	mpack_finish_array(&encoder->mpack);
}

/******************************************************************************
 * Array decoding
 ******************************************************************************/

static int
dpack_array_xtract_nr(struct mpack_reader_t * reader, unsigned int * nr)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);
	dpack_assert_intern(nr);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_array, &tag);
	if (err)
		return err;

	*nr = (unsigned int)mpack_tag_array_count(&tag);

	return 0;
}

static int
dpack_array_xtract_range(struct dpack_decoder * decoder,
                         unsigned int           min_nr,
                         unsigned int           max_nr,
                         unsigned int         * nr)
{
	dpack_assert_intern(decoder);
	dpack_assert_intern(min_nr);
	dpack_assert_intern(min_nr < max_nr);
	dpack_assert_intern(max_nr <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_intern(nr);

	int err;

	err = dpack_array_xtract_nr(&decoder->mpack, nr);
	if (err)
		return err;

	if ((*nr < min_nr) || (*nr > max_nr)) {
		dpack_decoder_intr(decoder, mpack_type_array, *nr);
		return -ENOMSG;
	}

	return 0;
}

static int
dpack_array_xtract_equ(struct dpack_decoder * decoder, unsigned int nr)
{
	dpack_assert_intern(decoder);
	dpack_assert_intern(nr);
	dpack_assert_intern(nr <= DPACK_ARRAY_ELMNR_MAX);

	unsigned int cnt;
	int          err;

	err = dpack_array_xtract_nr(&decoder->mpack, &cnt);
	if (err)
		return err;

	if (cnt != nr) {
		dpack_decoder_intr(decoder, mpack_type_array, cnt);
		return -ENOMSG;
	}

	return 0;
}

static int
dpack_array_decode_elems(struct dpack_decoder * decoder,
                         dpack_decode_item_fn * decode,
                         void                 * data,
                         unsigned int           nr)
{
	dpack_assert_intern(decoder);
	dpack_assert_intern(decode);
	dpack_assert_intern(nr);

	int          err;
	unsigned int idx = 0;

	do {
		err = decode(decoder, idx, data);
		idx++;
		if (err)
			goto intr;
	} while (idx < nr);

	/*
	 * Always close current array. This is safe and performs a no-op in case
	 * the array is in an error state.
	 * When mpack is built with debugging turned on, this allows it to track
	 * bytes read.
	 */
	mpack_done_array(&decoder->mpack);

	return 0;

intr:
	dpack_decoder_intr(decoder, mpack_type_array, nr - idx);

	return err;
}

int
dpack_array_decode(struct dpack_decoder * decoder,
                   dpack_decode_item_fn * decode,
                   void                 * data)
{
	dpack_assert_api(decoder);
	dpack_assert_api(decode);

	return dpack_array_decode_range(decoder,
	                                1,
	                                DPACK_ARRAY_ELMNR_MAX,
	                                decode,
	                                data);
}

int
dpack_array_decode_equ(struct dpack_decoder * decoder,
                       unsigned int           nr,
                       dpack_decode_item_fn * decode,
                       void                 * data)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(nr);
	dpack_assert_api(nr <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_api(decode);

	int err;

	err = dpack_array_xtract_equ(decoder, nr);
	if (err)
		return err;

	return dpack_array_decode_elems(decoder, decode, data, nr);
}

int
dpack_array_decode_min(struct dpack_decoder * decoder,
                     unsigned int           min_nr,
                     dpack_decode_item_fn * decode,
                     void                 * data)
{
	dpack_assert_api(decoder);
	dpack_assert_api(min_nr);
	dpack_assert_api(min_nr < DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_api(decode);

	return dpack_array_decode_range(decoder,
	                                min_nr,
	                                DPACK_ARRAY_ELMNR_MAX,
	                                decode,
	                                data);
}

int
dpack_array_decode_max(struct dpack_decoder * decoder,
                       unsigned int           max_nr,
                       dpack_decode_item_fn * decode,
                       void                 * data)
{
	dpack_assert_api(decoder);
	dpack_assert_api(max_nr);
	dpack_assert_api(max_nr <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_api(decode);

	return dpack_array_decode_range(decoder, 1, max_nr, decode, data);
}

int
dpack_array_decode_range(struct dpack_decoder * decoder,
                         unsigned int           min_nr,
                         unsigned int           max_nr,
                         dpack_decode_item_fn * decode,
                         void                 * data)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(min_nr);
	dpack_assert_api(min_nr < max_nr);
	dpack_assert_api(max_nr <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert_api(decode);

	unsigned int nr;
	int          err;

	err = dpack_array_xtract_range(decoder, min_nr, max_nr, &nr);
	if (err)
		return err;

	return dpack_array_decode_elems(decoder, decode, data, nr);
}
