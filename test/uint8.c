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

#define DPACKUT_UINT8(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.uint8 = _value \
	}

static void
dpackut_uint8_encode(const struct dpackut_scalar_data * data)
{
	struct dpack_encoder enc = { 0, };
	size_t               sz = data->size;
	char                 buff[sz];

	memset(buff, 0xa5, sz);
	dpack_encoder_init_buffer(&enc, buff, sz);

	cute_check_uint(data->size, greater_equal, DPACK_UINT8_SIZE_MIN);
	cute_check_uint(data->size, lower_equal, DPACK_UINT8_SIZE_MAX);

	cute_check_sint(dpack_encode_uint8(&enc, data->value.uint8),
	                equal,
	                data->error);
	cute_check_mem(buff, equal, data->packed, sz);

	cute_check_uint(dpack_encoder_space_used(&enc), equal, sz);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint8_encode_assert)
{
	uint8_t              val = false;
	int                  ret __unused;
#if defined(CONFIG_DPACK_DEBUG)
	struct dpack_encoder enc = { 0, };

	cute_expect_assertion(ret = dpack_encode_uint8(&enc, val));
#endif /* defined(CONFIG_DPACK_DEBUG) */
	cute_expect_assertion(ret = dpack_encode_uint8(NULL, val));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint8_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint8_encode_0)
{
	/* 0 */
	DPACKUT_UINT8(data, "\x00", 0, UINT8_C(0));
	dpackut_uint8_encode(&data);
}

CUTE_TEST(dpackut_uint8_encode_1)
{
	/* 1 */
	DPACKUT_UINT8(data, "\x01", 0, UINT8_C(1));
	dpackut_uint8_encode(&data);
}

CUTE_TEST(dpackut_uint8_encode_max)
{
	/* 255 */
	DPACKUT_UINT8(data, "\xcc\xff", 0, UINT8_MAX);
	dpackut_uint8_encode(&data);
}

