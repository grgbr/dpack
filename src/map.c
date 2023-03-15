#include "dpack/map.h"
#include "dpack/codec.h"
#include "common.h"

void
dpack_map_begin_encode(struct dpack_encoder * encoder, unsigned int nr)
{
	dpack_assert_encoder(encoder);
	dpack_assert(nr);
	dpack_assert(nr <= DPACK_MAP_FLDNR_MAX);

	mpack_start_map(&encoder->mpack, nr);
}

void
dpack_map_end_encode(struct dpack_encoder * encoder)
{
	dpack_assert_encoder(encoder);

	mpack_finish_map(&encoder->mpack);
}

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

static int
dpack_map_xtract_nr(struct mpack_reader_t * reader, unsigned int * nr)
{
	dpack_assert(reader);
	dpack_assert(mpack_reader_error(reader) == mpack_ok);
	dpack_assert(nr);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_map, &tag);
	if (err)
		return err;

	*nr = (unsigned int)mpack_tag_map_count(&tag);

	return 0;
}

int
dpack_map_begin_decode(struct dpack_decoder * decoder, unsigned int * nr)
{
	dpack_assert_decoder(decoder);
	dpack_assert(nr);

	return dpack_map_begin_decode_range(decoder,
	                                    1,
	                                    DPACK_MAP_FLDNR_MAX,
	                                    nr);
}

int
dpack_map_begin_decode_equ(struct dpack_decoder * decoder, unsigned int nr)
{
	dpack_assert_decoder(decoder);
	dpack_assert(nr);
	dpack_assert(nr <= DPACK_MAP_FLDNR_MAX);

	unsigned int cnt;
	int          err;

	err = dpack_map_xtract_nr(&decoder->mpack, &cnt);
	if (err)
		return err;

	if (cnt != nr) {
		mpack_reader_flag_error(&decoder->mpack, mpack_error_type);
		return -ENOMSG;
	}

	return 0;
}

int
dpack_map_begin_decode_min(struct dpack_decoder * decoder,
                           unsigned int           min_nr,
                           unsigned int         * nr)
{
	dpack_assert_decoder(decoder);
	dpack_assert(min_nr);
	dpack_assert(nr);

	return dpack_map_begin_decode_range(decoder,
	                                    min_nr,
	                                    DPACK_MAP_FLDNR_MAX,
	                                    nr);
}

int
dpack_map_begin_decode_max(struct dpack_decoder * decoder,
                           unsigned int           max_nr,
                           unsigned int         * nr)
{
	dpack_assert_decoder(decoder);
	dpack_assert(max_nr);
	dpack_assert(max_nr <= DPACK_MAP_FLDNR_MAX);
	dpack_assert(nr);

	return dpack_map_begin_decode_range(decoder,
	                                    1,
	                                    max_nr,
	                                    nr);
}

int
dpack_map_begin_decode_range(struct dpack_decoder * decoder,
                             unsigned int           min_nr,
                             unsigned int           max_nr,
                             unsigned int         * nr)
{
	dpack_assert_decoder(decoder);
	dpack_assert(min_nr);
	dpack_assert(min_nr < max_nr);
	dpack_assert(max_nr <= DPACK_MAP_FLDNR_MAX);
	dpack_assert(nr);

	int err;

	err = dpack_map_xtract_nr(&decoder->mpack, nr);
	if (err)
		return err;

	if ((*nr < min_nr) || (*nr > max_nr)) {
		mpack_reader_flag_error(&decoder->mpack, mpack_error_type);
		return -ENOMSG;
	}

	return 0;
}

void
dpack_map_end_decode(struct dpack_decoder * decoder)
{
	dpack_assert(decoder);

	mpack_done_map(&decoder->mpack);
}

/******************************************************************************
 * Boolean
 ******************************************************************************/

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_bool,
                               bool,
                               dpack_encode_bool)

/******************************************************************************
 * 8 bits integers
 ******************************************************************************/

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_uint8,
                               uint8_t,
                               dpack_encode_uint8)

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_int8,
                               int8_t,
                               dpack_encode_int8)

/******************************************************************************
 * 16 bits integers
 ******************************************************************************/

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_uint16,
                               uint16_t,
                               dpack_encode_uint16)

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_int16,
                               int16_t,
                               dpack_encode_int16)

/******************************************************************************
 * 32 bits integers
 ******************************************************************************/

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_uint32,
                               uint32_t,
                               dpack_encode_uint32)

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_int32,
                               int32_t,
                               dpack_encode_int32)

/******************************************************************************
 * 64 bits integers
 ******************************************************************************/

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_uint64,
                               uint64_t,
                               dpack_encode_uint64)

DPACK_MAP_DEFINE_ENCODE_SCALAR(dpack_map_encode_int64,
                               int64_t,
                               dpack_encode_int64)

/******************************************************************************
 * Strings
 ******************************************************************************/

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
