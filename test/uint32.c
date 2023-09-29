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

#define DPACKUT_UINT32(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint32 = _value \
	}

static void
dpackut_uint32_encode(const struct dpackut_scalar_data * data)
{
	struct dpack_encoder enc = { 0, };
	size_t               sz = data->size;
	char                 buff[sz];

	memset(buff, 0xa5, sz);
	dpack_encoder_init_buffer(&enc, buff, sz);

	cute_check_uint(data->size, greater_equal, DPACK_UINT32_SIZE_MIN);
	cute_check_uint(data->size, lower_equal, DPACK_UINT32_SIZE_MAX);

	cute_check_sint(dpack_encode_uint32(&enc, data->value.uint32),
	                equal,
	                data->error);
	cute_check_mem(buff, equal, data->packed, sz);

	cute_check_uint(dpack_encoder_space_used(&enc), equal, sz);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint32_encode_assert)
{
	uint32_t             val = false;
	int                  ret __unused;
#if defined(CONFIG_DPACK_DEBUG)
	struct dpack_encoder enc = { 0, };

	cute_expect_assertion(ret = dpack_encode_uint32(&enc, val));
#endif /* defined(CONFIG_DPACK_DEBUG) */
	cute_expect_assertion(ret = dpack_encode_uint32(NULL, val));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint32_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint32_encode_0)
{
	/* 0 */
	DPACKUT_UINT32(data, "\x00", 0, UINT32_C(0));
	dpackut_uint32_encode(&data);
}

CUTE_TEST(dpackut_uint32_encode_1)
{
	/* 1 */
	DPACKUT_UINT32(data, "\x01", 0, UINT32_C(1));
	dpackut_uint32_encode(&data);
}

CUTE_TEST(dpackut_uint32_encode_max)
{
	/* 4294967295 */
	DPACKUT_UINT32(data, "\xce\xff\xff\xff\xff", 0, UINT32_MAX);
	dpackut_uint32_encode(&data);
}

