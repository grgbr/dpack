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

#define DPACKUT_INT32(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int32 = _value \
	}

static void
dpackut_int32_encode(const struct dpackut_scalar_data * data)
{
	struct dpack_encoder enc = { 0, };
	size_t               sz = data->size;
	char                 buff[sz];

	memset(buff, 0xa5, sz);
	dpack_encoder_init_buffer(&enc, buff, sz);

	cute_check_uint(data->size, greater_equal, DPACK_INT32_SIZE_MIN);
	cute_check_uint(data->size, lower_equal, DPACK_INT32_SIZE_MAX);

	cute_check_sint(dpack_encode_int32(&enc, data->value.int32),
	                equal,
	                data->error);
	cute_check_mem(buff, equal, data->packed, sz);

	cute_check_uint(dpack_encoder_space_used(&enc), equal, sz);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int32_encode_assert)
{
	int32_t              val = false;
	int                  ret __unused;
#if defined(CONFIG_DPACK_DEBUG)
	struct dpack_encoder enc = { 0, };

	cute_expect_assertion(ret = dpack_encode_int32(&enc, val));
#endif /* defined(CONFIG_DPACK_DEBUG) */
	cute_expect_assertion(ret = dpack_encode_int32(NULL, val));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int32_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int32_encode__32768)
{
	/* -32768 */
	DPACKUT_INT32(data, "\xd2\x80\x00\x00\x00", 0, INT32_MIN);
	dpackut_int32_encode(&data);
}

CUTE_TEST(dpackut_int32_encode__32767)
{
	/* -32767 */
	DPACKUT_INT32(data, "\xd2\x80\x00\x00\x01", 0, INT32_MIN + 1);
	dpackut_int32_encode(&data);
}

CUTE_TEST(dpackut_int32_encode__1)
{
	/* -1 */
	DPACKUT_INT32(data, "\xff", 0, INT32_C(-1));
	dpackut_int32_encode(&data);
}

CUTE_TEST(dpackut_int32_encode_0)
{
	/* 0 */
	DPACKUT_INT32(data, "\x00", 0, INT32_C(0));
	dpackut_int32_encode(&data);
}

CUTE_TEST(dpackut_int32_encode_1)
{
	/* 1 */
	DPACKUT_INT32(data, "\x01", 0, INT32_C(1));
	dpackut_int32_encode(&data);
}

CUTE_TEST(dpackut_int32_encode_maxplus0)
{
	/* 2147483647 */
	DPACKUT_INT32(data, "\xce\x7f\xff\xff\xff", 0, INT32_MAX);
	dpackut_int32_encode(&data);
}

