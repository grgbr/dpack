/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/scalar.h"
#include "dpack/codec.h"
#include "common.h"
#if defined(CONFIG_DPACK_FLOAT) || defined(CONFIG_DPACK_DOUBLE)
#include <math.h>
#endif /* defined(CONFIG_DPACK_FLOAT) || defined(CONFIG_DPACK_DOUBLE) */

#define DPACK_SCALAR_DEFINE_ENCODE(_name, _type, _func) \
	int \
	_name(struct dpack_encoder * encoder, _type value) \
	{ \
		dpack_assert_api_encoder(encoder); \
		\
		_func(&encoder->mpack, value); \
		\
		return dpack_encoder_error_state(&encoder->mpack); \
	}

/******************************************************************************
 * Boolean
 ******************************************************************************/

DPACK_SCALAR_DEFINE_ENCODE(dpack_encode_bool, bool, mpack_write_bool)

int
dpack_decode_bool(struct dpack_decoder * decoder, bool * __restrict value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(value);

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

static int __dpack_nonull(1, 3) __dpack_nothrow __warn_result
dpack_xtract_uint64_min(struct mpack_reader_t * reader,
                        uint64_t                low,
                        uint64_t              * value)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);
	dpack_assert_intern(low);
	dpack_assert_intern(value);

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

static int __dpack_nonull(1, 3) __dpack_nothrow __warn_result
dpack_xtract_uint64_max(struct mpack_reader_t * reader,
                        uint64_t                high,
                        uint64_t * __restrict   value)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);
	dpack_assert_intern(high);
	dpack_assert_intern(high < UINT64_MAX);
	dpack_assert_intern(value);

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

static int __dpack_nonull(1, 4) __dpack_nothrow __warn_result
dpack_xtract_uint64_range(struct mpack_reader_t * reader,
                          uint64_t                low,
                          uint64_t                high,
                          uint64_t * __restrict   value)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);
	dpack_assert_intern(low);
	dpack_assert_intern(low < high);
	dpack_assert_intern(high < UINT64_MAX);
	dpack_assert_intern(value);

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
	_name(struct dpack_decoder * decoder, _type * __restrict value) \
	{ \
		dpack_assert_api_decoder(decoder); \
		dpack_assert_api(value); \
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
	_name(struct dpack_decoder * decoder, \
	      _type                  low, \
	      _type * __restrict     value) \
	{ \
		dpack_assert_api_decoder(decoder); \
		dpack_assert_api(low); \
		dpack_assert_api(low < (_high)); \
		dpack_assert_api(value); \
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
	_name(struct dpack_decoder * decoder, \
	      _type                  high, \
	      _type * __restrict     value) \
	{ \
		dpack_assert_api_decoder(decoder); \
		dpack_assert_api(high); \
		dpack_assert_api(high < (_high)); \
		dpack_assert_api(value); \
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
	      _type * __restrict     value) \
	{ \
		dpack_assert_api_decoder(decoder); \
		dpack_assert_api(low); \
		dpack_assert_api(low < high); \
		dpack_assert_api(high < (_high)); \
		dpack_assert_api(value); \
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

static int __dpack_nonull(1, 2) __dpack_nothrow __warn_result
dpack_xtract_int64(struct mpack_reader_t * reader, int64_t * __restrict value)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);
	dpack_assert_intern(value);

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
		if (*value < 0)
			/* 64 bits signed integer overflow. */
			return -ERANGE;
		break;

	default:
		mpack_reader_flag_error(reader, mpack_error_type);
		return -ENOMSG;
	}

	return 0;
}

static int __dpack_nonull(1, 4) __dpack_nothrow __warn_result
dpack_xtract_int64_range(struct mpack_reader_t * reader,
                         int64_t                 low,
                         int64_t                 high,
                         int64_t * __restrict    value)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);
	dpack_assert_intern(low > INT64_MIN);
	dpack_assert_intern(low < high);
	dpack_assert_intern(high < INT64_MAX);
	dpack_assert_intern(value);

	int err;

	err = dpack_xtract_int64(reader, value);
	if (err)
		return err;

	if ((*value < low) || (*value > high))
		return -ERANGE;

	return 0;
}

