/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "utest.h"
#include "dpack/scalar.h"
#include "dpack/codec.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <math.h>
#include <errno.h>

#define DPACKUT_FLOAT(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed    = _packed, \
		.size      = sizeof(_packed) - 1, \
		.error     = _error, \
		.value.f32 = _value \
	}

static void
dpackut_float_encode(const struct dpackut_scalar_data * data)
{
	struct dpack_encoder enc = { 0, };
	size_t               sz = data->size;
	char                 buff[sz];

	memset(buff, 0xa5, sz);
	dpack_encoder_init_buffer(&enc, buff, sz);

	cute_check_uint(data->size, equal, DPACK_FLOAT_SIZE);
	cute_check_sint(dpack_encode_float(&enc, data->value.f32),
	                equal,
	                data->error);
	cute_check_mem(buff, equal, data->packed, sz);

	cute_check_uint(dpack_encoder_space_used(&enc), equal, sz);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc, DPACK_DONE);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_float_encode_assert)
{
	float                val = NAN;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;
	char                 buff[DPACK_FLOAT_SIZE];

	cute_expect_assertion(ret = dpack_encode_float(NULL, val));
	cute_expect_assertion(ret = dpack_encode_float(&enc, val));

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_encode_float(&enc, NAN));
	dpack_encoder_fini(&enc, DPACK_DONE);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_float_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_float_encode_neg_inf)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_FLOAT(data, "\xca\xff\x80\x00\x00", 0, -INFINITY);

	dpackut_float_encode(&data);
}

CUTE_TEST(dpackut_float_encode_neg_max)
{
	/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT(data, "\xca\xff\x7f\xff\xff", 0, -MAXFLOAT);
	dpackut_float_encode(&data);
}

CUTE_TEST(dpackut_float_encode_neg_min)
{
	/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT(data, "\xca\x80\x80\x00\x00", 0, -MINFLOAT);
	dpackut_float_encode(&data);
}

CUTE_TEST(dpackut_float_encode_neg_zero)
{
	/* -0: dpack-utest-gen.py -s "- 0.0" */
	DPACKUT_FLOAT(data, "\xca\x80\x00\x00\x00", 0, -0.0f);
	dpackut_float_encode(&data);
}

CUTE_TEST(dpackut_float_encode_pos_zero)
{
	/* +0: dpack-utest-gen.py -s "0.0" */
	DPACKUT_FLOAT(data, "\xca\x00\x00\x00\x00", 0, 0.0f);
	dpackut_float_encode(&data);
}

CUTE_TEST(dpackut_float_encode_pos_min)
{
	/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT(data, "\xca\x00\x80\x00\x00", 0, MINFLOAT);
	dpackut_float_encode(&data);
}

CUTE_TEST(dpackut_float_encode_pos_max)
{
	/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT(data, "\xca\x7f\x7f\xff\xff", 0, MAXFLOAT);
	dpackut_float_encode(&data);
}

CUTE_TEST(dpackut_float_encode_pos_inf)
{
	/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
	DPACKUT_FLOAT(data, "\xca\x7f\x80\x00\x00", 0, INFINITY);
	dpackut_float_encode(&data);
}

