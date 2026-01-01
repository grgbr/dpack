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

#define DPACKUT_UINT16(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed       = (const uint8_t *)_packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint16 = _value \
	}

static void
dpackut_uint16_encode(const struct dpackut_scalar_data * data)
{
	struct dpack_encoder_buffer enc = { 0, };
	size_t                      sz = data->size;
	uint8_t                     buff[sz];

	memset(buff, 0xa5, sz);
	dpack_encoder_init_buffer(&enc, buff, sz);

	cute_check_uint(data->size, greater_equal, DPACK_UINT16_SIZE_MIN);
	cute_check_uint(data->size, lower_equal, DPACK_UINT16_SIZE_MAX);

	cute_check_sint(dpack_encode_uint16(&enc.base, data->value.uint16),
	                equal,
	                data->error);
	cute_check_mem(buff, equal, data->packed, sz);

	cute_check_uint(dpack_encoder_space_used(&enc.base), equal, sz);
	cute_check_uint(dpack_encoder_space_left(&enc.base), equal, 0);

	dpack_encoder_fini(&enc.base);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint16_encode_assert)
{
	uint16_t                    val = false;
	int                         ret __unused;
#if defined(CONFIG_DPACK_DEBUG)
	struct dpack_encoder_buffer enc = { 0, };

	cute_expect_assertion(ret = dpack_encode_uint16(&enc.base, val));
#endif /* defined(CONFIG_DPACK_DEBUG) */
	cute_expect_assertion(ret = dpack_encode_uint16(NULL, val));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint16_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint16_encode_0)
{
	/* 0 */
	DPACKUT_UINT16(data, "\x00", 0, UINT16_C(0));
	dpackut_uint16_encode(&data);
}

CUTE_TEST(dpackut_uint16_encode_1)
{
	/* 1 */
	DPACKUT_UINT16(data, "\x01", 0, UINT16_C(1));
	dpackut_uint16_encode(&data);
}

CUTE_TEST(dpackut_uint16_encode_65535)
{
	/* 65535 */
	DPACKUT_UINT16(data, "\xcd\xff\xff", 0, UINT16_MAX);
	dpackut_uint16_encode(&data);
}

