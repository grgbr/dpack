#include "dpack/map.h"
#include "common.h"

/******************************************************************************
 * Map encoding
 ******************************************************************************/

#define DPACK_MAP_DEFINE_ENCODE_SCALAR(_name, _type, _func) \
	int \
	_name(struct dpack_encoder * encoder, \
	      uint16_t               id, \
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

void
dpack_map_begin_encode(struct dpack_encoder * encoder, unsigned int nr)
{
	dpack_assert_api_encoder(encoder);
	dpack_assert_api(nr);
	dpack_assert_api(nr <= DPACK_MAP_FLDNR_MAX);

	mpack_start_map(&encoder->mpack, nr);
}

void
dpack_map_end_encode(struct dpack_encoder * encoder)
{
	dpack_assert_api_encoder(encoder);

	mpack_finish_map(&encoder->mpack);
}

/******************************************************************************
 * Map field identifiers encoding
 ******************************************************************************/

int
dpack_map_encode_fldid(struct dpack_encoder * encoder, unsigned int id)
{
	dpack_assert_api(id <= DPACK_MAP_FLDID_MAX);

	return dpack_encode_uint(encoder, id);
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
 * Map strings encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_STRING)

int
dpack_map_encode_str(struct dpack_encoder * encoder,
                     uint16_t               id,
                     const char           * value)
{
	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_encode_str(encoder, value);
}

int
dpack_map_encode_str_fix(struct dpack_encoder * encoder,
                         uint16_t               id,
                         const char           * value,
                         size_t                 len)
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
dpack_map_encode_lvstr(struct dpack_encoder *      encoder,
                       uint16_t                    id,
                       const struct stroll_lvstr * value)
{
	int err;

	err = dpack_map_encode_fldid(encoder, id);
	if (err)
		return err;

	return dpack_encode_lvstr(encoder, value);
}

#endif /* defined(CONFIG_DPACK_LVSTR) */

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
			goto done;

		err = decode(decoder, fid, data);
		nr--;
		if (err)
			goto done;
	} while (nr);

	/*
	 * Always close current map. This is safe and performs a no-op in case
	 * the map is in an error state.
	 * When mpack is built with debugging turned on, this allows it to track
	 * bytes read.
	 */
	mpack_done_map(&decoder->mpack);

	return 0;

done:
	dpack_decoder_intr(decoder, mpack_type_map, nr);

	return err;
}

int
dpack_map_decode_fldid(struct dpack_decoder * decoder, unsigned int * id)
{
	return dpack_decode_uint_max(decoder, DPACK_MAP_FLDID_MAX, id);
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
