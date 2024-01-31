/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/map.h"
#include "common.h"

size_t
dpack_map_size(unsigned int fld_nr, size_t data_size)
{
	dpack_assert_api(fld_nr);
	dpack_assert_api(fld_nr <= DPACK_MAP_FLDNR_MAX);
	dpack_assert_api(data_size);
	dpack_assert_api(data_size <= DPACK_MAP_DATA_SIZE_MAX);

	size_t head;

	switch (fld_nr) {
	case 1 ... _DPACK_FIXMAP_FLDNR_MAX:
		head = MPACK_TAG_SIZE_FIXMAP;
		break;

#if DPACK_MAP_FLDNR_MAX > _DPACK_FIXMAP_FLDNR_MAX
	case (_DPACK_FIXMAP_FLDNR_MAX + 1) ... _DPACK_MAP16_FLDNR_MAX:
		head = MPACK_TAG_SIZE_MAP16;
		break;
#endif

#if DPACK_MAP_FLDNR_MAX > _DPACK_MAP16_FLDNR_MAX
	case (_DPACK_MAP16_FLDNR_MAX + 1) ... _DPACK_MAP32_FLDNR_MAX:
		head = MPACK_TAG_SIZE_MAP32;
		break;
#endif
	default:
		dpack_assert_api(0);
		unreachable();
	}

	return head + data_size;
}

/******************************************************************************
 * Map encoding
 ******************************************************************************/

#define DPACK_MAP_DEFINE_ENCODE_SCALAR(_name, _type, _func) \
	int \
	_name(struct dpack_encoder * encoder, \
	      unsigned int           id, \
	      _type                  value) \
	{ \
		int err; \
		\
		err = dpack_map_encode_fldid(encoder, id); \
		if (err) \
			return err; \
		\
		return _func(encoder, value); \
	}

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
dpack_map_begin_encode(struct dpack_encoder * encoder, unsigned int nr)
{
	dpack_assert_api_encoder(encoder);
	dpack_assert_api(nr);
	dpack_assert_api(nr <= DPACK_MAP_FLDNR_MAX);

	mpack_start_map(&encoder->mpack, nr);

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
dpack_map_end_encode(struct dpack_encoder * encoder)
{
	dpack_assert_api_encoder(encoder);

	mpack_finish_map(&encoder->mpack);
}

/******************************************************************************
 * Map boolean encoding
 ******************************************************************************/

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_bool,
                               bool,
                               dpack_encode_bool)

/******************************************************************************
 * Map 8 bits integers encoding
 ******************************************************************************/

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_uint8,
                               uint8_t,
                               dpack_encode_uint8)
DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_int8,
                               int8_t,
                               dpack_encode_int8)

/******************************************************************************
 * Map 16 bits integers encoding
 ******************************************************************************/

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_uint16,
                               uint16_t,
                               dpack_encode_uint16)
DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_int16,
                               int16_t,
                               dpack_encode_int16)

/******************************************************************************
 * Map 32 bits integers encoding
 ******************************************************************************/

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_uint32,
                               uint32_t,
                               dpack_encode_uint32)
DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_int32,
                               int32_t,
                               dpack_encode_int32)

/******************************************************************************
 * Map 64 bits integers encoding
 ******************************************************************************/

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_uint64,
                               uint64_t,
                               dpack_encode_uint64)
DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_int64,
                               int64_t,
                               dpack_encode_int64)

/******************************************************************************
 * Map single precision floats encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_FLOAT)

int
dpack_map_encode_float(struct dpack_encoder * encoder,
                       unsigned int           id,
                       float                  value)
{
	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_encode_float(encoder, value);
}

#endif /* defined(CONFIG_DPACK_FLOAT) */

/******************************************************************************
 * Map double precision floats encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_DOUBLE)

int
dpack_map_encode_double(struct dpack_encoder * encoder,
                        unsigned int           id,
                        double                 value)
{
	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_encode_double(encoder, value);
}

#endif /* defined(CONFIG_DPACK_DOUBLE) */

/******************************************************************************
 * Map strings encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_STRING)

int
dpack_map_encode_str(struct dpack_encoder  * encoder,
                     unsigned int            id,
                     const char *__restrict  value)
{
	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_encode_str(encoder, value);
}

int
dpack_map_encode_str_fix(struct dpack_encoder  * encoder,
                         unsigned int            id,
                         const char *__restrict  value,
                         size_t                  len)
{
	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_encode_str_fix(encoder, value, len);
}

#endif /* defined(CONFIG_DPACK_STRING) */

/******************************************************************************
 * Map lvstr encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_LVSTR)

int
dpack_map_encode_lvstr(struct dpack_encoder *                 encoder,
                       unsigned int                           id,
                       const struct stroll_lvstr * __restrict value)
{
	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_encode_lvstr(encoder, value);
}

#endif /* defined(CONFIG_DPACK_LVSTR) */

/******************************************************************************
 * Map bins encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_BIN)

int
dpack_map_encode_bin(struct dpack_encoder *  encoder,
                     unsigned int            id,
                     const char * __restrict value,
                     size_t                  size)
{
	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_encode_bin(encoder, value, size);
}

#endif /* defined(CONFIG_DPACK_BIN) */

/******************************************************************************
 * Map Nil / NULL encoding
 ******************************************************************************/

int
dpack_map_encode_nil(struct dpack_encoder * encoder,
                     unsigned int           id)
{
	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_encode_nil(encoder);
}

/******************************************************************************
 * Nested collections encoding
 ******************************************************************************/

