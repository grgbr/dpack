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

#define DPACKUT_DOUBLE(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed    = _packed, \
		.size      = sizeof(_packed) - 1, \
		.error     = _error, \
		.value.f64 = _value \
	}

static void
dpackut_double_encode(const struct dpackut_scalar_data * data)
{
	struct dpack_encoder enc = { 0, };
	size_t               sz = data->size;
	char                 buff[sz];

	memset(buff, 0xa5, sz);
	dpack_encoder_init_buffer(&enc, buff, sz);

	cute_check_uint(data->size, equal, DPACK_DOUBLE_SIZE);
	cute_check_sint(dpack_encode_double(&enc, data->value.f64),
	                equal,
	                data->error);
	cute_check_mem(buff, equal, data->packed, sz);

	cute_check_uint(dpack_encoder_space_used(&enc), equal, sz);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_double_encode_assert)
{
	double               val = NAN;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;
	char                 buff[DPACK_DOUBLE_SIZE];

	cute_expect_assertion(ret = dpack_encode_double(NULL, val));
	cute_expect_assertion(ret = dpack_encode_double(&enc, val));

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_encode_double(&enc, NAN));
	dpack_encoder_fini(&enc);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_double_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_double_encode_neg_inf)
{
	/* -INFINITY: dpack-utest-gen.py "float('-inf')" */
	DPACKUT_DOUBLE(data,
	               "\xcb\xff\xf0\x00\x00\x00\x00\x00\x00",
	               0,
	               -INFINITY);

	dpackut_double_encode(&data);
}

CUTE_TEST(dpackut_double_encode_neg_max)
{
	/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE(data,
	               "\xcb\xff\xef\xff\xff\xff\xff\xff\xff",
	               0,
	               -MAXDOUBLE);

	dpackut_double_encode(&data);
}

CUTE_TEST(dpackut_double_encode_neg_min)
{
	/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE(data,
	               "\xcb\x80\x10\x00\x00\x00\x00\x00\x00",
	               0,
	               -MINDOUBLE);

	dpackut_double_encode(&data);
}

CUTE_TEST(dpackut_double_encode_neg_zero)
{
	/* -0: dpack-utest-gen.py "- 0.0" */
	DPACKUT_DOUBLE(data,
	               "\xcb\x80\x00\x00\x00\x00\x00\x00\x00",
	               0,
	               -0.0);

	dpackut_double_encode(&data);
}

CUTE_TEST(dpackut_double_encode_pos_zero)
{
	/* +0: dpack-utest-gen.py "0.0" */
	DPACKUT_DOUBLE(data,
	               "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	               0,
	               0.0);

	dpackut_double_encode(&data);
}

CUTE_TEST(dpackut_double_encode_pos_min)
{
	/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE(data,
	               "\xcb\x00\x10\x00\x00\x00\x00\x00\x00",
	               0,
	               MINDOUBLE);

	dpackut_double_encode(&data);
}

CUTE_TEST(dpackut_double_encode_pos_max)
{
	/* +MAXDOUBLE: dpack-utest-gen.py "1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE(data,
	               "\xcb\x7f\xef\xff\xff\xff\xff\xff\xff",
	               0,
	               MAXDOUBLE);

	dpackut_double_encode(&data);
}

CUTE_TEST(dpackut_double_encode_pos_inf)
{
	/* +INFINITY: dpack-utest-gen.py "float('inf')" */
	DPACKUT_DOUBLE(data,
	               "\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00",
	               0,
	               INFINITY);

	dpackut_double_encode(&data);
}

