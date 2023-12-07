/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/string.h"
#include "dpack/codec.h"
#include "utest.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <errno.h>

#if defined(ZERO_STR_SIZE_UTEST)

CUTE_TEST(dpackut_str_zero_size)
{
	/* Should fail to compile since 0 length string are not allowed. */
	int sz = DPACK_STR_SIZE(0);
}

#endif /* defined(ZERO_STR_SIZE_UTEST) */

#if defined(NONCONST_STR_SIZE_UTEST)

static void
CUTE_TEST(dpackut_str_nonconst_size)
{
	unsigned int len;

	/* Should fail to compile since len is not constant. */
	cute_check_uint(DPACK_STR_SIZE(len), equal, 0);
}

#endif /* defined(NONCONST_STR_SIZE_UTEST) */

#if defined(MAXLEN_STR_SIZE_UTEST)

static void
CUTE_TEST(dpackut_str_maxlen_str_size)
{
	/*
	 * Should fail to compile since requested length > maximum string length
	 * allowed.
	 */
	cute_check_uint(DPACK_STR_SIZE(DPACK_STRLEN_MAX + 1), equal, 0);
}

#endif /* defined(MAXLEN_STR_SIZE_UTEST) */

struct dpackut_str_data {
	size_t  len;       /* length of string to encode / decode */
	bool    null_term; /* Value should be null terminated */
	size_t  size;      /* size of encoded string */
	ssize_t error;
	size_t  equ;
	size_t  low;
	size_t  high;
	char *  packed;
	char *  value;
};

#define DPACKUT_STR_DATA(_var, _len, _term) \
	struct dpackut_str_data _var = { \
		.len       = _len, \
		.null_term = _term, \
		.error     = 0 \
	}

typedef void (dpackut_str_pack_fn)(struct dpack_encoder *,
                                   const struct dpackut_str_data *);

typedef void (dpackut_str_unpack_fn)(struct dpack_decoder *,
                                     const struct dpackut_str_data *);

static size_t
dpackut_str_size(size_t len)
{
	switch (len) {
	case 0 ... 31:
		return len + 1;
	case 32 ... 255:
		return len + 2;
	case 256 ... 65535:
		return len + 3;
	case 65536 ... 4294967295:
		return len + 5;
	default:
		cute_fail("unsupported MsgPack string size");
	}

	unreachable();
}

static void
dpackut_str_gen_data(struct dpackut_str_data * data)
{
	unsigned int b;
	char *       p;

	data->size = dpackut_str_size(data->len);
	data->packed = malloc(data->size);
	cute_check_ptr(data->packed, unequal, NULL);

	p = data->packed;
	switch (data->len) {
	case 0 ... 31:
		*p++ = (char)(0xa0 | (data->len & 0x1f));
		break;
	case 32 ... 255:
		*p++ = (char)0xd9;
		*p++ = (char)(data->len & 0xff);
		break;
	case 256 ... 65535:
		*p++ = (char)0xda;
		*p++ = (char)((data->len >> 8) & 0xff);
		*p++ = (char)(data->len & 0xff);
		break;
	case 65536 ... 4294967295:
		*p++ = (char)0xdb;
		*p++ = (char)((data->len >> 24) & 0xff);
		*p++ = (char)((data->len >> 16) & 0xff);
		*p++ = (char)((data->len >> 8) & 0xff);
		*p++ = (char)(data->len & 0xff);
		break;
	default:
		cute_fail("unsupported MsgPack string size");
	}
	for (b = 0; b < data->len; b++)
		*p++ = (char)('0' + (b % 10));

	data->value = malloc(data->len + 1);
	cute_check_ptr(data->value, unequal, NULL);

	p = data->value;
	for (b = 0; b < data->len; b++)
		*p++ = (char)('0' + (b % 10));

	if (data->null_term)
		data->value[data->len] = '\0';
	else
		data->value[data->len] = '\xff';
}

static void
dpackut_str_fini_data(struct dpackut_str_data * data)
{
	free(data->packed);
	free(data->value);
}

CUTE_TEST(dpackut_fixstr_sizes)
{
	cute_check_uint(DPACK_STR_SIZE(1), equal, 2);
	cute_check_uint(dpack_str_size(1), equal, 2);
	cute_check_uint(DPACK_STR_SIZE(2), equal, 3);
	cute_check_uint(dpack_str_size(2), equal, 3);
}

#if DPACK_STRLEN_MAX >= 30

CUTE_TEST(dpackut_fixstr_sizes_30)
{
	cute_check_uint(DPACK_STR_SIZE(30), equal, 31);
	cute_check_uint(dpack_str_size(30), equal, 31);
}

#else  /* !(DPACK_STRLEN_MAX >= 30) */

