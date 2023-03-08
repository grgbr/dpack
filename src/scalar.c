#include "dpack/scalar.h"
#include "dpack/codec.h"
#include "common.h"

int
dpack_encode_bool(struct dpack_encoder * encoder, bool value)
{
	dpack_assert_encoder(encoder);

	mpack_write_bool(&encoder->mpack, value);

	return dpack_encoder_error_state(&encoder->mpack);
}

int
dpack_decode_bool(struct dpack_decoder * decoder, bool * value)
{
	dpack_assert_decoder(decoder);
	dpack_assert(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(&decoder->mpack, mpack_type_bool, &tag);
	if (err)
		return err;

	*value = mpack_tag_bool_value(&tag);

	return 0;
}

static int
dpack_xtract_u64_min(struct mpack_reader_t * reader,
                     uint64_t                low,
                     uint64_t              * value)
{
	dpack_assert(mpack_reader_error(reader) == mpack_ok);
	dpack_assert(low);
	dpack_assert(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_uint, &tag);
	if (err)
		return err;

	*value = mpack_tag_uint_value(&tag);
	if (*value < low) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -ERANGE;
	}

	return 0;
}

static int
dpack_xtract_u64_max(struct mpack_reader_t * reader,
                     uint64_t                high,
                     uint64_t              * value)
{
	dpack_assert(mpack_reader_error(reader) == mpack_ok);
	dpack_assert(high);
	dpack_assert(high < UINT64_MAX);
	dpack_assert(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_uint, &tag);
	if (err)
		return err;

	*value = mpack_tag_uint_value(&tag);
	if (*value > high) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -ERANGE;
	}

	return 0;
}

static int
dpack_xtract_u64_range(struct mpack_reader_t * reader,
                       uint64_t                low,
                       uint64_t                high,
                       uint64_t              * value)
{
	dpack_assert(mpack_reader_error(reader) == mpack_ok);
	dpack_assert(low);
	dpack_assert(low < high);
	dpack_assert(high < UINT64_MAX);
	dpack_assert(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_uint, &tag);
	if (err)
		return err;

	*value = mpack_tag_uint_value(&tag);
	if ((*value < low) || (*value > high)) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -ERANGE;
	}

	return 0;
}

#define DPACK_STDINT_DEFINE_ENCODE(_name, _type, _func) \
	int \
	_name(struct dpack_encoder * encoder, _type value) \
	{ \
		dpack_assert_encoder(encoder); \
		\
		_func(&encoder->mpack, value); \
		\
		return dpack_encoder_error_state(&encoder->mpack); \
	}

#define DPACK_STDINT_DEFINE_DECODE(_name, _type, _high) \
	int \
	_name(struct dpack_decoder * decoder, _type * value) \
	{ \
		dpack_assert_decoder(decoder); \
		dpack_assert(value); \
		\
		uint64_t val; \
		int      err; \
		\
		err = dpack_xtract_u64_max(&decoder->mpack, \
		                           (uint64_t)(_high), \
		                           &val); \
		if (err) \
			return err; \
		\
		*value = (_type)val; \
		\
		return 0; \
	}

#define DPACK_STDINT_DEFINE_DECODE_MIN(_name, _type, _high) \
	int \
	_name(struct dpack_decoder * decoder, _type low, _type * value) \
	{ \
		dpack_assert_decoder(decoder); \
		dpack_assert(low); \
		dpack_assert(low < (_high)); \
		dpack_assert(value); \
		\
		uint64_t val; \
		int      err; \
		\
		err = dpack_xtract_u64_range(&decoder->mpack, \
		                             (uint64_t)low, \
		                             (uint64_t)(_high), \
		                             &val); \
		if (err) \
			return err; \
		\
		*value = (_type)val; \
		\
		return 0; \
	}

#define DPACK_STDINT_DEFINE_DECODE_MAX(_name, _type, _high) \
	int \
	_name(struct dpack_decoder * decoder, _type high, _type * value) \
	{ \
		dpack_assert_decoder(decoder); \
		dpack_assert(high); \
		dpack_assert(high < (_high)); \
		dpack_assert(value); \
		\
		uint64_t val; \
		int      err; \
		\
		err = dpack_xtract_u64_max(&decoder->mpack, \
		                           (uint64_t)high, \
		                           &val); \
		if (err) \
			return err; \
		\
		*value = (_type)val; \
		\
		return 0; \
	}

