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
		head = DPACK_FIXMAP_TAG_SIZE;
		break;

#if DPACK_MAP_FLDNR_MAX > _DPACK_FIXMAP_FLDNR_MAX
	case (_DPACK_FIXMAP_FLDNR_MAX + 1) ... _DPACK_MAP16_FLDNR_MAX:
		head = DPACK_MAP16_TAG_SIZE;
		break;
#endif

#if DPACK_MAP_FLDNR_MAX > _DPACK_MAP16_FLDNR_MAX
	case (_DPACK_MAP16_FLDNR_MAX + 1) ... _DPACK_MAP32_FLDNR_MAX:
		head = DPACK_MAP32_TAG_SIZE;
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
	_name(struct dpack_encoder * __restrict encoder, \
	      unsigned int                      id, \
	      _type                             value) \
	{ \
		dpack_encoder_assert_api(encoder); \
		\
		int err; \
		\
		err = dpack_map_encode_fldid(encoder, id); \
		if (err) \
			return err; \
		\
		return _func(encoder, value); \
	}

int
dpack_map_begin_encode(struct dpack_encoder * __restrict encoder,
                       unsigned int                      nr)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(nr);
	dpack_assert_api(nr <= DPACK_ARRAY_ELMNR_MAX);

	int err;

	switch (nr) {
	case 1 ... _DPACK_FIXMAP_FLDNR_MAX:
		err = dpack_write_tag(encoder, _DPACK_FIXMAP_TAG | (uint8_t)nr);
		break;
#if DPACK_MAP_FLDNR_MAX > _DPACK_FIXMAP_FLDNR_MAX
	case (_DPACK_FIXMAP_FLDNR_MAX + 1) ... _DPACK_MAP16_FLDNR_MAX:
		err = dpack_write_tag(encoder, DPACK_MAP16_TAG);
		if (!err) {
			uint16_t val = htobe16((uint16_t)nr);

			err = dpack_encoder_write(encoder,
			                          (const uint8_t *)&val,
			                          sizeof(val));
		}
		break;
#endif
#if DPACK_MAP_FLDNR_MAX > _DPACK_MAP16_FLDNR_MAX
	case (_DPACK_MAP16_FLDNR_MAX + 1) ... _DPACK_MAP32_FLDNR_MAX:
		err = dpack_write_tag(encoder, DPACK_MAP32_TAG);
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
dpack_map_encode_float(struct dpack_encoder * __restrict encoder,
                       unsigned int                      id,
                       float                             value)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(!isnanf(value));

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
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(!isnan(value));

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
dpack_map_encode_str(struct dpack_encoder * __restrict encoder,
                     unsigned int                      id,
                     const char * __restrict           value)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(value);
	dpack_assert_api(value[0]);

	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_encode_str(encoder, value);
}

int
dpack_map_encode_str_fix(struct dpack_encoder * __restrict encoder,
                         unsigned int                      id,
                         const char * __restrict           value,
                         size_t                            length)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(value);
	dpack_assert_api(value[0]);
	dpack_assert_api(length);
	dpack_assert_api(length <= DPACK_STRLEN_MAX);
	dpack_assert_api(length == strnlen(value, (DPACK_STRLEN_MAX + 1)));

	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_encode_str_fix(encoder, value, length);
}

#endif /* defined(CONFIG_DPACK_STRING) */

/******************************************************************************
 * Map lvstr encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_LVSTR)

int
dpack_map_encode_lvstr(struct dpack_encoder * __restrict      encoder,
                       unsigned int                           id,
                       const struct stroll_lvstr * __restrict value)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(value);
	dpack_assert_api(stroll_lvstr_cstr(value));
	dpack_assert_api(stroll_lvstr_len(value));
	dpack_assert_api(strnlen(stroll_lvstr_cstr(value),
	                         DPACK_LVSTRLEN_MAX + 1) ==
	                 stroll_lvstr_len(value));

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
dpack_map_encode_bin(struct dpack_encoder * __restrict encoder,
                     unsigned int                      id,
                     const uint8_t * __restrict        value,
                     size_t                            size)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(value);
	dpack_assert_api(size);
	dpack_assert_api(size <= DPACK_BINSZ_MAX);

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
dpack_map_encode_nil(struct dpack_encoder * __restrict encoder,
                     unsigned int                      id)
{
	dpack_encoder_assert_api(encoder);

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
dpack_map_begin_encode_nest_map(struct dpack_encoder * __restrict encoder,
                                unsigned int                      id,
                                unsigned int                      nr)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(nr);
	dpack_assert_api(nr <= DPACK_ARRAY_ELMNR_MAX);

	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_map_begin_encode(encoder, nr);
}

#if defined(CONFIG_DPACK_ARRAY)

#include "dpack/array.h"

int
dpack_map_begin_encode_nest_array(struct dpack_encoder * __restrict encoder,
                                  unsigned int                      id,
                                  unsigned int                      nr)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(nr);
	dpack_assert_api(nr <= DPACK_ARRAY_ELMNR_MAX);

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

finish me!!!!

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
