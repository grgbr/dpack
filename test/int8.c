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
#include <errno.h>

#define DPACKUT_INT8(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.int8 = _value \
	}

static void
dpackut_int8_encode(const struct dpackut_scalar_data * data)
{
	struct dpack_encoder enc = { 0, };
	size_t               sz = data->size;
	char                 buff[sz];

	memset(buff, 0xa5, sz);
	dpack_encoder_init_buffer(&enc, buff, sz);

	cute_check_uint(data->size, greater_equal, DPACK_INT8_SIZE_MIN);
	cute_check_uint(data->size, lower_equal, DPACK_INT8_SIZE_MAX);

	cute_check_sint(dpack_encode_int8(&enc, data->value.int8),
	                equal,
	                data->error);
	cute_check_mem(buff, equal, data->packed, sz);

	cute_check_uint(dpack_encoder_space_used(&enc), equal, sz);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int8_encode_assert)
{
	int8_t               val = false;
	int                  ret __unused;
#if defined(CONFIG_DPACK_DEBUG)
	struct dpack_encoder enc = { 0, };

	cute_expect_assertion(ret = dpack_encode_int8(&enc, val));
#endif /* defined(CONFIG_DPACK_DEBUG) */
	cute_expect_assertion(ret = dpack_encode_int8(NULL, val));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_encode__128)
{
	/* -128 */
	DPACKUT_INT8(data, "\xd0\x80", 0, INT8_C(-128));
	dpackut_int8_encode(&data);
}

CUTE_TEST(dpackut_int8_encode__127)
{
	/* -127 */
	DPACKUT_INT8(data, "\xd0\x81", 0, INT8_C(-127));
	dpackut_int8_encode(&data);
}

CUTE_TEST(dpackut_int8_encode__1)
{
	/* -1 */
	DPACKUT_INT8(data, "\xff", 0, INT8_C(-1));
	dpackut_int8_encode(&data);
}

CUTE_TEST(dpackut_int8_encode_0)
{
	/* 0 */
	DPACKUT_INT8(data, "\x00", 0, INT8_C(0));
	dpackut_int8_encode(&data);
}

CUTE_TEST(dpackut_int8_encode_1)
{
	/* 1 */
	DPACKUT_INT8(data, "\x01", 0, INT8_C(1));
	dpackut_int8_encode(&data);
}

CUTE_TEST(dpackut_int8_encode_127)
{
	/* 127 */
	DPACKUT_INT8(data, "\x7f", 0, INT8_C(127));
	dpackut_int8_encode(&data);
}