static void
dpackut_float_decode(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	float                val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_float(&dec, &val), equal, data->error);
	if (!data->error) {
		cute_check_flt(val, equal, data->value.f32);
		cute_check_uint(data->size, equal, DPACK_FLOAT_SIZE);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_float_decode_assert)
{
	float                val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_FLOAT_SIZE];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_float(NULL, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_float(&dec, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_float(&dec, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_float_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_float_decode_neg_nan)
{
	/* -Not A Number: dpack-utest-gen.py -s "float('-nan')" */
	DPACKUT_FLOAT(data, "\xca\xff\xc0\x00\x00", -ENOMSG, -NAN);
	dpackut_float_decode(&data);
}

CUTE_TEST(dpackut_float_decode_neg_inf)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_FLOAT(data, "\xca\xff\x80\x00\x00", 0, -INFINITY);
	dpackut_float_decode(&data);
}

CUTE_TEST(dpackut_float_decode_neg_max)
{
	/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT(data, "\xca\xff\x7f\xff\xff", 0, -MAXFLOAT);
	dpackut_float_decode(&data);
}

CUTE_TEST(dpackut_float_decode_neg_min)
{
	/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT(data, "\xca\x80\x80\x00\x00", 0, -MINFLOAT);
	dpackut_float_decode(&data);
}

CUTE_TEST(dpackut_float_decode_neg_zero)
{
	/* -0: dpack-utest-gen.py -s "- 0.0" */
	DPACKUT_FLOAT(data, "\xca\x80\x00\x00\x00", 0, -0.0f);
	dpackut_float_decode(&data);
}

CUTE_TEST(dpackut_float_decode_pos_zero)
{
	/* +0: dpack-utest-gen.py -s "0.0" */
	DPACKUT_FLOAT(data, "\xca\x00\x00\x00\x00", 0, 0.0f);
	dpackut_float_decode(&data);
}

CUTE_TEST(dpackut_float_decode_pos_min)
{
	/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT(data, "\xca\x00\x80\x00\x00", 0, MINFLOAT);
	dpackut_float_decode(&data);
}

CUTE_TEST(dpackut_float_decode_pos_max)
{
	/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT(data, "\xca\x7f\x7f\xff\xff", 0, MAXFLOAT);
	dpackut_float_decode(&data);
}

CUTE_TEST(dpackut_float_decode_pos_inf)
{
	/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
	DPACKUT_FLOAT(data, "\xca\x7f\x80\x00\x00", 0, INFINITY);
	dpackut_float_decode(&data);
}

CUTE_TEST(dpackut_float_decode_pos_nan)
{
	/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
	DPACKUT_FLOAT(data, "\xca\x7f\xc0\x00\x00", -ENOMSG, NAN);
	dpackut_float_decode(&data);
}

CUTE_TEST(dpackut_float_decode_pos_zero_dbl)
{
	/* +0 (double): dpack-utest-gen.py "float(0.0)" */
	DPACKUT_FLOAT(data, "\xcb\x00\x00\x00\x00\x00\x00\x00\x00", -ENOMSG, 0);
	dpackut_float_decode(&data);
}

#define DPACKUT_FLOAT_MIN(_var, _packed, _error, _value, _low) \
	const struct dpackut_scalar_data _var = { \
		.packed    = _packed, \
		.size      = sizeof(_packed) - 1, \
		.error     = _error, \
		.value.f32 = _value, \
		.low.f32   = _low \
	}

static void
dpackut_float_decode_min(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	float                val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_float_min(&dec, data->low.f32, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_flt(val, equal, data->value.f32);
		cute_check_uint(data->size, equal, DPACK_FLOAT_SIZE);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_float_decode_min_assert)
{
	float                val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_FLOAT_SIZE];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_float_min(NULL, 1.0f, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_float_min(&dec, 1.0f, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_float_min(&dec,
	                                                   -INFINITY,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_float_min(&dec,
	                                                   INFINITY,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_float_min(&dec, NAN, &val));
	cute_expect_assertion(ret = dpack_decode_float_min(&dec, -NAN, &val));
	cute_expect_assertion(ret = dpack_decode_float_min(&dec, 1.0f, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_float_decode_min_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_float_decode_min_neg_nan_pos_zero)
{
	/* -Not A Number: dpack-utest-gen.py -s "float('-nan')" */
	DPACKUT_FLOAT_MIN(data, "\xca\xff\xc0\x00\x00", -ENOMSG, -NAN, 0.0f);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_neg_inf_neg_max)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\xff\x80\x00\x00",
	                  -ERANGE,
	                  -INFINITY,
	                  -MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_neg_max_neg_max)
{
	/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\xff\x7f\xff\xff",
	                  0,
	                  -MAXFLOAT,
	                  -MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_neg_min_neg_max)
{
	/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x80\x80\x00\x00",
	                  0,
	                  -MINFLOAT,
	                  -MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_neg_zero_neg_max)
{
	/* -0: dpack-utest-gen.py -s "- 0.0" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x80\x00\x00\x00",
	                  0,
	                  -0.0f,
	                  -MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_zero_neg_max)
{
	/* +0: dpack-utest-gen.py -s "0.0" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x00\x00\x00\x00",
	                  0,
	                  0.0f,
	                  -MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_min_neg_max)
{
	/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x00\x80\x00\x00",
	                  0,
	                  MINFLOAT,
	                  -MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_max_neg_max)
{
	/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x7f\x7f\xff\xff",
	                  0,
	                  MAXFLOAT,
	                  -MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_inf_neg_max)
{
	/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x7f\x80\x00\x00",
	                  0,
	                  INFINITY,
	                  -MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_neg_inf_pos_zero)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\xff\x80\x00\x00",
	                  -ERANGE,
	                  -INFINITY,
	                  0.0f);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_neg_max_pos_zero)
{
	/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\xff\x7f\xff\xff",
	                  -ERANGE,
	                  -MAXFLOAT,
	                  0.0f);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_neg_min_pos_zero)
{
	/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x80\x80\x00\x00",
	                  -ERANGE,
	                  -MINFLOAT,
	                  0.0f);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_neg_zero_pos_zero)
{
	/* -0: dpack-utest-gen.py -s "- 0.0" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x80\x00\x00\x00",
	                  0,
	                  -0.0f,
	                  0.0f);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_zero_pos_zero)
{
	/* +0: dpack-utest-gen.py -s "0.0" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x00\x00\x00\x00",
	                  0,
	                  0.0f,
	                  0.0f);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_min_pos_zero)
{
	/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x00\x80\x00\x00",
	                  0,
	                  MINFLOAT,
	                  0.0f);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_max_pos_zero)
{
	/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x7f\x7f\xff\xff",
	                  0,
	                  MAXFLOAT,
	                  0.0f);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_inf_pos_zero)
{
	/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x7f\x80\x00\x00",
	                  0,
	                  INFINITY,
	                  0.0f);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_nan_pos_zero)
{
	/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x7f\xc0\x00\x00",
	                  -ENOMSG,
	                  NAN,
	                  0.0f);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_neg_inf_pos_max)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\xff\x80\x00\x00",
	                  -ERANGE,
	                  -INFINITY,
	                  MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_neg_max_pos_max)
{
	/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\xff\x7f\xff\xff",
	                  -ERANGE,
	                  -MAXFLOAT,
	                  MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_neg_min_pos_max)
{
	/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x80\x80\x00\x00",
	                  -ERANGE,
	                  -MINFLOAT,
	                  MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_neg_zero_pos_max)
{
	/* -0: dpack-utest-gen.py -s "- 0.0" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x80\x00\x00\x00",
	                  -ERANGE,
	                  -0.0f,
	                  MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_zero_pos_max)
{
	/* +0: dpack-utest-gen.py -s "0.0" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x00\x00\x00\x00",
	                  -ERANGE,
	                  0.0f,
	                  MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_min_pos_max)
{
	/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x00\x80\x00\x00",
	                  -ERANGE,
	                  MINFLOAT,
	                  MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_max_pos_max)
{
	/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x7f\x7f\xff\xff",
	                  0,
	                  MAXFLOAT,
	                  MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_info_pos_max)
{
	/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xca\x7f\x80\x00\x00",
	                  0,
	                  INFINITY,
	                  MAXFLOAT);
	dpackut_float_decode_min(&data);
}

CUTE_TEST(dpackut_float_decode_min_pos_zero_pos_zero_flt)
{
	/* +0 (double): dpack-utest-gen.py "float(0.0)" */
	DPACKUT_FLOAT_MIN(data,
	                  "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	                  -ENOMSG,
	                  0.0,
	                  0.0f);
	dpackut_float_decode_min(&data);
}

#define DPACKUT_FLOAT_MAX(_var, _packed, _error, _value, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.f32  = _value, \
		.high.f32   = _high \
	}

static void
dpackut_float_decode_max(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	float                val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_float_max(&dec, data->high.f32, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_flt(val, equal, data->value.f32);
		cute_check_uint(data->size, equal, DPACK_FLOAT_SIZE);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_float_decode_max_assert)
{
	float                val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_FLOAT_SIZE];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_float_max(NULL, 1.0f, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_float_max(&dec, 1.0f, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_float_max(&dec,
	                                                   -INFINITY,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_float_max(&dec,
	                                                   INFINITY,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_float_max(&dec, NAN, &val));
	cute_expect_assertion(ret = dpack_decode_float_max(&dec, -NAN, &val));
	cute_expect_assertion(ret = dpack_decode_float_max(&dec, 1.0f, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_float_decode_max_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_float_decode_max_neg_nan_pos_zero)
{
	/* -Not A Number: dpack-utest-gen.py -s "float('-nan')" */
	DPACKUT_FLOAT_MAX(data, "\xca\xff\xc0\x00\x00", -ENOMSG, -NAN, 0.0f);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_neg_inf_neg_max)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\xff\x80\x00\x00",
	                  0,
	                  -INFINITY,
	                  -MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_neg_max_neg_max)
{
	/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\xff\x7f\xff\xff",
	                  0,
	                  -MAXFLOAT,
	                  -MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_neg_min_neg_max)
{
	/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x80\x80\x00\x00",
	                  -ERANGE,
	                  -MINFLOAT,
	                  -MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_neg_zero_neg_max)
{
	/* -0: dpack-utest-gen.py -s "- 0.0" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x80\x00\x00\x00",
	                  -ERANGE,
	                  -0.0f,
	                  -MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_zero_neg_max)
{
	/* +0: dpack-utest-gen.py -s "0.0" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x00\x00\x00\x00",
	                  -ERANGE,
	                  0.0f,
	                  -MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_min_neg_max)
{
	/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x00\x80\x00\x00",
	                  -ERANGE,
	                  MINFLOAT,
	                  -MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_max_neg_max)
{
	/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x7f\x7f\xff\xff",
	                  -ERANGE,
	                  MAXFLOAT,
	                  -MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_inf_neg_max)
{
	/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x7f\x80\x00\x00",
	                  -ERANGE,
	                  INFINITY,
	                  -MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_neg_inf_pos_zero)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\xff\x80\x00\x00",
	                  0,
	                  -INFINITY,
	                  0.0f);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_neg_max_pos_zero)
{
	/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\xff\x7f\xff\xff",
	                  0,
	                  -MAXFLOAT,
	                  0.0f);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_neg_min_pos_zero)
{
	/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x80\x80\x00\x00",
	                  0,
	                  -MINFLOAT,
	                  0.0f);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_neg_zero_pos_zero)
{
	/* -0: dpack-utest-gen.py -s "- 0.0" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x80\x00\x00\x00",
	                  0,
	                  -0.0f,
	                  0.0f);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_zero_pos_zero)
{
	/* +0: dpack-utest-gen.py -s "0.0" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x00\x00\x00\x00",
	                  0,
	                  0.0f,
	                  0.0f);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_min_pos_zero)
{
	/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x00\x80\x00\x00",
	                  -ERANGE,
	                  MINFLOAT,
	                  0.0f);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_max_pos_zero)
{
	/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x7f\x7f\xff\xff",
	                  -ERANGE,
	                  MAXFLOAT,
	                  0.0f);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_inf_pos_zero)
{
	/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x7f\x80\x00\x00",
	                  -ERANGE,
	                  INFINITY,
	                  0.0f);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_neg_inf_pos_max)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\xff\x80\x00\x00",
	                  0,
	                  -INFINITY,
	                  MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_neg_max_pos_max)
{
	/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\xff\x7f\xff\xff",
	                  0,
	                  -MAXFLOAT,
	                  MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_neg_min_pos_max)
{
	/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x80\x80\x00\x00",
	                  0,
	                  -MINFLOAT,
	                  MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_neg_zero_pos_max)
{
	/* -0: dpack-utest-gen.py -s "- 0.0" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x80\x00\x00\x00",
	                  0,
	                  -0.0f,
	                  MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_zero_pos_max)
{
	/* +0: dpack-utest-gen.py -s "0.0" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x00\x00\x00\x00",
	                  0,
	                  0.0f,
	                  MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_min_pos_max)
{
	/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x00\x80\x00\x00",
	                  0,
	                  MINFLOAT,
	                  MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_max_pos_max)
{
	/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x7f\x7f\xff\xff",
	                  0,
	                  MAXFLOAT,
	                  MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_inf_pos_max)
{
	/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x7f\x80\x00\x00",
	                  -ERANGE,
	                  INFINITY,
	                  MAXFLOAT);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_nan_pos_zero)
{
	/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xca\x7f\xc0\x00\x00",
	                  -ENOMSG,
	                  NAN,
	                  0.0f);
	dpackut_float_decode_max(&data);
}

CUTE_TEST(dpackut_float_decode_max_pos_zero_pos_zero_dbl)
{
	/* +0 (double): dpack-utest-gen.py "float(0.0)" */
	DPACKUT_FLOAT_MAX(data,
	                  "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	                  -ENOMSG,
	                  0.0,
	                  0.0f);
	dpackut_float_decode_max(&data);
}

#define DPACKUT_FLOAT_RANGE(_var, _packed, _error, _value, _low, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.f32  = _value, \
		.low.f32    = _low, \
		.high.f32   = _high \
	}

static void
dpackut_float_decode_range(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	float                val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_float_range(&dec,
	                                         data->low.f32,
	                                         data->high.f32,
	                                         &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_flt(val, equal, data->value.f32);
		cute_check_uint(data->size, equal, DPACK_FLOAT_SIZE);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_float_decode_range_assert)
{
	float                val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_FLOAT_SIZE];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_float_range(NULL,
	                                                     1.0f,
	                                                     2.0f,
	                                                     &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_float_range(&dec,
	                                                     1.0f,
	                                                     2.0f,
	                                                     &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_float_range(&dec,
	                                                     -INFINITY,
	                                                     2.0f,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_float_range(&dec,
	                                                     1.0f,
	                                                     INFINITY,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_float_range(&dec,
	                                                     NAN,
	                                                     2.0f,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_float_range(&dec,
	                                                     -NAN,
	                                                     2.0f,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_float_range(&dec,
	                                                     1.0f,
	                                                     NAN,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_float_range(&dec,
	                                                     1.0f,
	                                                     -NAN,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_float_range(&dec,
	                                                     1.0f,
	                                                     2.0f,
	                                                     NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_float_decode_range_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_float_decode_range_neg_nan_pos_zero_one)
{
	/* -Not A Number: dpack-utest-gen.py -s "float('-nan')" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\xff\xc0\x00\x00",
	                    -ENOMSG,
	                    -NAN,
	                    0.0f,
	                    1.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_neg_inf_neg_max_pos_zero)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\xff\x80\x00\x00",
	                    -ERANGE,
	                    -INFINITY,
	                    -MAXFLOAT,
	                    0.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_neg_max_neg_max_pos_zero)
{
	/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\xff\x7f\xff\xff",
	                    0,
	                    -MAXFLOAT,
	                    -MAXFLOAT,
	                    0.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_neg_min_neg_max_pos_zero)
{
	/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x80\x80\x00\x00",
	                    0,
	                    -MINFLOAT,
	                    -MAXFLOAT,
	                    0.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_neg_zero_neg_max_pos_zero)
{
	/* -0: dpack-utest-gen.py -s "- 0.0" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x80\x00\x00\x00",
	                    0,
	                    -0.0f,
	                    -MAXFLOAT,
	                    0.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_zero_neg_max_pos_zero)
{
	/* +0: dpack-utest-gen.py -s "0.0" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x00\x00\x00\x00",
	                    0,
	                    0.0f,
	                    -MAXFLOAT,
	                    0.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_min_neg_max_pos_zero)
{
	/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x00\x80\x00\x00",
	                    -ERANGE,
	                    MINFLOAT,
	                    -MAXFLOAT,
	                    0.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_max_neg_max_pos_zero)
{
	/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x7f\x7f\xff\xff",
	                    -ERANGE,
	                    MAXFLOAT,
	                    -MAXFLOAT,
	                    0.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_inf_neg_max_pos_zero)
{
	/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x7f\x80\x00\x00",
	                    -ERANGE,
	                    INFINITY,
	                    -MAXFLOAT,
	                    0.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_neg_inf_pos_zero_pos_max)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\xff\x80\x00\x00",
	                    -ERANGE,
	                    -INFINITY,
	                    0.0f,
	                    MAXFLOAT);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_neg_max_pos_zero_pos_max)
{
	/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\xff\x7f\xff\xff",
	                    -ERANGE,
	                    -MAXFLOAT,
	                    0.0f,
	                    MAXFLOAT);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_neg_min_pos_zero_pos_max)
{
	/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x80\x80\x00\x00",
	                    -ERANGE,
	                    -MINFLOAT,
	                    0.0f,
	                    MAXFLOAT);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_neg_zero_pos_zero_pos_max)
{
	/* -0: dpack-utest-gen.py -s "- 0.0" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x80\x00\x00\x00",
	                    0,
	                    -0.0f,
	                    0.0f,
	                    MAXFLOAT);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_zero_pos_zero_pos_max)
{
	/* +0: dpack-utest-gen.py -s "0.0" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x00\x00\x00\x00",
	                    0,
	                    0.0f,
	                    0.0f,
	                    MAXFLOAT);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_min_pos_zero_pos_max)
{
	/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x00\x80\x00\x00",
	                    0,
	                    MINFLOAT,
	                    0.0f,
	                    MAXFLOAT);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_max_pos_zero_pos_max)
{
	/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x7f\x7f\xff\xff",
	                    0,
	                    MAXFLOAT,
	                    0.0f,
	                    MAXFLOAT);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_inf_pos_zero_pos_max)
{
	/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x7f\x80\x00\x00",
	                    -ERANGE,
	                    INFINITY,
	                    0.0f,
	                    MAXFLOAT);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_nan_pos_zero_pos_max)
{
	/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x7f\xc0\x00\x00",
	                    -ENOMSG,
	                    NAN,
	                    0.0f,
	                    MAXFLOAT);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_neg_info_one_two)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\xff\x80\x00\x00",
	                    -ERANGE,
	                    -INFINITY,
	                    1.0f,
	                    2.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_neg_max_one_two)
{
	/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\xff\x7f\xff\xff",
	                    -ERANGE,
	                    -MAXFLOAT,
	                    1.0f,
	                    2.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_neg_min_one_two)
{
	/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x80\x80\x00\x00",
	                    -ERANGE,
	                    -MINFLOAT,
	                    1.0f,
	                    2.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_neg_zero_one_two)
{
	/* -0: dpack-utest-gen.py -s "- 0.0" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x80\x00\x00\x00",
	                    -ERANGE,
	                    -0.0f,
	                    1.0f,
	                    2.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_zero_one_two)
{
	/* +0: dpack-utest-gen.py -s "0.0" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x00\x00\x00\x00",
	                    -ERANGE,
	                    0.0f,
	                    1.0f,
	                    2.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_one_one_two)
{
	/* +1.0: dpack-utest-gen.py -s "1.0" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x3f\x80\x00\x00",
	                    0,
	                    1.0f,
	                    1.0f,
	                    2.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_onenhalf_one_two)
{
	/* +1.5: dpack-utest-gen.py -s "1.5" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x3f\xc0\x00\x00",
	                    0,
	                    1.5f,
	                    1.0f,
	                    2.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_two_one_two)
{
	/* +2.0: dpack-utest-gen.py -s "2.0" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x40\x00\x00\x00",
	                    0,
	                    2.0f,
	                    1.0f,
	                    2.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_min_one_two)
{
	/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x00\x80\x00\x00",
	                    -ERANGE,
	                    MINFLOAT,
	                    1.0f,
	                    2.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_max_one_two)
{
	/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x7f\x7f\xff\xff",
	                    -ERANGE,
	                    MAXFLOAT,
	                    1.0f,
	                    2.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_inf_one_two)
{
	/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x7f\x80\x00\x00",
	                    -ERANGE,
	                    INFINITY,
	                    1.0f,
	                    2.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_nan_pos_zero_one)
{
	/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xca\x7f\xc0\x00\x00",
	                    -ENOMSG,
	                    NAN,
	                    0.0f,
	                    1.0f);
	dpackut_float_decode_range(&data);
}

CUTE_TEST(dpackut_float_decode_range_pos_zero_pos_zero_one)
{
	/* +0 (double): dpack-utest-gen.py "float(0.0)" */
	DPACKUT_FLOAT_RANGE(data,
	                    "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	                    -ENOMSG,
	                    0.0,
	                    0.0f,
	                    1.0f);
	dpackut_float_decode_range(&data);
}

CUTE_GROUP(dpackut_float_group) = {
	CUTE_REF(dpackut_float_encode_assert),
	CUTE_REF(dpackut_float_encode_neg_inf),
	CUTE_REF(dpackut_float_encode_neg_max),
	CUTE_REF(dpackut_float_encode_neg_min),
	CUTE_REF(dpackut_float_encode_neg_zero),
	CUTE_REF(dpackut_float_encode_pos_zero),
	CUTE_REF(dpackut_float_encode_pos_min),
	CUTE_REF(dpackut_float_encode_pos_max),
	CUTE_REF(dpackut_float_encode_pos_inf),

	CUTE_REF(dpackut_float_decode_assert),
	CUTE_REF(dpackut_float_decode_neg_nan),
	CUTE_REF(dpackut_float_decode_neg_inf),
	CUTE_REF(dpackut_float_decode_neg_max),
	CUTE_REF(dpackut_float_decode_neg_min),
	CUTE_REF(dpackut_float_decode_neg_zero),
	CUTE_REF(dpackut_float_decode_pos_zero),
	CUTE_REF(dpackut_float_decode_pos_min),
	CUTE_REF(dpackut_float_decode_pos_max),
	CUTE_REF(dpackut_float_decode_pos_inf),
	CUTE_REF(dpackut_float_decode_pos_nan),
	CUTE_REF(dpackut_float_decode_pos_zero_dbl),

	CUTE_REF(dpackut_float_decode_min_assert),
	CUTE_REF(dpackut_float_decode_min_neg_nan_pos_zero),
	CUTE_REF(dpackut_float_decode_min_neg_inf_neg_max),
	CUTE_REF(dpackut_float_decode_min_neg_max_neg_max),
	CUTE_REF(dpackut_float_decode_min_neg_min_neg_max),
	CUTE_REF(dpackut_float_decode_min_neg_zero_neg_max),
	CUTE_REF(dpackut_float_decode_min_pos_zero_neg_max),
	CUTE_REF(dpackut_float_decode_min_pos_min_neg_max),
	CUTE_REF(dpackut_float_decode_min_pos_max_neg_max),
	CUTE_REF(dpackut_float_decode_min_pos_inf_neg_max),
	CUTE_REF(dpackut_float_decode_min_neg_inf_pos_zero),
	CUTE_REF(dpackut_float_decode_min_neg_max_pos_zero),
	CUTE_REF(dpackut_float_decode_min_neg_min_pos_zero),
	CUTE_REF(dpackut_float_decode_min_neg_zero_pos_zero),
	CUTE_REF(dpackut_float_decode_min_pos_zero_pos_zero),
	CUTE_REF(dpackut_float_decode_min_pos_min_pos_zero),
	CUTE_REF(dpackut_float_decode_min_pos_max_pos_zero),
	CUTE_REF(dpackut_float_decode_min_pos_inf_pos_zero),
	CUTE_REF(dpackut_float_decode_min_pos_nan_pos_zero),
	CUTE_REF(dpackut_float_decode_min_neg_inf_pos_max),
	CUTE_REF(dpackut_float_decode_min_neg_max_pos_max),
	CUTE_REF(dpackut_float_decode_min_neg_min_pos_max),
	CUTE_REF(dpackut_float_decode_min_neg_zero_pos_max),
	CUTE_REF(dpackut_float_decode_min_pos_zero_pos_max),
	CUTE_REF(dpackut_float_decode_min_pos_min_pos_max),
	CUTE_REF(dpackut_float_decode_min_pos_max_pos_max),
	CUTE_REF(dpackut_float_decode_min_pos_info_pos_max),
	CUTE_REF(dpackut_float_decode_min_pos_zero_pos_zero_flt),

	CUTE_REF(dpackut_float_decode_max_assert),
	CUTE_REF(dpackut_float_decode_max_neg_nan_pos_zero),
	CUTE_REF(dpackut_float_decode_max_neg_inf_neg_max),
	CUTE_REF(dpackut_float_decode_max_neg_max_neg_max),
	CUTE_REF(dpackut_float_decode_max_neg_min_neg_max),
	CUTE_REF(dpackut_float_decode_max_neg_zero_neg_max),
	CUTE_REF(dpackut_float_decode_max_pos_zero_neg_max),
	CUTE_REF(dpackut_float_decode_max_pos_min_neg_max),
	CUTE_REF(dpackut_float_decode_max_pos_max_neg_max),
	CUTE_REF(dpackut_float_decode_max_pos_inf_neg_max),
	CUTE_REF(dpackut_float_decode_max_neg_inf_pos_zero),
	CUTE_REF(dpackut_float_decode_max_neg_max_pos_zero),
	CUTE_REF(dpackut_float_decode_max_neg_min_pos_zero),
	CUTE_REF(dpackut_float_decode_max_neg_zero_pos_zero),
	CUTE_REF(dpackut_float_decode_max_pos_zero_pos_zero),
	CUTE_REF(dpackut_float_decode_max_pos_min_pos_zero),
	CUTE_REF(dpackut_float_decode_max_pos_max_pos_zero),
	CUTE_REF(dpackut_float_decode_max_pos_inf_pos_zero),
	CUTE_REF(dpackut_float_decode_max_pos_nan_pos_zero),
	CUTE_REF(dpackut_float_decode_max_neg_inf_pos_max),
	CUTE_REF(dpackut_float_decode_max_neg_max_pos_max),
	CUTE_REF(dpackut_float_decode_max_neg_min_pos_max),
	CUTE_REF(dpackut_float_decode_max_neg_zero_pos_max),
	CUTE_REF(dpackut_float_decode_max_pos_zero_pos_max),
	CUTE_REF(dpackut_float_decode_max_pos_min_pos_max),
	CUTE_REF(dpackut_float_decode_max_pos_max_pos_max),
	CUTE_REF(dpackut_float_decode_max_pos_inf_pos_max),
	CUTE_REF(dpackut_float_decode_max_pos_nan_pos_zero),
	CUTE_REF(dpackut_float_decode_max_pos_zero_pos_zero_dbl),

	CUTE_REF(dpackut_float_decode_range_assert),
	CUTE_REF(dpackut_float_decode_range_neg_nan_pos_zero_one),
	CUTE_REF(dpackut_float_decode_range_neg_inf_neg_max_pos_zero),
	CUTE_REF(dpackut_float_decode_range_neg_max_neg_max_pos_zero),
	CUTE_REF(dpackut_float_decode_range_neg_min_neg_max_pos_zero),
	CUTE_REF(dpackut_float_decode_range_neg_zero_neg_max_pos_zero),
	CUTE_REF(dpackut_float_decode_range_pos_zero_neg_max_pos_zero),
	CUTE_REF(dpackut_float_decode_range_pos_min_neg_max_pos_zero),
	CUTE_REF(dpackut_float_decode_range_pos_max_neg_max_pos_zero),
	CUTE_REF(dpackut_float_decode_range_pos_inf_neg_max_pos_zero),
	CUTE_REF(dpackut_float_decode_range_neg_inf_pos_zero_pos_max),
	CUTE_REF(dpackut_float_decode_range_neg_max_pos_zero_pos_max),
	CUTE_REF(dpackut_float_decode_range_neg_min_pos_zero_pos_max),
	CUTE_REF(dpackut_float_decode_range_neg_zero_pos_zero_pos_max),
	CUTE_REF(dpackut_float_decode_range_pos_zero_pos_zero_pos_max),
	CUTE_REF(dpackut_float_decode_range_pos_min_pos_zero_pos_max),
	CUTE_REF(dpackut_float_decode_range_pos_max_pos_zero_pos_max),
	CUTE_REF(dpackut_float_decode_range_pos_inf_pos_zero_pos_max),
	CUTE_REF(dpackut_float_decode_range_pos_nan_pos_zero_pos_max),
	CUTE_REF(dpackut_float_decode_range_neg_info_one_two),
	CUTE_REF(dpackut_float_decode_range_neg_max_one_two),
	CUTE_REF(dpackut_float_decode_range_neg_min_one_two),
	CUTE_REF(dpackut_float_decode_range_neg_zero_one_two),
	CUTE_REF(dpackut_float_decode_range_pos_zero_one_two),
	CUTE_REF(dpackut_float_decode_range_one_one_two),
	CUTE_REF(dpackut_float_decode_range_onenhalf_one_two),
	CUTE_REF(dpackut_float_decode_range_two_one_two),
	CUTE_REF(dpackut_float_decode_range_pos_min_one_two),
	CUTE_REF(dpackut_float_decode_range_pos_max_one_two),
	CUTE_REF(dpackut_float_decode_range_pos_inf_one_two),
	CUTE_REF(dpackut_float_decode_range_pos_nan_pos_zero_one),
	CUTE_REF(dpackut_float_decode_range_pos_zero_pos_zero_one)
};

CUTE_SUITE_EXTERN(dpackut_float_suite,
                  dpackut_float_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