static void
dpackut_uint16_decode(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder_buffer dec = { 0, };
	uint16_t                    val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint16(&dec.base, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint16);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT16_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT16_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec.base), equal, 0);
	}

	dpack_decoder_fini(&dec.base);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint16_decode_assert)
{
	uint16_t                    val;
	struct dpack_decoder_buffer dec = { 0, };
	uint8_t                     buff[DPACK_UINT16_SIZE_MAX];
	int                         ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint16(NULL, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_uint16(&dec.base, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint16(&dec.base, NULL));
	dpack_decoder_fini(&dec.base);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint16_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint16_decode__1)
{
	/* -1 */
	DPACKUT_UINT16(data, "\xff", -ENOMSG, UINT16_C(0));
	dpackut_uint16_decode(&data);
}

CUTE_TEST(dpackut_uint16_decode_0)
{
	/* 0 */
	DPACKUT_UINT16(data, "\x00", 0, UINT16_C(0));
	dpackut_uint16_decode(&data);
}

CUTE_TEST(dpackut_uint16_decode_1)
{
	/* 1 */
	DPACKUT_UINT16(data, "\x01", 0, UINT16_C(1));
	dpackut_uint16_decode(&data);
}

CUTE_TEST(dpackut_uint16_decode_65535)
{
	/* 65535 */
	DPACKUT_UINT16(data, "\xcd\xff\xff", 0, UINT16_MAX);
	dpackut_uint16_decode(&data);
}

CUTE_TEST(dpackut_uint16_decode_65536)
{
	/* 65536 */
	DPACKUT_UINT16(data, "\xce\x00\x01\x00\x00", -ENOMSG, UINT16_C(0));
	dpackut_uint16_decode(&data);
}

#define DPACKUT_UINT16_MIN(_var, _packed, _error, _value, _low) \
	const struct dpackut_scalar_data _var = { \
		.packed       = (const uint8_t *)_packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint16 = _value, \
		.low.uint16   = _low \
	}

static void
dpackut_uint16_decode_min(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder_buffer dec = { 0, };
	uint16_t                    val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint16_min(&dec.base,
	                                        data->low.uint16,
	                                        &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint16);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT16_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT16_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec.base), equal, 0);
	}

	dpack_decoder_fini(&dec.base);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint16_decode_min_assert)
{
	uint16_t                    val;
	struct dpack_decoder_buffer dec = { 0, };
	uint8_t                     buff[DPACK_UINT16_SIZE_MAX];
	int                         ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint16_min(NULL, 1, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_uint16_min(&dec.base,
	                                                    1,
	                                                    &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint16_min(&dec.base,
	                                                    0,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_uint16_min(&dec.base,
	                                                    UINT16_MAX,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_uint16_min(&dec.base,
	                                                    1,
	                                                    NULL));
	dpack_decoder_fini(&dec.base);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint16_decode_min_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint16_decode_min__1_1)
{
	/* -1 */
	DPACKUT_UINT16_MIN(data, "\xff", -ENOMSG, 0, UINT16_C(1));
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_0_1)
{
	/* 0 */
	DPACKUT_UINT16_MIN(data, "\x00", -ERANGE, 0, UINT16_C(1));
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_1_1)
{
	/* 1 */
	DPACKUT_UINT16_MIN(data, "\x01", 0, UINT16_C(1), UINT16_C(1));
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_32767_1)
{
	/* 32767 */
	DPACKUT_UINT16_MIN(data, "\xcd\x7f\xff",
	                   0,
	                   UINT16_MAX / 2,
	                   UINT16_C(1));
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_65535_1)
{
	/* 65535 */
	DPACKUT_UINT16_MIN(data, "\xcd\xff\xff", 0, UINT16_MAX, UINT16_C(1));
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_65536_1)
{
	/* 65536 */
	DPACKUT_UINT16_MIN(data,
	                   "\xce\x00\x01\x00\x00",
	                   -ENOMSG,
	                   0,
	                   UINT16_C(1));
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min__1_32767)
{
	/* -1 */
	DPACKUT_UINT16_MIN(data, "\xff", -ENOMSG, 0, UINT16_MAX / 2);
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_0_32767)
{
	/* 0 */
	DPACKUT_UINT16_MIN(data, "\x00", -ERANGE, 0, UINT16_MAX / 2);
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_1_32767)
{
	/* 1 */
	DPACKUT_UINT16_MIN(data, "\x01", -ERANGE, 0, UINT16_MAX / 2);
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_32766_32767)
{
	/* 32766 */
	DPACKUT_UINT16_MIN(data, "\xcd\x7f\xfe", -ERANGE, 0, UINT16_MAX / 2);
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_32767_32767)
{
	/* 32767 */
	DPACKUT_UINT16_MIN(data,
	                   "\xcd\x7f\xff",
	                   0,
	                   UINT16_MAX / 2,
	                   UINT16_MAX / 2);
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_65535_32767)
{
	/* 65535 */
	DPACKUT_UINT16_MIN(data, "\xcd\xff\xff", 0, UINT16_MAX, UINT16_MAX / 2);
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_65536_32767)
{
	/* 65536 */
	DPACKUT_UINT16_MIN(data,
	                   "\xce\x00\x01\x00\x00",
	                   -ENOMSG,
	                   0,
	                   UINT16_MAX / 2);
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_65533_65534)
{
	/* 65533 */
	DPACKUT_UINT16_MIN(data, "\xcd\xff\xfd", -ERANGE, 0, UINT16_MAX - 1);
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_65534_65534)
{
	/* 65534 */
	DPACKUT_UINT16_MIN(data,
	                   "\xcd\xff\xfe",
	                   0,
	                   UINT16_MAX - 1,
	                   UINT16_MAX - 1);
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min__65535_65534)
{
	/* 65535 */
	DPACKUT_UINT16_MIN(data,
	                   "\xcd\xff\xff",
	                   0,
	                   UINT16_MAX,
	                   UINT16_MAX - 1);
	dpackut_uint16_decode_min(&data);
}

CUTE_TEST(dpackut_uint16_decode_min_65536_65534)
{
	/* 65536 */
	DPACKUT_UINT16_MIN(data,
	                   "\xce\x00\x01\x00\x00",
	                   -ENOMSG,
	                   0,
	                   UINT16_MAX - 1);
	dpackut_uint16_decode_min(&data);
}

#define DPACKUT_UINT16_MAX(_var, _packed, _error, _value, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed       = (const uint8_t *)_packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint16 = _value, \
		.high.uint16  = _high \
	}

static void
dpackut_uint16_decode_max(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder_buffer dec = { 0, };
	uint16_t                    val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint16_max(&dec.base,
	                                        data->high.uint16,
	                                        &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint16);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT16_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT16_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec.base), equal, 0);
	}

	dpack_decoder_fini(&dec.base);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint16_decode_max_assert)
{
	uint16_t                    val;
	struct dpack_decoder_buffer dec = { 0, };
	uint8_t                     buff[DPACK_UINT16_SIZE_MAX];
	int                         ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint16_max(NULL, 1, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_uint16_max(&dec.base,
	                                                    1,
	                                                    &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint16_max(&dec.base,
	                                                    0,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_uint16_max(&dec.base,
	                                                    UINT16_MAX,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_uint16_max(&dec.base,
	                                                    1,
	                                                    NULL));
	dpack_decoder_fini(&dec.base);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint16_decode_max_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint16_decode_max__1_1)
{
	/* -1 */
	DPACKUT_UINT16_MAX(data, "\xff", -ENOMSG, 0, UINT16_C(1));
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_0_1)
{
	/* 0 */
	DPACKUT_UINT16_MAX(data, "\x00", 0, UINT16_C(0), UINT16_C(1));
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_1_1)
{
	/* 1 */
	DPACKUT_UINT16_MAX(data, "\x01", 0, UINT16_C(1), UINT16_C(1));
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_255_1)
{
	/* 255 */
	DPACKUT_UINT16_MAX(data, "\xcc\xff", -ERANGE, 0, UINT16_C(1));
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_256_1)
{
	/* 256 */
	DPACKUT_UINT16_MAX(data, "\xcd\x01\x00", -ERANGE, 0, UINT16_C(1));
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_1_32767)
{
	/* 1 */
	DPACKUT_UINT16_MAX(data, "\x01", 0, UINT16_C(1), UINT16_MAX / 2);
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_32766_32767)
{
	/* 32766 */
	DPACKUT_UINT16_MAX(data,
	                   "\xcd\x7f\xfe",
	                   0,
	                   (UINT16_MAX / 2) - 1,
	                   UINT16_MAX / 2);
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_32767_32767)
{
	/* 32767 */
	DPACKUT_UINT16_MAX(data,
	                   "\xcd\x7f\xff",
	                   0,
	                   UINT16_MAX / 2,
	                   UINT16_MAX / 2);
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_32768_32767)
{
	/* 32768 */
	DPACKUT_UINT16_MAX(data,
	                   "\xcd\x80\x00",
	                   -ERANGE,
	                   0,
	                   UINT16_MAX / 2);
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_65535_32767)
{
	/* 65535 */
	DPACKUT_UINT16_MAX(data,
	                   "\xcd\xff\xff",
	                   -ERANGE,
	                   0,
	                   UINT16_MAX / 2);
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_65533_65534)
{
	/* 65533 */
	DPACKUT_UINT16_MAX(data,
	                   "\xcd\xff\xfd",
	                   0,
	                   UINT16_MAX - 2,
	                   UINT16_MAX - 1);
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_65534_65534)
{
	/* 65534 */
	DPACKUT_UINT16_MAX(data,
	                   "\xcd\xff\xfe",
	                   0,
	                   UINT16_MAX - 1,
	                   UINT16_MAX - 1);
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_65535_65534)
{
	/* 65535 */
	DPACKUT_UINT16_MAX(data,
	                   "\xcd\xff\xff",
	                   -ERANGE,
	                   0,
	                   UINT16_MAX - 1);
	dpackut_uint16_decode_max(&data);
}

CUTE_TEST(dpackut_uint16_decode_max_65536_65534)
{
	/* 65536 */
	DPACKUT_UINT16_MAX(data,
	                   "\xce\x00\x01\x00\x00" ,
	                   -ENOMSG,
	                   0,
	                   UINT16_MAX - 1);
	dpackut_uint16_decode_max(&data);
}

#define DPACKUT_UINT16_RANGE(_var, _packed, _error, _value, _low, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed       = (const uint8_t *)_packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint16 = _value, \
		.low.uint16   = _low, \
		.high.uint16  = _high \
	}

static void
dpackut_uint16_decode_range(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder_buffer dec = { 0, };
	uint16_t                    val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint16_range(&dec.base,
	                                          data->low.uint16,
	                                          data->high.uint16,
	                                          &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint16);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT16_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT16_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec.base), equal, 0);
	}

	dpack_decoder_fini(&dec.base);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint16_decode_range_assert)
{
	uint16_t                    val;
	struct dpack_decoder_buffer dec = { 0, };
	uint8_t                     buff[DPACK_UINT16_SIZE_MAX];
	int                         ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint16_range(NULL,
	                                                      1,
	                                                      2,
	                                                      &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_uint16_range(&dec.base,
	                                                      1,
	                                                      2,
	                                                      &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint16_range(&dec.base,
	                                                      0,
	                                                      2,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint16_range(&dec.base,
	                                                      1,
	                                                      UINT16_MAX,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint16_range(&dec.base,
	                                                      2,
	                                                      2,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint16_range(&dec.base,
	                                                      1,
	                                                      2,
	                                                      NULL));
	dpack_decoder_fini(&dec.base);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint16_decode_range_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint16_decode_range__1_1_2)
{
	/* -1 */
	DPACKUT_UINT16_RANGE(data,
	                     "\xff",
	                     -ENOMSG,
	                     0,
	                     UINT16_C(1),
	                     UINT16_C(2));
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_0_1_2)
{
	/* 0 */
	DPACKUT_UINT16_RANGE(data,
	                     "\x00",
	                     -ERANGE,
	                     0,
	                     UINT16_C(1),
	                     UINT16_C(2));
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_1_1_2)
{
	/* 1 */
	DPACKUT_UINT16_RANGE(data,
	                     "\x01",
	                     0,
	                     UINT16_C(1),
	                     UINT16_C(1),
	                     UINT16_C(2));
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_2_1_2)
{
	/* 2 */
	DPACKUT_UINT16_RANGE(data,
	                     "\x02",
	                     0,
	                     UINT16_C(2),
	                     UINT16_C(1),
	                     UINT16_C(2));
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_3_1_2)
{
	/* 3 */
	DPACKUT_UINT16_RANGE(data,
	                     "\x03",
	                     -ERANGE,
	                     0,
	                     UINT16_C(1),
	                     UINT16_C(2));
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_1_32767_32768)
{
	/* 1 */
	DPACKUT_UINT16_RANGE(data,
	                     "\x01",
	                     -ERANGE,
	                     0,
	                     UINT16_MAX / 2,
	                     (UINT16_MAX / 2) + 1);
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_32766_32767_32768)
{
	/* 32766 */
	DPACKUT_UINT16_RANGE(data,
	                     "\xcd\x7f\xfe",
	                     -ERANGE,
	                     0,
	                     UINT16_MAX / 2,
	                     (UINT16_MAX / 2) + 1);
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_32767_32767_32768)
{
	/* 32767 */
	DPACKUT_UINT16_RANGE(data,
	                     "\xcd\x7f\xff",
	                     0,
	                     UINT16_MAX / 2,
	                     UINT16_MAX / 2,
	                     (UINT16_MAX / 2) + 1);
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_32768_32767_32768)
{
	/* 32768 */
	DPACKUT_UINT16_RANGE(data,
	                     "\xcd\x80\x00",
	                     0,
	                     (UINT16_MAX / 2) + 1,
	                     UINT16_MAX / 2,
	                     (UINT16_MAX / 2) + 1);
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_32769_32767_32768)
{
	/* 32769 */
	DPACKUT_UINT16_RANGE(data,
	                     "\xcd\x80\x01",
	                     -ERANGE,
	                     0,
	                     UINT16_MAX / 2,
	                     (UINT16_MAX / 2) + 1);
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_65532_65533_65534)
{
	/* 65532 */
	DPACKUT_UINT16_RANGE(data,
	                     "\xcd\xff\xfc",
	                     -ERANGE,
	                     0,
	                     UINT16_MAX - 2,
	                     UINT16_MAX - 1);
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_65533_65533_65534)
{
	/* 65533 */
	DPACKUT_UINT16_RANGE(data,
	                     "\xcd\xff\xfd",
	                     0,
	                     UINT16_MAX - 2,
	                     UINT16_MAX - 2,
	                     UINT16_MAX - 1);
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_65534_65533_65534)
{
	/* 65534 */
	DPACKUT_UINT16_RANGE(data,
	                     "\xcd\xff\xfe",
	                     0,
	                     UINT16_MAX - 1,
	                     UINT16_MAX - 2,
	                     UINT16_MAX - 1);
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_65535_65533_65534)
{
	/* 65535 */
	DPACKUT_UINT16_RANGE(data,
	                     "\xcd\xff\xff",
	                     -ERANGE,
	                     0,
	                     UINT16_MAX - 2,
	                     UINT16_MAX - 1);
	dpackut_uint16_decode_range(&data);
}

CUTE_TEST(dpackut_uint16_decode_range_65536_65533_65534)
{
	/* 65536 */
	DPACKUT_UINT16_RANGE(data,
	                     "\xce\x00\x01\x00\x00" ,
	                     -ENOMSG,
	                     0,
	                     UINT16_MAX - 2,
	                     UINT16_MAX - 1);
	dpackut_uint16_decode_range(&data);
}

CUTE_GROUP(dpackut_uint16_group) = {
	CUTE_REF(dpackut_uint16_encode_assert),
	CUTE_REF(dpackut_uint16_encode_0),
	CUTE_REF(dpackut_uint16_encode_1),
	CUTE_REF(dpackut_uint16_encode_65535),

	CUTE_REF(dpackut_uint16_decode_assert),
	CUTE_REF(dpackut_uint16_decode__1),
	CUTE_REF(dpackut_uint16_decode_0),
	CUTE_REF(dpackut_uint16_decode_1),
	CUTE_REF(dpackut_uint16_decode_65535),
	CUTE_REF(dpackut_uint16_decode_65536),

	CUTE_REF(dpackut_uint16_decode_min_assert),
	CUTE_REF(dpackut_uint16_decode_min__1_1),
	CUTE_REF(dpackut_uint16_decode_min_0_1),
	CUTE_REF(dpackut_uint16_decode_min_1_1),
	CUTE_REF(dpackut_uint16_decode_min_32767_1),
	CUTE_REF(dpackut_uint16_decode_min_65535_1),
	CUTE_REF(dpackut_uint16_decode_min_65536_1),
	CUTE_REF(dpackut_uint16_decode_min__1_32767),
	CUTE_REF(dpackut_uint16_decode_min_0_32767),
	CUTE_REF(dpackut_uint16_decode_min_1_32767),
	CUTE_REF(dpackut_uint16_decode_min_32766_32767),
	CUTE_REF(dpackut_uint16_decode_min_32767_32767),
	CUTE_REF(dpackut_uint16_decode_min_65535_32767),
	CUTE_REF(dpackut_uint16_decode_min_65536_32767),
	CUTE_REF(dpackut_uint16_decode_min_65533_65534),
	CUTE_REF(dpackut_uint16_decode_min_65534_65534),
	CUTE_REF(dpackut_uint16_decode_min__65535_65534),
	CUTE_REF(dpackut_uint16_decode_min_65536_65534),

	CUTE_REF(dpackut_uint16_decode_max_assert),
	CUTE_REF(dpackut_uint16_decode_max__1_1),
	CUTE_REF(dpackut_uint16_decode_max_0_1),
	CUTE_REF(dpackut_uint16_decode_max_1_1),
	CUTE_REF(dpackut_uint16_decode_max_255_1),
	CUTE_REF(dpackut_uint16_decode_max_256_1),
	CUTE_REF(dpackut_uint16_decode_max_1_32767),
	CUTE_REF(dpackut_uint16_decode_max_32766_32767),
	CUTE_REF(dpackut_uint16_decode_max_32767_32767),
	CUTE_REF(dpackut_uint16_decode_max_32768_32767),
	CUTE_REF(dpackut_uint16_decode_max_65535_32767),
	CUTE_REF(dpackut_uint16_decode_max_65533_65534),
	CUTE_REF(dpackut_uint16_decode_max_65534_65534),
	CUTE_REF(dpackut_uint16_decode_max_65535_65534),
	CUTE_REF(dpackut_uint16_decode_max_65536_65534),

	CUTE_REF(dpackut_uint16_decode_range_assert),
	CUTE_REF(dpackut_uint16_decode_range__1_1_2),
	CUTE_REF(dpackut_uint16_decode_range_0_1_2),
	CUTE_REF(dpackut_uint16_decode_range_1_1_2),
	CUTE_REF(dpackut_uint16_decode_range_2_1_2),
	CUTE_REF(dpackut_uint16_decode_range_3_1_2),
	CUTE_REF(dpackut_uint16_decode_range_1_32767_32768),
	CUTE_REF(dpackut_uint16_decode_range_32766_32767_32768),
	CUTE_REF(dpackut_uint16_decode_range_32767_32767_32768),
	CUTE_REF(dpackut_uint16_decode_range_32768_32767_32768),
	CUTE_REF(dpackut_uint16_decode_range_32769_32767_32768),
	CUTE_REF(dpackut_uint16_decode_range_65532_65533_65534),
	CUTE_REF(dpackut_uint16_decode_range_65533_65533_65534),
	CUTE_REF(dpackut_uint16_decode_range_65534_65533_65534),
	CUTE_REF(dpackut_uint16_decode_range_65535_65533_65534),
	CUTE_REF(dpackut_uint16_decode_range_65536_65533_65534)
};

CUTE_SUITE_EXTERN(dpackut_uint16_suite,
                  dpackut_uint16_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
