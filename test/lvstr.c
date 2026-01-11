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

#if DPACK_LVSTRLEN_MAX > _DPACK_FIXSTR_LEN_MAX

CUTE_TEST(dpackut_lvstr8_sizes)
{
	/* Msgpack 8 bits string sizes. */
	cute_check_uint(DPACK_LVSTR_SIZE(32), equal, 34);
	cute_check_uint(dpack_lvstr_size(32), equal, 34);
}

#else  /* !(DPACK_LVSTRLEN_MAX > _DPACK_FIXSTR_LEN_MAX) */

CUTE_TEST(dpackut_lvstr8_sizes)
{
	cute_skip("lvstr length >= MsgPack fixstr length "
	          " support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX > _DPACK_FIXSTR_LEN_MAX */

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

#if DPACK_LVSTRLEN_MAX > _DPACK_STR8_LEN_MAX

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

#else  /* !(DPACK_LVSTRLEN_MAX > _DPACK_STR8_LEN_MAX) */

CUTE_TEST(dpackut_lvstr16_sizes)
{
	cute_skip("lvstr length >= MsgPack str8 length "
	          " support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX > _DPACK_STR8_LEN_MAX */

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

#if DPACK_LVSTRLEN_MAX > _DPACK_STR16_LEN_MAX

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

#else  /* !(DPACK_LVSTRLEN_MAX > _DPACK_STR16_LEN_MAX) */

CUTE_TEST(dpackut_lvstr32_sizes)
{
	cute_skip("lvstr length >= MsgPack str16 length "
	          " support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX > _DPACK_STR16_LEN_MAX */

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
	struct dpack_encoder_buffer enc = { 0, };
	char *                      buff;
	struct stroll_lvstr         null = STROLL_LVSTR_INIT;
	struct stroll_lvstr         lvstr = STROLL_LVSTR_INIT_LEND("test");
	int                         ret __unused;

	cute_expect_assertion(ret = dpack_encode_lvstr(NULL, &lvstr));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_encode_lvstr(&enc.base, &lvstr));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	buff = malloc(DPACK_LVSTRLEN_MAX + 2);
	cute_check_ptr(buff, unequal, NULL);
	memset(buff, 'f', DPACK_LVSTRLEN_MAX + 1);
	buff[DPACK_LVSTRLEN_MAX + 1] = '\0';

	dpack_encoder_init_buffer(&enc,
	                          (uint8_t *)buff,
	                          DPACK_LVSTRLEN_MAX + 2);
	cute_expect_assertion(ret = dpack_encode_lvstr(&enc.base, NULL));
	cute_expect_assertion(ret = dpack_encode_lvstr(&enc.base, &null));
	stroll_lvstr_lend(&lvstr, buff);
	cute_expect_assertion(ret = dpack_encode_lvstr(&enc.base, &lvstr));
	stroll_lvstr_lend(&lvstr, "");
	cute_expect_assertion(ret = dpack_encode_lvstr(&enc.base, &lvstr));
	dpack_encoder_fini(&enc.base);

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
	size_t              equ;
	size_t              low;
	size_t              high;
	uint8_t *           packed;
	struct stroll_lvstr value;
};

#define DPACKUT_LVSTR_DATA(_var, _len) \
	struct dpackut_lvstr_data _var = { \
		.len       = _len, \
		.error     = 0 \
	}

typedef void (dpackut_lvstr_pack_fn)(struct dpack_encoder *,
                                     const struct dpackut_lvstr_data *);

typedef void (dpackut_lvstr_unpack_fn)(struct dpack_decoder *,
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
	uint8_t *    p;
	uint8_t *    value;

	data->size = dpackut_lvstr_size(data->len);
	data->packed = malloc(data->size);
	cute_check_ptr(data->packed, unequal, NULL);

	p = data->packed;
	switch (data->len) {
	case 0 ... 31:
		*p++ = (uint8_t)(0xa0U | (data->len & 0x1fU));
		break;
	case 32 ... 255:
		*p++ = (uint8_t)0xd9U;
		*p++ = (uint8_t)(data->len & 0xffU);
		break;
	case 256 ... 65535:
		*p++ = (uint8_t)0xdaU;
		*p++ = (uint8_t)((data->len >> 8) & 0xffU);
		*p++ = (uint8_t)(data->len & 0xffU);
		break;
	case 65536 ... 4294967295:
		*p++ = (uint8_t)0xdbU;
		*p++ = (uint8_t)((data->len >> 24) & 0xffU);
		*p++ = (uint8_t)((data->len >> 16) & 0xffU);
		*p++ = (uint8_t)((data->len >> 8) & 0xffU);
		*p++ = (uint8_t)(data->len & 0xffU);
		break;
	default:
		cute_fail("unsupported MsgPack string size");
	}
	for (b = 0; b < data->len; b++)
		*p++ = (uint8_t)('0' + (b % 10));

	value = malloc(data->len + 1);
	cute_check_ptr(value, unequal, NULL);

	p = value;
	for (b = 0; b < data->len; b++)
		*p++ = (uint8_t)('0' + (b % 10));

	value[data->len] = '\0';

	stroll_lvstr_init_ncede(&data->value, (char *)value, data->len);
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
	char *                      buff;
	struct dpack_encoder_buffer enc;

	dpackut_lvstr_gen_data(data);
	cute_check_uint(data->len, equal, stroll_lvstr_len(&data->value));

	buff = malloc(data->size);
	cute_check_ptr(buff, unequal, NULL);
	memset(buff, 0xa5, data->size);

	dpack_encoder_init_buffer(&enc, (uint8_t *)buff, data->size);
	pack(&enc.base, data);
	cute_check_mem(buff, equal, data->packed, data->size);
	cute_check_uint(dpack_lvstr_size(stroll_lvstr_len(&data->value)),
	                equal,
	                data->size);
	cute_check_uint(dpack_encoder_space_used(&enc.base), equal, data->size);
	cute_check_uint(dpack_encoder_space_left(&enc.base), equal, 0);

	dpack_encoder_fini(&enc.base);

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

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_lvstr_decode_assert)
{
	struct dpack_decoder_buffer dec = { 0, };
	struct stroll_lvstr         lvstr = STROLL_LVSTR_INIT;
	char                        buff[8];
	ssize_t                     ret __unused;

	cute_expect_assertion(ret = dpack_decode_lvstr(NULL, &lvstr));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_lvstr(&dec.base, &lvstr));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, (uint8_t *)buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_lvstr(&dec.base, NULL));
	dpack_decoder_fini(&dec.base);
}

#else  /* !(defined(CONFIG_DPACK_ASSERT_API)) */

CUTE_TEST(dpackut_lvstr_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif  /* defined(CONFIG_DPACK_ASSERT_API) */

#define DPACKUT_LVSTR_DEC(_var, _len, _error) \
	struct dpackut_lvstr_data _var = { \
		.len       = _len, \
		.error     = _error \
	}

static void
dpackut_lvstr_decode(struct dpackut_lvstr_data * data,
                     dpackut_lvstr_unpack_fn *   unpack)
{
	struct dpack_decoder_buffer dec;

	dpackut_lvstr_gen_data(data);
	cute_check_uint(data->len, equal, stroll_lvstr_len(&data->value));

	cute_check_uint(dpackut_lvstr_size(data->len), equal, data->size);

	dpack_decoder_init_buffer(&dec, data->packed, data->size);
	unpack(&dec.base, data);
	if (!data->error)
		cute_check_uint(dpack_decoder_data_left(&dec.base), equal, 0);
	dpack_decoder_fini(&dec.base);

	dpackut_lvstr_fini_data(data);
}

static void
dpackut_lvstr_unpack(struct dpack_decoder *            decoder,
                     const struct dpackut_lvstr_data * data)
{
	struct stroll_lvstr val = STROLL_LVSTR_INIT;

	cute_check_sint(dpack_decode_lvstr(decoder, &val), equal, data->error);

	if (data->error >= 0) {
		cute_check_uint(stroll_lvstr_len(&val), equal, data->len);
		cute_check_str(stroll_lvstr_cstr(&val),
		               equal,
		               stroll_lvstr_cstr(&data->value));
		stroll_lvstr_fini(&val);
	}
}

CUTE_TEST(dpackut_lvstr_decode_0)
{
	DPACKUT_LVSTR_DEC(data, 0, -EBADMSG);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack);
}

CUTE_TEST(dpackut_lvstr_decode_1)
{
	DPACKUT_LVSTR_DEC(data, 1, 1);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack);
}

#if DPACK_LVSTRLEN_MAX >= 31

CUTE_TEST(dpackut_lvstr_decode_31)
{
	DPACKUT_LVSTR_DEC(data, 31, 31);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 31) */

CUTE_TEST(dpackut_lvstr_decode_31)
{
	cute_skip("lvstr length >= 31 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 31 */

#if DPACK_LVSTRLEN_MAX >= 32

CUTE_TEST(dpackut_lvstr_decode_32)
{
	DPACKUT_LVSTR_DEC(data, 32, 32);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 32) */

CUTE_TEST(dpackut_lvstr_decode_32)
{
	cute_skip("lvstr length >= 32 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 32 */

#if DPACK_LVSTRLEN_MAX >= 255

CUTE_TEST(dpackut_lvstr_decode_255)
{
	DPACKUT_LVSTR_DEC(data, 255, 255);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 255) */

CUTE_TEST(dpackut_lvstr_decode_255)
{
	cute_skip("lvstr length >= 255 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 255 */

#if DPACK_LVSTRLEN_MAX >= 256

CUTE_TEST(dpackut_lvstr_decode_256)
{
	DPACKUT_LVSTR_DEC(data, 256, 256);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 256) */

CUTE_TEST(dpackut_lvstr_decode_256)
{
	cute_skip("lvstr length >= 256 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 256 */

#if DPACK_LVSTRLEN_MAX >= 65535

CUTE_TEST(dpackut_lvstr_decode_65535)
{
	DPACKUT_LVSTR_DEC(data, 65535, 65535);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_lvstr_decode_65535)
{
	cute_skip("lvstr length >= 65535 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 65535 */

#if DPACK_LVSTRLEN_MAX >= 65536

CUTE_TEST(dpackut_lvstr_decode_65536)
{
	DPACKUT_LVSTR_DEC(data, 65536, 65536);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_lvstr_decode_65536)
{
	cute_skip("lvstr length >= 65536 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_lvstr_decode_max)
{
	DPACKUT_LVSTR_DEC(data, DPACK_LVSTRLEN_MAX, DPACK_LVSTRLEN_MAX);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack);
}

CUTE_TEST(dpackut_lvstr_decode_maxplus1)
{
	DPACKUT_LVSTR_DEC(data, DPACK_LVSTRLEN_MAX + 1, -ENOTSUP);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack);
}

CUTE_TEST(dpackut_lvstr_decode_fail)
{
	struct dpack_decoder_buffer dec;
	char                        buff[] = "\xa1\x30";
	struct stroll_lvstr         lvstr = STROLL_LVSTR_INIT;
	ssize_t                     ret __unused;

	dpack_decoder_init_buffer(&dec, (uint8_t *)buff, sizeof(buff));
	if (dpackut_expect_malloc()) {
		cute_check_sint(dpack_decode_lvstr(&dec.base, &lvstr),
		                equal,
		                1);
		cute_check_ptr(stroll_lvstr_cstr(&lvstr), unequal, NULL);
		stroll_lvstr_fini(&lvstr);
	}
	else
		cute_check_sint(dpack_decode_lvstr(&dec.base, &lvstr),
		                equal,
		                -ENOMEM);
	dpack_decoder_fini(&dec.base);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_lvstr_decode_equ_assert)
{
	struct dpack_decoder_buffer dec = { 0, };
	struct stroll_lvstr         lvstr = STROLL_LVSTR_INIT;
	char                        buff[8];
	ssize_t                     ret __unused;

	cute_expect_assertion(ret = dpack_decode_lvstr_equ(NULL, 2, &lvstr));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_lvstr_equ(&dec.base,
	                                                   2,
	                                                   &lvstr));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, (uint8_t *)buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_lvstr_equ(&dec.base,
	                                                   2,
	                                                   NULL));
	cute_expect_assertion(ret = dpack_decode_lvstr_equ(&dec.base,
	                                                   0,
	                                                   &lvstr));
	cute_expect_assertion(ret = dpack_decode_lvstr_equ(&dec.base,
	                                                    DPACK_LVSTRLEN_MAX +
	                                                    1,
	                                                    &lvstr));
	dpack_decoder_fini(&dec.base);
}

#else  /* !(defined(CONFIG_DPACK_ASSERT_API)) */

CUTE_TEST(dpackut_lvstr_decode_equ_assert)
{
	cute_skip("assertion unsupported");
}

#endif  /* defined(CONFIG_DPACK_ASSERT_API) */

#define DPACKUT_LVSTR_DEC_EQU(_len, _error, _equ) \
	((struct dpackut_lvstr_data) { \
		.len       = _len, \
		.error     = _error, \
		.equ       = _equ \
	 })

static void
dpackut_lvstr_unpack_equ(struct dpack_decoder *            decoder,
                         const struct dpackut_lvstr_data * data)
{
	struct stroll_lvstr val = STROLL_LVSTR_INIT;

	cute_check_sint(dpack_decode_lvstr_equ(decoder, data->equ, &val),
	                equal,
	                data->error);

	if (data->error >= 0) {
		cute_check_uint(stroll_lvstr_len(&val), equal, data->len);
		cute_check_str(stroll_lvstr_cstr(&val),
		               equal,
		               stroll_lvstr_cstr(&data->value));
		stroll_lvstr_fini(&val);
	}
}

CUTE_TEST(dpackut_lvstr_decode_equ_2)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_EQU(0, -EBADMSG, 1);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(1, 1, 1);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(2, -EMSGSIZE, 1);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);
}

#if DPACK_LVSTRLEN_MAX >= 31

CUTE_TEST(dpackut_lvstr_decode_equ_31)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_EQU(30, -EMSGSIZE, 31);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(31, 31, 31);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(32, -EMSGSIZE, 31);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 31) */

CUTE_TEST(dpackut_lvstr_decode_equ_31)
{
	cute_skip("lvstr length >= 31 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 31 */

#if DPACK_LVSTRLEN_MAX >= 32

CUTE_TEST(dpackut_lvstr_decode_equ_32)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_EQU(31, -EMSGSIZE, 32);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(32, 32, 32);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(33, -EMSGSIZE, 32);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 32) */

CUTE_TEST(dpackut_lvstr_decode_equ_32)
{
	cute_skip("lvstr length >= 32 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 32 */

#if DPACK_LVSTRLEN_MAX >= 255

CUTE_TEST(dpackut_lvstr_decode_equ_255)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_EQU(254, -EMSGSIZE, 255),
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(255, 255, 255),
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(256, -EMSGSIZE, 255),
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 255) */

CUTE_TEST(dpackut_lvstr_decode_equ_255)
{
	cute_skip("lvstr length >= 255 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 255 */

#if DPACK_LVSTRLEN_MAX >= 256

CUTE_TEST(dpackut_lvstr_decode_equ_256)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_EQU(255, -EMSGSIZE, 256),
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(256, 256, 256),
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(257, -EMSGSIZE, 256),
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 256) */

CUTE_TEST(dpackut_lvstr_decode_equ_256)
{
	cute_skip("lvstr length >= 256 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 256 */

#if DPACK_LVSTRLEN_MAX >= 65535

CUTE_TEST(dpackut_lvstr_decode_equ_65535)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_EQU(65534, -EMSGSIZE, 65535),
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(65535, 65535, 65535),
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(65536, -EMSGSIZE, 65535),
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_lvstr_decode_equ_65535)
{
	cute_skip("lvstr length >= 65535 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 65535 */

#if DPACK_LVSTRLEN_MAX >= 65536

CUTE_TEST(dpackut_lvstr_decode_equ_65536)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_EQU(65535, -EMSGSIZE, 65536);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(65536, 65536, 65536);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(65537, -EMSGSIZE, 65536);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_lvstr_decode_equ_65536)
{
	cute_skip("lvstr length >= 65536 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_lvstr_decode_equ_max)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_EQU(DPACK_LVSTRLEN_MAX - 1,
	                             -EMSGSIZE,
	                             DPACK_LVSTRLEN_MAX);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(DPACK_LVSTRLEN_MAX,
	                             DPACK_LVSTRLEN_MAX,
	                             DPACK_LVSTRLEN_MAX);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);

	data = DPACKUT_LVSTR_DEC_EQU(DPACK_LVSTRLEN_MAX + 1,
	                             -ENOTSUP,
	                             DPACK_LVSTRLEN_MAX);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_equ);
}

#define DPACKUT_LVSTR_DEC_MAX(_len, _error, _high) \
	((struct dpackut_lvstr_data) { \
		.len       = _len, \
		.error     = _error, \
		.high      = _high \
	 })

static void
dpackut_lvstr_unpack_max(struct dpack_decoder *            decoder,
                         const struct dpackut_lvstr_data * data)
{
	struct stroll_lvstr val = STROLL_LVSTR_INIT;

	cute_check_sint(dpack_decode_lvstr_max(decoder, data->high, &val),
	                equal,
	                data->error);

	if (data->error >= 0) {
		cute_check_uint(stroll_lvstr_len(&val), equal, data->len);
		cute_check_str(stroll_lvstr_cstr(&val),
		               equal,
		               stroll_lvstr_cstr(&data->value));
		stroll_lvstr_fini(&val);
	}
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_lvstr_decode_max_assert)
{
	struct dpack_decoder_buffer dec = { 0, };
	struct stroll_lvstr         lvstr = STROLL_LVSTR_INIT;
	char                        buff[8];
	ssize_t                     ret __unused;

	cute_expect_assertion(ret = dpack_decode_lvstr_max(NULL, 2, &lvstr));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_lvstr_max(&dec.base,
	                                                   2,
	                                                   &lvstr));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, (uint8_t *)buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_lvstr_max(&dec.base,
	                                                   2,
	                                                   NULL));
	cute_expect_assertion(ret = dpack_decode_lvstr_max(&dec.base,
	                                                   1,
	                                                   &lvstr));
	cute_expect_assertion(ret = dpack_decode_lvstr_max(&dec.base,
	                                                    DPACK_LVSTRLEN_MAX +
	                                                    1,
	                                                    &lvstr));
	dpack_decoder_fini(&dec.base);
}

#else  /* !(defined(CONFIG_DPACK_ASSERT_API)) */

CUTE_TEST(dpackut_lvstr_decode_max_assert)
{
	cute_skip("assertion unsupported");
}

#endif  /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_lvstr_decode_max_2)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_MAX(0, -EBADMSG, 2);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(2,  2, 2);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(3,  -EMSGSIZE, 2);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);
}

#if DPACK_LVSTRLEN_MAX >= 31

CUTE_TEST(dpackut_lvstr_decode_max_31)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_MAX(30, 30, 31);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(31, 31, 31);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(32, -EMSGSIZE, 31);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 31) */

CUTE_TEST(dpackut_lvstr_decode_max_31)
{
	cute_skip("lvstr length >= 31 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 31 */

#if DPACK_LVSTRLEN_MAX >= 32

CUTE_TEST(dpackut_lvstr_decode_max_32)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_MAX(31, 31, 32);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(32, 32, 32);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(33, -EMSGSIZE, 32);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 32) */

CUTE_TEST(dpackut_lvstr_decode_max_32)
{
	cute_skip("lvstr length >= 32 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 32 */

#if DPACK_LVSTRLEN_MAX >= 255

CUTE_TEST(dpackut_lvstr_decode_max_255)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_MAX(254, 254, 255);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(255, 255, 255);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(256, -EMSGSIZE, 255);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 255) */

CUTE_TEST(dpackut_lvstr_decode_max_255)
{
	cute_skip("lvstr length >= 255 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 255 */

#if DPACK_LVSTRLEN_MAX >= 256

CUTE_TEST(dpackut_lvstr_decode_max_256)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_MAX(255, 255, 256);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(256, 256, 256);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(257, -EMSGSIZE, 256);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 256) */

CUTE_TEST(dpackut_lvstr_decode_max_256)
{
	cute_skip("lvstr length >= 256 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 256 */

#if DPACK_LVSTRLEN_MAX >= 65535

CUTE_TEST(dpackut_lvstr_decode_max_65535)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_MAX(65534, 65534, 65535);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(65535, 65535, 65535);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(65536, -EMSGSIZE, 65535);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_lvstr_decode_max_65535)
{
	cute_skip("lvstr length >= 65535 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 65535 */

#if DPACK_LVSTRLEN_MAX >= 65536

CUTE_TEST(dpackut_lvstr_decode_max_65536)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_MAX(65535, 65535, 65536);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(65536, 65536, 65536);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(65537, -EMSGSIZE, 65536);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_lvstr_decode_max_65536)
{
	cute_skip("lvstr length >= 65536 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_lvstr_decode_max_max)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_MAX(DPACK_LVSTRLEN_MAX,
	                             DPACK_LVSTRLEN_MAX,
	                             DPACK_LVSTRLEN_MAX);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);

	data = DPACKUT_LVSTR_DEC_MAX(DPACK_LVSTRLEN_MAX + 1,
	                             -ENOTSUP,
	                             DPACK_LVSTRLEN_MAX);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_max);
}

#define DPACKUT_LVSTR_DEC_RANGE(_len, _error, _low, _high) \
	((struct dpackut_lvstr_data) { \
		.len       = _len, \
		.error     = _error, \
		.low       = _low, \
		.high      = _high \
	 })

static void
dpackut_lvstr_unpack_range(struct dpack_decoder *            decoder,
                           const struct dpackut_lvstr_data * data)
{
	struct stroll_lvstr val = STROLL_LVSTR_INIT;

	cute_check_sint(dpack_decode_lvstr_range(decoder,
	                                         data->low,
	                                         data->high,
	                                         &val),
	                equal,
	                data->error);

	if (data->error >= 0) {
		cute_check_uint(stroll_lvstr_len(&val), equal, data->len);
		cute_check_str(stroll_lvstr_cstr(&val),
		               equal,
		               stroll_lvstr_cstr(&data->value));
		stroll_lvstr_fini(&val);
	}
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_lvstr_decode_range_assert)
{
	struct dpack_decoder_buffer dec = { 0, };
	struct stroll_lvstr         lvstr = STROLL_LVSTR_INIT;
	char                        buff[8];
	ssize_t                     ret __unused;

	cute_expect_assertion(ret = dpack_decode_lvstr_range(NULL,
	                                                     1,
	                                                     2,
	                                                     &lvstr));
#if defined(CONFIG_DPACK_DEBUG)
	cute_expect_assertion(ret = dpack_decode_lvstr_range(&dec.base,
	                                                     1,
	                                                     2,
	                                                     &lvstr));
#endif /* defined(CONFIG_DPACK_DEBUG) */

	dpack_decoder_init_buffer(&dec, (uint8_t *)buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_lvstr_range(&dec.base,
	                                                     1,
	                                                     2,
	                                                     NULL));
	cute_expect_assertion(ret = dpack_decode_lvstr_range(&dec.base,
	                                                     0,
	                                                     2,
	                                                     &lvstr));
	cute_expect_assertion(ret = dpack_decode_lvstr_range(&dec.base,
	                                                     1,
	                                                     DPACK_LVSTRLEN_MAX
	                                                     + 1,
	                                                     &lvstr));
	cute_expect_assertion(ret = dpack_decode_lvstr_range(&dec.base,
	                                                     1,
	                                                     1,
	                                                     &lvstr));
	dpack_decoder_fini(&dec.base);
}

#else  /* !(defined(CONFIG_DPACK_ASSERT_API)) */

CUTE_TEST(dpackut_lvstr_decode_range_assert)
{
	cute_skip("assertion unsupported");
}

#endif  /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_lvstr_decode_range_1_2)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_RANGE(0, -EBADMSG, 1, 2);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(1, 1, 1, 2);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(2, 2, 1, 2);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(3, -EMSGSIZE, 1, 2);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);
}

#if DPACK_LVSTRLEN_MAX >= 31

CUTE_TEST(dpackut_lvstr_decode_range_30_31)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_RANGE(29, -EMSGSIZE, 30, 31);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(30, 30, 30, 31);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(31, 31, 30, 31);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(32, -EMSGSIZE, 30, 31);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 31) */

CUTE_TEST(dpackut_lvstr_decode_range_30_31)
{
	cute_skip("lvstr length >= 31 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 31 */

#if DPACK_LVSTRLEN_MAX >= 32

CUTE_TEST(dpackut_lvstr_decode_range_31_32)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_RANGE(30, -EMSGSIZE, 31, 32);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(31, 31, 31, 32);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(32, 32, 31, 32);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(33, -EMSGSIZE, 31, 32);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 32) */

CUTE_TEST(dpackut_lvstr_decode_range_31_32)
{
	cute_skip("lvstr length >= 32 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 32 */

#if DPACK_LVSTRLEN_MAX >= 255

CUTE_TEST(dpackut_lvstr_decode_range_254_255)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_RANGE(253, -EMSGSIZE, 254, 255);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(254, 254, 254, 255);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(255, 255, 254, 255);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(256, -EMSGSIZE, 254, 255);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 255) */

CUTE_TEST(dpackut_lvstr_decode_range_254_255)
{
	cute_skip("lvstr length >= 255 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 255 */

#if DPACK_LVSTRLEN_MAX >= 256

CUTE_TEST(dpackut_lvstr_decode_range_255_256)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_RANGE(254, -EMSGSIZE, 255, 256);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(255, 255, 255, 256);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(256, 256, 255, 256);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(257, -EMSGSIZE, 255, 256);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 256) */

CUTE_TEST(dpackut_lvstr_decode_range_255_256)
{
	cute_skip("lvstr length >= 256 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 256 */

#if DPACK_LVSTRLEN_MAX >= 65535

CUTE_TEST(dpackut_lvstr_decode_range_65535)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_RANGE(65533, -EMSGSIZE, 65534, 65535);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(65534, 65534, 65534, 65535);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(65535, 65535, 65534, 65535);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(65536, -EMSGSIZE, 65534, 65535);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 65535) */

CUTE_TEST(dpackut_lvstr_decode_range_65535)
{
	cute_skip("lvstr length >= 65535 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 65535 */

#if DPACK_LVSTRLEN_MAX >= 65536

CUTE_TEST(dpackut_lvstr_decode_range_65536)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_RANGE(65534, -EMSGSIZE, 65535, 65536);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(65535, 65535, 65535, 65536);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(65536, 65536, 65535, 65536);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(65537, -EMSGSIZE, 65535, 65536);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 65536) */

CUTE_TEST(dpackut_lvstr_decode_range_65536)
{
	cute_skip("lvstr length >= 65536 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 65536 */

CUTE_TEST(dpackut_lvstr_decode_range_maxminus1_max)
{
	struct dpackut_lvstr_data data;

	data = DPACKUT_LVSTR_DEC_RANGE(DPACK_LVSTRLEN_MAX - 2,
	                               -EMSGSIZE,
	                               DPACK_LVSTRLEN_MAX - 1,
	                               DPACK_LVSTRLEN_MAX);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(DPACK_LVSTRLEN_MAX - 1,
	                               DPACK_LVSTRLEN_MAX - 1,
	                               DPACK_LVSTRLEN_MAX - 1,
	                               DPACK_LVSTRLEN_MAX);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(DPACK_LVSTRLEN_MAX,
	                               DPACK_LVSTRLEN_MAX,
	                               DPACK_LVSTRLEN_MAX - 1,
	                               DPACK_LVSTRLEN_MAX);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);

	data = DPACKUT_LVSTR_DEC_RANGE(DPACK_LVSTRLEN_MAX + 1,
	                               -ENOTSUP,
	                               DPACK_LVSTRLEN_MAX - 1,
	                               DPACK_LVSTRLEN_MAX);
	dpackut_lvstr_decode(&data, dpackut_lvstr_unpack_range);
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

	CUTE_REF(dpackut_lvstr_decode_assert),
	CUTE_REF(dpackut_lvstr_decode_0),
	CUTE_REF(dpackut_lvstr_decode_1),
	CUTE_REF(dpackut_lvstr_decode_31),
	CUTE_REF(dpackut_lvstr_decode_32),
	CUTE_REF(dpackut_lvstr_decode_255),
	CUTE_REF(dpackut_lvstr_decode_256),
	CUTE_REF(dpackut_lvstr_decode_65535),
	CUTE_REF(dpackut_lvstr_decode_65536),
	CUTE_REF(dpackut_lvstr_decode_max),
	CUTE_REF(dpackut_lvstr_decode_maxplus1),
	CUTE_REF(dpackut_lvstr_decode_fail),

	CUTE_REF(dpackut_lvstr_decode_equ_assert),
	CUTE_REF(dpackut_lvstr_decode_equ_2),
	CUTE_REF(dpackut_lvstr_decode_equ_31),
	CUTE_REF(dpackut_lvstr_decode_equ_32),
	CUTE_REF(dpackut_lvstr_decode_equ_255),
	CUTE_REF(dpackut_lvstr_decode_equ_256),
	CUTE_REF(dpackut_lvstr_decode_equ_65535),
	CUTE_REF(dpackut_lvstr_decode_equ_65536),
	CUTE_REF(dpackut_lvstr_decode_equ_max),

	CUTE_REF(dpackut_lvstr_decode_max_assert),
	CUTE_REF(dpackut_lvstr_decode_max_2),
	CUTE_REF(dpackut_lvstr_decode_max_31),
	CUTE_REF(dpackut_lvstr_decode_max_32),
	CUTE_REF(dpackut_lvstr_decode_max_255),
	CUTE_REF(dpackut_lvstr_decode_max_256),
	CUTE_REF(dpackut_lvstr_decode_max_65535),
	CUTE_REF(dpackut_lvstr_decode_max_65536),
	CUTE_REF(dpackut_lvstr_decode_max_max),

	CUTE_REF(dpackut_lvstr_decode_range_assert),
	CUTE_REF(dpackut_lvstr_decode_range_1_2),
	CUTE_REF(dpackut_lvstr_decode_range_30_31),
	CUTE_REF(dpackut_lvstr_decode_range_31_32),
	CUTE_REF(dpackut_lvstr_decode_range_254_255),
	CUTE_REF(dpackut_lvstr_decode_range_255_256),
	CUTE_REF(dpackut_lvstr_decode_range_65535),
	CUTE_REF(dpackut_lvstr_decode_range_65536),
	CUTE_REF(dpackut_lvstr_decode_range_maxminus1_max)
};

CUTE_SUITE_EXTERN(dpackut_lvstr_suite,
                  dpackut_lvstr_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
