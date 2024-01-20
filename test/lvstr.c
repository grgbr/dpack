/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/lvstr.h"
#include "dpack/codec.h"
#include "utest.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <errno.h>

CUTE_TEST(dpackut_fixlvstr_sizes)
{
	cute_check_uint(DPACK_LVSTR_SIZE(1), equal, 2);
	cute_check_uint(dpack_lvstr_size(1), equal, 2);
	cute_check_uint(DPACK_LVSTR_SIZE(2), equal, 3);
	cute_check_uint(dpack_lvstr_size(2), equal, 3);
}

#if DPACK_LVSTRLEN_MAX >= 30

CUTE_TEST(dpackut_fixlvstr_sizes_30)
{
	cute_check_uint(DPACK_LVSTR_SIZE(30), equal, 31);
	cute_check_uint(dpack_lvstr_size(30), equal, 31);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 30) */

CUTE_TEST(dpackut_fixlvstr_sizes_30)
{
	cute_skip("lvstr length >= 30 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 30 */

#if DPACK_LVSTRLEN_MAX >= 31

CUTE_TEST(dpackut_fixlvstr_sizes_31)
{
	cute_check_uint(DPACK_LVSTR_SIZE(31), equal, 32);
	cute_check_uint(dpack_lvstr_size(31), equal, 32);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 31) */

CUTE_TEST(dpackut_fixlvstr_sizes_31)
{
	cute_skip("lvstr length >= 31 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 31 */

#if DPACK_LVSTRLEN_MAX > DPACK_FIXSTR_LEN_MAX

CUTE_TEST(dpackut_lvstr8_sizes)
{
	/* Msgpack 8 bits string sizes. */
	cute_check_uint(DPACK_LVSTR_SIZE(32), equal, 34);
	cute_check_uint(dpack_lvstr_size(32), equal, 34);
}

#else  /* !(DPACK_LVSTRLEN_MAX > DPACK_FIXSTR_LEN_MAX) */

CUTE_TEST(dpackut_lvstr8_sizes)
{
	cute_skip("lvstr length >= MsgPack fixstr length "
	          " support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX > DPACK_FIXSTR_LEN_MAX */

#if DPACK_LVSTRLEN_MAX >= 33

CUTE_TEST(dpackut_lvstr8_sizes_33)
{
	cute_check_uint(DPACK_LVSTR_SIZE(33), equal, 35);
	cute_check_uint(dpack_lvstr_size(33), equal, 35);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 33) */

CUTE_TEST(dpackut_lvstr8_sizes_33)
{
	cute_skip("lvstr length >= 33 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 33 */

#if DPACK_LVSTRLEN_MAX >= (UINT8_MAX - 2)

CUTE_TEST(dpackut_lvstr8_sizes_253)
{
	cute_check_uint(DPACK_LVSTR_SIZE(UINT8_MAX - 2), equal, UINT8_MAX);
	cute_check_uint(dpack_lvstr_size(UINT8_MAX - 2), equal, UINT8_MAX);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT8_MAX - 2)) */

CUTE_TEST(dpackut_lvstr8_sizes_253)
{
	cute_skip("lvstr length >= (UINT8_MAX - 2) support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT8_MAX - 2) */

#if DPACK_LVSTRLEN_MAX >= (UINT8_MAX - 1)

CUTE_TEST(dpackut_lvstr8_sizes_254)
{
	cute_check_uint(DPACK_LVSTR_SIZE(UINT8_MAX - 1), equal, UINT8_MAX + 1);
	cute_check_uint(dpack_lvstr_size(UINT8_MAX - 1), equal, UINT8_MAX + 1);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT8_MAX - 1)) */

CUTE_TEST(dpackut_lvstr8_sizes_254)
{
	cute_skip("lvstr length >= (UINT8_MAX - 1) support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT8_MAX - 1) */

#if DPACK_LVSTRLEN_MAX >= UINT8_MAX

CUTE_TEST(dpackut_lvstr8_sizes_255)
{
	cute_check_uint(DPACK_LVSTR_SIZE(UINT8_MAX), equal, UINT8_MAX + 2);
	cute_check_uint(dpack_lvstr_size(UINT8_MAX), equal, UINT8_MAX + 2);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= UINT8_MAX) */

CUTE_TEST(dpackut_lvstr8_sizes_255)
{
	cute_skip("lvstr length >= UINT8_MAX support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= UINT8_MAX */

#if DPACK_LVSTRLEN_MAX > DPACK_STR8_LEN_MAX

CUTE_TEST(dpackut_lvstr16_sizes)
{
	/* Msgpack 16 bits string sizes. */
	cute_check_uint(DPACK_LVSTR_SIZE(UINT8_MAX + 1),
	                equal,
	                UINT8_MAX + 1 + 3);
	cute_check_uint(dpack_lvstr_size(UINT8_MAX + 1),
	                equal,
	                UINT8_MAX + 1 + 3);
}

#else  /* !(DPACK_LVSTRLEN_MAX > DPACK_STR8_LEN_MAX) */

CUTE_TEST(dpackut_lvstr16_sizes)
{
	cute_skip("lvstr length >= MsgPack str8 length "
	          " support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX > DPACK_STR8_LEN_MAX */

#if DPACK_LVSTRLEN_MAX >= (UINT8_MAX + 2)

CUTE_TEST(dpackut_lvstr16_sizes_257)
{
	cute_check_uint(DPACK_LVSTR_SIZE(UINT8_MAX + 2),
	                equal,
	                UINT8_MAX + 2 + 3);
	cute_check_uint(dpack_lvstr_size(UINT8_MAX + 2),
	                equal,
	                UINT8_MAX + 2 + 3);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT8_MAX + 2)) */

CUTE_TEST(dpackut_lvstr16_sizes_257)
{
	cute_skip("lvstr length >= (UINT8_MAX + 2) support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT8_MAX + 2) */

#if DPACK_LVSTRLEN_MAX >= (UINT16_MAX - 2)

CUTE_TEST(dpackut_lvstr16_sizes_65533)
{
	cute_check_uint(DPACK_LVSTR_SIZE(UINT16_MAX - 2),
	                equal,
	                UINT16_MAX - 2 + 3);
	cute_check_uint(dpack_lvstr_size(UINT16_MAX - 2),
	                equal,
	                UINT16_MAX - 2 + 3);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT16_MAX - 2)) */

CUTE_TEST(dpackut_lvstr16_sizes_65533)
{
	cute_skip("lvstr length >= (UINT16_MAX - 2) support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT16_MAX - 2) */

#if DPACK_LVSTRLEN_MAX >= (UINT16_MAX - 1)

CUTE_TEST(dpackut_lvstr16_sizes_65534)
{
	cute_check_uint(DPACK_LVSTR_SIZE(UINT16_MAX - 1),
	                equal,
	                UINT16_MAX - 1 + 3);
	cute_check_uint(dpack_lvstr_size(UINT16_MAX - 1),
	                equal,
	                UINT16_MAX - 1 + 3);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT16_MAX - 1)) */

CUTE_TEST(dpackut_lvstr16_sizes_65534)
{
	cute_skip("lvstr length >= (UINT16_MAX - 1) support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT16_MAX - 1) */

#if DPACK_LVSTRLEN_MAX >= UINT16_MAX

CUTE_TEST(dpackut_lvstr16_sizes_65535)
{
	cute_check_uint(DPACK_LVSTR_SIZE(UINT16_MAX), equal, UINT16_MAX + 3);
	cute_check_uint(dpack_lvstr_size(UINT16_MAX), equal, UINT16_MAX + 3);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= UINT16_MAX) */

CUTE_TEST(dpackut_lvstr16_sizes_65535)
{
	cute_skip("lvstr length >= UINT16_MAX support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= UINT16_MAX */

#if DPACK_LVSTRLEN_MAX > DPACK_STR16_LEN_MAX

CUTE_TEST(dpackut_lvstr32_sizes)
{
	/* Msgpack 32 bits string sizes. */
	cute_check_uint(DPACK_LVSTR_SIZE(UINT16_MAX + 1),
	                equal,
	                UINT16_MAX + 1 + 5);
	cute_check_uint(dpack_lvstr_size(UINT16_MAX + 1),
	                equal,
	                UINT16_MAX + 1 + 5);
}

#else  /* !(DPACK_LVSTRLEN_MAX > DPACK_STR16_LEN_MAX) */

CUTE_TEST(dpackut_lvstr32_sizes)
{
	cute_skip("lvstr length >= MsgPack str16 length "
	          " support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX > DPACK_STR16_LEN_MAX */

#if DPACK_LVSTRLEN_MAX >= (UINT16_MAX + 2)

CUTE_TEST(dpackut_lvstr32_sizes_65537)
{
	cute_check_uint(DPACK_LVSTR_SIZE(UINT16_MAX + 2),
	                equal,
	                UINT16_MAX + 2 + 5);
	cute_check_uint(dpack_lvstr_size(UINT16_MAX + 2),
	                equal,
	                UINT16_MAX + 2 + 5);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT16_MAX + 2)) */

CUTE_TEST(dpackut_lvstr32_sizes_65537)
{
	cute_skip("lvstr length >= (UINT16_MAX + 2) support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT16_MAX + 2) */

#if DPACK_LVSTRLEN_MAX >= (UINT32_MAX - 2)

CUTE_TEST(dpackut_lvstr32_sizes_maxminus2)
{
	cute_check_uint(DPACK_LVSTR_SIZE(UINT32_MAX - 2),
	                equal,
	                (uintmax_t)UINT32_MAX - 2 + 5);
	cute_check_uint(dpack_lvstr_size(UINT32_MAX - 2),
	                equal,
	                (uintmax_t)UINT32_MAX - 2 + 5);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT32_MAX - 2)) */

CUTE_TEST(dpackut_lvstr32_sizes_maxminus2)
{
	cute_skip("lvstr length >= (UINT32_MAX - 2) support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT32_MAX - 2) */

#if DPACK_LVSTRLEN_MAX >= (UINT32_MAX - 1)

CUTE_TEST(dpackut_lvstr32_sizes_maxminus1)
{
	cute_check_uint(DPACK_LVSTR_SIZE(UINT32_MAX - 1),
	                equal,
	                (uintmax_t)UINT32_MAX - 1 + 5);
	cute_check_uint(dpack_lvstr_size(UINT32_MAX - 1),
	                equal,
	                (uintmax_t)UINT32_MAX - 1 + 5);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT32_MAX - 1)) */

CUTE_TEST(dpackut_lvstr32_sizes_maxminus1)
{
	cute_skip("lvstr length >= (UINT32_MAX - 1) support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT32_MAX - 1) */

#if DPACK_LVSTRLEN_MAX >= UINT32_MAX

CUTE_TEST(dpackut_lvstr32_sizes_max)
{
	cute_check_uint(DPACK_LVSTR_SIZE(UINT32_MAX),
	                equal,
	                (uintmax_t)UINT32_MAX + 5);
	cute_check_uint(dpack_lvstr_size(UINT32_MAX),
	                equal,
	                (uintmax_t)UINT32_MAX + 5);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= UINT32_MAX) */

CUTE_TEST(dpackut_lvstr32_sizes_max)
{
	cute_skip("lvstr length >= UINT32_MAX support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= UINT32_MAX */

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_lvstr_assert_sizes)
{
	size_t sz __unused;

	cute_expect_assertion(sz = dpack_lvstr_size(0));
	cute_expect_assertion(sz = dpack_lvstr_size(DPACK_LVSTRLEN_MAX + 1));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_lvstr_assert_sizes)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_lvstr_encode_assert)
{
	struct dpack_encoder enc = { 0, };
	char *               buff;
	struct stroll_lvstr  null = STROLL_LVSTR_INIT;
	struct stroll_lvstr  lvstr = STROLL_LVSTR_INIT_LEND("test");
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_encode_lvstr(NULL, &lvstr));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_encode_lvstr(&enc, &lvstr));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	buff = malloc(DPACK_LVSTRLEN_MAX + 2);
	cute_check_ptr(buff, unequal, NULL);
	memset(buff, 'f', DPACK_LVSTRLEN_MAX + 1);
	buff[DPACK_LVSTRLEN_MAX + 1] = '\0';

	dpack_encoder_init_buffer(&enc, buff, DPACK_LVSTRLEN_MAX + 2);
	cute_expect_assertion(ret = dpack_encode_lvstr(&enc, NULL));
	cute_expect_assertion(ret = dpack_encode_lvstr(&enc, &null));
	stroll_lvstr_lend(&lvstr, buff);
	cute_expect_assertion(ret = dpack_encode_lvstr(&enc, &lvstr));
	stroll_lvstr_lend(&lvstr, "");
	cute_expect_assertion(ret = dpack_encode_lvstr(&enc, &lvstr));
	dpack_encoder_fini(&enc, DPACK_DONE);

	free(buff);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_lvstr_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

struct dpackut_lvstr_data {
	size_t              len;       /* length of string to encode / decode */
	size_t              size;      /* size of encoded string */
	ssize_t             error;
	char *              packed;
	struct stroll_lvstr value;
};

#define DPACKUT_LVSTR_DATA(_var, _len) \
	struct dpackut_lvstr_data _var = { \
		.len       = _len, \
		.error     = 0 \
	}

typedef void (dpackut_lvstr_pack_fn)(struct dpack_encoder *,
                                     const struct dpackut_lvstr_data *);

static size_t
dpackut_lvstr_size(size_t len)
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
dpackut_lvstr_gen_data(struct dpackut_lvstr_data * data)
{
	unsigned int b;
	char *       p;
	char *       value;

	data->size = dpackut_lvstr_size(data->len);
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

	value = malloc(data->len + 1);
	cute_check_ptr(value, unequal, NULL);

	p = value;
	for (b = 0; b < data->len; b++)
		*p++ = (char)('0' + (b % 10));

	value[data->len] = '\0';

	stroll_lvstr_init_ncede(&data->value, value, data->len);
}

static void
dpackut_lvstr_fini_data(struct dpackut_lvstr_data * data)
{
	free(data->packed);
	stroll_lvstr_fini(&data->value);
}

static void
dpackut_lvstr_encode(struct dpackut_lvstr_data * data,
                     dpackut_lvstr_pack_fn *     pack)
{
	char *               buff;
	struct dpack_encoder enc;

	dpackut_lvstr_gen_data(data);
	cute_check_uint(data->len, equal, stroll_lvstr_len(&data->value));

	buff = malloc(data->size);
	cute_check_ptr(buff, unequal, NULL);
	memset(buff, 0xa5, data->size);

	dpack_encoder_init_buffer(&enc, buff, data->size);

	pack(&enc, data);
	cute_check_mem(buff, equal, data->packed, data->size);
	cute_check_uint(dpack_lvstr_size(stroll_lvstr_len(&data->value)),
	                equal,
	                data->size);
	cute_check_uint(dpack_encoder_space_used(&enc), equal, data->size);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc, DPACK_DONE);

	free(buff);

	dpackut_lvstr_fini_data(data);
}

static void
dpackut_lvstr_pack(struct dpack_encoder *            encoder,
                   const struct dpackut_lvstr_data * data)
{
	cute_check_sint(dpack_encode_lvstr(encoder, &data->value), equal, 0);
}

CUTE_TEST(dpackut_lvstr_encode_1)
{
	DPACKUT_LVSTR_DATA(data, 1);

	dpackut_lvstr_encode(&data, dpackut_lvstr_pack);
}

#if DPACK_LVSTRLEN_MAX >= 31

CUTE_TEST(dpackut_lvstr_encode_31)
{
	DPACKUT_LVSTR_DATA(data, 31);
	dpackut_lvstr_encode(&data, dpackut_lvstr_pack);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 31) */

CUTE_TEST(dpackut_lvstr_encode_31)
{
	cute_skip("lvstr length >= 31 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 31 */

#if DPACK_LVSTRLEN_MAX >= 32

CUTE_TEST(dpackut_lvstr_encode_32)
{
	DPACKUT_LVSTR_DATA(data, 32);
	dpackut_lvstr_encode(&data, dpackut_lvstr_pack);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 32) */

CUTE_TEST(dpackut_lvstr_encode_32)
{
	cute_skip("lvstr length >= 32 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 32 */

#if DPACK_LVSTRLEN_MAX >= 255

CUTE_TEST(dpackut_lvstr_encode_255)
{
	DPACKUT_LVSTR_DATA(data, 255);
	dpackut_lvstr_encode(&data, dpackut_lvstr_pack);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 255) */

CUTE_TEST(dpackut_lvstr_encode_255)
{
	cute_skip("lvstr length >= 255 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 255 */

#if DPACK_LVSTRLEN_MAX >= 256

CUTE_TEST(dpackut_lvstr_encode_256)
{
	DPACKUT_LVSTR_DATA(data, 256);
	dpackut_lvstr_encode(&data, dpackut_lvstr_pack);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 256) */

CUTE_TEST(dpackut_lvstr_encode_256)
{
	cute_skip("lvstr length >= 256 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 256 */

#if DPACK_LVSTRLEN_MAX >= 65535

CUTE_TEST(dpackut_lvstr_encode_65535)
{
	DPACKUT_LVSTR_DATA(data, 65535);
	dpackut_lvstr_encode(&data, dpackut_lvstr_pack);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_lvstr_encode_65535)
{
	cute_skip("lvstr length >= 65535 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 65535 */

#if DPACK_LVSTRLEN_MAX >= 65536

CUTE_TEST(dpackut_lvstr_encode_65536)
{
	DPACKUT_LVSTR_DATA(data, 65536);
	dpackut_lvstr_encode(&data, dpackut_lvstr_pack);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_lvstr_encode_65536)
{
	cute_skip("lvstr length >= 65536 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_lvstr_encode_max)
{
	DPACKUT_LVSTR_DATA(data, DPACK_LVSTRLEN_MAX);
	dpackut_lvstr_encode(&data, dpackut_lvstr_pack);
}

CUTE_GROUP(dpackut_lvstr_group) = {
	CUTE_REF(dpackut_fixlvstr_sizes),
	CUTE_REF(dpackut_fixlvstr_sizes_30),
	CUTE_REF(dpackut_fixlvstr_sizes_31),
	CUTE_REF(dpackut_lvstr8_sizes),
	CUTE_REF(dpackut_lvstr8_sizes_33),
	CUTE_REF(dpackut_lvstr8_sizes_253),
	CUTE_REF(dpackut_lvstr8_sizes_254),
	CUTE_REF(dpackut_lvstr8_sizes_255),
	CUTE_REF(dpackut_lvstr16_sizes),
	CUTE_REF(dpackut_lvstr16_sizes_257),
	CUTE_REF(dpackut_lvstr16_sizes_65533),
	CUTE_REF(dpackut_lvstr16_sizes_65534),
	CUTE_REF(dpackut_lvstr16_sizes_65535),
	CUTE_REF(dpackut_lvstr32_sizes),
	CUTE_REF(dpackut_lvstr32_sizes_65537),
	CUTE_REF(dpackut_lvstr32_sizes_maxminus2),
	CUTE_REF(dpackut_lvstr32_sizes_maxminus1),
	CUTE_REF(dpackut_lvstr32_sizes_max),
	CUTE_REF(dpackut_lvstr_assert_sizes),

	CUTE_REF(dpackut_lvstr_encode_assert),
	CUTE_REF(dpackut_lvstr_encode_1),
	CUTE_REF(dpackut_lvstr_encode_31),
	CUTE_REF(dpackut_lvstr_encode_32),
	CUTE_REF(dpackut_lvstr_encode_255),
	CUTE_REF(dpackut_lvstr_encode_256),
	CUTE_REF(dpackut_lvstr_encode_65535),
	CUTE_REF(dpackut_lvstr_encode_65536),
	CUTE_REF(dpackut_lvstr_encode_max),
};

CUTE_SUITE_EXTERN(dpackut_lvstr_suite,
                  dpackut_lvstr_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