CUTE_TEST(dpackut_fixstr_sizes_30)
{
	cute_skip("string length >= 30 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 30 */

#if DPACK_STRLEN_MAX >= 31

CUTE_TEST(dpackut_fixstr_sizes_31)
{
	cute_check_uint(DPACK_STR_SIZE(31), equal, 32);
	cute_check_uint(dpack_str_size(31), equal, 32);
}

#else  /* !(DPACK_STRLEN_MAX >= 31) */

CUTE_TEST(dpackut_fixstr_sizes_31)
{
	cute_skip("MsgPack string length >= 31 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 31 */

#if DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX

CUTE_TEST(dpackut_str8_sizes)
{
	/* Msgpack 8 bits string sizes. */
	cute_check_uint(DPACK_STR_SIZE(32), equal, 34);
	cute_check_uint(dpack_str_size(32), equal, 34);
}

#else  /* !(DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX) */

CUTE_TEST(dpackut_str8_sizes)
{
	cute_skip("string length >= MsgPack fixstr length "
	          " support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX */

#if DPACK_STRLEN_MAX >= 33

CUTE_TEST(dpackut_str8_sizes_33)
{
	cute_check_uint(DPACK_STR_SIZE(33), equal, 35);
	cute_check_uint(dpack_str_size(33), equal, 35);
}

#else  /* !(DPACK_STRLEN_MAX >= 33) */

CUTE_TEST(dpackut_str8_sizes_33)
{
	cute_skip("MsgPack string length >= 33 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 33 */

#if DPACK_STRLEN_MAX >= (UINT8_MAX - 2)

CUTE_TEST(dpackut_str8_sizes_253)
{
	cute_check_uint(DPACK_STR_SIZE(UINT8_MAX - 2), equal, UINT8_MAX);
	cute_check_uint(dpack_str_size(UINT8_MAX - 2), equal, UINT8_MAX);
}

#else  /* !(DPACK_STRLEN_MAX >= (UINT8_MAX - 2)) */

CUTE_TEST(dpackut_str8_sizes_253)
{
	cute_skip("MsgPack string length >= (UINT8_MAX - 2) support "
	          "not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= (UINT8_MAX - 2) */

#if DPACK_STRLEN_MAX >= (UINT8_MAX - 1)

CUTE_TEST(dpackut_str8_sizes_254)
{
	cute_check_uint(DPACK_STR_SIZE(UINT8_MAX - 1), equal, UINT8_MAX + 1);
	cute_check_uint(dpack_str_size(UINT8_MAX - 1), equal, UINT8_MAX + 1);
}

#else  /* !(DPACK_STRLEN_MAX >= (UINT8_MAX - 1)) */

CUTE_TEST(dpackut_str8_sizes_254)
{
	cute_skip("MsgPack string length >= (UINT8_MAX - 1) support "
	          "not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= (UINT8_MAX - 1) */

#if DPACK_STRLEN_MAX >= UINT8_MAX

CUTE_TEST(dpackut_str8_sizes_255)
{
	cute_check_uint(DPACK_STR_SIZE(UINT8_MAX), equal, UINT8_MAX + 2);
	cute_check_uint(dpack_str_size(UINT8_MAX), equal, UINT8_MAX + 2);
}

#else  /* !(DPACK_STRLEN_MAX >= UINT8_MAX) */

CUTE_TEST(dpackut_str8_sizes_255)
{
	cute_skip("MsgPack string length >= UINT8_MAX support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= UINT8_MAX */

#if DPACK_STRLEN_MAX > DPACK_STR8_LEN_MAX

CUTE_TEST(dpackut_str16_sizes)
{
	/* Msgpack 16 bits string sizes. */
	cute_check_uint(DPACK_STR_SIZE(UINT8_MAX + 1),
	                equal,
	                UINT8_MAX + 1 + 3);
	cute_check_uint(dpack_str_size(UINT8_MAX + 1),
	                equal,
	                UINT8_MAX + 1 + 3);
}

#else  /* !(DPACK_STRLEN_MAX > DPACK_STR8_LEN_MAX) */

CUTE_TEST(dpackut_str16_sizes)
{
	cute_skip("string length >= MsgPack str8 length "
	          " support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX > DPACK_STR8_LEN_MAX */

#if DPACK_STRLEN_MAX >= (UINT8_MAX + 2)

CUTE_TEST(dpackut_str16_sizes_257)
{
	cute_check_uint(DPACK_STR_SIZE(UINT8_MAX + 2),
	                equal,
	                UINT8_MAX + 2 + 3);
	cute_check_uint(dpack_str_size(UINT8_MAX + 2),
	                equal,
	                UINT8_MAX + 2 + 3);
}

#else  /* !(DPACK_STRLEN_MAX >= (UINT8_MAX + 2)) */

CUTE_TEST(dpackut_str16_sizes_257)
{
	cute_skip("MsgPack string length >= (UINT8_MAX + 2) support "
	          "not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= (UINT8_MAX + 2) */

#if DPACK_STRLEN_MAX >= (UINT16_MAX - 2)

CUTE_TEST(dpackut_str16_sizes_65533)
{
	cute_check_uint(DPACK_STR_SIZE(UINT16_MAX - 2),
	                equal,
	                UINT16_MAX - 2 + 3);
	cute_check_uint(dpack_str_size(UINT16_MAX - 2),
	                equal,
	                UINT16_MAX - 2 + 3);
}

#else  /* !(DPACK_STRLEN_MAX >= (UINT16_MAX - 2)) */

CUTE_TEST(dpackut_str16_sizes_65533)
{
	cute_skip("MsgPack string length >= (UINT16_MAX - 2) support "
	          "not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= (UINT16_MAX - 2) */

#if DPACK_STRLEN_MAX >= (UINT16_MAX - 1)

CUTE_TEST(dpackut_str16_sizes_65534)
{
	cute_check_uint(DPACK_STR_SIZE(UINT16_MAX - 1),
	                equal,
	                UINT16_MAX - 1 + 3);
	cute_check_uint(dpack_str_size(UINT16_MAX - 1),
	                equal,
	                UINT16_MAX - 1 + 3);
}

#else  /* !(DPACK_STRLEN_MAX >= (UINT16_MAX - 1)) */

CUTE_TEST(dpackut_str16_sizes_65534)
{
	cute_skip("MsgPack string length >= (UINT16_MAX - 1) support "
	          "not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= (UINT16_MAX - 1) */

#if DPACK_STRLEN_MAX >= UINT16_MAX

CUTE_TEST(dpackut_str16_sizes_65535)
{
	cute_check_uint(DPACK_STR_SIZE(UINT16_MAX), equal, UINT16_MAX + 3);
	cute_check_uint(dpack_str_size(UINT16_MAX), equal, UINT16_MAX + 3);
}

#else  /* !(DPACK_STRLEN_MAX >= UINT16_MAX) */

CUTE_TEST(dpackut_str16_sizes_65535)
{
	cute_skip("MsgPack string length >= UINT16_MAX support "
	          "not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= UINT16_MAX */

#if DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX

CUTE_TEST(dpackut_str32_sizes)
{
	/* Msgpack 32 bits string sizes. */
	cute_check_uint(DPACK_STR_SIZE(UINT16_MAX + 1),
	                equal,
	                UINT16_MAX + 1 + 5);
	cute_check_uint(dpack_str_size(UINT16_MAX + 1),
	                equal,
	                UINT16_MAX + 1 + 5);
}

#else  /* !(DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX) */

CUTE_TEST(dpackut_str32_sizes)
{
	cute_skip("string length >= MsgPack str16 length "
	          " support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX */

#if DPACK_STRLEN_MAX >= (UINT16_MAX + 2)

CUTE_TEST(dpackut_str32_sizes_65537)
{
	cute_check_uint(DPACK_STR_SIZE(UINT16_MAX + 2),
	                equal,
	                UINT16_MAX + 2 + 5);
	cute_check_uint(dpack_str_size(UINT16_MAX + 2),
	                equal,
	                UINT16_MAX + 2 + 5);
}

#else  /* !(DPACK_STRLEN_MAX >= (UINT16_MAX + 2)) */

CUTE_TEST(dpackut_str32_sizes_65537)
{
	cute_skip("MsgPack string length >= (UINT16_MAX + 2) support "
	          "not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= (UINT16_MAX + 2) */

#if DPACK_STRLEN_MAX >= (UINT32_MAX - 2)

CUTE_TEST(dpackut_str32_sizes_maxminus2)
{
	cute_check_uint(DPACK_STR_SIZE(UINT32_MAX - 2),
	                equal,
	                (uintmax_t)UINT32_MAX - 2 + 5);
	cute_check_uint(dpack_str_size(UINT32_MAX - 2),
	                equal,
	                (uintmax_t)UINT32_MAX - 2 + 5);
}

#else  /* !(DPACK_STRLEN_MAX >= (UINT32_MAX - 2)) */

CUTE_TEST(dpackut_str32_sizes_maxminus2)
{
	cute_skip("MsgPack string length >= (UINT32_MAX - 2) support "
	          "not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= (UINT32_MAX - 2) */

#if DPACK_STRLEN_MAX >= (UINT32_MAX - 1)

CUTE_TEST(dpackut_str32_sizes_maxminus1)
{
	cute_check_uint(DPACK_STR_SIZE(UINT32_MAX - 1),
	                equal,
	                (uintmax_t)UINT32_MAX - 1 + 5);
	cute_check_uint(dpack_str_size(UINT32_MAX - 1),
	                equal,
	                (uintmax_t)UINT32_MAX - 1 + 5);
}

#else  /* !(DPACK_STRLEN_MAX >= (UINT32_MAX - 1)) */

CUTE_TEST(dpackut_str32_sizes_maxminus1)
{
	cute_skip("MsgPack string length >= (UINT32_MAX - 1) support "
	          "not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= (UINT32_MAX - 1) */

#if DPACK_STRLEN_MAX >= UINT32_MAX

CUTE_TEST(dpackut_str32_sizes_max)
{
	cute_check_uint(DPACK_STR_SIZE(UINT32_MAX),
	                equal,
	                (uintmax_t)UINT32_MAX + 5);
	cute_check_uint(dpack_str_size(UINT32_MAX),
	                equal,
	                (uintmax_t)UINT32_MAX + 5);
}

#else  /* !(DPACK_STRLEN_MAX >= UINT32_MAX) */

CUTE_TEST(dpackut_str32_sizes_max)
{
	cute_skip("MsgPack string length >= UINT32_MAX support "
	          "not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= UINT32_MAX */

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_str_assert_sizes)
{
	size_t sz __unused;

	cute_expect_assertion(sz = dpack_str_size(0));
	cute_expect_assertion(sz = dpack_str_size(DPACK_STRLEN_MAX + 1));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_str_assert_sizes)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_str_encode_assert)
{
	struct dpack_encoder enc = { 0, };
	char *               buff;
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_encode_str(NULL, "test"));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_encode_str(&enc, "test"));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	buff = malloc(DPACK_STRLEN_MAX + 2);
	cute_check_ptr(buff, unequal, NULL);
	memset(buff, 'f', DPACK_STRLEN_MAX + 1);
	buff[DPACK_STRLEN_MAX + 1] = '\0';

	dpack_encoder_init_buffer(&enc, buff, DPACK_STRLEN_MAX + 2);
	cute_expect_assertion(ret = dpack_encode_str(&enc, buff));
	cute_expect_assertion(ret = dpack_encode_str(&enc, NULL));
	cute_expect_assertion(ret = dpack_encode_str(&enc, ""));
	dpack_encoder_fini(&enc, DPACK_DONE);

	free(buff);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_str_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

static void
dpackut_str_encode(struct dpackut_str_data * data,
                   dpackut_str_pack_fn *     pack)
{
	char *               buff;
	struct dpack_encoder enc;

	dpackut_str_gen_data(data);
	if (data->null_term)
		cute_check_uint(data->len, equal, strlen(data->value));
	else
		cute_check_uint(data->len, equal, strnlen(data->value,
		                                          data->len));

	buff = malloc(data->size);
	cute_check_ptr(buff, unequal, NULL);
	memset(buff, 0xa5, data->size);

	dpack_encoder_init_buffer(&enc, buff, data->size);

	pack(&enc, data);
	cute_check_mem(buff, equal, data->packed, data->size);
	cute_check_uint(dpack_str_size(data->len), equal, data->size);
	cute_check_uint(dpack_encoder_space_used(&enc), equal, data->size);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc, DPACK_DONE);

	free(buff);

	dpackut_str_fini_data(data);
}

static void
dpackut_str_pack(struct dpack_encoder *          encoder,
                 const struct dpackut_str_data * data)
{
	cute_check_sint(dpack_encode_str(encoder, data->value), equal, 0);
}

CUTE_TEST(dpackut_str_encode_1)
{
	DPACKUT_STR_DATA(data, 1, true);

	dpackut_str_encode(&data, dpackut_str_pack);
}

#if DPACK_STRLEN_MAX >= 31

CUTE_TEST(dpackut_str_encode_31)
{
	DPACKUT_STR_DATA(data, 31, true);
	dpackut_str_encode(&data, dpackut_str_pack);
}

#else  /* !(DPACK_STRLEN_MAX >= 31) */

CUTE_TEST(dpackut_str_encode_31)
{
	cute_skip("string length >= 31 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 31 */

#if DPACK_STRLEN_MAX >= 32

CUTE_TEST(dpackut_str_encode_32)
{
	DPACKUT_STR_DATA(data, 32, true);
	dpackut_str_encode(&data, dpackut_str_pack);
}

#else  /* !(DPACK_STRLEN_MAX >= 32) */

CUTE_TEST(dpackut_str_encode_32)
{
	cute_skip("string length >= 32 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 32 */

#if DPACK_STRLEN_MAX >= 255

CUTE_TEST(dpackut_str_encode_255)
{
	DPACKUT_STR_DATA(data, 255, true);
	dpackut_str_encode(&data, dpackut_str_pack);
}

#else  /* !(DPACK_STRLEN_MAX >= 255) */

CUTE_TEST(dpackut_str_encode_255)
{
	cute_skip("string length >= 255 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 255 */

#if DPACK_STRLEN_MAX >= 256

CUTE_TEST(dpackut_str_encode_256)
{
	DPACKUT_STR_DATA(data, 256, true);
	dpackut_str_encode(&data, dpackut_str_pack);
}

#else  /* !(DPACK_STRLEN_MAX >= 256) */

CUTE_TEST(dpackut_str_encode_256)
{
	cute_skip("string length >= 256 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 256 */

#if DPACK_STRLEN_MAX >= 65535

CUTE_TEST(dpackut_str_encode_65535)
{
	DPACKUT_STR_DATA(data, 65535, true);
	dpackut_str_encode(&data, dpackut_str_pack);
}

#else  /* !(DPACK_STRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_str_encode_65535)
{
	cute_skip("string length >= 65535 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65535 */

#if DPACK_STRLEN_MAX >= 65536

CUTE_TEST(dpackut_str_encode_65536)
{
	DPACKUT_STR_DATA(data, 65536, true);
	dpackut_str_encode(&data, dpackut_str_pack);
}

#else  /* !(DPACK_STRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_str_encode_65536)
{
	cute_skip("string length >= 65536 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_str_encode_max)
{
	DPACKUT_STR_DATA(data, DPACK_STRLEN_MAX, true);
	dpackut_str_encode(&data, dpackut_str_pack);
}

static void
dpackut_str_pack_fix(struct dpack_encoder *          encoder,
                     const struct dpackut_str_data * data)
{
	cute_check_sint(dpack_encode_str_fix(encoder, data->value, data->len),
	                equal,
	                0);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_str_encode_fix_assert)
{
	struct dpack_encoder enc = { 0, };
	char *               buff;
	int                  ret __unused;
	size_t               len = sizeof("test") - 1;
	const char *         str = "test";

	cute_expect_assertion(ret = dpack_encode_str_fix(NULL, str, len));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_encode_str_fix(&enc, str, len));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	buff = malloc(DPACK_STRLEN_MAX + 2);
	cute_check_ptr(buff, unequal, NULL);
	memset(buff, 'f', DPACK_STRLEN_MAX + 1);
	buff[DPACK_STRLEN_MAX + 1] = '\0';

	dpack_encoder_init_buffer(&enc, buff, DPACK_STRLEN_MAX + 2);
	cute_expect_assertion(ret = dpack_encode_str_fix(&enc, str, 0));
	cute_expect_assertion(ret = dpack_encode_str_fix(&enc,
	                                                 buff,
	                                                 DPACK_STRLEN_MAX + 1));
	cute_expect_assertion(ret = dpack_encode_str_fix(&enc, NULL, len));
	cute_expect_assertion(ret = dpack_encode_str_fix(&enc, "", 0));
	dpack_encoder_fini(&enc, DPACK_DONE);

	free(buff);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_str_encode_fix_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_str_encode_fix_1)
{
	DPACKUT_STR_DATA(data, 1, true);
	dpackut_str_encode(&data, dpackut_str_pack_fix);
}

#if DPACK_STRLEN_MAX >= 31

CUTE_TEST(dpackut_str_encode_fix_31)
{
	DPACKUT_STR_DATA(data, 31, true);
	dpackut_str_encode(&data, dpackut_str_pack_fix);
}

#else  /* !(DPACK_STRLEN_MAX >= 31) */

CUTE_TEST(dpackut_str_encode_fix_31)
{
	cute_skip("string length >= 31 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 31 */

#if DPACK_STRLEN_MAX >= 32

CUTE_TEST(dpackut_str_encode_fix_32)
{
	DPACKUT_STR_DATA(data, 32, true);
	dpackut_str_encode(&data, dpackut_str_pack_fix);
}

#else  /* !(DPACK_STRLEN_MAX >= 32) */

CUTE_TEST(dpackut_str_encode_fix_32)
{
	cute_skip("string length >= 32 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 32 */

#if DPACK_STRLEN_MAX >= 255

CUTE_TEST(dpackut_str_encode_fix_255)
{
	DPACKUT_STR_DATA(data, 255, true);
	dpackut_str_encode(&data, dpackut_str_pack_fix);
}

#else  /* !(DPACK_STRLEN_MAX >= 255) */

CUTE_TEST(dpackut_str_encode_fix_255)
{
	cute_skip("string length >= 255 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 255 */

#if DPACK_STRLEN_MAX >= 256

CUTE_TEST(dpackut_str_encode_fix_256)
{
	DPACKUT_STR_DATA(data, 256, true);
	dpackut_str_encode(&data, dpackut_str_pack_fix);
}

#else  /* !(DPACK_STRLEN_MAX >= 256) */

CUTE_TEST(dpackut_str_encode_fix_256)
{
	cute_skip("string length >= 256 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 256 */

#if DPACK_STRLEN_MAX >= 65535

CUTE_TEST(dpackut_str_encode_fix_65535)
{
	DPACKUT_STR_DATA(data, 65535, true);
	dpackut_str_encode(&data, dpackut_str_pack_fix);
}

#else  /* !(DPACK_STRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_str_encode_fix_65535)
{
	cute_skip("string length >= 65535 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65535 */

#if DPACK_STRLEN_MAX >= 65536

CUTE_TEST(dpackut_str_encode_fix_65536)
{
	DPACKUT_STR_DATA(data, 65536, true);
	dpackut_str_encode(&data, dpackut_str_pack_fix);
}

#else  /* !(DPACK_STRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_str_encode_fix_65536)
{
	cute_skip("string length >= 65536 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_str_encode_fix_max)
{
	DPACKUT_STR_DATA(data, DPACK_STRLEN_MAX, true);
	dpackut_str_encode(&data, dpackut_str_pack_fix);
}

static void
dpackut_str_decode(struct dpackut_str_data * data,
                   dpackut_str_unpack_fn *   unpack)
{
	struct dpack_decoder dec;

	dpackut_str_gen_data(data);
	cute_check_uint(data->len, equal, strlen(data->value));

	cute_check_uint(dpackut_str_size(data->len), equal, data->size);

	dpack_decoder_init_buffer(&dec, data->packed, data->size);
	unpack(&dec, data);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	dpack_decoder_fini(&dec);

	dpackut_str_fini_data(data);
}

static void
dpackut_str_unpack_dup(struct dpack_decoder *          decoder,
                       const struct dpackut_str_data * data)
{
	char * val;

	cute_check_sint(dpack_decode_strdup(decoder, &val), equal, data->error);

	if (data->error >= 0) {
		cute_check_uint(strlen(val), equal, data->len);
		cute_check_str(val, equal, data->value);
		free(val);
	}
}

#define DPACKUT_STR_DEC(_var, _len, _error) \
	struct dpackut_str_data _var = { \
		.len       = _len, \
		.null_term = true, \
		.error     = _error \
	}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_str_decode_dup_assert)
{
       struct dpack_decoder dec = { 0, };
       char *               str;
       char                 buff[8];
       ssize_t              ret __unused;

       cute_expect_assertion(ret = dpack_decode_strdup(NULL, &str));
#if defined(CONFIG_DPACK_DEBUG)
       cute_expect_assertion(ret = dpack_decode_strdup(&dec, &str));
#endif /* defined(CONFIG_DPACK_DEBUG) */

       dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
       cute_expect_assertion(ret = dpack_decode_strdup(&dec, NULL));
       dpack_decoder_fini(&dec);
}

#else  /* !(defined(CONFIG_DPACK_ASSERT_API)) */

CUTE_TEST(dpackut_str_decode_dup_assert)
{
	cute_skip("assertion unsupported");
}

#endif  /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_str_decode_dup_0)
{
	DPACKUT_STR_DEC(data, 0, -EMSGSIZE);
	dpackut_str_decode(&data, dpackut_str_unpack_dup);
}

CUTE_TEST(dpackut_str_decode_dup_1)
{
	DPACKUT_STR_DEC(data, 1, 1);
	dpackut_str_decode(&data, dpackut_str_unpack_dup);
}

#if DPACK_STRLEN_MAX >= 31

CUTE_TEST(dpackut_str_decode_dup_31)
{
	DPACKUT_STR_DEC(data, 31, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_dup);
}

#else  /* !(DPACK_STRLEN_MAX >= 31) */

CUTE_TEST(dpackut_str_decode_dup_31)
{
	cute_skip("string length >= 31 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 31 */

#if DPACK_STRLEN_MAX >= 32

CUTE_TEST(dpackut_str_decode_dup_32)
{
	DPACKUT_STR_DEC(data, 32, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_dup);
}

#else  /* !(DPACK_STRLEN_MAX >= 32) */

CUTE_TEST(dpackut_str_decode_dup_32)
{
	cute_skip("string length >= 32 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 32 */

#if DPACK_STRLEN_MAX >= 255

CUTE_TEST(dpackut_str_decode_dup_255)
{
	DPACKUT_STR_DEC(data, 255, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_dup);
}

#else  /* !(DPACK_STRLEN_MAX >= 255) */

CUTE_TEST(dpackut_str_decode_dup_255)
{
	cute_skip("string length >= 255 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 255 */

#if DPACK_STRLEN_MAX >= 256

CUTE_TEST(dpackut_str_decode_dup_256)
{
	DPACKUT_STR_DEC(data, 256, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_dup);
}

#else  /* !(DPACK_STRLEN_MAX >= 256) */

CUTE_TEST(dpackut_str_decode_dup_256)
{
	cute_skip("string length >= 256 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 256 */

#if DPACK_STRLEN_MAX >= 65535

CUTE_TEST(dpackut_str_decode_dup_65535)
{
	DPACKUT_STR_DEC(data, 65535, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_dup);
}

#else  /* !(DPACK_STRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_str_decode_dup_65535)
{
	cute_skip("string length >= 65535 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65535 */

#if DPACK_STRLEN_MAX >= 65536

CUTE_TEST(dpackut_str_decode_dup_65536)
{
	DPACKUT_STR_DEC(data, 65536, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_dup);
}

#else  /* !(DPACK_STRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_str_decode_dup_65536)
{
	cute_skip("string length >= 65536 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_str_decode_dup_max)
{
	DPACKUT_STR_DEC(data, DPACK_STRLEN_MAX, DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_dup);
}

CUTE_TEST(dpackut_str_decode_dup_maxplus1)
{
	DPACKUT_STR_DEC(data, DPACK_STRLEN_MAX + 1, -EMSGSIZE);
	dpackut_str_decode(&data, dpackut_str_unpack_dup);
}

CUTE_TEST(dpackut_str_decode_dup_fail)
{
	struct dpack_decoder dec;
	char                 buff[] = "\xa1\x30";
	char *               str;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	if (dpackut_expect_malloc()) {
		cute_check_sint(dpack_decode_strdup(&dec, &str), equal, 1);
		cute_check_ptr(str, unequal, NULL);
		free(str);
	}
	else
		cute_check_sint(dpack_decode_strdup(&dec, &str),
		                equal,
		                -ENOMEM);
	dpack_decoder_fini(&dec);
}

#define DPACKUT_STR_DEC_EQU(_len, _error, _equ) \
	((struct dpackut_str_data) { \
		.len       = _len, \
		.null_term = true, \
		.error     = _error, \
		.equ       = _equ \
	 })

static void
dpackut_str_unpack_dup_equ(struct dpack_decoder *          decoder,
                           const struct dpackut_str_data * data)
{
	char * val;

	cute_check_sint(dpack_decode_strdup_equ(decoder, data->equ, &val),
	                equal,
	                data->error);

	if (data->error >= 0) {
		cute_check_uint(strlen(val), equal, data->len);
		cute_check_str(val, equal, data->value);
		free(val);
	}
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_str_decode_dup_equ_assert)
{
	struct dpack_decoder dec = { 0, };
	char *               str;
	char                 buff[8];
	ssize_t              ret __unused;

	cute_expect_assertion(ret = dpack_decode_strdup_equ(NULL, 2, &str));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_strdup_equ(&dec, 2, &str));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_strdup_equ(&dec, 2, NULL));
	cute_expect_assertion(ret = dpack_decode_strdup_equ(&dec, 0, &str));
	cute_expect_assertion(ret = dpack_decode_strdup_equ(&dec,
	                                                    DPACK_STRLEN_MAX +
	                                                    1,
	                                                    &str));
	dpack_decoder_fini(&dec);
}

#else  /* !(defined(CONFIG_DPACK_ASSERT_API)) */

CUTE_TEST(dpackut_str_decode_dup_equ_assert)
{
	cute_skip("assertion unsupported");
}

#endif  /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_str_decode_dup_equ_2)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(0, -EMSGSIZE, 1);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(1, 1, 1);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(2, -EMSGSIZE, 1);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);
}

#if DPACK_STRLEN_MAX >= 31

CUTE_TEST(dpackut_str_decode_dup_equ_31)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(30, -EMSGSIZE, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(31, 31, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(32, -EMSGSIZE, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);
}

#else  /* !(DPACK_STRLEN_MAX >= 31) */

CUTE_TEST(dpackut_str_decode_dup_equ_31)
{
	cute_skip("string length >= 31 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 31 */

#if DPACK_STRLEN_MAX >= 32

CUTE_TEST(dpackut_str_decode_dup_equ_32)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(31, -EMSGSIZE, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(32, 32, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(33, -EMSGSIZE, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);
}

#else  /* !(DPACK_STRLEN_MAX >= 32) */

CUTE_TEST(dpackut_str_decode_dup_equ_32)
{
	cute_skip("string length >= 32 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 32 */

#if DPACK_STRLEN_MAX >= 255

CUTE_TEST(dpackut_str_decode_dup_equ_255)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(254, -EMSGSIZE, 255),
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(255, 255, 255),
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(256, -EMSGSIZE, 255),
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);
}

#else  /* !(DPACK_STRLEN_MAX >= 255) */

CUTE_TEST(dpackut_str_decode_dup_equ_255)
{
	cute_skip("string length >= 255 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 255 */

#if DPACK_STRLEN_MAX >= 256

CUTE_TEST(dpackut_str_decode_dup_equ_256)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(255, -EMSGSIZE, 256),
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(256, 256, 256),
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(257, -EMSGSIZE, 256),
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);
}

#else  /* !(DPACK_STRLEN_MAX >= 256) */

CUTE_TEST(dpackut_str_decode_dup_equ_256)
{
	cute_skip("string length >= 256 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 256 */

#if DPACK_STRLEN_MAX >= 65535

CUTE_TEST(dpackut_str_decode_dup_equ_65535)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(65534, -EMSGSIZE, 65535),
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(65535, 65535, 65535),
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(65536, -EMSGSIZE, 65535),
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);
}

#else  /* !(DPACK_STRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_str_decode_dup_equ_65535)
{
	cute_skip("string length >= 65535 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65535 */

#if DPACK_STRLEN_MAX >= 65536

CUTE_TEST(dpackut_str_decode_dup_equ_65536)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(65535, -EMSGSIZE, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(65536, 65536, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(65537, -EMSGSIZE, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);
}

#else  /* !(DPACK_STRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_str_decode_dup_equ_65536)
{
	cute_skip("string length >= 65536 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_str_decode_dup_equ_max)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(DPACK_STRLEN_MAX - 1,
	                           -EMSGSIZE,
	                           DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(DPACK_STRLEN_MAX,
	                           DPACK_STRLEN_MAX,
	                           DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);

	data = DPACKUT_STR_DEC_EQU(DPACK_STRLEN_MAX + 1,
	                           -EMSGSIZE,
	                           DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_equ);
}

#define DPACKUT_STR_DEC_MAX(_len, _error, _high) \
	((struct dpackut_str_data) { \
		.len       = _len, \
		.null_term = true, \
		.error     = _error, \
		.high      = _high \
	 })

static void
dpackut_str_unpack_dup_max(struct dpack_decoder *          decoder,
                           const struct dpackut_str_data * data)
{
	char * val;

	cute_check_sint(dpack_decode_strdup_max(decoder, data->high, &val),
	                equal,
	                data->error);

	if (data->error >= 0) {
		cute_check_uint(strlen(val), equal, data->len);
		cute_check_str(val, equal, data->value);
		free(val);
	}
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_str_decode_dup_max_assert)
{
	struct dpack_decoder dec = { 0, };
	char *               str;
	char                 buff[8];
	ssize_t              ret __unused;

	cute_expect_assertion(ret = dpack_decode_strdup_max(NULL, 2, &str));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_strdup_max(&dec, 2, &str));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_strdup_max(&dec, 2, NULL));
	cute_expect_assertion(ret = dpack_decode_strdup_max(&dec, 1, &str));
	cute_expect_assertion(ret = dpack_decode_strdup_max(&dec,
	                                                    DPACK_STRLEN_MAX + 1,
	                                                    &str));
	dpack_decoder_fini(&dec);
}

#else  /* !(defined(CONFIG_DPACK_ASSERT_API)) */

CUTE_TEST(dpackut_str_decode_dup_max_assert)
{
	cute_skip("assertion unsupported");
}

#endif  /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_str_decode_dup_max_2)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_MAX(0, -EMSGSIZE, 2);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(2,  2, 2);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(3,  -EMSGSIZE, 2);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);
}

#if DPACK_STRLEN_MAX >= 31

CUTE_TEST(dpackut_str_decode_dup_max_31)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_MAX(30, 30, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(31, 31, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(32, -EMSGSIZE, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);
}

#else  /* !(DPACK_STRLEN_MAX >= 31) */

CUTE_TEST(dpackut_str_decode_dup_max_31)
{
	cute_skip("string length >= 31 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 31 */

#if DPACK_STRLEN_MAX >= 32

CUTE_TEST(dpackut_str_decode_dup_max_32)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_MAX(31, 31, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(32, 32, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(33, -EMSGSIZE, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);
}

#else  /* !(DPACK_STRLEN_MAX >= 32) */

CUTE_TEST(dpackut_str_decode_dup_max_32)
{
	cute_skip("string length >= 32 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 32 */

#if DPACK_STRLEN_MAX >= 255

CUTE_TEST(dpackut_str_decode_dup_max_255)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_MAX(254, 254, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(255, 255, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(256, -EMSGSIZE, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);
}

#else  /* !(DPACK_STRLEN_MAX >= 255) */

CUTE_TEST(dpackut_str_decode_dup_max_255)
{
	cute_skip("string length >= 255 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 255 */

#if DPACK_STRLEN_MAX >= 256

CUTE_TEST(dpackut_str_decode_dup_max_256)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_MAX(255, 255, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(256, 256, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(257, -EMSGSIZE, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);
}

#else  /* !(DPACK_STRLEN_MAX >= 256) */

CUTE_TEST(dpackut_str_decode_dup_max_256)
{
	cute_skip("string length >= 256 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 256 */

#if DPACK_STRLEN_MAX >= 65535

CUTE_TEST(dpackut_str_decode_dup_max_65535)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_MAX(65534, 65534, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(65535, 65535, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(65536, -EMSGSIZE, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);
}

#else  /* !(DPACK_STRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_str_decode_dup_max_65535)
{
	cute_skip("string length >= 65535 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65535 */

#if DPACK_STRLEN_MAX >= 65536

CUTE_TEST(dpackut_str_decode_dup_max_65536)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_MAX(65535, 65535, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(65536, 65536, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(65537, -EMSGSIZE, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);
}

#else  /* !(DPACK_STRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_str_decode_dup_max_65536)
{
	cute_skip("string length >= 65536 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_str_decode_dup_max_max)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_MAX(DPACK_STRLEN_MAX,
	                           DPACK_STRLEN_MAX,
	                           DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);

	data = DPACKUT_STR_DEC_MAX(DPACK_STRLEN_MAX + 1,
	                           -EMSGSIZE,
	                           DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_max);
}

#define DPACKUT_STR_DEC_RANGE(_len, _error, _low, _high) \
	((struct dpackut_str_data) { \
		.len       = _len, \
		.null_term = true, \
		.error     = _error, \
		.low       = _low, \
		.high      = _high \
	 })

static void
dpackut_str_unpack_dup_range(struct dpack_decoder *          decoder,
                             const struct dpackut_str_data * data)
{
	char * val;

	cute_check_sint(dpack_decode_strdup_range(decoder,
	                                          data->low,
	                                          data->high,
	                                          &val),
	                equal,
	                data->error);

	if (data->error >= 0) {
		cute_check_uint(strlen(val), equal, data->len);
		cute_check_str(val, equal, data->value);
		free(val);
	}
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_str_decode_dup_range_assert)
{
	struct dpack_decoder dec = { 0, };
	char *               str;
	char                 buff[8];
	ssize_t              ret __unused;

	cute_expect_assertion(ret = dpack_decode_strdup_range(NULL,
	                                                      1,
	                                                      2,
	                                                      &str));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_strdup_range(&dec,
	                                                      1,
	                                                      2,
	                                                      &str));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_strdup_range(&dec,
	                                                      1,
	                                                      2,
	                                                      NULL));
	cute_expect_assertion(ret = dpack_decode_strdup_range(&dec,
	                                                      0,
	                                                      2,
	                                                      &str));
	cute_expect_assertion(ret = dpack_decode_strdup_range(&dec,
	                                                      1,
	                                                      DPACK_STRLEN_MAX +
	                                                      1,
	                                                      &str));
	cute_expect_assertion(ret = dpack_decode_strdup_range(&dec,
	                                                      1,
	                                                      1,
	                                                      &str));
	dpack_decoder_fini(&dec);
}

#else  /* !(defined(CONFIG_DPACK_ASSERT_API)) */

CUTE_TEST(dpackut_str_decode_dup_range_assert)
{
	cute_skip("assertion unsupported");
}

#endif  /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_str_decode_dup_range_1_2)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(0, -EMSGSIZE, 1, 2);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(1, 1, 1, 2);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(2, 2, 1, 2);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(3, -EMSGSIZE, 1, 2);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);
}

#if DPACK_STRLEN_MAX >= 31

CUTE_TEST(dpackut_str_decode_dup_range_30_31)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(29, -EMSGSIZE, 30, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(30, 30, 30, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(31, 31, 30, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(32, -EMSGSIZE, 30, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);
}

#else  /* !(DPACK_STRLEN_MAX >= 31) */

CUTE_TEST(dpackut_str_decode_dup_range_30_31)
{
	cute_skip("string length >= 31 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 31 */

#if DPACK_STRLEN_MAX >= 32

CUTE_TEST(dpackut_str_decode_dup_range_31_32)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(30, -EMSGSIZE, 31, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(31, 31, 31, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(32, 32, 31, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(33, -EMSGSIZE, 31, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);
}

#else  /* !(DPACK_STRLEN_MAX >= 32) */

CUTE_TEST(dpackut_str_decode_dup_range_31_32)
{
	cute_skip("string length >= 32 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 32 */

#if DPACK_STRLEN_MAX >= 255

CUTE_TEST(dpackut_str_decode_dup_range_254_255)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(253, -EMSGSIZE, 254, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(254, 254, 254, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(255, 255, 254, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(256, -EMSGSIZE, 254, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);
}

#else  /* !(DPACK_STRLEN_MAX >= 255) */

CUTE_TEST(dpackut_str_decode_dup_range_254_255)
{
	cute_skip("string length >= 255 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 255 */

#if DPACK_STRLEN_MAX >= 256

CUTE_TEST(dpackut_str_decode_dup_range_255_256)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(254, -EMSGSIZE, 255, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(255, 255, 255, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(256, 256, 255, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(257, -EMSGSIZE, 255, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);
}

#else  /* !(DPACK_STRLEN_MAX >= 256) */

CUTE_TEST(dpackut_str_decode_dup_range_255_256)
{
	cute_skip("string length >= 256 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 256 */

#if DPACK_STRLEN_MAX >= 65535

CUTE_TEST(dpackut_str_decode_dup_range_65535)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(65533, -EMSGSIZE, 65534, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(65534, 65534, 65534, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(65535, 65535, 65534, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(65536, -EMSGSIZE, 65534, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);
}

#else  /* !(DPACK_STRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_str_decode_dup_range_65535)
{
	cute_skip("string length >= 65535 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65535 */

#if DPACK_STRLEN_MAX >= 65536

CUTE_TEST(dpackut_str_decode_dup_range_65536)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(65534, -EMSGSIZE, 65535, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(65535, 65535, 65535, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(65536, 65536, 65535, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(65537, -EMSGSIZE, 65535, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);
}

#else  /* !(DPACK_STRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_str_decode_dup_range_65536)
{
	cute_skip("string length >= 65536 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_str_decode_dup_range_maxminus1_max)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(DPACK_STRLEN_MAX - 2,
	                             -EMSGSIZE,
	                             DPACK_STRLEN_MAX - 1,
	                             DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(DPACK_STRLEN_MAX - 1,
	                             DPACK_STRLEN_MAX - 1,
	                             DPACK_STRLEN_MAX - 1,
	                             DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(DPACK_STRLEN_MAX,
	                             DPACK_STRLEN_MAX,
	                             DPACK_STRLEN_MAX - 1,
	                             DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);

	data = DPACKUT_STR_DEC_RANGE(DPACK_STRLEN_MAX + 1,
	                             -EMSGSIZE,
	                             DPACK_STRLEN_MAX - 1,
	                             DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_dup_range);
}

static void
dpackut_str_unpack_strcpy(struct dpack_decoder *          decoder,
                          const struct dpackut_str_data * data)
{
	char * val;
	size_t sz = stroll_min(data->len + 2, (size_t)DPACK_STRLEN_MAX + 1);

	val = malloc(sz);
	memset(val, 0xff, sz);

	cute_check_sint(dpack_decode_strcpy(decoder, sz, val),
	                equal,
	                data->error);

	if (data->error >= 0) {
		cute_check_uint(strlen(val), equal, data->len);
		cute_check_str(val, equal, data->value);
		cute_check_sint(val[data->len], equal, 0);
		if (data->len < DPACK_STRLEN_MAX)
			cute_check_sint(val[data->len + 1], equal, (char)0xff);
	}

	free(val);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_str_decode_strcpy_assert)
{
	struct dpack_decoder dec = { .mpack.error = mpack_error_bug };
	char *               str = str;
	char                 buff[8];
	ssize_t              ret __unused;

	cute_expect_assertion(ret = dpack_decode_strcpy(NULL, 2, str));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_strcpy(&dec, 2, str));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_strcpy(&dec, 2, NULL));
	cute_expect_assertion(ret = dpack_decode_strcpy(&dec, 1, str));
	cute_expect_assertion(ret = dpack_decode_strcpy(&dec,
	                                                DPACK_STRLEN_MAX + 2,
	                                                str));
	dpack_decoder_fini(&dec);
}

#else  /* !(defined(CONFIG_DPACK_ASSERT_API)) */

CUTE_TEST(dpackut_str_decode_strcpy_assert)
{
	cute_skip("assertion unsupported");
}

#endif  /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_str_decode_strcpy_0)
{
	DPACKUT_STR_DEC(data, 0, -EMSGSIZE);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy);
}

CUTE_TEST(dpackut_str_decode_strcpy_1)
{
	DPACKUT_STR_DEC(data, 1, 1);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy);
}

#if DPACK_STRLEN_MAX >= 31

CUTE_TEST(dpackut_str_decode_strcpy_31)
{
	DPACKUT_STR_DEC(data, 31, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy);
}

#else  /* !(DPACK_STRLEN_MAX >= 31) */

CUTE_TEST(dpackut_str_decode_strcpy_31)
{
	cute_skip("string length >= 31 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 31 */

#if DPACK_STRLEN_MAX >= 32

CUTE_TEST(dpackut_str_decode_strcpy_32)
{
	DPACKUT_STR_DEC(data, 32, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy);
}

#else  /* !(DPACK_STRLEN_MAX >= 32) */

CUTE_TEST(dpackut_str_decode_strcpy_32)
{
	cute_skip("string length >= 32 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 32 */

#if DPACK_STRLEN_MAX >= 255

CUTE_TEST(dpackut_str_decode_strcpy_255)
{
	DPACKUT_STR_DEC(data, 255, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy);
}

#else  /* !(DPACK_STRLEN_MAX >= 255) */

CUTE_TEST(dpackut_str_decode_strcpy_255)
{
	cute_skip("string length >= 255 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 255 */

#if DPACK_STRLEN_MAX >= 256

CUTE_TEST(dpackut_str_decode_strcpy_256)
{
	DPACKUT_STR_DEC(data, 256, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy);
}

#else  /* !(DPACK_STRLEN_MAX >= 256) */

CUTE_TEST(dpackut_str_decode_strcpy_256)
{
	cute_skip("string length >= 256 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 256 */

#if DPACK_STRLEN_MAX >= 65535

CUTE_TEST(dpackut_str_decode_strcpy_65535)
{
	DPACKUT_STR_DEC(data, 65535, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy);
}

#else  /* !(DPACK_STRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_str_decode_strcpy_65535)
{
	cute_skip("string length >= 65535 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65535 */

#if DPACK_STRLEN_MAX >= 65536

CUTE_TEST(dpackut_str_decode_strcpy_65536)
{
	DPACKUT_STR_DEC(data, 65536, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy);
}

#else  /* !(DPACK_STRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_str_decode_strcpy_65536)
{
	cute_skip("string length >= 65536 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_str_decode_strcpy_max)
{
	DPACKUT_STR_DEC(data, DPACK_STRLEN_MAX, DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy);
}

CUTE_TEST(dpackut_str_decode_strcpy_maxplus1)
{
	DPACKUT_STR_DEC(data, DPACK_STRLEN_MAX + 1, -EMSGSIZE);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy);
}

static void
dpackut_str_unpack_strcpy_equ(struct dpack_decoder *          decoder,
                              const struct dpackut_str_data * data)
{
	char * val;
	size_t sz = stroll_min(data->equ + 2, (size_t)DPACK_STRLEN_MAX + 1);

	val = malloc(sz);
	memset(val, 0xff, sz);

	cute_check_sint(dpack_decode_strcpy_equ(decoder, data->equ + 1, val),
	                equal,
	                data->error);

	if (data->error >= 0) {
		cute_check_uint(strlen(val), equal, data->len);
		cute_check_str(val, equal, data->value);
		cute_check_sint(val[data->len], equal, 0);
		if (data->len < DPACK_STRLEN_MAX)
			cute_check_sint(val[data->len + 1], equal, (char)0xff);
	}

	free(val);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_str_decode_strcpy_equ_assert)
{
	struct dpack_decoder dec = { .mpack.error = mpack_error_bug };
	char *               str = str;
	char                 buff[8];
	ssize_t              ret __unused;

	cute_expect_assertion(ret = dpack_decode_strcpy_equ(NULL, 2, str));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_strcpy_equ(&dec, 2, str));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_strcpy_equ(&dec, 2, NULL));
	cute_expect_assertion(ret = dpack_decode_strcpy_equ(&dec, 1, str));
	cute_expect_assertion(ret = dpack_decode_strcpy_equ(&dec,
	                                                    DPACK_STRLEN_MAX +
	                                                    2,
	                                                    str));
	dpack_decoder_fini(&dec);
}

#else  /* !(defined(CONFIG_DPACK_ASSERT_API)) */

CUTE_TEST(dpackut_str_decode_strcpy_equ_assert)
{
	cute_skip("assertion unsupported");
}

#endif  /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_str_decode_strcpy_equ_1)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(0, -EMSGSIZE, 1);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(1,  1, 1);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(2,  -EMSGSIZE, 1);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);
}

#if DPACK_STRLEN_MAX >= 31

CUTE_TEST(dpackut_str_decode_strcpy_equ_31)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(30, -EMSGSIZE, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(31, 31, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(32, -EMSGSIZE, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);
}

#else  /* !(DPACK_STRLEN_MAX >= 31) */

CUTE_TEST(dpackut_str_decode_strcpy_equ_31)
{
	cute_skip("string length >= 31 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 31 */

#if DPACK_STRLEN_MAX >= 32

CUTE_TEST(dpackut_str_decode_strcpy_equ_32)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(31, -EMSGSIZE, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(32, 32, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(33, -EMSGSIZE, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);
}

#else  /* !(DPACK_STRLEN_MAX >= 32) */

CUTE_TEST(dpackut_str_decode_strcpy_equ_32)
{
	cute_skip("string length >= 32 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 32 */

#if DPACK_STRLEN_MAX >= 255

CUTE_TEST(dpackut_str_decode_strcpy_equ_255)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(254, -EMSGSIZE, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(255, 255, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(256, -EMSGSIZE, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);
}

#else  /* !(DPACK_STRLEN_MAX >= 255) */

CUTE_TEST(dpackut_str_decode_strcpy_equ_255)
{
	cute_skip("string length >= 255 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 255 */

#if DPACK_STRLEN_MAX >= 256

CUTE_TEST(dpackut_str_decode_strcpy_equ_256)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(255, -EMSGSIZE, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(256, 256, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(257, -EMSGSIZE, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);
}

#else  /* !(DPACK_STRLEN_MAX >= 256) */

CUTE_TEST(dpackut_str_decode_strcpy_equ_256)
{
	cute_skip("string length >= 256 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 256 */

#if DPACK_STRLEN_MAX >= 65535

CUTE_TEST(dpackut_str_decode_strcpy_equ_65535)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(65534, -EMSGSIZE, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(65535, 65535, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(65536, -EMSGSIZE, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);
}

#else  /* !(DPACK_STRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_str_decode_strcpy_equ_65535)
{
	cute_skip("string length >= 65535 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65535 */

#if DPACK_STRLEN_MAX >= 65536

CUTE_TEST(dpackut_str_decode_strcpy_equ_65536)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(65535, -EMSGSIZE, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(65536, 65536, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(65537, -EMSGSIZE, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);
}

#else  /* !(DPACK_STRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_str_decode_strcpy_equ_65536)
{
	cute_skip("string length >= 65536 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_str_decode_strcpy_equ_max)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_EQU(DPACK_STRLEN_MAX - 1,
	                           -EMSGSIZE,
	                           DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(DPACK_STRLEN_MAX,
	                           DPACK_STRLEN_MAX,
	                           DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);

	data = DPACKUT_STR_DEC_EQU(DPACK_STRLEN_MAX + 1,
	                           -EMSGSIZE,
	                           DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_equ);
}

static void
dpackut_str_unpack_strcpy_range(struct dpack_decoder *          decoder,
                                const struct dpackut_str_data * data)
{
	char * val;
	size_t sz = stroll_min(data->high + 2, (size_t)DPACK_STRLEN_MAX + 1);

	val = malloc(sz);
	memset(val, 0xff, sz);

	cute_check_sint(dpack_decode_strcpy_range(decoder,
	                                          data->low,
	                                          data->high + 1,
	                                          val),
	                equal,
	                data->error);

	if (data->error >= 0) {
		cute_check_uint(strlen(val), equal, data->len);
		cute_check_str(val, equal, data->value);
		cute_check_sint(val[data->len], equal, 0);
		if (data->len < DPACK_STRLEN_MAX)
			cute_check_sint(val[data->len + 1], equal, (char)0xff);
	}

	free(val);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_str_decode_strcpy_range_assert)
{
	struct dpack_decoder dec = { .mpack.error = mpack_error_bug };
	char *               str = str;
	char                 buff[8];
	ssize_t              ret __unused;

	cute_expect_assertion(ret = dpack_decode_strcpy_range(NULL,
	                                                      1,
	                                                      3,
	                                                      str));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_strcpy_range(&dec,
	                                                      1,
	                                                      3,
	                                                      str));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_strcpy_range(&dec,
	                                                      1,
	                                                      31,
	                                                      NULL));
	cute_expect_assertion(ret = dpack_decode_strcpy_range(&dec,
	                                                      0,
	                                                      3,
	                                                      str));
	cute_expect_assertion(ret = dpack_decode_strcpy_range(&dec,
	                                                      1,
	                                                      DPACK_STRLEN_MAX +
	                                                      2,
	                                                      str));
	cute_expect_assertion(ret = dpack_decode_strcpy_range(&dec,
	                                                      1,
	                                                      2,
	                                                      str));
	dpack_decoder_fini(&dec);
}

#else  /* !(defined(CONFIG_DPACK_ASSERT_API)) */

CUTE_TEST(dpackut_str_decode_strcpy_range_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_str_decode_strcpy_range_1_2)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(0, -EMSGSIZE, 1, 2);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(1, 1, 1, 2);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(2, 2, 1, 2);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(3, -EMSGSIZE, 1, 2);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
}

#if DPACK_STRLEN_MAX >= 31

CUTE_TEST(dpackut_str_decode_strcpy_range_30_31)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(29, -EMSGSIZE, 30, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(30, 30, 30, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(31, 31, 30, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
	
	data = DPACKUT_STR_DEC_RANGE(32, -EMSGSIZE, 30, 31);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
}

#else  /* !(DPACK_STRLEN_MAX >= 31) */

CUTE_TEST(dpackut_str_decode_strcpy_range_30_31)
{
	cute_skip("string length >= 31 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 31 */

#if DPACK_STRLEN_MAX >= 32

CUTE_TEST(dpackut_str_decode_strcpy_range_31_32)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(30, -EMSGSIZE, 31, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(31, 31, 31, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(32, 32, 31, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
	
	data = DPACKUT_STR_DEC_RANGE(33, -EMSGSIZE, 31, 32);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
}

#else  /* !(DPACK_STRLEN_MAX >= 32) */

CUTE_TEST(dpackut_str_decode_strcpy_range_31_32)
{
	cute_skip("string length >= 32 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 32 */

#if DPACK_STRLEN_MAX >= 255

CUTE_TEST(dpackut_str_decode_strcpy_range_254_255)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(253, -EMSGSIZE, 254, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(254, 254, 254, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(255, 255, 254, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
	
	data = DPACKUT_STR_DEC_RANGE(256, -EMSGSIZE, 254, 255);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
}

#else  /* !(DPACK_STRLEN_MAX >= 255) */

CUTE_TEST(dpackut_str_decode_strcpy_range_254_255)
{
	cute_skip("string length >= 255 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 255 */

#if DPACK_STRLEN_MAX >= 256

CUTE_TEST(dpackut_str_decode_strcpy_range_255_256)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(254, -EMSGSIZE, 255, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(255, 255, 255, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(256, 256, 255, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
	
	data = DPACKUT_STR_DEC_RANGE(257, -EMSGSIZE, 255, 256);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
}

#else  /* !(DPACK_STRLEN_MAX >= 256) */

CUTE_TEST(dpackut_str_decode_strcpy_range_255_256)
{
	cute_skip("string length >= 256 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 256 */

#if DPACK_STRLEN_MAX >= 65535

CUTE_TEST(dpackut_str_decode_strcpy_range_65534_65535)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(65533, -EMSGSIZE, 65534, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(65534, 65534, 65534, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(65535, 65535, 65534, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
	
	data = DPACKUT_STR_DEC_RANGE(65536, -EMSGSIZE, 65534, 65535);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
}

#else  /* !(DPACK_STRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_str_decode_strcpy_range_65534_65535)
{
	cute_skip("string length >= 65535 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65535 */

#if DPACK_STRLEN_MAX >= 65536

CUTE_TEST(dpackut_str_decode_strcpy_range_65535_65536)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(65534, -EMSGSIZE, 65535, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(65535, 65535, 65535, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(65536, 65536, 65535, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
	
	data = DPACKUT_STR_DEC_RANGE(65537, -EMSGSIZE, 65535, 65536);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
}

#else  /* !(DPACK_STRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_str_decode_strcpy_range_65535_65536)
{
	cute_skip("string length >= 65536 support not compiled-in");
}

#endif /* DPACK_STRLEN_MAX >= 65536 */

CUTE_TEST_STATIC(dpackut_str_decode_strcpy_range_maxminus1_max,
                 CUTE_NULL_SETUP,
                 CUTE_NULL_TEARDOWN,
                 10)
{
	struct dpackut_str_data data;

	data = DPACKUT_STR_DEC_RANGE(DPACK_STRLEN_MAX - 2,
	                             -EMSGSIZE,
	                             DPACK_STRLEN_MAX - 1,
	                             DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(DPACK_STRLEN_MAX - 1,
	                             DPACK_STRLEN_MAX - 1,
	                             DPACK_STRLEN_MAX - 1,
	                             DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(DPACK_STRLEN_MAX,
	                             DPACK_STRLEN_MAX,
	                             DPACK_STRLEN_MAX - 1,
	                             DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);

	data = DPACKUT_STR_DEC_RANGE(DPACK_STRLEN_MAX + 1,
	                             -EMSGSIZE,
	                             DPACK_STRLEN_MAX - 1,
	                             DPACK_STRLEN_MAX);
	dpackut_str_decode(&data, dpackut_str_unpack_strcpy_range);
}

CUTE_GROUP(dpackut_str_group) = {
	CUTE_REF(dpackut_fixstr_sizes),
	CUTE_REF(dpackut_fixstr_sizes_30),
	CUTE_REF(dpackut_fixstr_sizes_31),
	CUTE_REF(dpackut_str8_sizes),
	CUTE_REF(dpackut_str8_sizes_33),
	CUTE_REF(dpackut_str8_sizes_253),
	CUTE_REF(dpackut_str8_sizes_254),
	CUTE_REF(dpackut_str8_sizes_255),
	CUTE_REF(dpackut_str16_sizes),
	CUTE_REF(dpackut_str16_sizes_257),
	CUTE_REF(dpackut_str16_sizes_65533),
	CUTE_REF(dpackut_str16_sizes_65534),
	CUTE_REF(dpackut_str16_sizes_65535),
	CUTE_REF(dpackut_str32_sizes),
	CUTE_REF(dpackut_str32_sizes_65537),
	CUTE_REF(dpackut_str32_sizes_maxminus2),
	CUTE_REF(dpackut_str32_sizes_maxminus1),
	CUTE_REF(dpackut_str32_sizes_max),
	CUTE_REF(dpackut_str_assert_sizes),

	CUTE_REF(dpackut_str_encode_assert),
	CUTE_REF(dpackut_str_encode_1),
	CUTE_REF(dpackut_str_encode_31),
	CUTE_REF(dpackut_str_encode_32),
	CUTE_REF(dpackut_str_encode_255),
	CUTE_REF(dpackut_str_encode_256),
	CUTE_REF(dpackut_str_encode_65535),
	CUTE_REF(dpackut_str_encode_65536),
	CUTE_REF(dpackut_str_encode_max),

	CUTE_REF(dpackut_str_encode_fix_assert),
	CUTE_REF(dpackut_str_encode_fix_1),
	CUTE_REF(dpackut_str_encode_fix_31),
	CUTE_REF(dpackut_str_encode_fix_32),
	CUTE_REF(dpackut_str_encode_fix_255),
	CUTE_REF(dpackut_str_encode_fix_256),
	CUTE_REF(dpackut_str_encode_fix_65535),
	CUTE_REF(dpackut_str_encode_fix_65536),
	CUTE_REF(dpackut_str_encode_fix_max),

	CUTE_REF(dpackut_str_decode_dup_assert),
	CUTE_REF(dpackut_str_decode_dup_0),
	CUTE_REF(dpackut_str_decode_dup_1),
	CUTE_REF(dpackut_str_decode_dup_31),
	CUTE_REF(dpackut_str_decode_dup_32),
	CUTE_REF(dpackut_str_decode_dup_255),
	CUTE_REF(dpackut_str_decode_dup_256),
	CUTE_REF(dpackut_str_decode_dup_65535),
	CUTE_REF(dpackut_str_decode_dup_65536),
	CUTE_REF(dpackut_str_decode_dup_max),
	CUTE_REF(dpackut_str_decode_dup_maxplus1),
	CUTE_REF(dpackut_str_decode_dup_fail),

	CUTE_REF(dpackut_str_decode_dup_equ_assert),
	CUTE_REF(dpackut_str_decode_dup_equ_2),
	CUTE_REF(dpackut_str_decode_dup_equ_31),
	CUTE_REF(dpackut_str_decode_dup_equ_32),
	CUTE_REF(dpackut_str_decode_dup_equ_255),
	CUTE_REF(dpackut_str_decode_dup_equ_256),
	CUTE_REF(dpackut_str_decode_dup_equ_65535),
	CUTE_REF(dpackut_str_decode_dup_equ_65536),
	CUTE_REF(dpackut_str_decode_dup_equ_max),

	CUTE_REF(dpackut_str_decode_dup_max_assert),
	CUTE_REF(dpackut_str_decode_dup_max_2),
	CUTE_REF(dpackut_str_decode_dup_max_31),
	CUTE_REF(dpackut_str_decode_dup_max_32),
	CUTE_REF(dpackut_str_decode_dup_max_255),
	CUTE_REF(dpackut_str_decode_dup_max_256),
	CUTE_REF(dpackut_str_decode_dup_max_65535),
	CUTE_REF(dpackut_str_decode_dup_max_65536),
	CUTE_REF(dpackut_str_decode_dup_max_max),

	CUTE_REF(dpackut_str_decode_dup_range_assert),
	CUTE_REF(dpackut_str_decode_dup_range_1_2),
	CUTE_REF(dpackut_str_decode_dup_range_30_31),
	CUTE_REF(dpackut_str_decode_dup_range_31_32),
	CUTE_REF(dpackut_str_decode_dup_range_254_255),
	CUTE_REF(dpackut_str_decode_dup_range_255_256),
	CUTE_REF(dpackut_str_decode_dup_range_65535),
	CUTE_REF(dpackut_str_decode_dup_range_65536),
	CUTE_REF(dpackut_str_decode_dup_range_maxminus1_max),

	CUTE_REF(dpackut_str_decode_strcpy_assert),
	CUTE_REF(dpackut_str_decode_strcpy_0),
	CUTE_REF(dpackut_str_decode_strcpy_1),
	CUTE_REF(dpackut_str_decode_strcpy_31),
	CUTE_REF(dpackut_str_decode_strcpy_32),
	CUTE_REF(dpackut_str_decode_strcpy_255),
	CUTE_REF(dpackut_str_decode_strcpy_256),
	CUTE_REF(dpackut_str_decode_strcpy_65535),
	CUTE_REF(dpackut_str_decode_strcpy_65536),
	CUTE_REF(dpackut_str_decode_strcpy_max),
	CUTE_REF(dpackut_str_decode_strcpy_maxplus1),

	CUTE_REF(dpackut_str_decode_strcpy_equ_assert),
	CUTE_REF(dpackut_str_decode_strcpy_equ_1),
	CUTE_REF(dpackut_str_decode_strcpy_equ_31),
	CUTE_REF(dpackut_str_decode_strcpy_equ_32),
	CUTE_REF(dpackut_str_decode_strcpy_equ_255),
	CUTE_REF(dpackut_str_decode_strcpy_equ_256),
	CUTE_REF(dpackut_str_decode_strcpy_equ_65535),
	CUTE_REF(dpackut_str_decode_strcpy_equ_65536),
	CUTE_REF(dpackut_str_decode_strcpy_equ_max),

	CUTE_REF(dpackut_str_decode_strcpy_range_assert),
	CUTE_REF(dpackut_str_decode_strcpy_range_1_2),
	CUTE_REF(dpackut_str_decode_strcpy_range_30_31),
	CUTE_REF(dpackut_str_decode_strcpy_range_31_32),
	CUTE_REF(dpackut_str_decode_strcpy_range_254_255),
	CUTE_REF(dpackut_str_decode_strcpy_range_255_256),
	CUTE_REF(dpackut_str_decode_strcpy_range_65534_65535),
	CUTE_REF(dpackut_str_decode_strcpy_range_65535_65536),
	CUTE_REF(dpackut_str_decode_strcpy_range_maxminus1_max)
};

CUTE_SUITE_EXTERN(dpackut_str_suite,
                  dpackut_str_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