static void
dpackut_uint8_decode(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	uint8_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint8(&dec, &val), equal, data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint8);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT8_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT8_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint8_decode_assert)
{
	uint8_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_UINT8_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint8(NULL, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_uint8(&dec, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint8(&dec, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint8_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint8_decode__1)
{
	/* -1 */
	DPACKUT_UINT8(data, "\xff", -ENOMSG, 0);
	dpackut_uint8_decode(&data);
}

CUTE_TEST(dpackut_uint8_decode_0)
{
	/* 0 */
	DPACKUT_UINT8(data, "\x00", 0, UINT8_C(0));
	dpackut_uint8_decode(&data);
}

CUTE_TEST(dpackut_uint8_decode_1)
{
	/* 1 */
	DPACKUT_UINT8(data, "\x01", 0, UINT8_C(1));
	dpackut_uint8_decode(&data);
}

CUTE_TEST(dpackut_uint8_decode_255)
{
	/* 255 */
	DPACKUT_UINT8(data, "\xcc\xff", 0, UINT8_MAX);
	dpackut_uint8_decode(&data);
}

CUTE_TEST(dpackut_uint8_decode_256)
{
	/* 256 */
	DPACKUT_UINT8(data, "\xcd\x01\x00", -ERANGE, 0);
	dpackut_uint8_decode(&data);
}

#define DPACKUT_UINT8_MIN(_var, _packed, _error, _value, _low) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.uint8 = _value, \
		.low.uint8   = _low \
	}

static void
dpackut_uint8_decode_min(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	uint8_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint8_min(&dec, data->low.uint8, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint8);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT8_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT8_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint8_decode_min_assert)
{
	uint8_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_UINT8_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint8_min(NULL, 1, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_uint8_min(&dec, 1, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint8_min(&dec, 0, &val));
	cute_expect_assertion(ret = dpack_decode_uint8_min(&dec,
	                                                   UINT8_MAX,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_uint8_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint8_decode_min_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint8_decode_min__1_1)
{
	/* -1 */
	DPACKUT_UINT8_MIN(data, "\xff", -ENOMSG, 0, UINT8_C(1));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_0_1)
{
	/* 0 */
	DPACKUT_UINT8_MIN(data, "\x00", -ERANGE, 0, UINT8_C(1));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_1_1)
{
	/* 1 */
	DPACKUT_UINT8_MIN(data, "\x01", 0, UINT8_C(1), UINT8_C(1));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_max_1)
{
	/* 255 */
	DPACKUT_UINT8_MIN(data, "\xcc\xff", 0, UINT8_MAX, UINT8_C(1));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_256_1)
{
	/* 256 */
	DPACKUT_UINT8_MIN(data, "\xcd\x01\x00", -ERANGE, 0, UINT8_C(1));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_1_64)
{
	/* 1 */
	DPACKUT_UINT8_MIN(data, "\x01", -ERANGE, 0, UINT8_C(64));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_63_64)
{
	/* 63 */
	DPACKUT_UINT8_MIN(data, "\x3f", -ERANGE, 0, UINT8_C(64));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_64_64)
{
	/* 64 */
	DPACKUT_UINT8_MIN(data, "\x40", 0, UINT8_C(64), UINT8_C(64));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_65_64)
{
	/* 65 */
	DPACKUT_UINT8_MIN(data, "\x41", 0, UINT8_C(65), UINT8_C(64));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_max_64)
{
	/* 255 */
	DPACKUT_UINT8_MIN(data, "\xcc\xff", 0, UINT8_MAX, UINT8_C(64));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_253_254)
{
	/* 253 */
	DPACKUT_UINT8_MIN(data, "\xcc\xfd", -ERANGE, 0, UINT8_C(254));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_254_254)
{
	/* 254 */
	DPACKUT_UINT8_MIN(data, "\xcc\xfe", 0, UINT8_C(254), UINT8_C(254));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_max_254)
{
	/* 255 */
	DPACKUT_UINT8_MIN(data, "\xcc\xff", 0, UINT8_MAX, UINT8_C(254));
	dpackut_uint8_decode_min(&data);
}

CUTE_TEST(dpackut_uint8_decode_min_256_254)
{
	/* 256 */
	DPACKUT_UINT8_MIN(data, "\xcd\x01\x00", -ERANGE, 0, UINT8_C(254));
	dpackut_uint8_decode_min(&data);
}

#define DPACKUT_UINT8_MAX(_var, _packed, _error, _value, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.uint8 = _value, \
		.high.uint8  = _high \
	}

static void
dpackut_uint8_decode_max(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	uint8_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint8_max(&dec, data->high.uint8, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint8);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT8_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT8_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint8_decode_max_assert)
{
	uint8_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_UINT8_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint8_max(NULL, 1, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_uint8_max(&dec, 1, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint8_max(&dec, 0, &val));
	cute_expect_assertion(ret = dpack_decode_uint8_max(&dec,
	                                                   UINT8_MAX,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_uint8_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint8_decode_max_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint8_decode_max__1_1)
{
	/* -1 */
	DPACKUT_UINT8_MAX(data, "\xff", -ENOMSG, 0, UINT8_C(1));
	dpackut_uint8_decode_max(&data);
}

CUTE_TEST(dpackut_uint8_decode_max_0_1)
{
		/* 0 */
		DPACKUT_UINT8_MAX(data, "\x00", 0, UINT8_C(0), UINT8_C(1));
		dpackut_uint8_decode_max(&data);
}


CUTE_TEST(dpackut_uint8_decode_max_1_1)
{
		/* 1 */
		DPACKUT_UINT8_MAX(data, "\x01", 0, UINT8_C(1), UINT8_C(1));
		dpackut_uint8_decode_max(&data);
}

CUTE_TEST(dpackut_uint8_decode_max_255_1)
{
		/* 255 */
		DPACKUT_UINT8_MAX(data, "\xcc\xff", -ERANGE, 0, UINT8_C(1));
		dpackut_uint8_decode_max(&data);
}

CUTE_TEST(dpackut_uint8_decode_max_256_1)
{
		/* 256 */
		DPACKUT_UINT8_MAX(data, "\xcd\x01\x00", -ERANGE, 0, UINT8_C(1));
		dpackut_uint8_decode_max(&data);
}

CUTE_TEST(dpackut_uint8_decode_max_1_64)
{
		/* 1 */
		DPACKUT_UINT8_MAX(data, "\x01", 0, UINT8_C(1), UINT8_C(64));
		dpackut_uint8_decode_max(&data);
}

CUTE_TEST(dpackut_uint8_decode_max_63_64)
{
		/* 63 */
		DPACKUT_UINT8_MAX(data, "\x3f", 0, UINT8_C(63), UINT8_C(64));
		dpackut_uint8_decode_max(&data);
}

CUTE_TEST(dpackut_uint8_decode_max_64_64)
{
		/* 64 */
		DPACKUT_UINT8_MAX(data, "\x40", 0, UINT8_C(64), UINT8_C(64));
		dpackut_uint8_decode_max(&data);
}

CUTE_TEST(dpackut_uint8_decode_max_65_64)
{
		/* 65 */
		DPACKUT_UINT8_MAX(data, "\x41", -ERANGE, 0, UINT8_C(64));
		dpackut_uint8_decode_max(&data);
}

CUTE_TEST(dpackut_uint8_decode_max_255_64)
{
		/* 255 */
		DPACKUT_UINT8_MAX(data, "\xcc\xff", -ERANGE, 0, UINT8_C(64));
		dpackut_uint8_decode_max(&data);
}

CUTE_TEST(dpackut_uint8_decode_max_253_254)
{
		/* 253 */
		DPACKUT_UINT8_MAX(data,
		                  "\xcc\xfd",
		                  0,
		                  UINT8_C(253),
		                  UINT8_C(254));
		dpackut_uint8_decode_max(&data);
}

CUTE_TEST(dpackut_uint8_decode_max_254_254)
{
		/* 254 */
		DPACKUT_UINT8_MAX(data,
		                  "\xcc\xfe",
		                  0,
		                  UINT8_C(254),
		                  UINT8_C(254));
		dpackut_uint8_decode_max(&data);
}

CUTE_TEST(dpackut_uint8_decode_max_255_254)
{
		/* 255 */
		DPACKUT_UINT8_MAX(data, "\xcc\xff", -ERANGE, 0, UINT8_C(254));
		dpackut_uint8_decode_max(&data);
}

CUTE_TEST(dpackut_uint8_decode_max_256_254)
{
		/* 256 */
		DPACKUT_UINT8_MAX(data,
		                  "\xcd\x01\x00",
		                  -ERANGE,
		                  0,
		                  UINT8_C(254));
		dpackut_uint8_decode_max(&data);
}

#define DPACKUT_UINT8_RANGE(_var, _packed, _error, _value, _low, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.uint8 = _value, \
		.low.uint8   = _low, \
		.high.uint8  = _high \
	}

static void
dpackut_uint8_decode_range(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	uint8_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint8_range(&dec,
	                                         data->low.uint8,
	                                         data->high.uint8,
	                                         &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint8);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT8_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT8_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint8_decode_range_assert)
{
	uint8_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_UINT8_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint8_range(NULL, 1, 2, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_uint8_range(&dec, 1, 2, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint8_range(&dec, 0, 2, &val));
	cute_expect_assertion(ret = dpack_decode_uint8_range(&dec,
	                                                     1,
	                                                     UINT8_MAX,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_uint8_range(&dec, 2, 2, &val));
	cute_expect_assertion(ret = dpack_decode_uint8_range(&dec, 1, 2, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint8_decode_range_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint8_decode_range__1_1_2)
{
	/* -1 */
	DPACKUT_UINT8_RANGE(data, "\xff", -ENOMSG, 0, UINT8_C(1), UINT8_C(2));
	dpackut_uint8_decode_range(&data);
}


CUTE_TEST(dpackut_uint8_decode_range_0_1_2)
{
	/* 0 */
	DPACKUT_UINT8_RANGE(data,
	                    "\x00",
	                    -ERANGE,
	                    0,
	                    UINT8_C(1),
	                    UINT8_C(2));
	dpackut_uint8_decode_range(&data);
}

CUTE_TEST(dpackut_uint8_decode_range_1_1_2)
{
	/* 1 */
	DPACKUT_UINT8_RANGE(data,
	                    "\x01",
	                    0,
	                    UINT8_C(1),
	                    UINT8_C(1),
	                    UINT8_C(2));
	dpackut_uint8_decode_range(&data);
}

CUTE_TEST(dpackut_uint8_decode_range_2_1_2)
{
	/* 2 */
	DPACKUT_UINT8_RANGE(data,
	                    "\x02",
	                    0,
	                    UINT8_C(2),
	                    UINT8_C(1),
	                    UINT8_C(2));
	dpackut_uint8_decode_range(&data);
}

CUTE_TEST(dpackut_uint8_decode_range_3_1_2)
{
	/* 3 */
	DPACKUT_UINT8_RANGE(data,
	                    "\x03",
	                    -ERANGE,
	                    0,
	                    UINT8_C(1),
	                    UINT8_C(2));
	dpackut_uint8_decode_range(&data);
}

CUTE_TEST(dpackut_uint8_decode_range_255_1_2)
{
	/* 255 */
	DPACKUT_UINT8_RANGE(data,
	                    "\xcc\xff",
	                    -ERANGE,
	                    0,
	                    UINT8_C(1),
	                    UINT8_C(2));
	dpackut_uint8_decode_range(&data);
}

CUTE_TEST(dpackut_uint8_decode_range_256_1_2)
{
	/* 256 */
	DPACKUT_UINT8_RANGE(data,
	                    "\xcd\x01\x00",
	                    -ERANGE,
	                    0,
	                    UINT8_C(1),
	                    UINT8_C(2));
	dpackut_uint8_decode_range(&data);
}

CUTE_TEST(dpackut_uint8_decode_range_252_253_254)
{
	/* 252 */
	DPACKUT_UINT8_RANGE(data,
	                    "\xcc\xfc",
	                    -ERANGE,
	                    0,
	                    UINT8_C(253),
	                    UINT8_C(254));
	dpackut_uint8_decode_range(&data);
}

CUTE_TEST(dpackut_uint8_decode_range_253_253_254)
{
	/* 253 */
	DPACKUT_UINT8_RANGE(data,
	                    "\xcc\xfd",
	                    0,
	                    UINT8_C(253),
	                    UINT8_C(253),
	                    UINT8_C(254));
	dpackut_uint8_decode_range(&data);
}

CUTE_TEST(dpackut_uint8_decode_range_254_253_254)
{
	/* 254 */
	DPACKUT_UINT8_RANGE(data,
	                    "\xcc\xfe",
	                    0,
	                    UINT8_C(254),
	                    UINT8_C(253),
	                    UINT8_C(254));
	dpackut_uint8_decode_range(&data);
}

CUTE_TEST(dpackut_uint8_decode_range_255_253_254)
{
	/* 255 */
	DPACKUT_UINT8_RANGE(data,
	                    "\xcc\xff",
	                    -ERANGE,
	                    0,
	                    UINT8_C(253),
	                    UINT8_C(254));
	dpackut_uint8_decode_range(&data);
}

CUTE_TEST(dpackut_uint8_decode_range_256_253_254)
{
	/* 256 */
	DPACKUT_UINT8_RANGE(data,
	                    "\xcd\x01\x00",
	                    -ERANGE,
	                    0,
	                    UINT8_C(253),
	                    UINT8_C(254));
	dpackut_uint8_decode_range(&data);
}

CUTE_GROUP(dpackut_uint8_group) = {
	CUTE_REF(dpackut_uint8_encode_assert),
	CUTE_REF(dpackut_uint8_encode_0),
	CUTE_REF(dpackut_uint8_encode_1),
	CUTE_REF(dpackut_uint8_encode_max),

	CUTE_REF(dpackut_uint8_decode_assert),
	CUTE_REF(dpackut_uint8_decode__1),
	CUTE_REF(dpackut_uint8_decode_0),
	CUTE_REF(dpackut_uint8_decode_1),
	CUTE_REF(dpackut_uint8_decode_255),
	CUTE_REF(dpackut_uint8_decode_256),

	CUTE_REF(dpackut_uint8_decode_min_assert),
	CUTE_REF(dpackut_uint8_decode_min__1_1),
	CUTE_REF(dpackut_uint8_decode_min_0_1),
	CUTE_REF(dpackut_uint8_decode_min_1_1),
	CUTE_REF(dpackut_uint8_decode_min_max_1),
	CUTE_REF(dpackut_uint8_decode_min_256_1),
	CUTE_REF(dpackut_uint8_decode_min_1_64),
	CUTE_REF(dpackut_uint8_decode_min_63_64),
	CUTE_REF(dpackut_uint8_decode_min_64_64),
	CUTE_REF(dpackut_uint8_decode_min_65_64),
	CUTE_REF(dpackut_uint8_decode_min_max_64),
	CUTE_REF(dpackut_uint8_decode_min_253_254),
	CUTE_REF(dpackut_uint8_decode_min_254_254),
	CUTE_REF(dpackut_uint8_decode_min_max_254),
	CUTE_REF(dpackut_uint8_decode_min_256_254),

	CUTE_REF(dpackut_uint8_decode_max_assert),
	CUTE_REF(dpackut_uint8_decode_max__1_1),
	CUTE_REF(dpackut_uint8_decode_max_0_1),
	CUTE_REF(dpackut_uint8_decode_max_1_1),
	CUTE_REF(dpackut_uint8_decode_max_255_1),
	CUTE_REF(dpackut_uint8_decode_max_256_1),
	CUTE_REF(dpackut_uint8_decode_max_1_64),
	CUTE_REF(dpackut_uint8_decode_max_63_64),
	CUTE_REF(dpackut_uint8_decode_max_64_64),
	CUTE_REF(dpackut_uint8_decode_max_65_64),
	CUTE_REF(dpackut_uint8_decode_max_255_64),
	CUTE_REF(dpackut_uint8_decode_max_253_254),
	CUTE_REF(dpackut_uint8_decode_max_254_254),
	CUTE_REF(dpackut_uint8_decode_max_255_254),
	CUTE_REF(dpackut_uint8_decode_max_256_254),

	CUTE_REF(dpackut_uint8_decode_range_assert),
	CUTE_REF(dpackut_uint8_decode_range__1_1_2),
	CUTE_REF(dpackut_uint8_decode_range_0_1_2),
	CUTE_REF(dpackut_uint8_decode_range_1_1_2),
	CUTE_REF(dpackut_uint8_decode_range_2_1_2),
	CUTE_REF(dpackut_uint8_decode_range_3_1_2),
	CUTE_REF(dpackut_uint8_decode_range_255_1_2),
	CUTE_REF(dpackut_uint8_decode_range_256_1_2),
	CUTE_REF(dpackut_uint8_decode_range_252_253_254),
	CUTE_REF(dpackut_uint8_decode_range_253_253_254),
	CUTE_REF(dpackut_uint8_decode_range_254_253_254),
	CUTE_REF(dpackut_uint8_decode_range_255_253_254),
	CUTE_REF(dpackut_uint8_decode_range_256_253_254)
};

CUTE_SUITE_EXTERN(dpackut_uint8_suite,
                  dpackut_uint8_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