#define DPACK_STDINT_DEFINE_DECODE_RANGE(_name, _type, _high) \
	int \
	_name(struct dpack_decoder * decoder, \
	      _type                  low, \
	      _type                  high, \
	      _type                * value) \
	{ \
		dpack_assert_decoder(decoder); \
		dpack_assert(low); \
		dpack_assert(low < high); \
		dpack_assert(high < (_high)); \
		dpack_assert(value); \
		\
		uint64_t val; \
		int      err; \
		\
		err = dpack_xtract_u64_range(&decoder->mpack, \
		                             (uint64_t)low, \
		                             (uint64_t)high, \
		                             &val); \
		if (err) \
			return err; \
		\
		*value = (_type)val; \
		\
		return 0; \
	}

/******************************************************************************
 * 8 bits integer
 ******************************************************************************/

DPACK_STDINT_DEFINE_ENCODE(dpack_encode_u8, uint8_t, mpack_write_u8)
DPACK_STDINT_DEFINE_DECODE(dpack_decode_u8, uint8_t, UINT8_MAX)
DPACK_STDINT_DEFINE_DECODE_MIN(dpack_decode_u8_min, uint8_t, UINT8_MAX)
DPACK_STDINT_DEFINE_DECODE_MAX(dpack_decode_u8_max, uint8_t, UINT8_MAX)
DPACK_STDINT_DEFINE_DECODE_RANGE(dpack_decode_u8_range, uint8_t, UINT8_MAX)

/******************************************************************************
 * 16 bits integer
 ******************************************************************************/

DPACK_STDINT_DEFINE_ENCODE(dpack_encode_u16, uint16_t, mpack_write_u16)
DPACK_STDINT_DEFINE_DECODE(dpack_decode_u16, uint16_t, UINT16_MAX)
DPACK_STDINT_DEFINE_DECODE_MIN(dpack_decode_u16_min, uint16_t, UINT16_MAX)
DPACK_STDINT_DEFINE_DECODE_MAX(dpack_decode_u16_max, uint16_t, UINT16_MAX)
DPACK_STDINT_DEFINE_DECODE_RANGE(dpack_decode_u16_range, uint16_t, UINT16_MAX)

/******************************************************************************
 * 32 bits integer
 ******************************************************************************/

DPACK_STDINT_DEFINE_ENCODE(dpack_encode_u32, uint32_t, mpack_write_u32)
DPACK_STDINT_DEFINE_DECODE(dpack_decode_u32, uint32_t, UINT32_MAX)
DPACK_STDINT_DEFINE_DECODE_MIN(dpack_decode_u32_min, uint32_t, UINT32_MAX)
DPACK_STDINT_DEFINE_DECODE_MAX(dpack_decode_u32_max, uint32_t, UINT32_MAX)
DPACK_STDINT_DEFINE_DECODE_RANGE(dpack_decode_u32_range, uint32_t, UINT32_MAX)

/******************************************************************************
 * 64 bits integer
 ******************************************************************************/

DPACK_STDINT_DEFINE_ENCODE(dpack_encode_u64, uint64_t, mpack_write_u64)
DPACK_STDINT_DEFINE_DECODE_MAX(dpack_decode_u64_max, uint64_t, UINT64_MAX)
DPACK_STDINT_DEFINE_DECODE_RANGE(dpack_decode_u64_range, uint64_t, UINT64_MAX)

int
dpack_decode_u64(struct dpack_decoder * decoder, uint64_t * value)
{
	dpack_assert_decoder(decoder);
	dpack_assert(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(&decoder->mpack, mpack_type_uint, &tag);
	if (err)
		return err;

	*value = mpack_tag_uint_value(&tag);

	return 0;
}

int
dpack_decode_u64_min(struct dpack_decoder * decoder,
                     uint64_t               low,
                     uint64_t             * value)
{
	dpack_assert_decoder(decoder);
	dpack_assert(low);
	dpack_assert(low < UINT64_MAX);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_min(&decoder->mpack, low, &val);
	if (err)
		return err;

	*value = val;

	return 0;
}