static void
dpackut_uint32_decode(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	uint32_t             val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint32(&dec, &val), equal, data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint32);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT32_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT32_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint32_decode_assert)
{
	uint32_t             val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_UINT32_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint32(NULL, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_uint32(&dec, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint32(&dec, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint32_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint32_decode__1)
{
	/* -1 */
	DPACKUT_UINT32(data, "\xff", -ENOMSG, UINT32_C(0));
	dpackut_uint32_decode(&data);
}

CUTE_TEST(dpackut_uint32_decode_0)
{
	/* 0 */
	DPACKUT_UINT32(data, "\x00", 0, UINT32_C(0));
	dpackut_uint32_decode(&data);
}

CUTE_TEST(dpackut_uint32_decode_1)
{
	/* 1 */
	DPACKUT_UINT32(data, "\x01", 0, UINT32_C(1));
	dpackut_uint32_decode(&data);
}

CUTE_TEST(dpackut_uint32_decode_maxplus0)
{
	/* 4294967295 */
	DPACKUT_UINT32(data, "\xce\xff\xff\xff\xff", 0, UINT32_MAX);
	dpackut_uint32_decode(&data);
}

CUTE_TEST(dpackut_uint32_decode_maxplus1)
{
	/* 4294967296 */
	DPACKUT_UINT32(data,
	               "\xcf\x00\x00\x00\x01\x00\x00\x00\x00",
	               -ERANGE,
	               (0));
	dpackut_uint32_decode(&data);
}

#define DPACKUT_UINT32_MIN(_var, _packed, _error, _value, _low) \
	const struct dpackut_scalar_data _var = { \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint32 = _value, \
		.low.uint32   = _low \
	}

static void
dpackut_uint32_decode_min(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	uint32_t             val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint32_min(&dec, data->low.uint32, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint32);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT32_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT32_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint32_decode_min_assert)
{
	uint32_t             val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_UINT32_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint32_min(NULL, 1, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_uint32_min(&dec, 1, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint32_min(&dec, 0, &val));
	cute_expect_assertion(ret = dpack_decode_uint32_min(&dec,
	                                                    UINT32_MAX,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_uint32_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint32_decode_min_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint32_decode_min__1_1)
{
	/* -1 */
	DPACKUT_UINT32_MIN(data, "\xff", -ENOMSG, 0, UINT32_C(1));
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_0_1)
{
	/* 0 */
	DPACKUT_UINT32_MIN(data,
	                   "\x00",
	                   -ERANGE,
	                   0,
	                   UINT32_C(1));
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_1_1)
{
	/* 1 */
	DPACKUT_UINT32_MIN(data,
	                   "\x01",
	                   0,
	                   UINT32_C(1),
	                   UINT32_C(1));
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_halfmax_1)
{
	/* 2147483647 */
	DPACKUT_UINT32_MIN(data,
	                   "\xce\x7f\xff\xff\xff",
	                   0,
	                   UINT32_MAX / 2,
	                   UINT32_C(1));
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_max_1)
{
	/* 4294967295 */
	DPACKUT_UINT32_MIN(data,
	                   "\xce\xff\xff\xff\xff",
	                   0,
	                   UINT32_MAX,
	                   UINT32_C(1));
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_maxplus1_1)
{
	/* 4294967296 */
	DPACKUT_UINT32_MIN(data,
	                   "\xcf\x00\x00\x00\x01\x00\x00\x00\x00",
	                   -ERANGE,
	                   0,
	                   UINT32_C(1));
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min__1_halfmax)
{
	/* -1 */
	DPACKUT_UINT32_MIN(data,
	                   "\xff",
	                   -ENOMSG,
	                   0,
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_0_halfmax)
{
	/* 0 */
	DPACKUT_UINT32_MIN(data,
	                   "\x00",
	                   -ERANGE,
	                   0,
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_1_halfmax)
{
	/* 1 */
	DPACKUT_UINT32_MIN(data,
	                   "\x01",
	                   -ERANGE,
	                   0,
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_halfmaxminus1_halfmax)
{
	/* 2147483646 */
	DPACKUT_UINT32_MIN(data,
	                   "\xce\x7f\xff\xff\xfe",
	                   -ERANGE,
	                   0,
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_halfmax_halfmax)
{
	/* 2147483647 */
	DPACKUT_UINT32_MIN(data,
	                   "\xce\x7f\xff\xff\xff",
	                   0,
	                   UINT32_MAX / 2,
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_max_halfmax)
{
	/* 4294967295 */
	DPACKUT_UINT32_MIN(data,
	                   "\xce\xff\xff\xff\xff",
	                   0,
	                   UINT32_MAX,
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_maxplus1_halfmax)
{
	/* 4294967296 */
	DPACKUT_UINT32_MIN(data,
	                   "\xcf\x00\x00\x00\x01\x00\x00\x00\x00",
	                   -ERANGE,
	                   0,
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_maxminus2_maxminus1)
{
	/* 4294967293 */
	DPACKUT_UINT32_MIN(data,
	                   "\xce\xff\xff\xff\xfd",
	                   -ERANGE,
	                   0,
	                   UINT32_MAX - 1);
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_maxminus1_maxminus1)
{
	/* 4294967294 */
	DPACKUT_UINT32_MIN(data,
	                   "\xce\xff\xff\xff\xfe",
	                   0,
	                   UINT32_MAX - 1,
	                   UINT32_MAX - 1);
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_max_maxminus1)
{
	/* 4294967295 */
	DPACKUT_UINT32_MIN(data,
	                   "\xce\xff\xff\xff\xff",
	                   0,
	                   UINT32_MAX,
	                   UINT32_MAX - 1);
	dpackut_uint32_decode_min(&data);
}

CUTE_TEST(dpackut_uint32_decode_min_maxplus1_maxminus1)
{
	/* 4294967296 */
	DPACKUT_UINT32_MIN(data,
	                   "\xcf\x00\x00\x00\x01\x00\x00\x00\x00",
	                   -ERANGE,
	                   0,
	                   UINT32_MAX - 1);
	dpackut_uint32_decode_min(&data);
}

#define DPACKUT_UINT32_MAX(_var, _packed, _error, _value, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint32 = _value, \
		.high.uint32  = _high \
	}

static void
dpackut_uint32_decode_max(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	uint32_t             val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint32_max(&dec, data->high.uint32, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint32);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT32_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT32_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint32_decode_max_assert)
{
	uint32_t             val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_UINT32_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint32_max(NULL, 1, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_uint32_max(&dec, 1, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint32_max(&dec, 0, &val));
	cute_expect_assertion(ret = dpack_decode_uint32_max(&dec,
	                                                    UINT32_MAX,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_uint32_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint32_decode_max_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint32_decode_max__1_1)
{
	/* -1 */
	DPACKUT_UINT32_MAX(data, "\xff", -ENOMSG, 0, UINT32_C(1));
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_0_1)
{
	/* 0 */
	DPACKUT_UINT32_MAX(data,
	                   "\x00",
	                   0,
	                   UINT32_C(0),
	                   UINT32_C(1));
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_1_1)
{
	/* 1 */
	DPACKUT_UINT32_MAX(data,
	                   "\x01",
	                   0,
	                   UINT32_C(1),
	                   UINT32_C(1));
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_halfmax_1)
{
	/* 2147483647 */
	DPACKUT_UINT32_MAX(data,
	                   "\xce\x7f\xff\xff\xff",
	                   -ERANGE,
	                   0,
	                   UINT32_C(1));
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_max_1)
{
	/* 4294967295 */
	DPACKUT_UINT32_MAX(data,
	                   "\xce\xff\xff\xff\xff",
	                   -ERANGE,
	                   0,
	                   UINT32_C(1));
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_maxplus1_1)
{
	/* 4294967296 */
	DPACKUT_UINT32_MAX(data,
	                   "\xcf\x00\x00\x00\x01\x00\x00\x00\x00",
	                   -ERANGE,
	                   0,
	                   UINT32_C(1));
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max__1_halfmax)
{
	/* -1 */
	DPACKUT_UINT32_MAX(data,
	                   "\xff",
	                   -ENOMSG,
	                   0,
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_0_halfmax)
{
	/* 0 */
	DPACKUT_UINT32_MAX(data,
	                   "\x00",
	                   0,
	                   UINT32_C(0),
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_1_halfmax)
{
	/* 1 */
	DPACKUT_UINT32_MAX(data,
	                   "\x01",
	                   0,
	                   UINT32_C(1),
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_halfmaxminus1_halfmax)
{
	/* 2147483646 */
	DPACKUT_UINT32_MAX(data,
	                   "\xce\x7f\xff\xff\xfe",
	                   0,
	                   (UINT32_MAX / 2) - 1,
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_halfmax_halfmax)
{
	/* 2147483647 */
	DPACKUT_UINT32_MAX(data,
	                   "\xce\x7f\xff\xff\xff",
	                   0,
	                   UINT32_MAX / 2,
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_max_halfmax)
{
	/* 4294967295 */
	DPACKUT_UINT32_MAX(data,
	                   "\xce\xff\xff\xff\xff",
	                   -ERANGE,
	                   0,
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_maxplus1_halfmax)
{
	/* 4294967296 */
	DPACKUT_UINT32_MAX(data,
	                   "\xcf\x00\x00\x00\x01\x00\x00\x00\x00",
	                   -ERANGE,
	                   0,
	                   UINT32_MAX / 2);
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_maxminus2_halfmaxminus1)
{
	/* 4294967293 */
	DPACKUT_UINT32_MAX(data,
	                   "\xce\xff\xff\xff\xfd",
	                   0,
	                   UINT32_MAX - 2,
	                   UINT32_MAX - 1);
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_maxminus1_maxminus1)
{
	/* 4294967294 */
	DPACKUT_UINT32_MAX(data,
	                   "\xce\xff\xff\xff\xfe",
	                   0,
	                   UINT32_MAX - 1,
	                   UINT32_MAX - 1);
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_max_maxminus1)
{
	/* 4294967295 */
	DPACKUT_UINT32_MAX(data,
	                   "\xce\xff\xff\xff\xff",
	                   -ERANGE,
	                   0,
	                   UINT32_MAX - 1);
	dpackut_uint32_decode_max(&data);
}

CUTE_TEST(dpackut_uint32_decode_max_maxplus1_maxminus1)
{
	/* 4294967296 */
	DPACKUT_UINT32_MAX(data,
	                   "\xcf\x00\x00\x00\x01\x00\x00\x00\x00",
	                   -ERANGE,
	                   0,
	                   UINT32_MAX - 1);
	dpackut_uint32_decode_max(&data);
}

#define DPACKUT_UINT32_RANGE(_var, _packed, _error, _value, _low, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint32 = _value, \
		.low.uint32   = _low, \
		.high.uint32  = _high \
	}

static void
dpackut_uint32_decode_range(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	uint32_t             val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint32_range(&dec,
	                                         data->low.uint32,
	                                         data->high.uint32,
	                                         &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint32);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT32_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT32_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint32_decode_range_assert)
{
	uint32_t             val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_UINT32_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint32_range(NULL,
	                                                      1,
	                                                      2,
	                                                      &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_uint32_range(&dec,
	                                                      1,
	                                                      2,
	                                                      &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint32_range(&dec,
	                                                      0,
	                                                      2,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint32_range(&dec,
	                                                      1,
	                                                      UINT32_MAX,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint32_range(&dec,
	                                                      2,
	                                                      2,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint32_range(&dec,
	                                                      1,
	                                                      2,
	                                                      NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint32_decode_range_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint32_decode_range__1_1_2)
{
	/* -1 */
	DPACKUT_UINT32_RANGE(data,
	                     "\xff",
	                     -ENOMSG,
	                     0,
	                     UINT32_C(1),
	                     UINT32_C(2));
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_0_1_2)
{
	/* 0 */
	DPACKUT_UINT32_RANGE(data,
	                     "\x00",
	                     -ERANGE,
	                     0,
	                     UINT32_C(1),
	                     UINT32_C(2));
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_1_1_2)
{
	/* 1 */
	DPACKUT_UINT32_RANGE(data,
	                     "\x01",
	                     0,
	                     UINT32_C(1),
	                     UINT32_C(1),
	                     UINT32_C(2));
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_2_1_2)
{
	/* 2 */
	DPACKUT_UINT32_RANGE(data,
	                     "\x02",
	                     0,
	                     UINT32_C(2),
	                     UINT32_C(1),
	                     UINT32_C(2));
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_3_1_2)
{
	/* 3 */
	DPACKUT_UINT32_RANGE(data,
	                     "\x03",
	                     -ERANGE,
	                     0,
	                     UINT32_C(1),
	                     UINT32_C(2));
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_1_halfmax_halfmaxplus1)
{
	/* 1 */
	DPACKUT_UINT32_RANGE(data,
	                     "\x01",
	                     -ERANGE,
	                     0,
	                     UINT32_MAX / 2,
	                     (UINT32_MAX / 2) + 1);
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_halfmaxminus1_halfmax_halfmaxplus1)
{
	/* 2147483646 */
	DPACKUT_UINT32_RANGE(data,
	                     "\xce\x7f\xff\xff\xfe",
	                     -ERANGE,
	                     0,
	                     UINT32_MAX / 2,
	                     (UINT32_MAX / 2) + 1);
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_halfmax_halfmax_halfmaxplus1)
{
	/* 2147483647 */
	DPACKUT_UINT32_RANGE(data,
	                     "\xce\x7f\xff\xff\xff",
	                     0,
	                     UINT32_MAX / 2,
	                     UINT32_MAX / 2,
	                     (UINT32_MAX / 2) + 1);
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_halfmaxplus1_halfmax_halfmaxplus1)
{
	/* 2147483648 */
	DPACKUT_UINT32_RANGE(data,
	                     "\xce\x80\x00\x00\x00",
	                     0,
	                     (UINT32_MAX / 2) + 1,
	                     UINT32_MAX / 2,
	                     (UINT32_MAX / 2) + 1);
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_halfmaxplus2_halfmax_halfmaxplus1)
{
	/* 2147483649 */
	DPACKUT_UINT32_RANGE(data,
	                     "\xce\x80\x00\x00\x01",
	                     -ERANGE,
	                     0,
	                     UINT32_MAX / 2,
	                     (UINT32_MAX / 2) + 1);
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_maxminus3_maxminus2_maxminus1)
{
	/* 4294967292*/
	DPACKUT_UINT32_RANGE(data,
	                     "\xce\xff\xff\xff\xfc",
	                     -ERANGE,
	                     0,
	                     UINT32_MAX - 2,
	                     UINT32_MAX - 1);
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_maxminus2_maxminus2_maxminus1)
{
	/* 4294967293*/
	DPACKUT_UINT32_RANGE(data,
	                     "\xce\xff\xff\xff\xfd",
	                     0,
	                     UINT32_MAX - 2,
	                     UINT32_MAX - 2,
	                     UINT32_MAX - 1);
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_maxminus1_maxminus2_maxminus1)
{
	/* 4294967294*/
	DPACKUT_UINT32_RANGE(data,
	                     "\xce\xff\xff\xff\xfe",
	                     0,
	                     UINT32_MAX - 1,
	                     UINT32_MAX - 2,
	                     UINT32_MAX - 1);
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_max_maxminus2_maxminus1)
{
	/* 4294967295*/
	DPACKUT_UINT32_RANGE(data,
	                     "\xce\xff\xff\xff\xff",
	                     -ERANGE,
	                     0,
	                     UINT32_MAX - 2,
	                     UINT32_MAX - 1);
	dpackut_uint32_decode_range(&data);
}

CUTE_TEST(dpackut_uint32_decode_range_maxplus1_maxminus2_maxminus1)
{
	/* 4294967296*/
	DPACKUT_UINT32_RANGE(data,
	                     "\xcf\x00\x00\x00\x01\x00\x00\x00\x00",
	                     -ERANGE,
	                     0,
	                     UINT32_MAX - 2,
	                     UINT32_MAX - 1);
	dpackut_uint32_decode_range(&data);
}

CUTE_GROUP(dpackut_uint32_group) = {
	CUTE_REF(dpackut_uint32_encode_assert),
	CUTE_REF(dpackut_uint32_encode_0),
	CUTE_REF(dpackut_uint32_encode_1),
	CUTE_REF(dpackut_uint32_encode_max),

	CUTE_REF(dpackut_uint32_decode_assert),
	CUTE_REF(dpackut_uint32_decode__1),
	CUTE_REF(dpackut_uint32_decode_0),
	CUTE_REF(dpackut_uint32_decode_1),
	CUTE_REF(dpackut_uint32_decode_maxplus0),
	CUTE_REF(dpackut_uint32_decode_maxplus1),

	CUTE_REF(dpackut_uint32_decode_min_assert),
	CUTE_REF(dpackut_uint32_decode_min__1_1),
	CUTE_REF(dpackut_uint32_decode_min_0_1),
	CUTE_REF(dpackut_uint32_decode_min_1_1),
	CUTE_REF(dpackut_uint32_decode_min_halfmax_1),
	CUTE_REF(dpackut_uint32_decode_min_max_1),
	CUTE_REF(dpackut_uint32_decode_min_maxplus1_1),
	CUTE_REF(dpackut_uint32_decode_min__1_halfmax),
	CUTE_REF(dpackut_uint32_decode_min_0_halfmax),
	CUTE_REF(dpackut_uint32_decode_min_1_halfmax),
	CUTE_REF(dpackut_uint32_decode_min_halfmaxminus1_halfmax),
	CUTE_REF(dpackut_uint32_decode_min_halfmax_halfmax),
	CUTE_REF(dpackut_uint32_decode_min_max_halfmax),
	CUTE_REF(dpackut_uint32_decode_min_maxplus1_halfmax),
	CUTE_REF(dpackut_uint32_decode_min_maxminus2_maxminus1),
	CUTE_REF(dpackut_uint32_decode_min_maxminus1_maxminus1),
	CUTE_REF(dpackut_uint32_decode_min_max_maxminus1),
	CUTE_REF(dpackut_uint32_decode_min_maxplus1_maxminus1),

	CUTE_REF(dpackut_uint32_decode_max_assert),
	CUTE_REF(dpackut_uint32_decode_max__1_1),
	CUTE_REF(dpackut_uint32_decode_max_0_1),
	CUTE_REF(dpackut_uint32_decode_max_1_1),
	CUTE_REF(dpackut_uint32_decode_max_halfmax_1),
	CUTE_REF(dpackut_uint32_decode_max_max_1),
	CUTE_REF(dpackut_uint32_decode_max_maxplus1_1),
	CUTE_REF(dpackut_uint32_decode_max__1_halfmax),
	CUTE_REF(dpackut_uint32_decode_max_0_halfmax),
	CUTE_REF(dpackut_uint32_decode_max_1_halfmax),
	CUTE_REF(dpackut_uint32_decode_max_halfmaxminus1_halfmax),
	CUTE_REF(dpackut_uint32_decode_max_halfmax_halfmax),
	CUTE_REF(dpackut_uint32_decode_max_max_halfmax),
	CUTE_REF(dpackut_uint32_decode_max_maxplus1_halfmax),
	CUTE_REF(dpackut_uint32_decode_max_maxminus2_halfmaxminus1),
	CUTE_REF(dpackut_uint32_decode_max_maxminus1_maxminus1),
	CUTE_REF(dpackut_uint32_decode_max_max_maxminus1),
	CUTE_REF(dpackut_uint32_decode_max_maxplus1_maxminus1),

	CUTE_REF(dpackut_uint32_decode_range_assert),
	CUTE_REF(dpackut_uint32_decode_range__1_1_2),
	CUTE_REF(dpackut_uint32_decode_range_0_1_2),
	CUTE_REF(dpackut_uint32_decode_range_1_1_2),
	CUTE_REF(dpackut_uint32_decode_range_2_1_2),
	CUTE_REF(dpackut_uint32_decode_range_3_1_2),
	CUTE_REF(dpackut_uint32_decode_range_1_halfmax_halfmaxplus1),
	CUTE_REF(dpackut_uint32_decode_range_halfmaxminus1_halfmax_halfmaxplus1),
	CUTE_REF(dpackut_uint32_decode_range_halfmax_halfmax_halfmaxplus1),
	CUTE_REF(dpackut_uint32_decode_range_halfmaxplus1_halfmax_halfmaxplus1),
	CUTE_REF(dpackut_uint32_decode_range_halfmaxplus2_halfmax_halfmaxplus1),
	CUTE_REF(dpackut_uint32_decode_range_maxminus3_maxminus2_maxminus1),
	CUTE_REF(dpackut_uint32_decode_range_maxminus2_maxminus2_maxminus1),
	CUTE_REF(dpackut_uint32_decode_range_maxminus1_maxminus2_maxminus1),
	CUTE_REF(dpackut_uint32_decode_range_max_maxminus2_maxminus1),
	CUTE_REF(dpackut_uint32_decode_range_maxplus1_maxminus2_maxminus1)
};

CUTE_SUITE_EXTERN(dpackut_uint32_suite,
                  dpackut_uint32_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