static void
dpackut_double_decode(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	double               val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_double(&dec, &val), equal, data->error);
	if (!data->error) {
		cute_check_flt(val, equal, data->value.f64);
		cute_check_uint(data->size, equal, DPACK_DOUBLE_SIZE);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_double_decode_assert)
{
	double               val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;
	char                 buff[DPACK_DOUBLE_SIZE];

	cute_expect_assertion(ret = dpack_decode_double(NULL, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_double(&dec, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_double(&dec, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_double_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_double_decode_neg_nan)
{
	/* -Not A Number: dpack-utest-gen.py "float('-nan')" */
	DPACKUT_DOUBLE(data,
	               "\xcb\xff\xf8\x00\x00\x00\x00\x00\x00",
	               -ENOMSG,
	               -NAN);

	dpackut_double_decode(&data);
}

CUTE_TEST(dpackut_double_decode_neg_inf)
{
	/* -INFINITY: dpack-utest-gen.py "float('-inf')" */
	DPACKUT_DOUBLE(data,
	               "\xcb\xff\xf0\x00\x00\x00\x00\x00\x00",
	               0,
	               -INFINITY);

	dpackut_double_decode(&data);
}

CUTE_TEST(dpackut_double_decode_neg_max)
{
	/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE(data,
	               "\xcb\xff\xef\xff\xff\xff\xff\xff\xff",
	               0,
	               -MAXDOUBLE);

	dpackut_double_decode(&data);
}

CUTE_TEST(dpackut_double_decode_neg_min)
{
	/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE(data,
	               "\xcb\x80\x10\x00\x00\x00\x00\x00\x00",
	               0,
	               -MINDOUBLE);

	dpackut_double_decode(&data);
}

CUTE_TEST(dpackut_double_decode_neg_zero)
{
	/* -0: dpack-utest-gen.py "- 0.0" */
	DPACKUT_DOUBLE(data,
	               "\xcb\x80\x00\x00\x00\x00\x00\x00\x00",
	               0,
	               -0.0);

	dpackut_double_decode(&data);
}

CUTE_TEST(dpackut_double_decode_pos_zero)
{
	/* +0: dpack-utest-gen.py "0.0" */
	DPACKUT_DOUBLE(data,
	               "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	               0,
	               0.0);

	dpackut_double_decode(&data);
}

CUTE_TEST(dpackut_double_decode_pos_min)
{
	/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE(data,
	               "\xcb\x00\x10\x00\x00\x00\x00\x00\x00",
	               0,
	               MINDOUBLE);

	dpackut_double_decode(&data);
}

CUTE_TEST(dpackut_double_decode_pos_max)
{
	/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE(data,
	               "\xcb\x7f\xef\xff\xff\xff\xff\xff\xff",
	               0,
	               MAXDOUBLE);

	dpackut_double_decode(&data);
}

CUTE_TEST(dpackut_double_decode_pos_inf)
{
	/* +INFINITY: dpack-utest-gen.py "float('inf')" */
	DPACKUT_DOUBLE(data,
	               "\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00",
	               0,
	               INFINITY);

	dpackut_double_decode(&data);
}

CUTE_TEST(dpackut_double_decode_pos_nan)
{
	/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
	DPACKUT_DOUBLE(data, "\xca\x7f\xc0\x00\x00", -ENOMSG, NAN);

	dpackut_double_decode(&data);
}

CUTE_TEST(dpackut_double_decode_pos_zero_flt)
{
	/* +0 (float): dpack-utest-gen.py -s "0.0" */
	DPACKUT_DOUBLE(data, "\xca\x00\x00\x00\x00", -ENOMSG, 0);

	dpackut_double_decode(&data);
}

#define DPACKUT_DOUBLE_MIN(_var, _packed, _error, _value, _low) \
	const struct dpackut_scalar_data _var = { \
		.packed    = _packed, \
		.size      = sizeof(_packed) - 1, \
		.error     = _error, \
		.value.f64 = _value, \
		.low.f64   = _low \
	}

static void
dpackut_double_decode_min(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	double               val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_double_min(&dec, data->low.f64, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_flt(val, equal, data->value.f64);
		cute_check_uint(data->size, equal, DPACK_DOUBLE_SIZE);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_double_decode_min_assert)
{
	double               val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_DOUBLE_SIZE];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_double_min(NULL, 1.0, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_double_min(&dec, 1.0, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_double_min(&dec,
	                                                    -INFINITY,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_double_min(&dec,
	                                                    INFINITY,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_double_min(&dec, NAN, &val));
	cute_expect_assertion(ret = dpack_decode_double_min(&dec, -NAN, &val));
	cute_expect_assertion(ret = dpack_decode_double_min(&dec, 1.0, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_double_decode_min_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_double_decode_min_neg_nan_zero)
{
	/* -Not A Number: dpack-utest-gen.py "float('-nan')" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\xff\xf8\x00\x00\x00\x00\x00\x00",
	                   -ENOMSG,
	                   -NAN,
	                   0.0);

	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_neg_inf_neg_max)
{
	/* -INFINITY: dpack-utest-gen.py "float('-inf')" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\xff\xf0\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   -INFINITY,
	                   -MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_neg_max_neg_max)
{
	/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\xff\xef\xff\xff\xff\xff\xff\xff",
	                   0,
	                   -MAXDOUBLE,
	                   -MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_neg_min_neg_max)
{
	/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x80\x10\x00\x00\x00\x00\x00\x00",
	                   0,
	                   -MINDOUBLE,
	                   -MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_neg_zero_neg_max)
{
	/* -0: dpack-utest-gen.py "- 0.0" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x80\x00\x00\x00\x00\x00\x00\x00",
	                   0,
	                   -0.0,
	                   -MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_zero_neg_max)
{
	/* +0: dpack-utest-gen.py "0.0" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	                   0,
	                   0.0,
	                   -MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_min_neg_max)
{
	/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x00\x10\x00\x00\x00\x00\x00\x00",
	                   0,
	                   MINDOUBLE,
	                   -MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_max_neg_max)
{
	/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x7f\xef\xff\xff\xff\xff\xff\xff",
	                   0,
	                   MAXDOUBLE,
	                   -MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_inf_neg_max)
{
	/* +INFINITY: dpack-utest-gen.py "float('inf')" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00",
	                   0,
	                   INFINITY,
	                   -MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_neg_inf_pos_zero)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\xff\xf0\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   -INFINITY,
	                   0.0);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_neg_max_pos_zero)
{
	/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\xff\xef\xff\xff\xff\xff\xff\xff",
	                   -ERANGE,
	                   -MAXDOUBLE,
	                   0.0);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_neg_min_pos_zero)
{
	/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x80\x10\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   -MINDOUBLE,
	                   0.0);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_neg_zero_pos_zero)
{
	/* -0: dpack-utest-gen.py "- 0.0" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x80\x00\x00\x00\x00\x00\x00\x00",
	                   0,
	                   -0.0,
	                   0.0);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_zero_pos_zero)
{
	/* +0: dpack-utest-gen.py "0.0" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	                   0,
	                   0.0,
	                   0.0);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_min_pos_zero)
{
	/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x00\x10\x00\x00\x00\x00\x00\x00",
	                   0,
	                   MINDOUBLE,
	                   0.0);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_max_pos_zero)
{
	/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x7f\xef\xff\xff\xff\xff\xff\xff",
	                   0,
	                   MAXDOUBLE,
	                   0.0);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_inf_pos_zero)
{
	/* +INFINITY: dpack-utest-gen.py "float('inf')" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00",
	                   0,
	                   INFINITY,
	                   0.0);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_nan_pos_zero)
{
	/* +Not A Number: dpack-utest-gen.py "float('nan')" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x7f\xf8\x00\x00\x00\x00\x00\x00",
	                   -ENOMSG,
	                   NAN,
	                   0.0);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_neg_inf_pos_max)
{
	/* -INFINITY: dpack-utest-gen.py "float('-inf')" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\xff\xf0\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   -INFINITY,
	                   MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_neg_max_pos_max)
{
	/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\xff\xef\xff\xff\xff\xff\xff\xff",
	                   -ERANGE,
	                   -MAXDOUBLE,
	                   MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_neg_min_pos_max)
{
	/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x80\x10\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   -MINDOUBLE,
	                   MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_neg_zero_pos_max)
{
	/* -0: dpack-utest-gen.py "- 0.0" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x80\x00\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   -0.0,
	                   MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_zero_pos_max)
{
	/* +0: dpack-utest-gen.py "0.0" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   0.0,
	                   MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_min_pos_max)
{
	/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x00\x10\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   MINDOUBLE,
	                   MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_max_pos_max)
{
	/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x7f\xef\xff\xff\xff\xff\xff\xff",
	                   0,
	                   MAXDOUBLE,
	                   MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_inf_pos_max)
{
	/* +INFINITY: dpack-utest-gen.py "float('inf')" */
	DPACKUT_DOUBLE_MIN(data,
	                   "\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00",
	                   0,
	                   INFINITY,
	                   MAXDOUBLE);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_nan_pos_zero_flt)
{
	/* +Not A Number (float): dpack-utest-gen.py -s "float('nan')" */
	DPACKUT_DOUBLE_MIN(data, "\xca\x7f\xc0\x00\x00", -ENOMSG, NAN, 0.0);
	dpackut_double_decode_min(&data);
}

CUTE_TEST(dpackut_double_decode_min_pos_zero_pos_zero_flt)
{
	/* +0 (float): dpack-utest-gen.py -s "0.0" */
	DPACKUT_DOUBLE_MIN(data, "\xca\x00\x00\x00\x00", -ENOMSG, 0.0, 0.0);
	dpackut_double_decode_min(&data);
}

#define DPACKUT_DOUBLE_MAX(_var, _packed, _error, _value, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.f64  = _value, \
		.high.f64   = _high \
	}

static void
dpackut_double_decode_max(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	double               val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_double_max(&dec, data->high.f64, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_flt(val, equal, data->value.f64);
		cute_check_uint(data->size, equal, DPACK_DOUBLE_SIZE);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_double_decode_max_assert)
{
	double               val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_DOUBLE_SIZE];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_double_max(NULL, 1.0, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_double_max(&dec, 1.0, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_double_max(&dec,
	                                                    -INFINITY,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_double_max(&dec,
	                                                    INFINITY,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_double_max(&dec, NAN, &val));
	cute_expect_assertion(ret = dpack_decode_double_max(&dec, -NAN, &val));
	cute_expect_assertion(ret = dpack_decode_double_max(&dec, 1.0, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_double_decode_max_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_double_decode_max_neg_nan_pos_zero)
{
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\xff\xf8\x00\x00\x00\x00\x00\x00",
	                   -ENOMSG,
	                   -NAN,
	                   0.0);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_neg_inf_neg_max)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\xff\xf0\x00\x00\x00\x00\x00\x00",
	                   0,
	                   -INFINITY,
	                   -MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_neg_max_neg_max)
{
	/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\xff\xef\xff\xff\xff\xff\xff\xff",
	                   0,
	                   -MAXDOUBLE,
	                   -MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_neg_min_neg_max)
{
	/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x80\x10\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   -MINDOUBLE,
	                   -MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_neg_zero_neg_max)
{
	/* -0: dpack-utest-gen.py "- 0.0" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x80\x00\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   -0.0,
	                   -MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_zero_neg_max)
{
	/* +0: dpack-utest-gen.py "0.0" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   0.0,
	                   -MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_min_neg_max)
{
	/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x00\x10\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   MINDOUBLE,
	                   -MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_max_neg_max)
{
	/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x7f\xef\xff\xff\xff\xff\xff\xff",
	                   -ERANGE,
	                   MAXDOUBLE,
	                   -MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_inf_neg_max)
{
	/* +INFINITY: dpack-utest-gen.py "float('inf')" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   INFINITY,
	                   -MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_neg_inf_pos_zero)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\xff\xf0\x00\x00\x00\x00\x00\x00",
	                   0,
	                   -INFINITY,
	                   0.0);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_neg_max_pos_zero)
{
	/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\xff\xef\xff\xff\xff\xff\xff\xff",
	                   0,
	                   -MAXDOUBLE,
	                   0.0);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_neg_min_pos_zero)
{
	/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x80\x10\x00\x00\x00\x00\x00\x00",
	                   0,
	                   -MINDOUBLE,
	                   0.0);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_neg_zero_pos_zero)
{
	/* -0: dpack-utest-gen.py "- 0.0" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x80\x00\x00\x00\x00\x00\x00\x00",
	                   0,
	                   -0.0,
	                   0.0);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_zero_pos_zero)
{
	/* +0: dpack-utest-gen.py "0.0" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	                   0,
	                   0.0,
	                   0.0);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_min_pos_zero)
{
	/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x00\x10\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   MINDOUBLE,
	                   0.0);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_max_pos_zero)
{
	/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x7f\xef\xff\xff\xff\xff\xff\xff",
	                   -ERANGE,
	                   MAXDOUBLE,
	                   0.0);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_inf_pos_zero)
{
	/* +INFINITY: dpack-utest-gen.py "float('inf')" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   INFINITY,
	                   0.0);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_nan_pos_zero)
{
	/* +Not A Number: dpack-utest-gen.py "float('nan')" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x7f\xf8\x00\x00\x00\x00\x00\x00",
	                   -ENOMSG,
	                   NAN,
	                   0.0);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_neg_info_pos_max)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\xff\xf0\x00\x00\x00\x00\x00\x00",
	                   0,
	                   -INFINITY,
	                   MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_neg_max_pos_max)
{
	/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\xff\xef\xff\xff\xff\xff\xff\xff",
	                   0,
	                   -MAXDOUBLE,
	                   MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_neg_min_pos_max)
{
	/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x80\x10\x00\x00\x00\x00\x00\x00",
	                   0,
	                   -MINDOUBLE,
	                   MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_neg_zero_pos_max)
{
	/* -0: dpack-utest-gen.py "- 0.0" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x80\x00\x00\x00\x00\x00\x00\x00",
	                   0,
	                   -0.0,
	                   MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_zero_pos_max)
{
	/* +0: dpack-utest-gen.py "0.0" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	                   0,
	                   0.0,
	                   MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_min_pos_max)
{
	/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x00\x10\x00\x00\x00\x00\x00\x00",
	                   0,
	                   MINDOUBLE,
	                   MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_max_pos_max)
{
	/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x7f\xef\xff\xff\xff\xff\xff\xff",
	                   0,
	                   MAXDOUBLE,
	                   MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_inf_pos_max)
{
	/* +INFINITY: dpack-utest-gen.py "float('inf')" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00",
	                   -ERANGE,
	                   INFINITY,
	                   MAXDOUBLE);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_nan_pos_zero_flt)
{
	/* +Not A Number (float): dpack-utest-gen.py -s "float('nan')" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xca\x7f\xc0\x00\x00",
	                   -ENOMSG,
	                   NAN,
	                   0.0);
	dpackut_double_decode_max(&data);
}

CUTE_TEST(dpackut_double_decode_max_pos_zero_pos_zero_flt)
{
	/* +0 (float): dpack-utest-gen.py -s "0.0" */
	DPACKUT_DOUBLE_MAX(data,
	                   "\xca\x00\x00\x00\x00",
	                   -ENOMSG,
	                   0.0,
	                   0.0);
	dpackut_double_decode_max(&data);
}

#define DPACKUT_DOUBLE_RANGE(_var, _packed, _error, _value, _low, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.f64  = _value, \
		.low.f64    = _low, \
		.high.f64   = _high \
	}

static void
dpackut_double_decode_range(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	double               val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_double_range(&dec,
	                                          data->low.f64,
	                                          data->high.f64,
	                                          &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_flt(val, equal, data->value.f64);
		cute_check_uint(data->size, equal, DPACK_DOUBLE_SIZE);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_double_decode_range_assert)
{
	double               val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_DOUBLE_SIZE];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_double_range(NULL,
	                                                      1.0,
	                                                      2.0,
	                                                      &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_double_range(&dec,
	                                                      1.0,
	                                                      2.0,
	                                                      &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_double_range(&dec,
	                                                      -INFINITY,
	                                                      2.0,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_double_range(&dec,
	                                                      1.0,
	                                                      INFINITY,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_double_range(&dec,
	                                                      NAN,
	                                                      2.0,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_double_range(&dec,
	                                                      -NAN,
	                                                      2.0,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_double_range(&dec,
	                                                      1.0,
	                                                      NAN,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_double_range(&dec,
	                                                      1.0,
	                                                      -NAN,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_double_range(&dec,
	                                                      1.0,
	                                                      2.0,
	                                                      NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_double_decode_range_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_double_decode_range_neg_nan_pos_zero_one)
{
	/* -Not A Number: dpack-utest-gen.py "float('-nan')" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\xff\xf8\x00\x00\x00\x00\x00\x00",
	                     -ENOMSG,
	                     -NAN,
	                     0.0,
	                     1.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_neg_inf_neg_max_pos_zero)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\xff\xf0\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     -INFINITY,
	                     -MAXDOUBLE,
	                     0.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_neg_max_neg_max_pos_zero)
{
	/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\xff\xef\xff\xff\xff\xff\xff\xff",
	                     0,
	                     -MAXDOUBLE,
	                     -MAXDOUBLE,
	                     0.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_neg_min_neg_max_pos_zero)
{
	/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x80\x10\x00\x00\x00\x00\x00\x00",
	                     0,
	                     -MINDOUBLE,
	                     -MAXDOUBLE,
	                     0.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_neg_zero_neg_max_pos_zero)
{
	/* -0: dpack-utest-gen.py "- 0.0" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x80\x00\x00\x00\x00\x00\x00\x00",
	                     0,
	                     -0.0,
	                     -MAXDOUBLE,
	                     0.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_zero_neg_max_pos_zero)
{
	/* +0: dpack-utest-gen.py "0.0" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	                     0,
	                     0.0,
	                     -MAXDOUBLE,
	                     0.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_min_neg_max_pos_zero)
{
	/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x00\x10\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     MINDOUBLE,
	                     -MAXDOUBLE,
	                     0.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_max_neg_max_pos_zero)
{
	/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x7f\xef\xff\xff\xff\xff\xff\xff",
	                     -ERANGE,
	                     MAXDOUBLE,
	                     -MAXDOUBLE,
	                     0.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_inf_neg_max_pos_zero)
{
	/* +INFINITY: dpack-utest-gen.py "float('inf')" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     INFINITY,
	                     -MAXDOUBLE,
	                     0.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_neg_inf_pos_zero_pos_max)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\xff\xf0\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     -INFINITY,
	                     0.0,
	                     MAXDOUBLE);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_neg_max_pos_zero_pos_max)
{
	/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\xff\xef\xff\xff\xff\xff\xff\xff",
	                     -ERANGE,
	                     -MAXDOUBLE,
	                     0.0,
	                     MAXDOUBLE);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_neg_min_pos_zero_pos_max)
{
	/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x80\x10\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     -MINDOUBLE,
	                     0.0,
	                     MAXDOUBLE);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_neg_zero_pos_zero_pos_max)
{
	/* -0: dpack-utest-gen.py "- 0.0" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x80\x00\x00\x00\x00\x00\x00\x00",
	                     0,
	                     -0.0,
	                     0.0,
	                     MAXDOUBLE);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_zero_pos_zero_pos_max)
{
	/* +0: dpack-utest-gen.py "0.0" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	                     0,
	                     0.0,
	                     0.0,
	                     MAXDOUBLE);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_min_pos_zero_pos_max)
{
	/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x00\x10\x00\x00\x00\x00\x00\x00",
	                     0,
	                     MINDOUBLE,
	                     0.0,
	                     MAXDOUBLE);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_max_pos_zero_pos_max)
{
	/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x7f\xef\xff\xff\xff\xff\xff\xff",
	                     0,
	                     MAXDOUBLE,
	                     0.0,
	                     MAXDOUBLE);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_info_pos_zero_pos_max)
{
	/* +INFINITY: dpack-utest-gen.py "float('inf')" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     INFINITY,
	                     0.0,
	                     MAXDOUBLE);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_nan_pos_zero_pos_max)
{
	/* +Not A Number: dpack-utest-gen.py "float('nan')" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x7f\xf8\x00\x00\x00\x00\x00\x00",
	                     -ENOMSG,
	                     NAN,
	                     0.0,
	                     MAXDOUBLE);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_neg_inf_one_two)
{
	/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\xff\xf0\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     -INFINITY,
	                     1.0,
	                     2.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_neg_max_one_two)
{
	/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\xff\xef\xff\xff\xff\xff\xff\xff",
	                     -ERANGE,
	                     -MAXDOUBLE,
	                     1.0,
	                     2.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_neg_min_one_two)
{
	/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x80\x10\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     -MINDOUBLE,
	                     1.0,
	                     2.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_neg_zero_one_two)
{
	/* -0: dpack-utest-gen.py "- 0.0" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x80\x00\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     -0.0,
	                     1.0,
	                     2.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_zero_one_two)
{
	/* +0: dpack-utest-gen.py "0.0" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x00\x00\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     0.0,
	                     1.0,
	                     2.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_one_one_two)
{
	/* +1.0: dpack-utest-gen.py "1.0" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x3f\xf0\x00\x00\x00\x00\x00\x00",
	                     0,
	                     1.0,
	                     1.0,
	                     2.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_onenhalf_one_two)
{
	/* +1.5: dpack-utest-gen.py "1.5" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x3f\xf8\x00\x00\x00\x00\x00\x00",
	                     0,
	                     1.5f,
	                     1.0,
	                     2.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_two_one_two)
{
	/* +2.0: dpack-utest-gen.py "2.0" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x40\x00\x00\x00\x00\x00\x00\x00",
	                     0,
	                     2.0,
	                     1.0,
	                     2.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_min_one_two)
{
	/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x00\x10\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     MINDOUBLE,
	                     1.0,
	                     2.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_max_one_two)
{
	/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x7f\xef\xff\xff\xff\xff\xff\xff",
	                     -ERANGE,
	                     MAXDOUBLE,
	                     1.0,
	                     2.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_inf_one_two)
{
	/* +INFINITY: dpack-utest-gen.py "float('inf')" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     INFINITY,
	                     1.0,
	                     2.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_nan_pos_zero_one)
{
	/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xca\x7f\xc0\x00\x00",
	                     -ENOMSG,
	                     NAN,
	                     0.0,
	                     1.0);
	dpackut_double_decode_range(&data);
}

CUTE_TEST(dpackut_double_decode_range_pos_zero_pos_zero_one)
{
	/* +0 (float): dpack-utest-gen.py "0.0" */
	DPACKUT_DOUBLE_RANGE(data,
	                     "\xca\x00\x00\x00\x00",
	                     -ENOMSG,
	                     0.0,
	                     0.0,
	                     1.0);
	dpackut_double_decode_range(&data);
}

CUTE_GROUP(dpackut_double_group) = {
	CUTE_REF(dpackut_double_encode_assert),
	CUTE_REF(dpackut_double_encode_neg_inf),
	CUTE_REF(dpackut_double_encode_neg_max),
	CUTE_REF(dpackut_double_encode_neg_min),
	CUTE_REF(dpackut_double_encode_neg_zero),
	CUTE_REF(dpackut_double_encode_pos_zero),
	CUTE_REF(dpackut_double_encode_pos_min),
	CUTE_REF(dpackut_double_encode_pos_max),
	CUTE_REF(dpackut_double_encode_pos_inf),

	CUTE_REF(dpackut_double_decode_assert),
	CUTE_REF(dpackut_double_decode_neg_nan),
	CUTE_REF(dpackut_double_decode_neg_inf),
	CUTE_REF(dpackut_double_decode_neg_max),
	CUTE_REF(dpackut_double_decode_neg_min),
	CUTE_REF(dpackut_double_decode_neg_zero),
	CUTE_REF(dpackut_double_decode_pos_zero),
	CUTE_REF(dpackut_double_decode_pos_min),
	CUTE_REF(dpackut_double_decode_pos_max),
	CUTE_REF(dpackut_double_decode_pos_inf),
	CUTE_REF(dpackut_double_decode_pos_nan),
	CUTE_REF(dpackut_double_decode_pos_zero_flt),

	CUTE_REF(dpackut_double_decode_min_assert),
	CUTE_REF(dpackut_double_decode_min_neg_nan_zero),
	CUTE_REF(dpackut_double_decode_min_neg_inf_neg_max),
	CUTE_REF(dpackut_double_decode_min_neg_max_neg_max),
	CUTE_REF(dpackut_double_decode_min_neg_min_neg_max),
	CUTE_REF(dpackut_double_decode_min_neg_zero_neg_max),
	CUTE_REF(dpackut_double_decode_min_pos_zero_neg_max),
	CUTE_REF(dpackut_double_decode_min_pos_min_neg_max),
	CUTE_REF(dpackut_double_decode_min_pos_max_neg_max),
	CUTE_REF(dpackut_double_decode_min_pos_inf_neg_max),
	CUTE_REF(dpackut_double_decode_min_neg_inf_pos_zero),
	CUTE_REF(dpackut_double_decode_min_neg_max_pos_zero),
	CUTE_REF(dpackut_double_decode_min_neg_min_pos_zero),
	CUTE_REF(dpackut_double_decode_min_neg_zero_pos_zero),
	CUTE_REF(dpackut_double_decode_min_pos_zero_pos_zero),
	CUTE_REF(dpackut_double_decode_min_pos_min_pos_zero),
	CUTE_REF(dpackut_double_decode_min_pos_max_pos_zero),
	CUTE_REF(dpackut_double_decode_min_pos_inf_pos_zero),
	CUTE_REF(dpackut_double_decode_min_pos_nan_pos_zero),
	CUTE_REF(dpackut_double_decode_min_neg_inf_pos_max),
	CUTE_REF(dpackut_double_decode_min_neg_max_pos_max),
	CUTE_REF(dpackut_double_decode_min_neg_min_pos_max),
	CUTE_REF(dpackut_double_decode_min_neg_zero_pos_max),
	CUTE_REF(dpackut_double_decode_min_pos_zero_pos_max),
	CUTE_REF(dpackut_double_decode_min_pos_min_pos_max),
	CUTE_REF(dpackut_double_decode_min_pos_max_pos_max),
	CUTE_REF(dpackut_double_decode_min_pos_inf_pos_max),
	CUTE_REF(dpackut_double_decode_min_pos_nan_pos_zero_flt),
	CUTE_REF(dpackut_double_decode_min_pos_zero_pos_zero_flt),

	CUTE_REF(dpackut_double_decode_max_assert),
	CUTE_REF(dpackut_double_decode_max_neg_nan_pos_zero),
	CUTE_REF(dpackut_double_decode_max_neg_nan_pos_zero),
	CUTE_REF(dpackut_double_decode_max_neg_inf_neg_max),
	CUTE_REF(dpackut_double_decode_max_neg_max_neg_max),
	CUTE_REF(dpackut_double_decode_max_neg_min_neg_max),
	CUTE_REF(dpackut_double_decode_max_neg_zero_neg_max),
	CUTE_REF(dpackut_double_decode_max_pos_zero_neg_max),
	CUTE_REF(dpackut_double_decode_max_pos_min_neg_max),
	CUTE_REF(dpackut_double_decode_max_pos_max_neg_max),
	CUTE_REF(dpackut_double_decode_max_pos_inf_neg_max),
	CUTE_REF(dpackut_double_decode_max_neg_inf_pos_zero),
	CUTE_REF(dpackut_double_decode_max_neg_max_pos_zero),
	CUTE_REF(dpackut_double_decode_max_neg_min_pos_zero),
	CUTE_REF(dpackut_double_decode_max_neg_zero_pos_zero),
	CUTE_REF(dpackut_double_decode_max_pos_zero_pos_zero),
	CUTE_REF(dpackut_double_decode_max_pos_min_pos_zero),
	CUTE_REF(dpackut_double_decode_max_pos_max_pos_zero),
	CUTE_REF(dpackut_double_decode_max_pos_inf_pos_zero),
	CUTE_REF(dpackut_double_decode_max_pos_nan_pos_zero),
	CUTE_REF(dpackut_double_decode_max_neg_info_pos_max),
	CUTE_REF(dpackut_double_decode_max_neg_max_pos_max),
	CUTE_REF(dpackut_double_decode_max_neg_min_pos_max),
	CUTE_REF(dpackut_double_decode_max_neg_zero_pos_max),
	CUTE_REF(dpackut_double_decode_max_pos_zero_pos_max),
	CUTE_REF(dpackut_double_decode_max_pos_min_pos_max),
	CUTE_REF(dpackut_double_decode_max_pos_max_pos_max),
	CUTE_REF(dpackut_double_decode_max_pos_inf_pos_max),
	CUTE_REF(dpackut_double_decode_max_pos_nan_pos_zero_flt),
	CUTE_REF(dpackut_double_decode_max_pos_zero_pos_zero_flt),

	CUTE_REF(dpackut_double_decode_range_assert),
	CUTE_REF(dpackut_double_decode_range_neg_nan_pos_zero_one),
	CUTE_REF(dpackut_double_decode_range_neg_inf_neg_max_pos_zero),
	CUTE_REF(dpackut_double_decode_range_neg_max_neg_max_pos_zero),
	CUTE_REF(dpackut_double_decode_range_neg_min_neg_max_pos_zero),
	CUTE_REF(dpackut_double_decode_range_neg_zero_neg_max_pos_zero),
	CUTE_REF(dpackut_double_decode_range_pos_zero_neg_max_pos_zero),
	CUTE_REF(dpackut_double_decode_range_pos_min_neg_max_pos_zero),
	CUTE_REF(dpackut_double_decode_range_pos_max_neg_max_pos_zero),
	CUTE_REF(dpackut_double_decode_range_pos_inf_neg_max_pos_zero),
	CUTE_REF(dpackut_double_decode_range_neg_inf_pos_zero_pos_max),
	CUTE_REF(dpackut_double_decode_range_neg_max_pos_zero_pos_max),
	CUTE_REF(dpackut_double_decode_range_neg_min_pos_zero_pos_max),
	CUTE_REF(dpackut_double_decode_range_neg_zero_pos_zero_pos_max),
	CUTE_REF(dpackut_double_decode_range_pos_zero_pos_zero_pos_max),
	CUTE_REF(dpackut_double_decode_range_pos_min_pos_zero_pos_max),
	CUTE_REF(dpackut_double_decode_range_pos_max_pos_zero_pos_max),
	CUTE_REF(dpackut_double_decode_range_pos_info_pos_zero_pos_max),
	CUTE_REF(dpackut_double_decode_range_pos_nan_pos_zero_pos_max),
	CUTE_REF(dpackut_double_decode_range_neg_inf_one_two),
	CUTE_REF(dpackut_double_decode_range_neg_max_one_two),
	CUTE_REF(dpackut_double_decode_range_neg_min_one_two),
	CUTE_REF(dpackut_double_decode_range_neg_zero_one_two),
	CUTE_REF(dpackut_double_decode_range_pos_zero_one_two),
	CUTE_REF(dpackut_double_decode_range_one_one_two),
	CUTE_REF(dpackut_double_decode_range_onenhalf_one_two),
	CUTE_REF(dpackut_double_decode_range_two_one_two),
	CUTE_REF(dpackut_double_decode_range_pos_min_one_two),
	CUTE_REF(dpackut_double_decode_range_pos_max_one_two),
	CUTE_REF(dpackut_double_decode_range_pos_inf_one_two),
	CUTE_REF(dpackut_double_decode_range_pos_nan_pos_zero_one),
	CUTE_REF(dpackut_double_decode_range_pos_zero_pos_zero_one)
};

CUTE_SUITE_EXTERN(dpackut_double_suite,
                  dpackut_double_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