static void
dpackut_int32_decode(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int32_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int32(&dec, &val), equal, data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int32);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT32_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT32_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int32_decode_assert)
{
	int32_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT32_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int32(NULL, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_int32(&dec, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int32(&dec, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int32_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int32_decode__32769)
{
	/* -32769 */
	DPACKUT_INT32(data,
	              "\xd3\xff\xff\xff\xff\x7f\xff\xff\xff",
	              -ERANGE,
	              INT32_C(0));
	dpackut_int32_decode(&data);
}

CUTE_TEST(dpackut_int32_decode__32768)
{
	/* -32768 */
	DPACKUT_INT32(data,
	              "\xd2\x80\x00\x00\x00",
	              0,
	              INT32_MIN);
	dpackut_int32_decode(&data);
}

CUTE_TEST(dpackut_int32_decode__32767)
{
	/* -32767 */
	DPACKUT_INT32(data,
	              "\xd2\x80\x00\x00\x01",
	              0,
	              INT32_MIN + 1);
	dpackut_int32_decode(&data);
}

CUTE_TEST(dpackut_int32_decode__1)
{
	/* -1 */
	DPACKUT_INT32(data, "\xff", 0, INT32_C(-1));
	dpackut_int32_decode(&data);
}

CUTE_TEST(dpackut_int32_decode_0)
{
	/* 0 */
	DPACKUT_INT32(data, "\x00", 0, INT32_C(0));
	dpackut_int32_decode(&data);
}

CUTE_TEST(dpackut_int32_decode_1)
{
	/* 1 */
	DPACKUT_INT32(data, "\x01", 0, INT32_C(1));
	dpackut_int32_decode(&data);
}

CUTE_TEST(dpackut_int32_decode_maxplus0)
{
	/* 2147483647 */
	DPACKUT_INT32(data, "\xce\x7f\xff\xff\xff", 0, INT32_MAX);
	dpackut_int32_decode(&data);
}

CUTE_TEST(dpackut_int32_decode_maxplus1)
{
	/* 2147483648 */
	DPACKUT_INT32(data, "\xce\x80\x00\x00\x00", -ERANGE, INT32_C(0));
	dpackut_int32_decode(&data);
}

#define DPACKUT_INT32_MIN(_var, _packed, _error, _value, _low) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int32 = _value, \
		.low.int32   = _low \
	}

static void
dpackut_int32_decode_min(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int32_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int32_min(&dec, data->low.int32, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int32);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT32_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT32_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if  defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int32_decode_min_assert)
{
	int32_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT32_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int32_min(NULL, 1, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_int32_min(&dec, 1, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */
	cute_expect_assertion(ret = dpack_decode_int32_min(&dec,
	                                                   INT32_MIN,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_int32_min(&dec,
	                                                   INT32_MAX,
	                                                   &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int32_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int32_decode_min_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int32_decode_min_minminus1_minplus1)
{
	/* -2147483649 */
	DPACKUT_INT32_MIN(data,
	                  "\xd3\xff\xff\xff\xff\x7f\xff\xff\xff",
	                  -ERANGE,
	                  0,
	                  INT32_MIN + 1);
	dpackut_int32_decode_min(&data);
}

CUTE_TEST(dpackut_int32_decode_min_min_minplus1)
{
	/* -2147483648 */
	DPACKUT_INT32_MIN(data,
	                  "\xd2\x80\x00\x00\x00",
	                  -ERANGE,
	                  0,
	                  INT32_MIN + 1);
	dpackut_int32_decode_min(&data);
}

CUTE_TEST(dpackut_int32_decode_min_minplus1_minplus1)
{
	/* -2147483647 */
	DPACKUT_INT32_MIN(data,
	                  "\xd2\x80\x00\x00\x01",
	                  0,
	                  INT32_MIN + 1,
	                  INT32_MIN + 1);
	dpackut_int32_decode_min(&data);
}

CUTE_TEST(dpackut_int32_decode_min_minplus2_minplus1)
{
	/* -2147483646 */
	DPACKUT_INT32_MIN(data,
	                  "\xd2\x80\x00\x00\x02",
	                  0,
	                  INT32_MIN + 2,
	                  INT32_MIN + 1);
	dpackut_int32_decode_min(&data);
}

CUTE_TEST(dpackut_int32_decode_min__1_0)
{
	/* -1 */
	DPACKUT_INT32_MIN(data,
	                  "\xff",
	                  -ERANGE,
	                  0,
	                  INT32_C(0));
	dpackut_int32_decode_min(&data);
}

CUTE_TEST(dpackut_int32_decode_min_0_0)
{
	/* 0 */
	DPACKUT_INT32_MIN(data,
	                  "\x00",
	                  0,
	                  INT32_C(0),
	                  INT32_C(0));
	dpackut_int32_decode_min(&data);
}

CUTE_TEST(dpackut_int32_decode_min_1_0)
{
	/* 1 */
	DPACKUT_INT32_MIN(data,
	                  "\x01",
	                  0,
	                  INT32_C(1),
	                  INT32_C(0));
	dpackut_int32_decode_min(&data);
}

CUTE_TEST(dpackut_int32_decode_min_max_0)
{
	/* 2147483647 */
	DPACKUT_INT32_MIN(data,
	                  "\xce\x7f\xff\xff\xff",
	                  0,
	                  INT32_MAX,
	                  INT32_C(0));
	dpackut_int32_decode_min(&data);
}

CUTE_TEST(dpackut_int32_decode_min_maxplus1_0)
{
	/* 2147483648 */
	DPACKUT_INT32_MIN(data,
	                  "\xce\x80\x00\x00\x00",
	                  -ERANGE,
	                  0,
	                  INT32_C(0));
	dpackut_int32_decode_min(&data);
}

CUTE_TEST(dpackut_int32_decode_min_maxminus2_maxminus1)
{
	/* 2147483645 */
	DPACKUT_INT32_MIN(data,
	                  "\xce\x7f\xff\xff\xfd",
	                  -ERANGE,
	                  0,
	                  INT32_MAX - 1);
	dpackut_int32_decode_min(&data);
}

CUTE_TEST(dpackut_int32_decode_min_maxminus1_maxminus1)
{
	/* 2147483646 */
	DPACKUT_INT32_MIN(data,
	                  "\xce\x7f\xff\xff\xfe",
	                  0,
	                  INT32_MAX - 1,
	                  INT32_MAX - 1);
	dpackut_int32_decode_min(&data);
}

CUTE_TEST(dpackut_int32_decode_min_max_maxminus1)
{
	/* 2147483647 */
	DPACKUT_INT32_MIN(data,
	                  "\xce\x7f\xff\xff\xff",
	                  0,
	                  INT32_MAX,
	                  INT32_MAX - 1);
	dpackut_int32_decode_min(&data);
}

CUTE_TEST(dpackut_int32_decode_min_maxplus1_maxminus1)
{
	/* 2147483648 */
	DPACKUT_INT32_MIN(data,
	                  "\xce\x80\x00\x00\x00",
	                  -ERANGE,
	                  0,
	                  INT32_MAX - 1);
	dpackut_int32_decode_min(&data);
}

#define DPACKUT_INT32_MAX(_var, _packed, _error, _value, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int32 = _value, \
		.high.int32  = _high \
	}

static void
dpackut_int32_decode_max(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int32_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int32_max(&dec, data->high.int32, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int32);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT32_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT32_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if  defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int32_decode_max_assert)
{
	int32_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT32_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int32_max(NULL, 1, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_int32_max(&dec, 1, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int32_max(&dec,
	                                                   INT32_MIN,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_int32_max(&dec,
	                                                   INT32_MAX,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_int32_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int32_decode_max_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int32_decode_max_minminus1_minplus1)
{
	/* -2147483649 */
	DPACKUT_INT32_MAX(data,
	                  "\xd3\xff\xff\xff\xff\x7f\xff\xff\xff",
	                  -ERANGE,
	                  0,
	                  INT32_MIN + 1);
	dpackut_int32_decode_max(&data);
}

CUTE_TEST(dpackut_int32_decode_max_min_minplus1)
{
	/* -2147483648 */
	DPACKUT_INT32_MAX(data,
	                  "\xd2\x80\x00\x00\x00",
	                  0,
	                  INT32_MIN,
	                  INT32_MIN + 1);
	dpackut_int32_decode_max(&data);
}

CUTE_TEST(dpackut_int32_decode_max_minplus1_minplus1)
{
	/* -2147483647 */
	DPACKUT_INT32_MAX(data,
	                  "\xd2\x80\x00\x00\x01",
	                  0,
	                  INT32_MIN + 1,
	                  INT32_MIN + 1);
	dpackut_int32_decode_max(&data);
}

CUTE_TEST(dpackut_int32_decode_max_minplus2_minplus1)
{
	/* -2147483646 */
	DPACKUT_INT32_MAX(data,
	                  "\xd2\x80\x00\x00\x02",
	                  -ERANGE,
	                  0,
	                  INT32_MIN + 1);
	dpackut_int32_decode_max(&data);
}

CUTE_TEST(dpackut_int32_decode_max__1_0)
{
	/* -1 */
	DPACKUT_INT32_MAX(data,
	                  "\xff",
	                  0,
	                  INT32_C(-1),
	                  INT32_C(0));
	dpackut_int32_decode_max(&data);
}

CUTE_TEST(dpackut_int32_decode_max_0_0)
{
	/* 0 */
	DPACKUT_INT32_MAX(data,
	                  "\x00",
	                  0,
	                  INT32_C(0),
	                  INT32_C(0));
	dpackut_int32_decode_max(&data);
}

CUTE_TEST(dpackut_int32_decode_max_1_0)
{
	/* 1 */
	DPACKUT_INT32_MAX(data,
	                  "\x01",
	                  -ERANGE,
	                  0,
	                  INT32_C(0));
	dpackut_int32_decode_max(&data);
}

CUTE_TEST(dpackut_int32_decode_max_max_0)
{
	/* 2147483647 */
	DPACKUT_INT32_MAX(data,
	                  "\xce\x7f\xff\xff\xff",
	                  -ERANGE,
	                  0,
	                  INT32_C(0));
	dpackut_int32_decode_max(&data);
}

CUTE_TEST(dpackut_int32_decode_max_maxplus1_0)
{
	/* 2147483648 */
	DPACKUT_INT32_MAX(data,
	                  "\xce\x80\x00\x00\x00",
	                  -ERANGE,
	                  0,
	                  INT32_C(0));
	dpackut_int32_decode_max(&data);
}

CUTE_TEST(dpackut_int32_decode_max_maxminus2_maxminus1)
{
	/* 2147483645 */
	DPACKUT_INT32_MAX(data,
	                  "\xce\x7f\xff\xff\xfd",
	                  0,
	                  INT32_MAX - 2,
	                  INT32_MAX - 1);
	dpackut_int32_decode_max(&data);
}

CUTE_TEST(dpackut_int32_decode_max_maxminus1_maxminus1)
{
	/* 2147483646 */
	DPACKUT_INT32_MAX(data,
	                  "\xce\x7f\xff\xff\xfe",
	                  0,
	                  INT32_MAX - 1,
	                  INT32_MAX - 1);
	dpackut_int32_decode_max(&data);
}

CUTE_TEST(dpackut_int32_decode_max_max_maxminus1)
{
	/* 2147483647 */
	DPACKUT_INT32_MAX(data,
	                  "\xce\x7f\xff\xff\xff",
	                  -ERANGE,
	                  0,
	                  INT32_MAX - 1);
	dpackut_int32_decode_max(&data);
}

CUTE_TEST(dpackut_int32_decode_max__maxminus1)
{
	/* 2147483648 */
	DPACKUT_INT32_MAX(data,
	                  "\xce\x80\x00\x00\x00",
	                  -ERANGE,
	                  0,
	                  INT32_MAX - 1);
	dpackut_int32_decode_max(&data);
}

#define DPACKUT_INT32_RANGE(_var, _packed, _error, _value, _low, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int32 = _value, \
		.low.int32   = _low, \
		.high.int32  = _high \
	}

static void
dpackut_int32_decode_range(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int32_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int32_range(&dec,
	                                         data->low.int32,
	                                         data->high.int32,
	                                         &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int32);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT32_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT32_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if  defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int32_decode_range_assert)
{
	int32_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT32_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int32_range(NULL, 1, 2, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_int32_range(&dec, 1, 2, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int32_range(&dec,
	                                                     INT32_MIN,
	                                                     2,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_int32_range(&dec,
	                                                     1,
	                                                     INT32_MAX,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_int32_range(&dec, 2, 2, &val));
	cute_expect_assertion(ret = dpack_decode_int32_range(&dec, 1, 2, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int32_decode_range_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int32_decode_range_minminus1_minplus1_minplus2)
{
	/* -2147483649 */
	DPACKUT_INT32_RANGE(data,
	                    "\xd3\xff\xff\xff\xff\x7f\xff\xff\xff",
	                    -ERANGE,
	                    0,
	                    INT32_MIN + 1,
	                    INT32_MIN + 2);
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range_min_minplus1_minplus2)
{
	/* -2147483648 */
	DPACKUT_INT32_RANGE(data,
	                    "\xd2\x80\x00\x00\x00",
	                    -ERANGE,
	                    0,
	                    INT32_MIN + 1,
	                    INT32_MIN + 2);
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range_minplus1_minplus1_minplus2)
{
	/* -2147483647 */
	DPACKUT_INT32_RANGE(data,
	                    "\xd2\x80\x00\x00\x01",
	                    0,
	                    INT32_MIN + 1,
	                    INT32_MIN + 1,
	                    INT32_MIN + 2);
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range_minplus2_minplus1_minplus2)
{
	/* -2147483646 */
	DPACKUT_INT32_RANGE(data,
	                    "\xd2\x80\x00\x00\x02",
	                    0,
	                    INT32_MIN + 2,
	                    INT32_MIN + 1,
	                    INT32_MIN + 2);
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range_minplus3_minplus1_minplus2)
{
	/* -2147483645 */
	DPACKUT_INT32_RANGE(data,
	                    "\xd2\x80\x00\x00\x03",
	                    -ERANGE,
	                    0,
	                    INT32_MIN + 1,
	                    INT32_MIN + 2);
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range__1_0_1)
{
	/* -1 */
	DPACKUT_INT32_RANGE(data,
	                    "\xff",
	                    -ERANGE,
	                    0,
	                    INT32_C(0),
	                    INT32_C(1));
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range_0_0_1)
{
	/* 0 */
	DPACKUT_INT32_RANGE(data,
	                    "\x00",
	                    0,
	                    INT32_C(0),
	                    INT32_C(0),
	                    INT32_C(1));
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range_1_0_1)
{
	/* 1 */
	DPACKUT_INT32_RANGE(data,
	                    "\x01",
	                    0,
	                    INT32_C(1),
	                    INT32_C(0),
	                    INT32_C(1));
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range_2_0_1)
{
	/* 2 */
	DPACKUT_INT32_RANGE(data,
	                    "\x02",
	                    -ERANGE,
	                    0,
	                    INT32_C(0),
	                    INT32_C(1));
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range_maxminus3_maxminus2_maxminus1)
{
	/* 2147483644 */
	DPACKUT_INT32_RANGE(data,
	                    "\xce\x7f\xff\xff\xfc",
	                    -ERANGE,
	                    0,
	                    INT32_MAX - 2,
	                    INT32_MAX - 1);
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range_maxminus2_maxminus2_maxminus1)
{
	/* 2147483645 */
	DPACKUT_INT32_RANGE(data,
	                    "\xce\x7f\xff\xff\xfd",
	                    0,
	                    INT32_MAX - 2,
	                    INT32_MAX - 2,
	                    INT32_MAX - 1);
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range_maxminus1_maxminus2_maxminus1)
{
	/* 2147483646 */
	DPACKUT_INT32_RANGE(data,
	                    "\xce\x7f\xff\xff\xfe",
	                    0,
	                    INT32_MAX - 1,
	                    INT32_MAX - 2,
	                    INT32_MAX - 1);
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range_max_maxminus2_maxminus1)
{
	/* 2147483647 */
	DPACKUT_INT32_RANGE(data,
	                    "\xce\x7f\xff\xff\xff",
	                    -ERANGE,
	                    0,
	                    INT32_MAX - 2,
	                    INT32_MAX - 1);
	dpackut_int32_decode_range(&data);
}

CUTE_TEST(dpackut_int32_decode_range_maxplus1_maxminus2_maxminus1)
{
	/* 2147483648 */
	DPACKUT_INT32_RANGE(data,
	                    "\xce\x80\x00\x00\x00",
	                    -ERANGE,
	                    0,
	                    INT32_MAX - 2,
	                    INT32_MAX - 1);
	dpackut_int32_decode_range(&data);
}

CUTE_GROUP(dpackut_int32_group) = {
	CUTE_REF(dpackut_int32_encode_assert),
	CUTE_REF(dpackut_int32_encode__32768),
	CUTE_REF(dpackut_int32_encode__32767),
	CUTE_REF(dpackut_int32_encode__1),
	CUTE_REF(dpackut_int32_encode_0),
	CUTE_REF(dpackut_int32_encode_1),
	CUTE_REF(dpackut_int32_encode_maxplus0),

	CUTE_REF(dpackut_int32_decode_assert),
	CUTE_REF(dpackut_int32_decode__32769),
	CUTE_REF(dpackut_int32_decode__32768),
	CUTE_REF(dpackut_int32_decode__32767),
	CUTE_REF(dpackut_int32_decode__1),
	CUTE_REF(dpackut_int32_decode_0),
	CUTE_REF(dpackut_int32_decode_1),
	CUTE_REF(dpackut_int32_decode_maxplus0),
	CUTE_REF(dpackut_int32_decode_maxplus1),

	CUTE_REF(dpackut_int32_decode_min_assert),
	CUTE_REF(dpackut_int32_decode_min_minminus1_minplus1),
	CUTE_REF(dpackut_int32_decode_min_min_minplus1),
	CUTE_REF(dpackut_int32_decode_min_minplus1_minplus1),
	CUTE_REF(dpackut_int32_decode_min_minplus2_minplus1),
	CUTE_REF(dpackut_int32_decode_min__1_0),
	CUTE_REF(dpackut_int32_decode_min_0_0),
	CUTE_REF(dpackut_int32_decode_min_1_0),
	CUTE_REF(dpackut_int32_decode_min_max_0),
	CUTE_REF(dpackut_int32_decode_min_maxplus1_0),
	CUTE_REF(dpackut_int32_decode_min_maxminus2_maxminus1),
	CUTE_REF(dpackut_int32_decode_min_maxminus1_maxminus1),
	CUTE_REF(dpackut_int32_decode_min_max_maxminus1),
	CUTE_REF(dpackut_int32_decode_min_maxplus1_maxminus1),

	CUTE_REF(dpackut_int32_decode_max_assert),
	CUTE_REF(dpackut_int32_decode_max_minminus1_minplus1),
	CUTE_REF(dpackut_int32_decode_max_min_minplus1),
	CUTE_REF(dpackut_int32_decode_max_minplus1_minplus1),
	CUTE_REF(dpackut_int32_decode_max_minplus2_minplus1),
	CUTE_REF(dpackut_int32_decode_max__1_0),
	CUTE_REF(dpackut_int32_decode_max_0_0),
	CUTE_REF(dpackut_int32_decode_max_1_0),
	CUTE_REF(dpackut_int32_decode_max_max_0),
	CUTE_REF(dpackut_int32_decode_max_maxplus1_0),
	CUTE_REF(dpackut_int32_decode_max_maxminus2_maxminus1),
	CUTE_REF(dpackut_int32_decode_max_maxminus1_maxminus1),
	CUTE_REF(dpackut_int32_decode_max_max_maxminus1),
	CUTE_REF(dpackut_int32_decode_max__maxminus1),

	CUTE_REF(dpackut_int32_decode_range_assert),
	CUTE_REF(dpackut_int32_decode_range_minminus1_minplus1_minplus2),
	CUTE_REF(dpackut_int32_decode_range_min_minplus1_minplus2),
	CUTE_REF(dpackut_int32_decode_range_minplus1_minplus1_minplus2),
	CUTE_REF(dpackut_int32_decode_range_minplus2_minplus1_minplus2),
	CUTE_REF(dpackut_int32_decode_range_minplus3_minplus1_minplus2),
	CUTE_REF(dpackut_int32_decode_range__1_0_1),
	CUTE_REF(dpackut_int32_decode_range_0_0_1),
	CUTE_REF(dpackut_int32_decode_range_1_0_1),
	CUTE_REF(dpackut_int32_decode_range_2_0_1),
	CUTE_REF(dpackut_int32_decode_range_maxminus3_maxminus2_maxminus1),
	CUTE_REF(dpackut_int32_decode_range_maxminus2_maxminus2_maxminus1),
	CUTE_REF(dpackut_int32_decode_range_maxminus1_maxminus2_maxminus1),
	CUTE_REF(dpackut_int32_decode_range_max_maxminus2_maxminus1),
	CUTE_REF(dpackut_int32_decode_range_maxplus1_maxminus2_maxminus1)
};

CUTE_SUITE_EXTERN(dpackut_int32_suite,
                  dpackut_int32_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