#define DPACK_INT_DEFINE_DECODE(_name, _type, _low, _high) \
	int \
	_name(struct dpack_decoder * decoder, _type * __restrict value) \
	{ \
		dpack_assert_api_decoder(decoder); \
		dpack_assert_api((_low) > INT64_MIN); \
		dpack_assert_api((_low) < (_high)); \
		dpack_assert_api((_high) < INT64_MAX); \
		dpack_assert_api(value); \
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
	_name(struct dpack_decoder * decoder, \
	      _type                  low, \
	      _type * __restrict     value) \
	{ \
		dpack_assert_api_decoder(decoder); \
		dpack_assert_api((_low) > INT64_MIN); \
		dpack_assert_api((_low) < (_high)); \
		dpack_assert_api((_high) < INT64_MAX); \
		dpack_assert_api(low > (_low)); \
		dpack_assert_api(low < (_high)); \
		dpack_assert_api(value); \
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
		dpack_assert_api_decoder(decoder); \
		dpack_assert_api((_low) > INT64_MIN); \
		dpack_assert_api((_low) < (_high)); \
		dpack_assert_api((_high) < INT64_MAX); \
		dpack_assert_api(high > (_low)); \
		dpack_assert_api(high < (_high)); \
		dpack_assert_api(value); \
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
		dpack_assert_api_decoder(decoder); \
		dpack_assert_api((_low) > INT64_MIN); \
		dpack_assert_api((_low) < (_high)); \
		dpack_assert_api((_high) < INT64_MAX); \
		dpack_assert_api(low > (_low)); \
		dpack_assert_api(low < high); \
		dpack_assert_api(high < (_high)); \
		dpack_assert_api(value); \
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

DPACK_SCALAR_DEFINE_ENCODE(dpack_encode_uint8, uint8_t, mpack_write_u8)
DPACK_UINT_DEFINE_DECODE(dpack_decode_uint8, uint8_t, UINT8_MAX)
DPACK_UINT_DEFINE_DECODE_MIN(dpack_decode_uint8_min, uint8_t, UINT8_MAX)
DPACK_UINT_DEFINE_DECODE_MAX(dpack_decode_uint8_max, uint8_t, UINT8_MAX)
DPACK_UINT_DEFINE_DECODE_RANGE(dpack_decode_uint8_range, uint8_t, UINT8_MAX)

DPACK_SCALAR_DEFINE_ENCODE(dpack_encode_int8, int8_t, mpack_write_i8)
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

DPACK_SCALAR_DEFINE_ENCODE(dpack_encode_uint16, uint16_t, mpack_write_u16)
DPACK_UINT_DEFINE_DECODE(dpack_decode_uint16, uint16_t, UINT16_MAX)
DPACK_UINT_DEFINE_DECODE_MIN(dpack_decode_uint16_min, uint16_t, UINT16_MAX)
DPACK_UINT_DEFINE_DECODE_MAX(dpack_decode_uint16_max, uint16_t, UINT16_MAX)
DPACK_UINT_DEFINE_DECODE_RANGE(dpack_decode_uint16_range, uint16_t, UINT16_MAX)

DPACK_SCALAR_DEFINE_ENCODE(dpack_encode_int16, int16_t, mpack_write_i16)
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

DPACK_SCALAR_DEFINE_ENCODE(dpack_encode_uint32, uint32_t, mpack_write_u32)
DPACK_UINT_DEFINE_DECODE(dpack_decode_uint32, uint32_t, UINT32_MAX)
DPACK_UINT_DEFINE_DECODE_MIN(dpack_decode_uint32_min, uint32_t, UINT32_MAX)
DPACK_UINT_DEFINE_DECODE_MAX(dpack_decode_uint32_max, uint32_t, UINT32_MAX)
DPACK_UINT_DEFINE_DECODE_RANGE(dpack_decode_uint32_range, uint32_t, UINT32_MAX)

DPACK_SCALAR_DEFINE_ENCODE(dpack_encode_int32, int32_t, mpack_write_i32)
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

DPACK_SCALAR_DEFINE_ENCODE(dpack_encode_uint64, uint64_t, mpack_write_u64)

int
dpack_decode_uint64(struct dpack_decoder * decoder, uint64_t * __restrict value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(value);

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
                        uint64_t * __restrict  value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(low);
	dpack_assert_api(low < UINT64_MAX);
	dpack_assert_api(value);

	return dpack_xtract_uint64_min(&decoder->mpack, low, value);
}

int
dpack_decode_uint64_max(struct dpack_decoder * decoder,
                        uint64_t               high,
                        uint64_t * __restrict  value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(high);
	dpack_assert_api(high < UINT64_MAX);
	dpack_assert_api(value);

	return dpack_xtract_uint64_max(&decoder->mpack, high, value);
}

int
dpack_decode_uint64_range(struct dpack_decoder * decoder,
                          uint64_t               low,
                          uint64_t               high,
                          uint64_t * __restrict  value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(low);
	dpack_assert_api(low < high);
	dpack_assert_api(high < UINT64_MAX);
	dpack_assert_api(value);

	return dpack_xtract_uint64_range(&decoder->mpack, low, high, value);
}

DPACK_SCALAR_DEFINE_ENCODE(dpack_encode_int64, int64_t, mpack_write_i64)

int
dpack_decode_int64(struct dpack_decoder * decoder, int64_t * __restrict value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(value);

	return dpack_xtract_int64(&decoder->mpack, value);
}

int
dpack_decode_int64_min(struct dpack_decoder * decoder,
                       int64_t                low,
                       int64_t * __restrict   value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(low > INT64_MIN);
	dpack_assert_api(low < INT64_MAX);
	dpack_assert_api(value);

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
                       int64_t * __restrict   value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(high > INT64_MIN);
	dpack_assert_api(high < INT64_MAX);
	dpack_assert_api(value);

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
                         int64_t * __restrict   value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(low > INT64_MIN);
	dpack_assert_api(low < high);
	dpack_assert_api(high < INT64_MAX);
	dpack_assert_api(value);

	return dpack_xtract_int64_range(&decoder->mpack, low, high, value);
}

/******************************************************************************
 * Single precision floating point
 ******************************************************************************/

#if defined(CONFIG_DPACK_FLOAT)

int
dpack_encode_float(struct dpack_encoder * encoder, float value)
{
	dpack_assert_api_encoder(encoder);
	dpack_assert_api(!isnan(value));

	mpack_write_float(&encoder->mpack, value);

	return dpack_encoder_error_state(&encoder->mpack);
}

int
dpack_decode_float(struct dpack_decoder * decoder, float * __restrict value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(&decoder->mpack, mpack_type_float, &tag);
	if (err)
		return err;

	*value = mpack_tag_float_value(&tag);
	if (isnan(*value))
		return -ENOMSG;

	return 0;
}

int
dpack_decode_float_min(struct dpack_decoder * decoder,
                       float                  low,
                       float * __restrict     value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(isfinite(low));
	dpack_assert_api(value);

	int err;

	err = dpack_decode_float(decoder, value);
	if (err)
		return err;

	if (*value < low)
		return -ERANGE;

	return 0;
}

int
dpack_decode_float_max(struct dpack_decoder * decoder,
                       float                  high,
                       float * __restrict     value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(isfinite(high));
	dpack_assert_api(value);

	int err;

	err = dpack_decode_float(decoder, value);
	if (err)
		return err;

	if (*value > high)
		return -ERANGE;

	return 0;
}

int
dpack_decode_float_range(struct dpack_decoder * decoder,
                         float                  low,
                         float                  high,
                         float * __restrict     value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(isfinite(low));
	dpack_assert_api(isfinite(high));
	dpack_assert_api(low < high);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_float(decoder, value);
	if (err)
		return err;

	if ((*value < low) || (*value > high))
		return -ERANGE;

	return 0;
}

#endif /* defined(CONFIG_DPACK_FLOAT) */

/******************************************************************************
 * Double precision floating point
 ******************************************************************************/

#if defined(CONFIG_DPACK_DOUBLE)

int
dpack_encode_double(struct dpack_encoder * encoder, double value)
{
	dpack_assert_api_encoder(encoder);
	dpack_assert_api(!isnan(value));

	mpack_write_double(&encoder->mpack, value);

	return dpack_encoder_error_state(&encoder->mpack);
}

int
dpack_decode_double(struct dpack_decoder * decoder, double * __restrict value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(&decoder->mpack, mpack_type_double, &tag);
	if (err)
		return err;

	*value = mpack_tag_double_value(&tag);
	if (isnan(*value))
		return -ENOMSG;

	return 0;
}

int
dpack_decode_double_min(struct dpack_decoder * decoder,
                        double                 low,
                        double * __restrict    value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(isfinite(low));
	dpack_assert_api(value);


	int err;

	err = dpack_decode_double(decoder, value);
	if (err)
		return err;

	if (*value < low)
		return -ERANGE;

	return 0;
}

int
dpack_decode_double_max(struct dpack_decoder * decoder,
                        double                 high,
                        double * __restrict    value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(isfinite(high));
	dpack_assert_api(value);

	int err;

	err = dpack_decode_double(decoder, value);
	if (err)
		return err;

	if (*value > high)
		return -ERANGE;

	return 0;
}

int
dpack_decode_double_range(struct dpack_decoder * decoder,
                          double                 low,
                          double                 high,
                          double * __restrict    value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(isfinite(low));
	dpack_assert_api(isfinite(high));
	dpack_assert_api(low < high);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_double(decoder, value);
	if (err)
		return err;

	if ((*value < low) || (*value > high))
		return -ERANGE;

	return 0;
}

#endif /* defined(CONFIG_DPACK_DOUBLE) */
