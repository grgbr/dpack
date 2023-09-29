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

#define DPACKUT_BOOL(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed        = _packed, \
		.size          = sizeof(_packed) - 1, \
		.error         = _error, \
		.value.boolean = _value \
	}

static void
dpackut_bool_encode(const struct dpackut_scalar_data * data)
{
	struct dpack_encoder enc = { 0, };
	size_t               sz = data->size;
	char                 buff[sz];

	memset(buff, 0xa5, sz);
	dpack_encoder_init_buffer(&enc, buff, sz);

	cute_check_uint(data->size, equal, DPACK_BOOL_SIZE);
	cute_check_sint(dpack_encode_bool(&enc, data->value.boolean),
	                equal,
	                data->error);
	cute_check_mem(buff, equal, data->packed, sz);

	cute_check_uint(dpack_encoder_space_used(&enc), equal, sz);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_bool_encode_assert)
{
	bool                 val = false;
	int                  ret __unused;
#if defined(CONFIG_DPACK_DEBUG)
	struct dpack_encoder enc = { 0, };

	cute_expect_assertion(ret = dpack_encode_bool(&enc, val));
#endif /* defined(CONFIG_DPACK_DEBUG) */
	cute_expect_assertion(ret = dpack_encode_bool(NULL, val));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_bool_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_bool_encode_true)
{
	/* True */
	DPACKUT_BOOL(data, "\xc3", 0, true);

	dpackut_bool_encode(&data);
}

CUTE_TEST(dpackut_bool_encode_false)
{
	/* False */
	DPACKUT_BOOL(data, "\xc2", 0, false);

	dpackut_bool_encode(&data);
}

static void
dpackut_bool_decode(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	bool                 val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_bool(&dec, &val), equal, data->error);
	if (!data->error) {
		cute_check_bool(val, is, data->value.boolean);
		cute_check_uint(data->size, equal, DPACK_BOOL_SIZE);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_bool_decode_assert)
{
	bool                 val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_BOOL_SIZE];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_bool(NULL, &val));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_bool(&dec, &val));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_bool(&dec, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_bool_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_bool_decode_one)
{
	/* 1 */
	DPACKUT_BOOL(data, "\x01", -ENOMSG, 0);
	dpackut_bool_decode(&data);
}

CUTE_TEST(dpackut_bool_decode_zero)
{
	/* 0 */
	DPACKUT_BOOL(data, "\x00", -ENOMSG, 0);
	dpackut_bool_decode(&data);
}

CUTE_TEST(dpackut_bool_decode_true)
{
	/* True */
	DPACKUT_BOOL(data, "\xc3", 0, true);
	dpackut_bool_decode(&data);
}

CUTE_TEST(dpackut_bool_decode_false)
{
	/* False */
	DPACKUT_BOOL(data, "\xc2", 0, false);
	dpackut_bool_decode(&data);
}

CUTE_GROUP(dpackut_bool_group) = {
	CUTE_REF(dpackut_bool_encode_assert),
	CUTE_REF(dpackut_bool_encode_true),
	CUTE_REF(dpackut_bool_encode_false),
	CUTE_REF(dpackut_bool_decode_assert),
	CUTE_REF(dpackut_bool_decode_one),
	CUTE_REF(dpackut_bool_decode_zero),
	CUTE_REF(dpackut_bool_decode_true),
	CUTE_REF(dpackut_bool_decode_false)
};

CUTE_SUITE_EXTERN(dpackut_bool_suite,
                  dpackut_bool_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
