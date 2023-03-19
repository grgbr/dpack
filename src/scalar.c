#include "dpack/scalar.h"
#include "dpack/codec.h"
#include "common.h"

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

/******************************************************************************
 * Boolean
 ******************************************************************************/

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

/******************************************************************************
 * Unsigned integers common logic
 ******************************************************************************/

static int
dpack_xtract_uint64_min(struct mpack_reader_t * reader,
                        uint64_t                low,
                        uint64_t              * value)
{
	dpack_assert(reader);
	dpack_assert(mpack_reader_error(reader) == mpack_ok);
	dpack_assert(low);
	dpack_assert(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_uint, &tag);
	if (err)
		return err;

	*value = mpack_tag_uint_value(&tag);
	if (*value < low)
		return -ERANGE;

	return 0;
}

static int
dpack_xtract_uint64_max(struct mpack_reader_t * reader,
                        uint64_t                high,
                        uint64_t              * value)
{
	dpack_assert(reader);
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
	if (*value > high)
		return -ERANGE;

	return 0;
}

static int
dpack_xtract_uint64_range(struct mpack_reader_t * reader,
                          uint64_t                low,
                          uint64_t                high,
                          uint64_t              * value)
{
	dpack_assert(reader);
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
	if ((*value < low) || (*value > high))
		return -ERANGE;

	return 0;
}

#define DPACK_UINT_DEFINE_DECODE(_name, _type, _high) \
	int \
	_name(struct dpack_decoder * decoder, _type * value) \
	{ \
		dpack_assert_decoder(decoder); \
		dpack_assert(value); \
		\
		uint64_t val; \
		int      err; \
		\
		err = dpack_xtract_uint64_max(&decoder->mpack, \
		                              (uint64_t)(_high), \
		                              &val); \
		if (err) \
			return err; \
		\
		*value = (_type)val; \
		\
		return 0; \
	}

#define DPACK_UINT_DEFINE_DECODE_MIN(_name, _type, _high) \
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
		err = dpack_xtract_uint64_range(&decoder->mpack, \
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

#define DPACK_UINT_DEFINE_DECODE_MAX(_name, _type, _high) \
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
		err = dpack_xtract_uint64_max(&decoder->mpack, \
		                              (uint64_t)high, \
		                              &val); \
		if (err) \
			return err; \
		\
		*value = (_type)val; \
		\
		return 0; \
	}

