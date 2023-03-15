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