int
dpack_map_begin_encode_nest_map(struct dpack_encoder * encoder,
                                unsigned int           id,
                                unsigned int           nr)
{
	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_map_begin_encode(encoder, nr);
}

#if defined(CONFIG_DPACK_ARRAY)

#include "dpack/array.h"

int
dpack_map_begin_encode_nest_array(struct dpack_encoder * encoder,
                                  unsigned int           id,
                                  unsigned int           nr)
{
	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_array_begin_encode(encoder, nr);
}

#endif /* defined(CONFIG_DPACK_ARRAY) */

/******************************************************************************
 * Map decoding
 ******************************************************************************/

static int
dpack_map_xtract_nr(struct mpack_reader_t * reader, unsigned int * nr)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);
	dpack_assert_intern(nr);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_map, &tag);
	if (err)
		return err;

	*nr = (unsigned int)mpack_tag_map_count(&tag);

	return 0;
}

static int
dpack_map_xtract_equ(struct dpack_decoder * decoder, unsigned int nr)
{
	dpack_assert_intern(decoder);
	dpack_assert_intern(nr);
	dpack_assert_intern(nr <= DPACK_MAP_FLDNR_MAX);

	unsigned int cnt;
	int          err;

	err = dpack_map_xtract_nr(&decoder->mpack, &cnt);
	if (err)
		return err;

	if (cnt != nr) {
		dpack_decoder_intr(decoder, mpack_type_map, 2 * cnt);
		return -ENOMSG;
	}

	return 0;
}

static int
dpack_map_xtract_range(struct dpack_decoder * decoder,
                       unsigned int           min_nr,
                       unsigned int           max_nr,
                       unsigned int         * nr)
{
	dpack_assert_intern(decoder);
	dpack_assert_intern(min_nr);
	dpack_assert_intern(min_nr < max_nr);
	dpack_assert_intern(max_nr <= DPACK_MAP_FLDNR_MAX);
	dpack_assert_intern(nr);

	int err;

	err = dpack_map_xtract_nr(&decoder->mpack, nr);
	if (err)
		return err;

	if ((*nr < min_nr) || (*nr > max_nr)) {
		dpack_decoder_intr(decoder, mpack_type_map, 2 * *nr);
		return -ENOMSG;
	}

	return 0;
}

static int
dpack_map_decode_fields(struct dpack_decoder * decoder,
                        dpack_decode_item_fn * decode,
                        void                 * data,
                        unsigned int           nr)
{
	dpack_assert_intern(decoder);
	dpack_assert_intern(decode);
	dpack_assert_intern(nr);

	int err;

	nr *= 2;
	do {
		unsigned int fid;

		err = dpack_map_decode_fldid(decoder, &fid);
		nr--;
		if (err)
			goto intr;

		err = decode(decoder, fid, data);
		nr--;
		if (err)
			goto intr;
	} while (nr);

	/*
	 * Always close current map. This is safe and performs a no-op in case
	 * the map is in an error state.
	 * When mpack is built with debugging turned on, this allows it to track
	 * bytes read.
	 */
	mpack_done_map(&decoder->mpack);

	return 0;

intr:
	dpack_decoder_intr(decoder, mpack_type_map, nr);

	return err;
}

int
dpack_map_decode(struct dpack_decoder * decoder,
                 dpack_decode_item_fn * decode,
                 void                 * data)
{
	dpack_assert_api(decoder);
	dpack_assert_api(decode);

	return dpack_map_decode_range(decoder,
	                              1,
	                              DPACK_MAP_FLDNR_MAX,
	                              decode,
	                              data);
}

int
dpack_map_decode_equ(struct dpack_decoder * decoder,
                     unsigned int           nr,
                     dpack_decode_item_fn * decode,
                     void                 * data)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(nr);
	dpack_assert_api(nr <= DPACK_MAP_FLDNR_MAX);
	dpack_assert_api(decode);

	int err;

	err = dpack_map_xtract_equ(decoder, nr);
	if (err)
		return err;

	return dpack_map_decode_fields(decoder, decode, data, nr);
}

int
dpack_map_decode_min(struct dpack_decoder * decoder,
                     unsigned int           min_nr,
                     dpack_decode_item_fn * decode,
                     void                 * data)
{
	dpack_assert_api(decoder);
	dpack_assert_api(min_nr);
	dpack_assert_api(min_nr < DPACK_MAP_FLDNR_MAX);
	dpack_assert_api(decode);

	return dpack_map_decode_range(decoder,
	                              min_nr,
	                              DPACK_MAP_FLDNR_MAX,
	                              decode,
	                              data);
}

int
dpack_map_decode_max(struct dpack_decoder * decoder,
                     unsigned int           max_nr,
                     dpack_decode_item_fn * decode,
                     void                 * data)
{
	dpack_assert_api(decoder);
	dpack_assert_api(max_nr);
	dpack_assert_api(max_nr <= DPACK_MAP_FLDNR_MAX);
	dpack_assert_api(decode);

	return dpack_map_decode_range(decoder, 1, max_nr, decode, data);
}

int
dpack_map_decode_range(struct dpack_decoder * decoder,
                       unsigned int           min_nr,
                       unsigned int           max_nr,
                       dpack_decode_item_fn * decode,
                       void                 * data)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(min_nr);
	dpack_assert_api(min_nr < max_nr);
	dpack_assert_api(max_nr <= DPACK_MAP_FLDNR_MAX);
	dpack_assert_api(decode);

	unsigned int nr;
	int          err;

	err = dpack_map_xtract_range(decoder, min_nr, max_nr, &nr);
	if (err)
		return err;

	return dpack_map_decode_fields(decoder, decode, data, nr);
}