#define DPACK_UINT_DEFINE_DECODE_RANGE(_name, _type, _high) \
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
		err = dpack_xtract_uint64_range(&decoder->mpack, \
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
 * Signed integers common logic
 ******************************************************************************/

static int
dpack_xtract_int64_range(struct mpack_reader_t * reader,
                         int64_t                 low,
                         int64_t                 high,
                         int64_t               * value)
{
	dpack_assert(reader);
	dpack_assert(mpack_reader_error(reader) == mpack_ok);
	dpack_assert(low > INT64_MIN);
	dpack_assert(low < high);
	dpack_assert(high < INT64_MAX);
	dpack_assert(value);

	struct mpack_tag_t tag;
	int                err;

	tag = mpack_read_tag(reader);
	err = mpack_reader_error(reader);
	if (err != mpack_ok)
		return dpack_errno_from_mpack(err);

	/*
	 * Positive integers are encoded using mpack_type_uint type, negative
	 * ones the mpack_type_int type. Check for both.
	 */
	switch (mpack_tag_type(&tag)) {
	case mpack_type_int:
		*value = mpack_tag_int_value(&tag);
		break;

	case mpack_type_uint:
		*value = (int64_t)mpack_tag_uint_value(&tag);
		break;

	default:
		mpack_reader_flag_error(reader, mpack_error_type);
		return -ENOMSG;
	}

	if ((*value < low) || (*value > high))
		return -ERANGE;

	return 0;
}

#define DPACK_INT_DEFINE_DECODE(_name, _type, _low, _high) \
	int \
	_name(struct dpack_decoder * decoder, _type * value) \
	{ \
		dpack_assert_decoder(decoder); \
		dpack_assert((_low) > INT64_MIN); \
		dpack_assert((_low) < (_high)); \
		dpack_assert((_high) < INT64_MAX); \
		dpack_assert(value); \
		\
		int64_t val; \
		int     err; \
		\
		err = dpack_xtract_int64_range(&decoder->mpack, \
		                               (int64_t)(_low), \
		                               (int64_t)(_high), \
		                               &val); \
		if (err) \
			return err; \
		\
		*value = (_type)val; \
		\
		return 0; \
	}

#define DPACK_INT_DEFINE_DECODE_MIN(_name, _type, _low, _high) \
	int \
	_name(struct dpack_decoder * decoder, _type low, _type * value) \
	{ \
		dpack_assert_decoder(decoder); \
		dpack_assert((_low) > INT64_MIN); \
		dpack_assert((_low) < (_high)); \
		dpack_assert((_high) < INT64_MAX); \
		dpack_assert(low > (_low)); \
		dpack_assert(low < (_high)); \
		dpack_assert(value); \
		\
		int64_t val; \
		int     err; \
		\
		err = dpack_xtract_int64_range(&decoder->mpack, \
		                               (int64_t)low, \
		                               (int64_t)(_high), \
		                               &val); \
		if (err) \
			return err; \
		\
		*value = (_type)val; \
		\
		return 0; \
	}

#define DPACK_INT_DEFINE_DECODE_MAX(_name, _type, _low, _high) \
	int \
	_name(struct dpack_decoder * decoder, _type high, _type * value) \
	{ \
		dpack_assert_decoder(decoder); \
		dpack_assert((_low) > INT64_MIN); \
		dpack_assert((_low) < (_high)); \
		dpack_assert((_high) < INT64_MAX); \
		dpack_assert(high > (_low)); \
		dpack_assert(high < (_high)); \
		dpack_assert(value); \
		\
		int64_t val; \
		int     err; \
		\
		err = dpack_xtract_int64_range(&decoder->mpack, \
		                               (int64_t)(_low), \
		                               (int64_t)high, \
		                               &val); \
		if (err) \
			return err; \
		\
		*value = (_type)val; \
		\
		return 0; \
	}

#define DPACK_INT_DEFINE_DECODE_RANGE(_name, _type, _low, _high) \
	int \
	_name(struct dpack_decoder * decoder, \
	      _type                  low, \
	      _type                  high, \
	      _type                * value) \
	{ \
		dpack_assert_decoder(decoder); \
		dpack_assert((_low) > INT64_MIN); \
		dpack_assert((_low) < (_high)); \
		dpack_assert((_high) < INT64_MAX); \
		dpack_assert(low > (_low)); \
		dpack_assert(low < high); \
		dpack_assert(high < (_high)); \
		dpack_assert(value); \
		\
		int64_t val; \
		int     err; \
		\
		err = dpack_xtract_int64_range(&decoder->mpack, \
		                               (int64_t)low, \
		                               (int64_t)high, \
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

DPACK_STDINT_DEFINE_ENCODE(dpack_encode_uint8, uint8_t, mpack_write_u8)
DPACK_UINT_DEFINE_DECODE(dpack_decode_uint8, uint8_t, UINT8_MAX)
DPACK_UINT_DEFINE_DECODE_MIN(dpack_decode_uint8_min, uint8_t, UINT8_MAX)
DPACK_UINT_DEFINE_DECODE_MAX(dpack_decode_uint8_max, uint8_t, UINT8_MAX)
DPACK_UINT_DEFINE_DECODE_RANGE(dpack_decode_uint8_range, uint8_t, UINT8_MAX)

DPACK_STDINT_DEFINE_ENCODE(dpack_encode_int8, int8_t, mpack_write_i8)
DPACK_INT_DEFINE_DECODE(dpack_decode_int8, int8_t, INT8_MIN, INT8_MAX)
DPACK_INT_DEFINE_DECODE_MIN(dpack_decode_int8_min, int8_t, INT8_MIN, INT8_MAX)
DPACK_INT_DEFINE_DECODE_MAX(dpack_decode_int8_max, int8_t, INT8_MIN, INT8_MAX)
DPACK_INT_DEFINE_DECODE_RANGE(dpack_decode_int8_range,
                              int8_t,
                              INT8_MIN,
                              INT8_MAX)

/******************************************************************************
 * 16 bits integer
 ******************************************************************************/

DPACK_STDINT_DEFINE_ENCODE(dpack_encode_uint16, uint16_t, mpack_write_u16)
DPACK_UINT_DEFINE_DECODE(dpack_decode_uint16, uint16_t, UINT16_MAX)
DPACK_UINT_DEFINE_DECODE_MIN(dpack_decode_uint16_min, uint16_t, UINT16_MAX)
DPACK_UINT_DEFINE_DECODE_MAX(dpack_decode_uint16_max, uint16_t, UINT16_MAX)
DPACK_UINT_DEFINE_DECODE_RANGE(dpack_decode_uint16_range, uint16_t, UINT16_MAX)

DPACK_STDINT_DEFINE_ENCODE(dpack_encode_int16, int16_t, mpack_write_i16)
DPACK_INT_DEFINE_DECODE(dpack_decode_int16, int16_t, INT16_MIN, INT16_MAX)
DPACK_INT_DEFINE_DECODE_MIN(dpack_decode_int16_min,
                            int16_t,
                            INT16_MIN,
                            INT16_MAX)
DPACK_INT_DEFINE_DECODE_MAX(dpack_decode_int16_max,
                            int16_t,
                            INT16_MIN,
                            INT16_MAX)
DPACK_INT_DEFINE_DECODE_RANGE(dpack_decode_int16_range,
                              int16_t,
                              INT16_MIN,
                              INT16_MAX)

/******************************************************************************
 * 32 bits integer
 ******************************************************************************/

DPACK_STDINT_DEFINE_ENCODE(dpack_encode_uint32, uint32_t, mpack_write_u32)
DPACK_UINT_DEFINE_DECODE(dpack_decode_uint32, uint32_t, UINT32_MAX)
DPACK_UINT_DEFINE_DECODE_MIN(dpack_decode_uint32_min, uint32_t, UINT32_MAX)
DPACK_UINT_DEFINE_DECODE_MAX(dpack_decode_uint32_max, uint32_t, UINT32_MAX)
DPACK_UINT_DEFINE_DECODE_RANGE(dpack_decode_uint32_range, uint32_t, UINT32_MAX)

DPACK_STDINT_DEFINE_ENCODE(dpack_encode_int32, int32_t, mpack_write_i32)
DPACK_INT_DEFINE_DECODE(dpack_decode_int32, int32_t, INT32_MIN, INT32_MAX)
DPACK_INT_DEFINE_DECODE_MIN(dpack_decode_int32_min,
                            int32_t,
                            INT32_MIN,
                            INT32_MAX)
DPACK_INT_DEFINE_DECODE_MAX(dpack_decode_int32_max,
                            int32_t,
                            INT32_MIN,
                            INT32_MAX)
DPACK_INT_DEFINE_DECODE_RANGE(dpack_decode_int32_range,
                              int32_t,
                              INT32_MIN,
                              INT32_MAX)

/******************************************************************************
 * 64 bits integer
 ******************************************************************************/

DPACK_STDINT_DEFINE_ENCODE(dpack_encode_uint64, uint64_t, mpack_write_u64)

int
dpack_decode_uint64(struct dpack_decoder * decoder, uint64_t * value)
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
dpack_decode_uint64_min(struct dpack_decoder * decoder,
                        uint64_t               low,
                        uint64_t             * value)
{
	dpack_assert_decoder(decoder);
	dpack_assert(low);
	dpack_assert(low < UINT64_MAX);
	dpack_assert(value);

	return dpack_xtract_uint64_min(&decoder->mpack, low, value);
}

int
dpack_decode_uint64_max(struct dpack_decoder * decoder,
                        uint64_t               high,
                        uint64_t             * value)
{
	dpack_assert_decoder(decoder);
	dpack_assert(high);
	dpack_assert(high < UINT64_MAX);
	dpack_assert(value);

	return dpack_xtract_uint64_max(&decoder->mpack, high, value);
}

int
dpack_decode_uint64_range(struct dpack_decoder * decoder,
                          uint64_t               low,
                          uint64_t               high,
                          uint64_t             * value)
{
	dpack_assert_decoder(decoder);
	dpack_assert(low);
	dpack_assert(low < high);
	dpack_assert(high < UINT64_MAX);
	dpack_assert(value);

	return dpack_xtract_uint64_range(&decoder->mpack, low, high, value);
}

DPACK_STDINT_DEFINE_ENCODE(dpack_encode_int64, int64_t, mpack_write_i64)

int
dpack_decode_int64(struct dpack_decoder * decoder, int64_t * value)
{
	dpack_assert_decoder(decoder);
	dpack_assert(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(&decoder->mpack, mpack_type_int, &tag);
	if (err)
		return err;

	*value = mpack_tag_int_value(&tag);

	return 0;
}

int
dpack_decode_int64_min(struct dpack_decoder * decoder,
                       int64_t                low,
                       int64_t              * value)
{
	dpack_assert_decoder(decoder);
	dpack_assert(low > INT64_MIN);
	dpack_assert(low < INT64_MAX);
	dpack_assert(value);

	int err;

	err = dpack_decode_int64(decoder, value);
	if (err)
		return err;

	if (*value < low)
		return -ERANGE;

	return 0;
}

int
dpack_decode_int64_max(struct dpack_decoder * decoder,
                       int64_t                high,
                       int64_t              * value)
{
	dpack_assert_decoder(decoder);
	dpack_assert(high > INT64_MIN);
	dpack_assert(high < INT64_MAX);
	dpack_assert(value);

	int err;

	err = dpack_decode_int64(decoder, value);
	if (err)
		return err;

	if (*value > high)
		return -ERANGE;

	return 0;
}

int
dpack_decode_int64_range(struct dpack_decoder * decoder,
                         int64_t                low,
                         int64_t                high,
                         int64_t              * value)
{
	dpack_assert_decoder(decoder);
	dpack_assert(low > INT64_MIN);
	dpack_assert(low < high);
	dpack_assert(high < INT64_MAX);
	dpack_assert(value);

	return dpack_xtract_int64_range(&decoder->mpack, low, high, value);
}