static void
dpackut_int8_decode(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int8_t               val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int8(&dec, &val), equal, data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int8);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT8_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT8_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int8_decode_assert)
{
	int8_t               val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT8_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int8(NULL, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_int8(&dec, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int8(&dec, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_decode__129)
{
	/* -129 */
	DPACKUT_INT8(data, "\xd1\xff\x7f", -ERANGE, 0);
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode__128)
{
	/* -128 */
	DPACKUT_INT8(data, "\xd0\x80", 0, INT8_C(-128));
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode__127)
{
	/* -127 */
	DPACKUT_INT8(data, "\xd0\x81", 0, INT8_C(-127));
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode__1)
{
	/* -1 */
	DPACKUT_INT8(data, "\xff", 0, INT8_C(-1));
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode_0)
{
	/* 0 */
	DPACKUT_INT8(data, "\x00", 0, INT8_C(0));
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode_1)
{
	/* 1 */
	DPACKUT_INT8(data, "\x01", 0, INT8_C(1));
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode_127)
{
	/* 127 */
	DPACKUT_INT8(data, "\x7f", 0, INT8_C(127));
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode_128)
{
	/* 128 */
	DPACKUT_INT8(data, "\xcc\x80", -ERANGE, 0);
	dpackut_int8_decode(&data);
}

#define DPACKUT_INT8_MIN(_var, _packed, _error, _value, _low) \
	const struct dpackut_scalar_data _var = { \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.int8 = _value, \
		.low.int8   = _low \
	}

static void
dpackut_int8_decode_min(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int8_t               val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int8_min(&dec, data->low.int8, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int8);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT8_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT8_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if  defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int8_decode_min_assert)
{
	int8_t               val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT8_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int8_min(NULL, 1, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_int8_min(&dec, 1, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */
	cute_expect_assertion(ret = dpack_decode_int8_min(&dec,
	                                                  INT8_MIN,
	                                                  &val));
	cute_expect_assertion(ret = dpack_decode_int8_min(&dec,
	                                                  INT8_MAX,
	                                                  &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int8_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_decode_min_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_decode_min__129_0)
{
	/* -129 */
	DPACKUT_INT8_MIN(data, "\xd1\xff\x7f", -ERANGE, 0, INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min__128_0)
{
	/* -128 */
	DPACKUT_INT8_MIN(data, "\xd0\x80", -ERANGE, 0, INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min__127__127)
{
	/* -127 */
	DPACKUT_INT8_MIN(data, "\xd0\x81", 0, INT8_C(-127), INT8_C(-127));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min__126__127)
{
	/* -126 */
	DPACKUT_INT8_MIN(data, "\xd0\x82", 0, INT8_C(-126), INT8_C(-127));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min__1_0)
{
	/* -1 */
	DPACKUT_INT8_MIN(data, "\xff", -ERANGE, 0, INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_0_0)
{
	/* 0 */
	DPACKUT_INT8_MIN(data, "\x00", 0, INT8_C(0), INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_1_0)
{
	/* 1 */
	DPACKUT_INT8_MIN(data, "\x01", 0, INT8_C(1), INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_127_0)
{
	/* 127 */
	DPACKUT_INT8_MIN(data, "\x7f", 0, INT8_C(127), INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_128_0)
{
	/* 128 */
	DPACKUT_INT8_MIN(data, "\xcc\x80", -ERANGE, 0, INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_125_126)
{
	/* 125 */
	DPACKUT_INT8_MIN(data, "\x7d", -ERANGE, 0, INT8_C(126));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_126_126)
{
	/* 126 */
	DPACKUT_INT8_MIN(data, "\x7e", 0, INT8_C(126), INT8_C(126));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_127_126)
{
	/* 127 */
	DPACKUT_INT8_MIN(data, "\x7f", 0, INT8_C(127), INT8_C(126));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_128_128)
{
	/* 128 */
	DPACKUT_INT8_MIN(data, "\xcc\x80", -ERANGE, 0, INT8_C(126));
	dpackut_int8_decode_min(&data);
}

#define DPACKUT_INT8_MAX(_var, _packed, _error, _value, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.int8 = _value, \
		.high.int8  = _high \
	}

static void
dpackut_int8_decode_max(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int8_t               val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int8_max(&dec, data->high.int8, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int8);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT8_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT8_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if  defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int8_decode_max_assert)
{
	int8_t               val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT8_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int8_max(NULL, 1, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_int8_max(&dec, 1, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int8_max(&dec,
	                                                  INT8_MIN,
	                                                  &val));
	cute_expect_assertion(ret = dpack_decode_int8_max(&dec,
	                                                  INT8_MAX,
	                                                  &val));
	cute_expect_assertion(ret = dpack_decode_int8_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_decode_max_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_decode_max__129_0)
{
	/* -129 */
	DPACKUT_INT8_MAX(data, "\xd1\xff\x7f", -ERANGE, 0, INT8_C(0));
	dpackut_int8_decode_max(&data);
}

CUTE_TEST(dpackut_int8_decode_max__128_0)
{
	/* -128 */
	DPACKUT_INT8_MAX(data, "\xd0\x80", 0, INT8_C(-128), INT8_C(0));
	dpackut_int8_decode_max(&data);
}

CUTE_TEST(dpackut_int8_decode_max__127__127)
{
	/* -127 */
	DPACKUT_INT8_MAX(data, "\xd0\x81", 0, INT8_C(-127), INT8_C(-127));
	dpackut_int8_decode_max(&data);
}

CUTE_TEST(dpackut_int8_decode_max__126__127)
{
	/* -126 */
	DPACKUT_INT8_MAX(data, "\xd0\x82", -ERANGE, 0, INT8_C(-127));
	dpackut_int8_decode_max(&data);
}

CUTE_TEST(dpackut_int8_decode_max__1_0)
{
	/* -1 */
	DPACKUT_INT8_MAX(data, "\xff", 0, INT8_C(-1), INT8_C(0));
	dpackut_int8_decode_max(&data);
}

CUTE_TEST(dpackut_int8_decode_max_0_0)
{
	/* 0 */
	DPACKUT_INT8_MAX(data, "\x00", 0, INT8_C(0), INT8_C(0));
	dpackut_int8_decode_max(&data);
}

CUTE_TEST(dpackut_int8_decode_max_1_0)
{
	/* 1 */
	DPACKUT_INT8_MAX(data, "\x01", -ERANGE, 0, INT8_C(0));
	dpackut_int8_decode_max(&data);
}

CUTE_TEST(dpackut_int8_decode_max_127_0)
{
	/* 127 */
	DPACKUT_INT8_MAX(data, "\x7f", -ERANGE, 0, INT8_C(0));
	dpackut_int8_decode_max(&data);
}

CUTE_TEST(dpackut_int8_decode_max_128_0)
{
	/* 128 */
	DPACKUT_INT8_MAX(data, "\xcc\x80", -ERANGE, 0, INT8_C(0));
	dpackut_int8_decode_max(&data);
}

CUTE_TEST(dpackut_int8_decode_max_125_126)
{
	/* 125 */
	DPACKUT_INT8_MAX(data, "\x7d", 0, INT8_C(125), INT8_C(126));
	dpackut_int8_decode_max(&data);
}

CUTE_TEST(dpackut_int8_decode_max_126_126)
{
	/* 126 */
	DPACKUT_INT8_MAX(data, "\x7e", 0, INT8_C(126), INT8_C(126));
	dpackut_int8_decode_max(&data);
}

CUTE_TEST(dpackut_int8_decode_max_127_126)
{
	/* 127 */
	DPACKUT_INT8_MAX(data, "\x7f", -ERANGE, 0, INT8_C(126));
	dpackut_int8_decode_max(&data);
}

CUTE_TEST(dpackut_int8_decode_max_128_126)
{
	/* 128 */
	DPACKUT_INT8_MAX(data, "\xcc\x80", -ERANGE, 0, INT8_C(126));
	dpackut_int8_decode_max(&data);
}

#define DPACKUT_INT8_RANGE(_var, _packed, _error, _value, _low, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.int8 = _value, \
		.low.int8   = _low, \
		.high.int8  = _high \
	}

static void
dpackut_int8_decode_range(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int8_t               val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int8_range(&dec,
	                                        data->low.int8,
	                                        data->high.int8,
	                                        &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int8);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT8_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT8_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if  defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int8_decode_range_assert)
{
	int8_t               val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT8_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int8_range(NULL, 1, 2, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_int8_range(&dec, 1, 2, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int8_range(&dec,
	                                                    INT8_MIN,
	                                                    2,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_int8_range(&dec,
	                                                    1,
	                                                    INT8_MAX,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_int8_range(&dec, 2, 2, &val));
	cute_expect_assertion(ret = dpack_decode_int8_range(&dec, 1, 2, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_decode_range_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_decode_range__129__127__126)
{
	/* -129 */
	DPACKUT_INT8_RANGE(data,
	                   "\xd1\xff\x7f",
	                   -ERANGE,
	                   0,
	                   INT8_MIN + 1,
	                   INT8_MIN + 2);
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range__128__127__126)
{
	/* -128 */
	DPACKUT_INT8_RANGE(data,
	                   "\xd0\x80",
	                   -ERANGE,
	                   0,
	                   INT8_MIN + 1,
	                   INT8_MIN + 2);
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range__127__127__126)
{
	/* -127 */
	DPACKUT_INT8_RANGE(data,
	                   "\xd0\x81",
	                   0,
	                   INT8_MIN + 1,
	                   INT8_MIN + 1,
	                   INT8_MIN + 2);
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range__126__127__126)
{
	/* -126 */
	DPACKUT_INT8_RANGE(data,
	                   "\xd0\x82",
	                   0,
	                   INT8_MIN + 2,
	                   INT8_MIN + 1,
	                   INT8_MIN + 2);
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range__125__127__126)
{
	/* -125 */
	DPACKUT_INT8_RANGE(data,
	                   "\xd0\x83",
	                   -ERANGE,
	                   0,
	                   INT8_MIN + 1,
	                   INT8_MIN + 2);
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range__1_0_1)
{
	/* -1 */
	DPACKUT_INT8_RANGE(data,
	                   "\xff",
	                   -ERANGE,
	                   0,
	                   INT8_C(0),
	                   INT8_C(1));
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range_0_0_1)
{
	/* 0 */
	DPACKUT_INT8_RANGE(data,
	                   "\x00",
	                   0,
	                   INT8_C(0),
	                   INT8_C(0),
	                   INT8_C(1));
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range_1_0_1)
{
	/* 1 */
	DPACKUT_INT8_RANGE(data,
	                   "\x01",
	                   0,
	                   INT8_C(1),
	                   INT8_C(0),
	                   INT8_C(1));
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range_2_0_1)
{
	/* 2 */
	DPACKUT_INT8_RANGE(data,
	                   "\x02",
	                   -ERANGE,
	                   0,
	                   INT8_C(0),
	                   INT8_C(1));
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range_124_125_126)
{
	/* 124 */
	DPACKUT_INT8_RANGE(data,
	                   "\x7c",
	                   -ERANGE,
	                   0,
	                   INT8_MAX - 2,
	                   INT8_MAX - 1);
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range_125_125_126)
{
	/* 125 */
	DPACKUT_INT8_RANGE(data,
	                   "\x7d",
	                   0,
	                   INT8_MAX - 2,
	                   INT8_MAX - 2,
	                   INT8_MAX - 1);
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range_126_125_126)
{
	/* 126 */
	DPACKUT_INT8_RANGE(data,
	                   "\x7e",
	                   0,
	                   INT8_MAX - 1,
	                   INT8_MAX - 2,
	                   INT8_MAX - 1);
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range_127_125_126)
{
	/* 127 */
	DPACKUT_INT8_RANGE(data,
	                   "\x7f",
	                   -ERANGE,
	                   0,
	                   INT8_MAX - 2,
	                   INT8_MAX - 1);
	dpackut_int8_decode_range(&data);
}

CUTE_TEST(dpackut_int8_decode_range_128_125_126)
{
	/* 128 */
	DPACKUT_INT8_RANGE(data,
	                   "\xcc\x80",
	                   -ERANGE,
	                   0,
	                   INT8_MAX - 2,
	                   INT8_MAX - 1);
	dpackut_int8_decode_range(&data);
}

CUTE_GROUP(dpackut_int8_group) = {
	CUTE_REF(dpackut_int8_encode_assert),
	CUTE_REF(dpackut_int8_encode__128),
	CUTE_REF(dpackut_int8_encode__127),
	CUTE_REF(dpackut_int8_encode__1),
	CUTE_REF(dpackut_int8_encode_0),
	CUTE_REF(dpackut_int8_encode_1),
	CUTE_REF(dpackut_int8_encode_127),

	CUTE_REF(dpackut_int8_decode_assert),
	CUTE_REF(dpackut_int8_decode__129),
	CUTE_REF(dpackut_int8_decode__128),
	CUTE_REF(dpackut_int8_decode__127),
	CUTE_REF(dpackut_int8_decode__1),
	CUTE_REF(dpackut_int8_decode_0),
	CUTE_REF(dpackut_int8_decode_1),
	CUTE_REF(dpackut_int8_decode_127),
	CUTE_REF(dpackut_int8_decode_128),

	CUTE_REF(dpackut_int8_decode_min_assert),
	CUTE_REF(dpackut_int8_decode_min__129_0),
	CUTE_REF(dpackut_int8_decode_min__128_0),
	CUTE_REF(dpackut_int8_decode_min__127__127),
	CUTE_REF(dpackut_int8_decode_min__126__127),
	CUTE_REF(dpackut_int8_decode_min__1_0),
	CUTE_REF(dpackut_int8_decode_min_0_0),
	CUTE_REF(dpackut_int8_decode_min_1_0),
	CUTE_REF(dpackut_int8_decode_min_127_0),
	CUTE_REF(dpackut_int8_decode_min_128_0),
	CUTE_REF(dpackut_int8_decode_min_125_126),
	CUTE_REF(dpackut_int8_decode_min_126_126),
	CUTE_REF(dpackut_int8_decode_min_127_126),
	CUTE_REF(dpackut_int8_decode_min_128_128),

	CUTE_REF(dpackut_int8_decode_max_assert),
	CUTE_REF(dpackut_int8_decode_max__129_0),
	CUTE_REF(dpackut_int8_decode_max__128_0),
	CUTE_REF(dpackut_int8_decode_max__127__127),
	CUTE_REF(dpackut_int8_decode_max__126__127),
	CUTE_REF(dpackut_int8_decode_max__1_0),
	CUTE_REF(dpackut_int8_decode_max_0_0),
	CUTE_REF(dpackut_int8_decode_max_1_0),
	CUTE_REF(dpackut_int8_decode_max_127_0),
	CUTE_REF(dpackut_int8_decode_max_128_0),
	CUTE_REF(dpackut_int8_decode_max_125_126),
	CUTE_REF(dpackut_int8_decode_max_126_126),
	CUTE_REF(dpackut_int8_decode_max_127_126),
	CUTE_REF(dpackut_int8_decode_max_128_126),

	CUTE_REF(dpackut_int8_decode_range_assert),
	CUTE_REF(dpackut_int8_decode_range__129__127__126),
	CUTE_REF(dpackut_int8_decode_range__128__127__126),
	CUTE_REF(dpackut_int8_decode_range__127__127__126),
	CUTE_REF(dpackut_int8_decode_range__126__127__126),
	CUTE_REF(dpackut_int8_decode_range__125__127__126),
	CUTE_REF(dpackut_int8_decode_range__1_0_1),
	CUTE_REF(dpackut_int8_decode_range_0_0_1),
	CUTE_REF(dpackut_int8_decode_range_1_0_1),
	CUTE_REF(dpackut_int8_decode_range_2_0_1),
	CUTE_REF(dpackut_int8_decode_range_124_125_126),
	CUTE_REF(dpackut_int8_decode_range_125_125_126),
	CUTE_REF(dpackut_int8_decode_range_126_125_126),
	CUTE_REF(dpackut_int8_decode_range_127_125_126),
	CUTE_REF(dpackut_int8_decode_range_128_125_126)
};

CUTE_SUITE_EXTERN(dpackut_int8_suite,
                  dpackut_int8_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
