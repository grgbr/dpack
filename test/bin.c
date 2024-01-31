/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/bin.h"
#include "dpack/codec.h"
#include "utest.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <errno.h>

#if defined(ZERO_BIN_SIZE_UTEST)

CUTE_TEST(dpackut_zero_bin_size)
{
	/* Should fail to compile since 0 size bins are not allowed. */
	int sz = DPACK_BIN_SIZE(0);
}

#endif /* defined(ZERO_BIN_SIZE_UTEST) */

#if defined(NONCONST_BIN_SIZE_UTEST)

CUTE_TEST(dpackut_nonconst_bin_size)
{
	unsigned int sz;

	/* Should fail to compile since sz is not constant. */
	cute_check_uint(DPACK_BIN_SIZE(sz), equal, 0);
}

#endif /* defined(NONCONST_BIN_SIZE_UTEST) */

#if defined(MAXSZ_BIN_SIZE_UTEST)

CUTE_TEST(dpackut_maxsz_bin_size)
{
	/*
	 * Should fail to compile since requested size > maximum bin size
	 * allowed.
	 */
	cute_check_uint(DPACK_BIN_SIZE(DPACK_BINSZ_MAX + 1), equal, 0);
}

#endif /* defined(MAXSZ_BIN_SIZE_UTEST) */

CUTE_TEST(dpackut_bin8_sizes)
{
	/* Msgpack bin8 sizes. */
	cute_check_uint(DPACK_BIN_SIZE(1), equal,  3);
	cute_check_uint(dpack_bin_size(1), equal,  3);
	cute_check_uint(DPACK_BIN_SIZE(2), equal,  4);
	cute_check_uint(dpack_bin_size(2), equal,  4);
#if DPACK_BINSZ_MAX >= UINT8_MAX
	cute_check_uint(DPACK_BIN_SIZE(255), equal, 257);
	cute_check_uint(dpack_bin_size(255), equal, 257);
#endif
}

#if DPACK_BINSZ_MAX > _DPACK_BIN8_SIZE_MAX

CUTE_TEST(dpackut_bin16_sizes)
{
	/* Msgpack 16 bits bin sizes. */
	cute_check_uint(DPACK_BIN_SIZE(UINT8_MAX + 1), equal,  UINT8_MAX + 1 + 3);
	cute_check_uint(dpack_bin_size(UINT8_MAX + 1), equal,  UINT8_MAX + 1 + 3);
#if DPACK_BINSZ_MAX >= (UINT8_MAX + 2)
	cute_check_uint(DPACK_BIN_SIZE(UINT8_MAX + 2), equal,  UINT8_MAX + 2 + 3);
	cute_check_uint(dpack_bin_size(UINT8_MAX + 2), equal,  UINT8_MAX + 2 + 3);
#endif
#if DPACK_BINSZ_MAX >= (UINT16_MAX - 2)
	cute_check_uint(DPACK_BIN_SIZE(UINT16_MAX - 2), equal, UINT16_MAX - 2 + 3);
	cute_check_uint(dpack_bin_size(UINT16_MAX - 2), equal, UINT16_MAX - 2 + 3);
#endif
#if DPACK_BINSZ_MAX >= (UINT16_MAX - 1)
	cute_check_uint(DPACK_BIN_SIZE(UINT16_MAX - 1), equal, UINT16_MAX - 1 + 3);
	cute_check_uint(dpack_bin_size(UINT16_MAX - 1), equal, UINT16_MAX - 1 + 3);
#endif
#if DPACK_BINSZ_MAX >= UINT16_MAX
	cute_check_uint(DPACK_BIN_SIZE(UINT16_MAX),     equal, UINT16_MAX + 3);
	cute_check_uint(dpack_bin_size(UINT16_MAX),     equal, UINT16_MAX + 3);
#endif
}

#else  /* !(DPACK_BINSZ_MAX > _DPACK_BIN8_SIZE_MAX) */

CUTE_TEST(dpackut_bin16_sizes)
{
	cute_skip("16-bits MessagePack bin support not compiled-in");
}

#endif /* DPACK_BINSZ_MAX > _DPACK_BIN8_SIZE_MAX */

#if DPACK_BINSZ_MAX > _DPACK_BIN16_SIZE_MAX

CUTE_TEST(dpackut_bin32_sizes)
{
	/* Msgpack 32 bits bin sizes. */
	cute_check_uint(DPACK_BIN_SIZE(UINT16_MAX + 1), equal, UINT16_MAX + 1 + 5);
	cute_check_uint(dpack_bin_size(UINT16_MAX + 1), equal, UINT16_MAX + 1 + 5);
#if DPACK_BINSZ_MAX >= (UINT16_MAX + 2)
	cute_check_uint(DPACK_BIN_SIZE(UINT16_MAX + 2), equal, UINT16_MAX + 2 + 5);
	cute_check_uint(dpack_bin_size(UINT16_MAX + 2), equal, UINT16_MAX + 2 + 5);
#endif
#if DPACK_BINSZ_MAX >= (UINT32_MAX - 2)
	cute_check_uint(DPACK_BIN_SIZE(UINT32_MAX - 2), equal, UINT32_MAX - 2 + 5);
	cute_check_uint(dpack_bin_size(UINT32_MAX - 2), equal, UINT32_MAX - 2 + 5);
#endif
#if DPACK_BINSZ_MAX >= (UINT32_MAX - 1)
	cute_check_uint(DPACK_BIN_SIZE(UINT32_MAX - 1), equal, UINT32_MAX - 1 + 5);
	cute_check_uint(dpack_bin_size(UINT32_MAX - 1), equal, UINT32_MAX - 1 + 5);
#endif
#if DPACK_BINSZ_MAX >= UINT32_MAX
	cute_check_uint(DPACK_BIN_SIZE(UINT32_MAX),     equal, UINT32_MAX + 5);
	cute_check_uint(dpack_bin_size(UINT32_MAX),     equal, UINT32_MAX + 5);
#endif
}

#else  /* !(DPACK_BINSZ_MAX > _DPACK_BIN16_SIZE_MAX) */

CUTE_TEST(dpackut_bin32_sizes)
{
	cute_skip("32-bits MessagePack bin support not compiled-in");
}

#endif /* DPACK_BINSZ_MAX > _DPACK_BIN16_SIZE_MAX */

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_bin_assert_sizes)
{
	size_t sz __unused;

	cute_expect_assertion(sz = dpack_bin_size(0));
	cute_expect_assertion(sz = dpack_bin_size(DPACK_BINSZ_MAX + 1));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_bin_assert_sizes)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

static size_t
dpackut_bin_size(size_t size)
{
	switch (size) {
	case 0 ... UINT8_MAX:
		return size + 2;
	case (UINT8_MAX + 1) ... UINT16_MAX:
		return size + 3;
	case (UINT16_MAX + 1) ... UINT32_MAX:
		return size + 5;
	default:
		cute_fail("unsupported MsgPack bin size");
	}

	unreachable();
}

static char dpackut_bin_values[DPACK_BINSZ_MAX + 1];

struct dpackut_bin_data {
	size_t       value_size;
	const char * value_buff;
	size_t       pack_size;
	char *       pack_buff;
};

typedef void (dpackut_bin_pack_fn)(struct dpack_encoder *,
                                   int,
                                   const char *,
                                   size_t);

typedef void (dpackut_bin_unpack_fn)(struct dpack_decoder *,
                                     ssize_t,
                                     const struct dpackut_bin_data *);

static void
dpackut_bin_gen_data(struct dpackut_bin_data * data, size_t size)
{
	cute_check_uint(size, lower_equal, array_nr(dpackut_bin_values));
	data->value_size = size;
	data->value_buff = dpackut_bin_values;

	data->pack_size = dpackut_bin_size(size);
	data->pack_buff = malloc(data->pack_size);
	cute_check_ptr(data->pack_buff, unequal, NULL);

	switch (size) {
	case 0 ... UINT8_MAX:
		data->pack_buff[0] = (char)0xc4;
		data->pack_buff[1] = (char)(size);
		break;
#if DPACK_BINSZ_MAX > _DPACK_BIN8_SIZE_MAX
	case (UINT8_MAX + 1) ... UINT16_MAX:
		data->pack_buff[0] = (char)0xc5;
		data->pack_buff[1] = (char)((size >> 8) & 0xff);
		data->pack_buff[2] = (char)(size & 0xff);
		break;
#endif
#if DPACK_BINSZ_MAX > _DPACK_BIN16_SIZE_MAX
	case (UINT16_MAX + 1) ... UINT32_MAX:
		data->pack_buff[0] = (char)0xc6;
		data->pack_buff[1] = (char)((size >> 24) & 0xff);
		data->pack_buff[2] = (char)((size >> 16) & 0xff);
		data->pack_buff[3] = (char)((size >> 8) & 0xff);
		data->pack_buff[4] = (char)(size & 0xff);
		break;
#endif
	default:
		cute_fail("unsupported MsgPack bin size");
	}

	memcpy(&data->pack_buff[data->pack_size - size],
	       dpackut_bin_values,
	       size);
}

static void
dpackut_bin_fini_data(struct dpackut_bin_data * data)
{
	free(data->pack_buff);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_bin_encode_null_enc)
{
	const char * data = data;
	int          ret __unused;

	cute_expect_assertion(ret = dpack_encode_bin(NULL, data, 1));
}

CUTE_TEST(dpackut_bin_encode_null_data)
{
	struct dpack_encoder enc = { 0, };
	char                 data[8];
	int                  ret __unused;

	dpack_encoder_init_buffer(&enc, data, array_nr(data));
	cute_expect_assertion(ret = dpack_encode_bin(&enc, NULL, 1));
	dpack_encoder_fini(&enc, DPACK_DONE);
}

CUTE_TEST(dpackut_bin_encode_zero)
{
	struct dpack_encoder enc = { 0, };
	char                 data[8];
	int                  ret __unused;

	dpack_encoder_init_buffer(&enc, data, array_nr(data));
	cute_expect_assertion(ret = dpack_encode_bin(&enc, data, 0));
	dpack_encoder_fini(&enc, DPACK_DONE);
}

CUTE_TEST(dpackut_bin_encode_huge)
{
	struct dpack_encoder enc = { 0, };
	char                 data[8];
	int                  ret __unused;

	dpack_encoder_init_buffer(&enc, data, array_nr(data));
	cute_expect_assertion(ret = dpack_encode_bin(&enc,
	                                             data,
	                                             DPACK_BINSZ_MAX + 1));
	dpack_encoder_fini(&enc, DPACK_DONE);
}

#else /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_bin_encode_null_enc)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_encode_null_data)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_encode_zero)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_encode_huge)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_DEBUG)

CUTE_TEST(dpackut_bin_encode_uninit_enc)
{
	struct dpack_encoder enc = { 0, };
	const char *         data = (const char *)&enc;
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_encode_bin(&enc, data, 1));
}

#else /* !defined(CONFIG_DPACK_DEBUG) */

CUTE_TEST(dpackut_bin_encode_uninit_enc)
{
	cute_skip("debug build disabled");
}

#endif /* defined(CONFIG_DPACK_DEBUG) */

static void
dpackut_bin_enc(const struct dpackut_bin_data * data,
                int                             ret,
                size_t                          size,
                dpackut_bin_pack_fn *           pack)
{
	char *               buff = NULL;
	unsigned int         b;
	struct dpack_encoder enc;

	buff = malloc(data->pack_size + 128);
	cute_check_ptr(buff, unequal, NULL);
	memset(buff, 0xa5, data->pack_size + 128);

	dpack_encoder_init_buffer(&enc, buff + 64, data->pack_size);

	pack(&enc, ret, data->value_buff, size);
	if (ret < 0)
		goto fini;

	for (b = 0; b < 64; b++)
		cute_check_sint(buff[b], equal, (char)0xa5);

	cute_check_mem(buff + 64, equal, data->pack_buff, data->pack_size);
	cute_check_uint(dpack_bin_size(data->value_size),
	                equal,
	                data->pack_size);
	cute_check_uint(dpack_encoder_space_used(&enc), equal, data->pack_size);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	for (b = 0; b < 64; b++)
		cute_check_sint(buff[64 + data->pack_size + b],
		                equal,
		                (char)0xa5);

fini:
	dpack_encoder_fini(&enc, DPACK_DONE);

	free(buff);
}

static void
dpackut_bin_pack(struct dpack_encoder * encoder,
                 int                    ret,
                 const char *           buffer,
                 size_t                 size)
{
	cute_check_sint(dpack_encode_bin(encoder, buffer, size), equal, ret);
}

/**
 * @internal
 *
 * Perform a bin encoding test
 *
 * @param[in] ret       expected code returned by dpack_encode_bin()
 * @param[in] ref_size  size of (unpacked) data from which to generate
 *                      reference packed data
 * @param[in] test_size size of (unpacked) data given to dpack_encode_bin()
 *                      for packing data to compare with reference packed data
 */
static void
dpackut_bin_encode(int ret, size_t ref_size, size_t test_size)
{
	struct dpackut_bin_data      data;
	const struct cute_uint_range range = CUTE_UINT_RANGE(1,
	                                                     DPACK_BINSZ_MAX +
	                                                     1);

	cute_check_uint_range(ref_size, in, range);
	cute_check_uint_range(test_size, in, range);

	dpackut_bin_gen_data(&data, ref_size);
	dpackut_bin_enc(&data, ret, test_size, dpackut_bin_pack);
	dpackut_bin_fini_data(&data);
}

CUTE_TEST(dpackut_bin_encode_ok_1)
{
	dpackut_bin_encode(0, 1, 1);
}

CUTE_TEST(dpackut_bin_encode_nok_1)
{
	dpackut_bin_encode(-EMSGSIZE, 1, 2);
}

CUTE_TEST(dpackut_bin_encode_ok_2)
{
	dpackut_bin_encode(0, 2, 2);
}

CUTE_TEST(dpackut_bin_encode_nok_2)
{
	dpackut_bin_encode(-EMSGSIZE, 2, 3);
}

#if DPACK_BINSZ_MAX >= (UINT8_MAX)

CUTE_TEST(dpackut_bin_encode_ok_uint8)
{
	dpackut_bin_encode(0, UINT8_MAX, UINT8_MAX);
}

#else /* !(DPACK_BINSZ_MAX >= (UINT8_MAX)) */

CUTE_TEST(dpackut_bin_encode_ok_uint8)
{
	cute_skip("maximum MsgPack bin size < 255");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX) */

#if DPACK_BINSZ_MAX >= (UINT8_MAX + 1)

CUTE_TEST(dpackut_bin_encode_nok_uint8)
{
	dpackut_bin_encode(-EMSGSIZE, UINT8_MAX, UINT8_MAX + 1);
}

CUTE_TEST(dpackut_bin_encode_ok_uint8_1)
{
	dpackut_bin_encode(0, UINT8_MAX + 1, UINT8_MAX + 1);
}

#else /* !(DPACK_BINSZ_MAX >= (UINT8_MAX + 1)) */

CUTE_TEST(dpackut_bin_encode_nok_uint8)
{
	cute_skip("maximum MsgPack bin size < 256");
}

CUTE_TEST(dpackut_bin_encode_ok_uint8_1)
{
	cute_skip("maximum MsgPack bin size < 256");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX + 1) */

#if DPACK_BINSZ_MAX >= (UINT8_MAX + 2)

CUTE_TEST(dpackut_bin_encode_nok_uint8_1)
{
	dpackut_bin_encode(-EMSGSIZE, UINT8_MAX + 1, UINT8_MAX + 2);
}

#else /* !(DPACK_BINSZ_MAX >= (UINT8_MAX + 2)) */

CUTE_TEST(dpackut_bin_encode_nok_uint8_1)
{
	cute_skip("maximum MsgPack bin size < 257");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX + 2) */

#if DPACK_BINSZ_MAX >= (UINT16_MAX)

CUTE_TEST(dpackut_bin_encode_ok_uint16)
{
	dpackut_bin_encode(0, UINT16_MAX, UINT16_MAX);
}

#else /* !(DPACK_BINSZ_MAX >= (UINT16_MAX)) */

CUTE_TEST(dpackut_bin_encode_ok_uint16)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX) */

#if DPACK_BINSZ_MAX >= (UINT16_MAX + 1)

CUTE_TEST(dpackut_bin_encode_nok_uint16)
{
	dpackut_bin_encode(-EMSGSIZE, UINT16_MAX, UINT16_MAX + 1);
}

CUTE_TEST(dpackut_bin_encode_ok_uint16_1)
{
	dpackut_bin_encode(0, UINT16_MAX + 1, UINT16_MAX + 1);
}

#else /* !(DPACK_BINSZ_MAX >= (UINT16_MAX + 1)) */

CUTE_TEST(dpackut_bin_encode_nok_uint16)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

CUTE_TEST(dpackut_bin_encode_ok_uint16_1)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX + 1) */

#if DPACK_BINSZ_MAX >= (UINT16_MAX + 2)

CUTE_TEST(dpackut_bin_encode_nok_uint16_1)
{
	dpackut_bin_encode(-EMSGSIZE, UINT16_MAX + 1, UINT16_MAX + 2);
}

#else /* !(DPACK_BINSZ_MAX >= (UINT16_MAX + 2)) */

CUTE_TEST(dpackut_bin_encode_nok_uint16_1)
{
	cute_skip("maximum MsgPack bin size < 65537");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX + 2) */

CUTE_TEST(dpackut_bin_encode_ok_max)
{
	dpackut_bin_encode(0, DPACK_BINSZ_MAX, DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_encode_nok_max)
{
	dpackut_bin_encode(-EMSGSIZE, DPACK_BINSZ_MAX - 1, DPACK_BINSZ_MAX);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_bin_decode_dup_null_dec)
{
	char *  data;
	ssize_t ret __unused;

	cute_expect_assertion(ret = dpack_decode_bindup(NULL, &data));
}

CUTE_TEST(dpackut_bin_decode_dup_null_data)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bindup(&dec, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_bin_decode_dup_null_dec)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_dup_null_data)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_DEBUG)

CUTE_TEST(dpackut_bin_decode_dup_uninit_dec)
{
	struct dpack_decoder dec = { 0, };
	char *               data;
	ssize_t              ret __unused;

	cute_expect_assertion(ret = dpack_decode_bindup(&dec, &data));
}

#else  /* !defined(CONFIG_DPACK_DEBUG) */

CUTE_TEST(dpackut_bin_decode_dup_uninit_dec)
{
	cute_skip("debug build disabled");
}

#endif /* defined(CONFIG_DPACK_DEBUG) */

/**
 * @internal
 *
 * Perform a duplicated bin decoding test
 *
 * @param[in] ret       expected code returned by dpack_decode_bindup()
 * @param[in] ref_size  size of (unpacked) data from which to generate
 *                      reference packed data
 * @param[in] test_size size of (unpacked) data from which to compute encoded
 *                      data buffer size given to dpack_decoder_init_buffer()
 */
static void
dpackut_bin_decode_dup(ssize_t ret, size_t ref_size, size_t test_size)
{
	const struct cute_uint_range range = CUTE_UINT_RANGE(1,
	                                                     DPACK_BINSZ_MAX +
	                                                     1);
	struct dpackut_bin_data      data;
	struct dpack_decoder         dec;
	char *                       val;
	ssize_t                      sz;

	cute_check_uint_range(ref_size, in, range);
	cute_check_uint_range(test_size, in, range);

	dpackut_bin_gen_data(&data, ref_size);

	dpack_decoder_init_buffer(&dec,
	                          data.pack_buff,
	                          dpack_bin_size(test_size));

	sz = dpack_decode_bindup(&dec, &val);
	cute_check_sint(sz, equal, ret);

	if (sz >= 0) {
		cute_check_uint((size_t)sz, equal, data.value_size);
		cute_check_mem(val, equal, data.value_buff, (size_t)sz);
		cute_check_uint(dpack_bin_size((size_t)sz),
		                equal,
		                data.pack_size);
		free(val);
	}

	dpack_decoder_fini(&dec);

	dpackut_bin_fini_data(&data);
}

CUTE_TEST(dpackut_bin_decode_dup_ok_1)
{
	dpackut_bin_decode_dup(1, 1, 1);
}

CUTE_TEST(dpackut_bin_decode_dup_nok_2_short)
{
	dpackut_bin_decode_dup(-EPROTO, 2, 1);
}

CUTE_TEST(dpackut_bin_decode_dup_ok_2)
{
	dpackut_bin_decode_dup(2, 2, 2);
}

CUTE_TEST(dpackut_bin_decode_dup_nok_3_short)
{
	dpackut_bin_decode_dup(-EPROTO, 3, 2);
}

#if DPACK_BINSZ_MAX >= (UINT8_MAX)

CUTE_TEST(dpackut_bin_decode_dup_ok_uint8)
{
	dpackut_bin_decode_dup(UINT8_MAX, UINT8_MAX, UINT8_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT8_MAX)) */

CUTE_TEST(dpackut_bin_decode_dup_ok_uint8)
{
	cute_skip("maximum MsgPack bin size < 255");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX) */

#if DPACK_BINSZ_MAX >= (UINT8_MAX + 1)

CUTE_TEST(dpackut_bin_decode_dup_nok_uint8plus_short)
{
	dpackut_bin_decode_dup(-EPROTO, UINT8_MAX + 1, UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_ok_uint8plus)
{
	dpackut_bin_decode_dup(UINT8_MAX + 1, UINT8_MAX + 1, UINT8_MAX + 1);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT8_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_dup_nok_uint8plus_short)
{
	cute_skip("maximum MsgPack bin size < 256");
}

CUTE_TEST(dpackut_bin_decode_dup_ok_uint8plus)
{
	cute_skip("maximum MsgPack bin size < 256");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX + 1) */

#if DPACK_BINSZ_MAX >= (UINT16_MAX)

CUTE_TEST(dpackut_bin_decode_dup_ok_uint16)
{
	dpackut_bin_decode_dup(UINT16_MAX, UINT16_MAX, UINT16_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT16_MAX)) */

CUTE_TEST(dpackut_bin_decode_dup_ok_uint16)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX) */

#if DPACK_BINSZ_MAX >= (UINT16_MAX + 1)

CUTE_TEST(dpackut_bin_decode_dup_nok_uint16plus_short)
{
	dpackut_bin_decode_dup(-EPROTO, UINT16_MAX + 1, UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_ok_uint16plus)
{
	dpackut_bin_decode_dup(UINT16_MAX + 1, UINT16_MAX + 1, UINT16_MAX + 1);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT16_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_dup_nok_uint16plus_short)
{
	cute_skip("maximum MsgPack bin size < 65537");
}

CUTE_TEST(dpackut_bin_decode_dup_ok_uint16plus)
{
	cute_skip("maximum MsgPack bin size < 65537");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX + 1) */

CUTE_TEST(dpackut_bin_decode_dup_ok_max)
{
	dpackut_bin_decode_dup(DPACK_BINSZ_MAX,
	                       DPACK_BINSZ_MAX,
	                       DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_nok_max_short)
{
	dpackut_bin_decode_dup(-EPROTO, DPACK_BINSZ_MAX, DPACK_BINSZ_MAX - 1);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_bin_decode_dup_equ_null_dec)
{
	char *  data;
	ssize_t ret __unused;

	cute_expect_assertion(ret = dpack_decode_bindup_equ(NULL, 2, &data));
}

CUTE_TEST(dpackut_bin_decode_dup_equ_null_data)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bindup_equ(&dec, 2, NULL));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_0)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	char *               bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bindup_equ(&dec, 0, &bin));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_binsz)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	char *               bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bindup_equ(&dec,
	                                                    DPACK_BINSZ_MAX + 1,
	                                                    &bin));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_bin_decode_dup_equ_null_dec)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_dup_equ_null_data)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_dup_equ_0)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_dup_equ_binsz)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_DEBUG)

CUTE_TEST(dpackut_bin_decode_dup_equ_uninit_dec)
{
	struct dpack_decoder dec = { 0, };
	char *               data;
	ssize_t              ret __unused;

	cute_expect_assertion(ret = dpack_decode_bindup_equ(&dec, 2, &data));
}

#else  /* !defined(CONFIG_DPACK_DEBUG) */

CUTE_TEST(dpackut_bin_decode_dup_equ_uninit_dec)
{
	cute_skip("debug build disabled");
}

#endif /* defined(CONFIG_DPACK_DEBUG) */

/**
 * @internal
 *
 * Perform a duplicated bin decoding test with requested size
 *
 * @param[in] ret       expected code returned by dpack_decode_bindup_equ()
 * @param[in] ref_size  size of (unpacked) data from which to generate
 *                      reference packed data
 * @param[in] test_size size of (unpacked) data from which to compute encoded
 *                      data buffer size given to dpack_decoder_init_buffer()
 * @param[in] equ_size  expected size of (packed) data given to
 *                      dpack_decode_bindup_equ() for unpacking data to compare
 *                      with reference unpacked data
 */
static void
dpackut_bin_decode_dup_equ(ssize_t ret,
                           size_t  ref_size,
                           size_t  test_size,
                           size_t  equ_size)
{
	const struct cute_uint_range range = CUTE_UINT_RANGE(1,
	                                                     DPACK_BINSZ_MAX +
	                                                     1);
	struct dpackut_bin_data      data;
	struct dpack_decoder         dec;
	char *                       val;
	ssize_t                      sz;

	cute_check_uint_range(ref_size, in, range);
	cute_check_uint_range(test_size, in, range);
	cute_check_uint_range(equ_size,
	                      in,
	                      CUTE_UINT_RANGE(1, DPACK_BINSZ_MAX));

	dpackut_bin_gen_data(&data, ref_size);

	dpack_decoder_init_buffer(&dec,
	                          data.pack_buff,
	                          dpack_bin_size(test_size));

	sz = dpack_decode_bindup_equ(&dec, equ_size, &val);
	cute_check_sint(sz, equal, ret);

	if (sz >= 0) {
		cute_check_uint((size_t)sz, equal, data.value_size);
		cute_check_mem(val, equal, data.value_buff, (size_t)sz);
		cute_check_uint(dpack_bin_size((size_t)sz),
		                equal,
		                data.pack_size);
		free(val);
	}

	dpack_decoder_fini(&dec);

	dpackut_bin_fini_data(&data);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_ok_1)
{
	dpackut_bin_decode_dup_equ(1, 1, 1, 1);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_1_sup)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE, 1, 1, 2);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_2_short_inf)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE, 2, 1, 1);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_2_short_equ)
{
	dpackut_bin_decode_dup_equ(-EPROTO, 2, 1, 2);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_2_short_sup)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE, 2, 1, 3);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_2_inf)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE, 2, 2, 1);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_ok_2)
{
	dpackut_bin_decode_dup_equ(2, 2, 2, 2);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_2_sup)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE, 2, 2, 3);
}

#if DPACK_BINSZ_MAX >= (UINT8_MAX)

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint8_short_inf)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE,
	                           UINT8_MAX,
	                           UINT8_MAX - 1,
	                           UINT8_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint8_short_equ)
{
	dpackut_bin_decode_dup_equ(-EPROTO,
	                           UINT8_MAX,
	                           UINT8_MAX - 1,
	                           UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint8_inf)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE,
	                           UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_ok_uint8)
{
	dpackut_bin_decode_dup_equ(UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT8_MAX)) */

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint8_short_inf)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint8_short_equ)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint8_inf)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_dup_equ_ok_uint8)
{
	cute_skip("maximum MsgPack bin size < 255");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX) */

#if DPACK_BINSZ_MAX >= (UINT8_MAX + 1)

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint8_short_sup)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE,
	                           UINT8_MAX,
	                           UINT8_MAX - 1,
	                           UINT8_MAX + 1);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint8_sup)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE,
	                           UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX + 1);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT8_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint8_short_sup)
{
	cute_skip("maximum MsgPack bin size < 256");
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint8_sup)
{
	cute_skip("maximum MsgPack bin size < 256");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX + 1) */

#if DPACK_BINSZ_MAX >= (UINT16_MAX)

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint16_short_inf)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE,
	                           UINT16_MAX,
	                           UINT16_MAX - 1,
	                           UINT16_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint16_short_equ)
{
	dpackut_bin_decode_dup_equ(-EPROTO,
	                           UINT16_MAX,
	                           UINT16_MAX - 1,
	                           UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint16_inf)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE,
	                           UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_ok_uint16)
{
	dpackut_bin_decode_dup_equ(UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT16_MAX)) */

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint16_short_inf)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint16_short_equ)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint16_inf)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

CUTE_TEST(dpackut_bin_decode_dup_equ_ok_uint16)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX) */

#if DPACK_BINSZ_MAX >= (UINT16_MAX + 1)

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint16_short_sup)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE,
	                           UINT16_MAX,
	                           UINT16_MAX - 1,
	                           UINT16_MAX + 1);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint16_sup)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE,
	                           UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX + 1);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT16_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint16_short_sup)
{
	cute_skip("maximum MsgPack bin size < 65537");
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_uint16_sup)
{
	cute_skip("maximum MsgPack bin size < 65537");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX + 1) */

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_binsz_sup)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_binsz_short_sup)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX - 2,
	                           DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_binsz_short_inf)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_binsz_short_equ)
{
	dpackut_bin_decode_dup_equ(-EPROTO,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_nok_binsz_inf)
{
	dpackut_bin_decode_dup_equ(-EMSGSIZE,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_dup_equ_ok_binsz)
{
	dpackut_bin_decode_dup_equ(DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_bin_decode_dup_max_null_dec)
{
	char *  data;
	ssize_t ret __unused;

	cute_expect_assertion(ret = dpack_decode_bindup_max(NULL, 2, &data));
}

CUTE_TEST(dpackut_bin_decode_dup_max_null_data)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bindup_max(&dec, 2, NULL));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_dup_max_0)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	char *               bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bindup_max(&dec, 0, &bin));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_dup_max_binsz)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	char *               bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bindup_max(&dec,
	                                                    DPACK_BINSZ_MAX + 1,
	                                                    &bin));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_bin_decode_dup_max_null_dec)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_dup_max_null_data)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_dup_max_0)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_dup_max_binsz)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_DEBUG)

CUTE_TEST(dpackut_bin_decode_dup_max_uninit_dec)
{
	struct dpack_decoder dec = { 0, };
	char *               data;
	ssize_t              ret __unused;

	cute_expect_assertion(ret = dpack_decode_bindup_max(&dec, 2, &data));
}

#else  /* !defined(CONFIG_DPACK_DEBUG) */

CUTE_TEST(dpackut_bin_decode_dup_max_uninit_dec)
{
	cute_skip("debug build disabled");
}

#endif /* defined(CONFIG_DPACK_DEBUG) */

/**
 * @internal
 *
 * Perform a duplicated bin decoding test with requested maximum size
 *
 * @param[in] ret       expected code returned by dpack_decode_bindup_max()
 * @param[in] ref_size  size of (unpacked) data from which to generate
 *                      reference packed data
 * @param[in] test_size size of (unpacked) data from which to compute encoded
 *                      data buffer size given to dpack_decoder_init_buffer()
 * @param[in] max_size  maximum size of (packed) data given to
 *                      dpack_decode_bindup_max() for unpacking data to compare
 *                      with reference unpacked data
 */
static void
dpackut_bin_decode_dup_max(ssize_t ret,
                           size_t  ref_size,
                           size_t  test_size,
                           size_t  max_size)
{
	const struct cute_uint_range range = CUTE_UINT_RANGE(1,
	                                                     DPACK_BINSZ_MAX +
	                                                     1);
	struct dpackut_bin_data      data;
	struct dpack_decoder         dec;
	char *                       val;
	ssize_t                      sz;

	cute_check_uint_range(ref_size, in, range);
	cute_check_uint_range(test_size, in, range);
	cute_check_uint_range(max_size,
	                      in,
	                      CUTE_UINT_RANGE(1, DPACK_BINSZ_MAX));

	dpackut_bin_gen_data(&data, ref_size);

	dpack_decoder_init_buffer(&dec,
	                          data.pack_buff,
	                          dpack_bin_size(test_size));

	sz = dpack_decode_bindup_max(&dec, max_size, &val);
	cute_check_sint(sz, equal, ret);

	if (sz >= 0) {
		cute_check_uint((size_t)sz, equal, data.value_size);
		cute_check_mem(val, equal, data.value_buff, (size_t)sz);
		cute_check_uint(dpack_bin_size((size_t)sz),
		                equal,
		                data.pack_size);
		free(val);
	}

	dpack_decoder_fini(&dec);

	dpackut_bin_fini_data(&data);
}

CUTE_TEST(dpackut_bin_decode_dup_max_ok_1_sup)
{
	dpackut_bin_decode_dup_max(1, 1, 1, 2);
}

CUTE_TEST(dpackut_bin_decode_dup_max_ok_2_equ)
{
	dpackut_bin_decode_dup_max(2, 2, 2, 2);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_2_short_equ)
{
	dpackut_bin_decode_dup_max(-EPROTO, 2, 1, 2);
}

CUTE_TEST(dpackut_bin_decode_dup_max_ok_2_sup)
{
	dpackut_bin_decode_dup_max(2, 2, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_2_short_sup)
{
	dpackut_bin_decode_dup_max(-EPROTO, 2, 1, 3);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_3_inf)
{
	dpackut_bin_decode_dup_max(-EMSGSIZE, 3, 3, 2);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_3_short_inf)
{
	dpackut_bin_decode_dup_max(-EMSGSIZE, 3, 2, 2);
}

CUTE_TEST(dpackut_bin_decode_dup_max_ok_3_equ)
{
	dpackut_bin_decode_dup_max(3, 3, 3, 3);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_3_short_equ)
{
	dpackut_bin_decode_dup_max(-EPROTO, 3, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_dup_max_ok_3_sup)
{
	dpackut_bin_decode_dup_max(3, 3, 3, 4);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_3_short_sup)
{
	dpackut_bin_decode_dup_max(-EPROTO, 3, 2, 4);
}

#if DPACK_BINSZ_MAX >= UINT8_MAX

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint8_inf)
{
	dpackut_bin_decode_dup_max(-EMSGSIZE,
	                           UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint8_short_inf)
{
	dpackut_bin_decode_dup_max(-EMSGSIZE,
	                           UINT8_MAX,
	                           UINT8_MAX - 1,
	                           UINT8_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_dup_max_ok_uint8_equ)
{
	dpackut_bin_decode_dup_max(UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint8_short_equ)
{
	dpackut_bin_decode_dup_max(-EPROTO,
	                           UINT8_MAX,
	                           UINT8_MAX - 1,
	                           UINT8_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= UINT8_MAX) */

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint8_inf)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint8_short_inf)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_dup_max_ok_uint8_equ)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint8_short_equ)
{
	cute_skip("maximum MsgPack bin size < 255");
}

#endif /* DPACK_BINSZ_MAX >= UINT8_MAX */

#if DPACK_BINSZ_MAX >= (UINT8_MAX + 1)

CUTE_TEST(dpackut_bin_decode_dup_max_ok_uint8_sup)
{
	dpackut_bin_decode_dup_max(UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX + 1);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint8_short_sup)
{
	dpackut_bin_decode_dup_max(-EPROTO,
	                           UINT8_MAX,
	                           UINT8_MAX - 1,
	                           UINT8_MAX + 1);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT8_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_dup_max_ok_uint8_sup)
{
	cute_skip("maximum MsgPack bin size < 256");
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint8_short_sup)
{
	cute_skip("maximum MsgPack bin size < 256");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX + 1) */

#if DPACK_BINSZ_MAX >= UINT16_MAX

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint16_inf)
{
	dpackut_bin_decode_dup_max(-EMSGSIZE,
	                           UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint16_short_inf)
{
	dpackut_bin_decode_dup_max(-EMSGSIZE,
	                           UINT16_MAX,
	                           UINT16_MAX - 1,
	                           UINT16_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_dup_max_ok_uint16_equ)
{
	dpackut_bin_decode_dup_max(UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint16_short_equ)
{
	dpackut_bin_decode_dup_max(-EPROTO,
	                           UINT16_MAX,
	                           UINT16_MAX - 1,
	                           UINT16_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= UINT16_MAX) */

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint16_inf)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint16_short_inf)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

CUTE_TEST(dpackut_bin_decode_dup_max_ok_uint16_equ)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint16_short_equ)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

#endif /* DPACK_BINSZ_MAX >= UINT16_MAX */

#if DPACK_BINSZ_MAX >= (UINT16_MAX + 1)

CUTE_TEST(dpackut_bin_decode_dup_max_ok_uint16_sup)
{
	dpackut_bin_decode_dup_max(UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX + 1);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint16_short_sup)
{
	dpackut_bin_decode_dup_max(-EPROTO,
	                           UINT16_MAX,
	                           UINT16_MAX - 1,
	                           UINT16_MAX + 1);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT16_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_dup_max_ok_uint16_sup)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_uint16_short_sup)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX + 1) */

CUTE_TEST(dpackut_bin_decode_dup_max_ok_binsz_sup)
{
	dpackut_bin_decode_dup_max(DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_binsz_short_sup)
{
	dpackut_bin_decode_dup_max(-EPROTO,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX - 2,
	                           DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_binsz_inf)
{
	dpackut_bin_decode_dup_max(-EMSGSIZE,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_binsz_short_inf)
{
	dpackut_bin_decode_dup_max(-EMSGSIZE,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_dup_max_ok_binsz_equ)
{
	dpackut_bin_decode_dup_max(DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_max_nok_binsz_short_equ)
{
	dpackut_bin_decode_dup_max(-EPROTO,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_bin_decode_dup_range_null_dec)
{
	char *  data;
	ssize_t ret __unused;

	cute_expect_assertion(ret = dpack_decode_bindup_range(NULL,
	                                                      1,
	                                                      2,
	                                                      &data));
}

CUTE_TEST(dpackut_bin_decode_dup_range_null_data)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bindup_range(&dec,
	                                                      1,
	                                                      2,
	                                                      NULL));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_dup_range_02)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	char *               bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bindup_range(&dec,
	                                                      0,
	                                                      2,
	                                                      &bin));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_dup_range_10)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	char *               bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bindup_range(&dec,
	                                                      1,
	                                                      0,
	                                                      &bin));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_dup_range_21)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	char *               bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bindup_range(&dec,
	                                                      2,
	                                                      1,
	                                                      &bin));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_dup_range_binsz)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	char *               bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bindup_range(&dec,
	                                                      1,
	                                                      DPACK_BINSZ_MAX +
	                                                      1,
	                                                      &bin));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_bin_decode_dup_range_null_dec)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_dup_range_null_data)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_dup_range_02)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_dup_range_10)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_dup_range_21)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_dup_range_binsz)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_DEBUG)

CUTE_TEST(dpackut_bin_decode_dup_range_uninit_dec)
{
	struct dpack_decoder dec = { 0, };
	char *               data;
	ssize_t              ret __unused;

	cute_expect_assertion(ret = dpack_decode_bindup_range(&dec,
	                                                      1,
	                                                      2,
	                                                      &data));
}

#else  /* !defined(CONFIG_DPACK_DEBUG) */

CUTE_TEST(dpackut_bin_decode_dup_range_uninit_dec)
{
	cute_skip("debug build disabled");
}

#endif /* defined(CONFIG_DPACK_DEBUG) */

/**
 * @internal
 *
 * Perform a duplicated bin decoding test with requested minimum and maximum
 * size
 *
 * @param[in] ret       expected code returned by dpack_decode_bindup_range()
 * @param[in] ref_size  size of (unpacked) data from which to generate
 *                      reference packed data
 * @param[in] test_size size of (unpacked) data from which to compute encoded
 *                      data buffer size given to dpack_decoder_init_buffer()
 * @param[in] min_size  minimum size of (packed) data given to
 *                      dpack_decode_bindup_range() for unpacking data to
 *                      compare with reference unpacked data
 * @param[in] max_size  maximum size of (packed) data given to
 *                      dpack_decode_bindup_range() for unpacking data to
 *                      compare with reference unpacked data
 */
static void
dpackut_bin_decode_dup_range(ssize_t ret,
                             size_t  ref_size,
                             size_t  test_size,
                             size_t  min_size,
                             size_t  max_size)
{
	const struct cute_uint_range range = CUTE_UINT_RANGE(1,
	                                                     DPACK_BINSZ_MAX +
	                                                     1);
	struct dpackut_bin_data data;
	struct dpack_decoder    dec;
	char *                  val;
	ssize_t                 sz;

	cute_check_uint_range(ref_size, in, range);
	cute_check_uint_range(test_size, in, range);
	cute_check_uint_range(min_size, in, CUTE_UINT_RANGE(1,
	                                                    DPACK_BINSZ_MAX -
	                                                    1));
	cute_check_uint_range(max_size, in, CUTE_UINT_RANGE(2,
	                                                    DPACK_BINSZ_MAX));

	dpackut_bin_gen_data(&data, ref_size);

	dpack_decoder_init_buffer(&dec,
	                          data.pack_buff,
	                          dpack_bin_size(test_size));

	sz = dpack_decode_bindup_range(&dec, min_size, max_size, &val);
	cute_check_sint(sz, equal, ret);

	if (sz >= 0) {
		cute_check_uint((size_t)sz, equal, data.value_size);
		cute_check_mem(val, equal, data.value_buff, (size_t)sz);
		cute_check_uint(dpack_bin_size((size_t)sz),
		                equal,
		                data.pack_size);
		free(val);
	}

	dpack_decoder_fini(&dec);

	dpackut_bin_fini_data(&data);
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_1)
{
	dpackut_bin_decode_dup_range(1, 1, 1, 1, 2);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_1_short)
{
	dpackut_bin_decode_dup_range(-EPROTO, 2, 1, 1, 2);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_1)
{
	dpackut_bin_decode_dup_range(-EMSGSIZE, 1, 1, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_2)
{
	dpackut_bin_decode_dup_range(2, 2, 2, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_2_short)
{
	dpackut_bin_decode_dup_range(-EPROTO, 2, 1, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_3)
{
	dpackut_bin_decode_dup_range(3, 3, 3, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_3_short)
{
	dpackut_bin_decode_dup_range(-EPROTO, 3, 2, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_4)
{
	dpackut_bin_decode_dup_range(-EMSGSIZE, 4, 4, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_4_short)
{
	dpackut_bin_decode_dup_range(-EMSGSIZE, 4, 3, 2, 3);
}

#if DPACK_BINSZ_MAX >= UINT8_MAX

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint8minus2_sup)
{
	dpackut_bin_decode_dup_range(-EMSGSIZE,
	                             UINT8_MAX - 2,
	                             UINT8_MAX - 2,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_uint8minus_sup)
{
	dpackut_bin_decode_dup_range(UINT8_MAX - 1,
	                             UINT8_MAX - 1,
	                             UINT8_MAX - 1,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint8minus_short_sup)
{
	dpackut_bin_decode_dup_range(-EPROTO,
	                             UINT8_MAX - 1,
	                             UINT8_MAX - 2,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_uint8)
{
	dpackut_bin_decode_dup_range(UINT8_MAX,
	                             UINT8_MAX,
	                             UINT8_MAX,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint8_short)
{
	dpackut_bin_decode_dup_range(-EPROTO,
	                             UINT8_MAX,
	                             UINT8_MAX - 1,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= UINT8_MAX) */

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint8minus2_sup)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_uint8minus_sup)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint8minus_short_sup)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_uint8)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint8_short)
{
	cute_skip("maximum MsgPack bin size < 255");
}

#endif /* !(DPACK_BINSZ_MAX >= UINT8_MAX) */

#if  DPACK_BINSZ_MAX >= (UINT8_MAX + 1)

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint8plus_sup)
{
	dpackut_bin_decode_dup_range(-EMSGSIZE,
	                             UINT8_MAX + 1,
	                             UINT8_MAX + 1,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint8plus_short_sup)
{
	dpackut_bin_decode_dup_range(-EMSGSIZE,
	                             UINT8_MAX + 1,
	                             UINT8_MAX,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT8_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint8plus_sup)
{
	cute_skip("maximum MsgPack bin size < 256");
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint8plus_short_sup)
{
	cute_skip("maximum MsgPack bin size < 256");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX + 1) */

#if DPACK_BINSZ_MAX >= UINT16_MAX

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint16minus2_sup)
{
	dpackut_bin_decode_dup_range(-EMSGSIZE,
	                             UINT16_MAX - 2,
	                             UINT16_MAX - 2,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_uint16minus_sup)
{
	dpackut_bin_decode_dup_range(UINT16_MAX - 1,
	                             UINT16_MAX - 1,
	                             UINT16_MAX - 1,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint16minus_short_sup)
{
	dpackut_bin_decode_dup_range(-EPROTO,
	                             UINT16_MAX - 1,
	                             UINT16_MAX - 2,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_uint16)
{
	dpackut_bin_decode_dup_range(UINT16_MAX,
	                             UINT16_MAX,
	                             UINT16_MAX,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint16_short)
{
	dpackut_bin_decode_dup_range(-EPROTO,
	                             UINT16_MAX,
	                             UINT16_MAX - 1,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= UINT16_MAX) */

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint16minus2_sup)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_uint16minus_sup)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint16minus_short_sup)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_uint16)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint16_short)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

#endif /* !(DPACK_BINSZ_MAX >= UINT16_MAX) */

#if  DPACK_BINSZ_MAX >= (UINT16_MAX + 1)

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint16plus_sup)
{
	dpackut_bin_decode_dup_range(-EMSGSIZE,
	                             UINT16_MAX + 1,
	                             UINT16_MAX + 1,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint16plus_short_sup)
{
	dpackut_bin_decode_dup_range(-EMSGSIZE,
	                             UINT16_MAX + 1,
	                             UINT16_MAX,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT16_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint16plus_sup)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_uint16plus_short_sup)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX + 1) */

CUTE_TEST(dpackut_bin_decode_dup_range_nok_binszminus2_sup)
{
	dpackut_bin_decode_dup_range(-EMSGSIZE,
	                             DPACK_BINSZ_MAX - 2,
	                             DPACK_BINSZ_MAX - 2,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_binszminus_sup)
{
	dpackut_bin_decode_dup_range(DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_binszminus_short_sup)
{
	dpackut_bin_decode_dup_range(-EPROTO,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX - 2,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_ok_binsz)
{
	dpackut_bin_decode_dup_range(DPACK_BINSZ_MAX,
	                             DPACK_BINSZ_MAX,
	                             DPACK_BINSZ_MAX,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_binsz_short)
{
	dpackut_bin_decode_dup_range(-EPROTO,
	                             DPACK_BINSZ_MAX,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_dup_range_nok_binszplus_short_sup)
{
	dpackut_bin_decode_dup_range(-EMSGSIZE,
	                             DPACK_BINSZ_MAX + 1,
	                             DPACK_BINSZ_MAX,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_bin_decode_cpy_null_dec)
{
	char *  data = data;
	ssize_t ret __unused;

	cute_expect_assertion(ret = dpack_decode_bincpy(NULL, 2, data));
}

CUTE_TEST(dpackut_bin_decode_cpy_null_data)
{
	struct dpack_decoder dec;
	char                 data = data;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, &data, 1);
	cute_expect_assertion(ret = dpack_decode_bincpy(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_cpy_zero)
{
	struct dpack_decoder dec;
	char                 data = data;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, &data, 1);
	cute_expect_assertion(ret = dpack_decode_bincpy(&dec, 0, &data));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_cpy_huge)
{
	struct dpack_decoder dec;
	char                 data = data;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, &data, 1);
	cute_expect_assertion(ret = dpack_decode_bincpy(&dec,
	                                                DPACK_BINSZ_MAX + 1,
	                                                &data));
	dpack_decoder_fini(&dec);
}

#else  /* !(defined(CONFIG_DPACK_ASSERT_API)) */

CUTE_TEST(dpackut_bin_decode_cpy_null_dec)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_cpy_null_data)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_cpy_zero)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_cpy_huge)
{
	cute_skip("assertion unsupported");
}

#endif  /* defined(CONFIG_DPACK_ASSERT_API) */

/**
 * @internal
 *
 * Perform a duplicated bin decoding test
 *
 * @param[in] ret       expected code returned by dpack_decode_bindup()
 * @param[in] ref_size  size of (unpacked) data from which to generate
 *                      reference packed data
 * @param[in] test_size size of (unpacked) data from which to compute encoded
 *                      data buffer size given to dpack_decoder_init_buffer()
 */
static void
dpackut_bin_decode_cpy(ssize_t ret, size_t ref_size, size_t test_size)
{
	const struct cute_uint_range range = CUTE_UINT_RANGE(1,
	                                                     DPACK_BINSZ_MAX);
	struct dpackut_bin_data      data;
	struct dpack_decoder         dec;
	char *                       val;
	ssize_t                      sz;

	cute_check_uint_range(ref_size, in, range);
	cute_check_uint_range(test_size, in, range);

	dpackut_bin_gen_data(&data, ref_size);

	val = malloc(test_size);
	memset(val, 0xff, test_size);

	dpack_decoder_init_buffer(&dec,
	                          data.pack_buff,
	                          dpack_bin_size(test_size));

	sz = dpack_decode_bincpy(&dec, test_size, val);
	cute_check_sint(sz, equal, ret);

	if (sz >= 0) {
		cute_check_uint((size_t)sz, equal, data.value_size);
		cute_check_mem(val, equal, data.value_buff, (size_t)sz);
		cute_check_uint(dpack_bin_size((size_t)sz),
		                equal,
		                data.pack_size);
	}

	dpack_decoder_fini(&dec);

	free(val);

	dpackut_bin_fini_data(&data);
}

CUTE_TEST(dpackut_bin_decode_cpy_ok_1)
{
	dpackut_bin_decode_cpy(1, 1, 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_nok_2_short)
{
	dpackut_bin_decode_cpy(-EMSGSIZE, 2, 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_ok_2)
{
	dpackut_bin_decode_cpy(2, 2, 2);
}

CUTE_TEST(dpackut_bin_decode_cpy_nok_3_short)
{
	dpackut_bin_decode_cpy(-EMSGSIZE, 3, 2);
}

#if DPACK_BINSZ_MAX >= (UINT8_MAX)

CUTE_TEST(dpackut_bin_decode_cpy_ok_uint8)
{
	dpackut_bin_decode_cpy(UINT8_MAX, UINT8_MAX, UINT8_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT8_MAX)) */

CUTE_TEST(dpackut_bin_decode_cpy_ok_uint8)
{
	cute_skip("maximum MsgPack bin size < 255");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX) */

#if DPACK_BINSZ_MAX >= (UINT8_MAX + 1)

CUTE_TEST(dpackut_bin_decode_cpy_nok_uint8plus_short)
{
	dpackut_bin_decode_cpy(-EMSGSIZE, UINT8_MAX + 1, UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_ok_uint8plus)
{
	dpackut_bin_decode_cpy(UINT8_MAX + 1, UINT8_MAX + 1, UINT8_MAX + 1);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT8_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_cpy_nok_uint8plus_short)
{
	cute_skip("maximum MsgPack bin size < 256");
}

CUTE_TEST(dpackut_bin_decode_cpy_ok_uint8plus)
{
	cute_skip("maximum MsgPack bin size < 256");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX + 1) */

#if DPACK_BINSZ_MAX >= (UINT16_MAX)

CUTE_TEST(dpackut_bin_decode_cpy_ok_uint16)
{
	dpackut_bin_decode_cpy(UINT16_MAX, UINT16_MAX, UINT16_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT16_MAX)) */

CUTE_TEST(dpackut_bin_decode_cpy_ok_uint16)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX) */

#if DPACK_BINSZ_MAX >= (UINT16_MAX + 1)

CUTE_TEST(dpackut_bin_decode_cpy_nok_uint16plus_short)
{
	dpackut_bin_decode_cpy(-EMSGSIZE, UINT16_MAX + 1, UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_ok_uint16plus)
{
	dpackut_bin_decode_cpy(UINT16_MAX + 1, UINT16_MAX + 1, UINT16_MAX + 1);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT16_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_cpy_nok_uint16plus_short)
{
	cute_skip("maximum MsgPack bin size < 65537");
}

CUTE_TEST(dpackut_bin_decode_cpy_ok_uint16plus)
{
	cute_skip("maximum MsgPack bin size < 65537");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX + 1) */

CUTE_TEST(dpackut_bin_decode_cpy_ok_max)
{
	dpackut_bin_decode_cpy(DPACK_BINSZ_MAX,
	                       DPACK_BINSZ_MAX,
	                       DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_nok_max_short)
{
	dpackut_bin_decode_cpy(-EMSGSIZE, DPACK_BINSZ_MAX, DPACK_BINSZ_MAX - 1);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_bin_decode_cpy_equ_null_dec)
{
	char *  data = data;
	ssize_t ret __unused;

	cute_expect_assertion(ret = dpack_decode_bincpy_equ(NULL, 2, data));
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_null_data)
{
	struct dpack_decoder dec = { 0, };
	char                 data = data;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, &data, 1);
	cute_expect_assertion(ret = dpack_decode_bincpy_equ(&dec, 2, NULL));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_0)
{
	struct dpack_decoder dec = { 0, };
	char                 data = data;
	char *               bin = bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, &data, 1);
	cute_expect_assertion(ret = dpack_decode_bincpy_equ(&dec, 0, bin));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_binsz)
{
	struct dpack_decoder dec = { 0, };
	char                 data = data;
	char *               bin = bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, &data, 1);
	cute_expect_assertion(ret = dpack_decode_bincpy_equ(&dec,
	                                                    DPACK_BINSZ_MAX + 1,
	                                                    bin));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_bin_decode_cpy_equ_null_dec)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_null_data)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_0)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_binsz)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_DEBUG)

CUTE_TEST(dpackut_bin_decode_cpy_equ_uninit_dec)
{
	struct dpack_decoder dec = { 0, };
	char *               data = data;
	ssize_t              ret __unused;

	cute_expect_assertion(ret = dpack_decode_bincpy_equ(&dec, 2, data));
}

#else  /* !defined(CONFIG_DPACK_DEBUG) */

CUTE_TEST(dpackut_bin_decode_cpy_equ_uninit_dec)
{
	cute_skip("debug build disabled");
}

#endif /* defined(CONFIG_DPACK_DEBUG) */

/**
 * @internal
 *
 * Perform a bin decoding test with requested size
 *
 * @param[in] ret       expected code returned by dpack_decode_bincpy_equ()
 * @param[in] ref_size  size of (unpacked) data from which to generate
 *                      reference packed data
 * @param[in] test_size size of (unpacked) data from which to compute encoded
 *                      data buffer size given to dpack_decoder_init_buffer()
 * @param[in] equ_size  expected size of (packed) data given to
 *                      dpack_decode_bincpy_equ() for unpacking data to compare
 *                      with reference unpacked data
 */
static void
dpackut_bin_decode_cpy_equ(ssize_t ret,
                           size_t  ref_size,
                           size_t  test_size,
                           size_t  equ_size)
{
	const struct cute_uint_range range = CUTE_UINT_RANGE(1,
	                                                     DPACK_BINSZ_MAX);
	struct dpackut_bin_data      data;
	struct dpack_decoder         dec;
	char *                       val;
	ssize_t                      sz;
	size_t                       alloc;

	cute_check_uint_range(ref_size, in, range);
	cute_check_uint_range(test_size, in, range);
	cute_check_uint_range(equ_size,
	                      in,
	                      CUTE_UINT_RANGE(1, DPACK_BINSZ_MAX));

	dpackut_bin_gen_data(&data, ref_size);

	alloc = stroll_max(test_size, equ_size);
	val = malloc(alloc);
	memset(val, 0xff, alloc);

	dpack_decoder_init_buffer(&dec,
	                          data.pack_buff,
	                          dpack_bin_size(test_size));

	sz = dpack_decode_bincpy_equ(&dec, equ_size, val);
	cute_check_sint(sz, equal, ret);

	if (sz >= 0) {
		cute_check_uint((size_t)sz, equal, data.value_size);
		cute_check_mem(val, equal, data.value_buff, (size_t)sz);
		cute_check_uint(dpack_bin_size((size_t)sz),
		                equal,
		                data.pack_size);
	}

	dpack_decoder_fini(&dec);

	free(val);

	dpackut_bin_fini_data(&data);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_ok_1)
{
	dpackut_bin_decode_cpy_equ(1, 1, 1, 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_1_sup)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE, 1, 1, 2);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_2_short_inf)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE, 2, 1, 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_2_short_equ)
{
	dpackut_bin_decode_cpy_equ(-EPROTO, 2, 1, 2);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_2_short_sup)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE, 2, 1, 3);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_2_inf)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE, 2, 2, 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_ok_2)
{
	dpackut_bin_decode_cpy_equ(2, 2, 2, 2);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_2_sup)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE, 2, 2, 3);
}

#if DPACK_BINSZ_MAX >= (UINT8_MAX)

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint8_short_inf)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE,
	                           UINT8_MAX,
	                           UINT8_MAX - 1,
	                           UINT8_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint8_short_equ)
{
	dpackut_bin_decode_cpy_equ(-EPROTO,
	                           UINT8_MAX,
	                           UINT8_MAX - 1,
	                           UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint8_inf)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE,
	                           UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_ok_uint8)
{
	dpackut_bin_decode_cpy_equ(UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT8_MAX)) */

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint8_short_inf)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint8_short_equ)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint8_inf)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_cpcpyqu_ok_uint8)
{
	cute_skip("maximum MsgPack bin size < 255");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX) */

#if DPACK_BINSZ_MAX >= (UINT8_MAX + 1)

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint8_short_sup)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE,
	                           UINT8_MAX,
	                           UINT8_MAX - 1,
	                           UINT8_MAX + 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint8_sup)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE,
	                           UINT8_MAX,
	                           UINT8_MAX,
	                           UINT8_MAX + 1);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT8_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint8_short_sup)
{
	cute_skip("maximum MsgPack bin size < 256");
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint8_sup)
{
	cute_skip("maximum MsgPack bin size < 256");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX + 1) */

#if DPACK_BINSZ_MAX >= (UINT16_MAX)

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint16_short_inf)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE,
	                           UINT16_MAX,
	                           UINT16_MAX - 1,
	                           UINT16_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint16_short_equ)
{
	dpackut_bin_decode_cpy_equ(-EPROTO,
	                           UINT16_MAX,
	                           UINT16_MAX - 1,
	                           UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint16_inf)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE,
	                           UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_ok_uint16)
{
	dpackut_bin_decode_cpy_equ(UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT16_MAX)) */

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint16_short_inf)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint16_short_equ)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint16_inf)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_ok_uint16)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX) */

#if DPACK_BINSZ_MAX >= (UINT16_MAX + 1)

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint16_short_sup)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE,
	                           UINT16_MAX,
	                           UINT16_MAX - 1,
	                           UINT16_MAX + 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint16_sup)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE,
	                           UINT16_MAX,
	                           UINT16_MAX,
	                           UINT16_MAX + 1);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT16_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint16_short_sup)
{
	cute_skip("maximum MsgPack bin size < 65537");
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_uint16_sup)
{
	cute_skip("maximum MsgPack bin size < 65537");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX + 1) */

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_binsz_sup)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_binsz_short_sup)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX - 2,
	                           DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_binsz_short_inf)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_binsz_short_equ)
{
	dpackut_bin_decode_cpy_equ(-EPROTO,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX - 1,
	                           DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_nok_binsz_inf)
{
	dpackut_bin_decode_cpy_equ(-EMSGSIZE,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX - 1);
}

CUTE_TEST(dpackut_bin_decode_cpy_equ_ok_binsz)
{
	dpackut_bin_decode_cpy_equ(DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX,
	                           DPACK_BINSZ_MAX);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_bin_decode_cpy_range_null_dec)
{
	char *  data = data;
	ssize_t ret __unused;

	cute_expect_assertion(ret = dpack_decode_bincpy_range(NULL,
	                                                      1,
	                                                      2,
	                                                      data));
}

CUTE_TEST(dpackut_bin_decode_cpy_range_null_data)
{
	struct dpack_decoder dec = { 0, };
	char                 data = data;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, &data, 1);
	cute_expect_assertion(ret = dpack_decode_bincpy_range(&dec,
	                                                      1,
	                                                      2,
	                                                      NULL));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_02)
{
	struct dpack_decoder dec = { 0, };
	char                 data = data;
	char *               bin = bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, &data, 1);
	cute_expect_assertion(ret = dpack_decode_bincpy_range(&dec,
	                                                      0,
	                                                      2,
	                                                      bin));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_10)
{
	struct dpack_decoder dec = { 0, };
	char                 data = data;
	char *               bin = bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, &data, 1);
	cute_expect_assertion(ret = dpack_decode_bincpy_range(&dec,
	                                                      1,
	                                                      0,
	                                                      bin));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_21)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	char *               bin = bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bincpy_range(&dec,
	                                                      2,
	                                                      1,
	                                                      bin));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_binsz)
{
	struct dpack_decoder dec = { 0, };
	char                 data[8] = { 0, };
	char *               bin = bin;
	ssize_t              ret __unused;

	dpack_decoder_init_buffer(&dec, data, 1);
	cute_expect_assertion(ret = dpack_decode_bincpy_range(&dec,
	                                                      1,
	                                                      DPACK_BINSZ_MAX +
	                                                      1,
	                                                      bin));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_bin_decode_cpy_range_null_dec)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_null_data)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_02)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_10)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_21)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_binsz)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_DEBUG)

CUTE_TEST(dpackut_bin_decode_cpy_range_uninit_dec)
{
	struct dpack_decoder dec = { 0, };
	char *               data = data;
	ssize_t              ret __unused;

	cute_expect_assertion(ret = dpack_decode_bincpy_range(&dec,
	                                                      1,
	                                                      2,
	                                                      data));
}

#else  /* !defined(CONFIG_DPACK_DEBUG) */

CUTE_TEST(dpackut_bin_decode_cpy_range_uninit_dec)
{
	cute_skip("debug build disabled");
}

#endif /* defined(CONFIG_DPACK_DEBUG) */

/**
 * @internal
 *
 * Perform a duplicated bin decoding test with requested minimum and maximum
 * size
 *
 * @param[in] ret       expected code returned by dpack_decode_bindup_range()
 * @param[in] ref_size  size of (unpacked) data from which to generate
 *                      reference packed data
 * @param[in] test_size size of (unpacked) data from which to compute encoded
 *                      data buffer size given to dpack_decoder_init_buffer()
 * @param[in] min_size  minimum size of (packed) data given to
 *                      dpack_decode_bindup_range() for unpacking data to
 *                      compare with reference unpacked data
 * @param[in] max_size  maximum size of (packed) data given to
 *                      dpack_decode_bindup_range() for unpacking data to
 *                      compare with reference unpacked data
 */
static void
dpackut_bin_decode_cpy_range(ssize_t ret,
                             size_t  ref_size,
                             size_t  test_size,
                             size_t  min_size,
                             size_t  max_size)
{
	const struct cute_uint_range range = CUTE_UINT_RANGE(1,
	                                                     DPACK_BINSZ_MAX);
	struct dpackut_bin_data data;
	struct dpack_decoder    dec;
	char *                  val;
	ssize_t                 sz;
	size_t                  alloc;

	cute_check_uint_range(ref_size, in, range);
	cute_check_uint_range(test_size, in, range);
	cute_check_uint_range(min_size, in, CUTE_UINT_RANGE(1,
	                                                    DPACK_BINSZ_MAX -
	                                                    1));
	cute_check_uint_range(max_size, in, CUTE_UINT_RANGE(2,
	                                                    DPACK_BINSZ_MAX));

	dpackut_bin_gen_data(&data, ref_size);

	alloc = stroll_max(test_size, max_size);
	val = malloc(alloc);
	memset(val, 0xff, alloc);

	dpack_decoder_init_buffer(&dec,
	                          data.pack_buff,
	                          dpack_bin_size(test_size));

	sz = dpack_decode_bincpy_range(&dec, min_size, max_size, val);
	cute_check_sint(sz, equal, ret);

	if (sz >= 0) {
		cute_check_uint((size_t)sz, equal, data.value_size);
		cute_check_mem(val, equal, data.value_buff, (size_t)sz);
		cute_check_uint(dpack_bin_size((size_t)sz),
		                equal,
		                data.pack_size);
	}

	dpack_decoder_fini(&dec);

	free(val);

	dpackut_bin_fini_data(&data);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_1)
{
	dpackut_bin_decode_cpy_range(1, 1, 1, 1, 2);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_1_short)
{
	dpackut_bin_decode_cpy_range(-EPROTO, 2, 1, 1, 2);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_1)
{
	dpackut_bin_decode_cpy_range(-EMSGSIZE, 1, 1, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_2)
{
	dpackut_bin_decode_cpy_range(2, 2, 2, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_2_short)
{
	dpackut_bin_decode_cpy_range(-EPROTO, 2, 1, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_3)
{
	dpackut_bin_decode_cpy_range(3, 3, 3, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_3_short)
{
	dpackut_bin_decode_cpy_range(-EPROTO, 3, 2, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_4)
{
	dpackut_bin_decode_cpy_range(-EMSGSIZE, 4, 4, 2, 3);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_4_short)
{
	dpackut_bin_decode_cpy_range(-EMSGSIZE, 4, 3, 2, 3);
}

#if DPACK_BINSZ_MAX >= UINT8_MAX

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint8minus2_sup)
{
	dpackut_bin_decode_cpy_range(-EMSGSIZE,
	                             UINT8_MAX - 2,
	                             UINT8_MAX - 2,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_uint8minus_sup)
{
	dpackut_bin_decode_cpy_range(UINT8_MAX - 1,
	                             UINT8_MAX - 1,
	                             UINT8_MAX - 1,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint8minus_short_sup)
{
	dpackut_bin_decode_cpy_range(-EPROTO,
	                             UINT8_MAX - 1,
	                             UINT8_MAX - 2,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_uint8)
{
	dpackut_bin_decode_cpy_range(UINT8_MAX,
	                             UINT8_MAX,
	                             UINT8_MAX,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint8_short)
{
	dpackut_bin_decode_cpy_range(-EPROTO,
	                             UINT8_MAX,
	                             UINT8_MAX - 1,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= UINT8_MAX) */

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint8minus2_sup)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_uint8minus_sup)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint8minus_short_sup)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_uint8)
{
	cute_skip("maximum MsgPack bin size < 255");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint8_short)
{
	cute_skip("maximum MsgPack bin size < 255");
}

#endif /* !(DPACK_BINSZ_MAX >= UINT8_MAX) */

#if  DPACK_BINSZ_MAX >= (UINT8_MAX + 1)

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint8plus_sup)
{
	dpackut_bin_decode_cpy_range(-EMSGSIZE,
	                             UINT8_MAX + 1,
	                             UINT8_MAX + 1,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint8plus_short_sup)
{
	dpackut_bin_decode_cpy_range(-EMSGSIZE,
	                             UINT8_MAX + 1,
	                             UINT8_MAX,
	                             UINT8_MAX - 1,
	                             UINT8_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT8_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint8plus_sup)
{
	cute_skip("maximum MsgPack bin size < 256");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint8plus_short_sup)
{
	cute_skip("maximum MsgPack bin size < 256");
}

#endif /* DPACK_BINSZ_MAX >= (UINT8_MAX + 1) */

#if DPACK_BINSZ_MAX >= UINT16_MAX

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint16minus2_sup)
{
	dpackut_bin_decode_cpy_range(-EMSGSIZE,
	                             UINT16_MAX - 2,
	                             UINT16_MAX - 2,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_uint16minus_sup)
{
	dpackut_bin_decode_cpy_range(UINT16_MAX - 1,
	                             UINT16_MAX - 1,
	                             UINT16_MAX - 1,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint16minus_short_sup)
{
	dpackut_bin_decode_cpy_range(-EPROTO,
	                             UINT16_MAX - 1,
	                             UINT16_MAX - 2,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_uint16)
{
	dpackut_bin_decode_cpy_range(UINT16_MAX,
	                             UINT16_MAX,
	                             UINT16_MAX,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint16_short)
{
	dpackut_bin_decode_cpy_range(-EPROTO,
	                             UINT16_MAX,
	                             UINT16_MAX - 1,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= UINT16_MAX) */

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint16minus2_sup)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_uint16minus_sup)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint16minus_short_sup)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_uint16)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint16_short)
{
	cute_skip("maximum MsgPack bin size < 65535");
}

#endif /* !(DPACK_BINSZ_MAX >= UINT16_MAX) */

#if  DPACK_BINSZ_MAX >= (UINT16_MAX + 1)

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint16plus_sup)
{
	dpackut_bin_decode_cpy_range(-EMSGSIZE,
	                             UINT16_MAX + 1,
	                             UINT16_MAX + 1,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint16plus_short_sup)
{
	dpackut_bin_decode_cpy_range(-EMSGSIZE,
	                             UINT16_MAX + 1,
	                             UINT16_MAX,
	                             UINT16_MAX - 1,
	                             UINT16_MAX);
}

#else  /* !(DPACK_BINSZ_MAX >= (UINT16_MAX + 1)) */

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint16plus_sup)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_uint16plus_short_sup)
{
	cute_skip("maximum MsgPack bin size < 65536");
}

#endif /* DPACK_BINSZ_MAX >= (UINT16_MAX + 1) */

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_binszminus2_sup)
{
	dpackut_bin_decode_cpy_range(-EMSGSIZE,
	                             DPACK_BINSZ_MAX - 2,
	                             DPACK_BINSZ_MAX - 2,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_binszminus_sup)
{
	dpackut_bin_decode_cpy_range(DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_binszminus_short_sup)
{
	dpackut_bin_decode_cpy_range(-EPROTO,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX - 2,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_ok_binsz)
{
	dpackut_bin_decode_cpy_range(DPACK_BINSZ_MAX,
	                             DPACK_BINSZ_MAX,
	                             DPACK_BINSZ_MAX,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX);
}

CUTE_TEST(dpackut_bin_decode_cpy_range_nok_binsz_short)
{
	dpackut_bin_decode_cpy_range(-EPROTO,
	                             DPACK_BINSZ_MAX,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX - 1,
	                             DPACK_BINSZ_MAX);
}

CUTE_GROUP(dpackut_bin_group) = {
	CUTE_REF(dpackut_bin8_sizes),
	CUTE_REF(dpackut_bin16_sizes),
	CUTE_REF(dpackut_bin32_sizes),
	CUTE_REF(dpackut_bin_assert_sizes),

	CUTE_REF(dpackut_bin_encode_null_enc),
	CUTE_REF(dpackut_bin_encode_uninit_enc),
	CUTE_REF(dpackut_bin_encode_null_data),
	CUTE_REF(dpackut_bin_encode_zero),
	CUTE_REF(dpackut_bin_encode_huge),
	CUTE_REF(dpackut_bin_encode_ok_1),
	CUTE_REF(dpackut_bin_encode_nok_1),
	CUTE_REF(dpackut_bin_encode_ok_2),
	CUTE_REF(dpackut_bin_encode_nok_2),
	CUTE_REF(dpackut_bin_encode_ok_uint8),
	CUTE_REF(dpackut_bin_encode_nok_uint8),
	CUTE_REF(dpackut_bin_encode_ok_uint8_1),
	CUTE_REF(dpackut_bin_encode_nok_uint8_1),
	CUTE_REF(dpackut_bin_encode_ok_uint16),
	CUTE_REF(dpackut_bin_encode_nok_uint16),
	CUTE_REF(dpackut_bin_encode_ok_uint16_1),
	CUTE_REF(dpackut_bin_encode_nok_uint16_1),
	CUTE_REF(dpackut_bin_encode_ok_max),
	CUTE_REF(dpackut_bin_encode_nok_max),

	CUTE_REF(dpackut_bin_decode_dup_null_dec),
	CUTE_REF(dpackut_bin_decode_dup_null_data),
	CUTE_REF(dpackut_bin_decode_dup_uninit_dec),
	CUTE_REF(dpackut_bin_decode_dup_ok_1),
	CUTE_REF(dpackut_bin_decode_dup_nok_2_short),
	CUTE_REF(dpackut_bin_decode_dup_ok_2),
	CUTE_REF(dpackut_bin_decode_dup_nok_3_short),
	CUTE_REF(dpackut_bin_decode_dup_ok_uint8),
	CUTE_REF(dpackut_bin_decode_dup_nok_uint8plus_short),
	CUTE_REF(dpackut_bin_decode_dup_ok_uint8plus),
	CUTE_REF(dpackut_bin_decode_dup_ok_uint16),
	CUTE_REF(dpackut_bin_decode_dup_nok_uint16plus_short),
	CUTE_REF(dpackut_bin_decode_dup_ok_uint16plus),
	CUTE_REF(dpackut_bin_decode_dup_ok_max),
	CUTE_REF(dpackut_bin_decode_dup_nok_max_short),

	CUTE_REF(dpackut_bin_decode_dup_equ_null_dec),
	CUTE_REF(dpackut_bin_decode_dup_equ_uninit_dec),
	CUTE_REF(dpackut_bin_decode_dup_equ_null_data),
	CUTE_REF(dpackut_bin_decode_dup_equ_0),
	CUTE_REF(dpackut_bin_decode_dup_equ_binsz),
	CUTE_REF(dpackut_bin_decode_dup_equ_ok_1),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_1_sup),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_2_short_inf),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_2_short_equ),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_2_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_2_inf),
	CUTE_REF(dpackut_bin_decode_dup_equ_ok_2),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_2_sup),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_uint8_sup),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_uint8_short_inf),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_uint8_short_equ),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_uint8_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_uint8_inf),
	CUTE_REF(dpackut_bin_decode_dup_equ_ok_uint8),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_uint8_sup),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_uint16_sup),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_uint16_short_inf),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_uint16_short_equ),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_uint16_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_uint16_inf),
	CUTE_REF(dpackut_bin_decode_dup_equ_ok_uint16),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_uint16_sup),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_binsz_sup),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_binsz_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_binsz_short_inf),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_binsz_short_equ),
	CUTE_REF(dpackut_bin_decode_dup_equ_nok_binsz_inf),
	CUTE_REF(dpackut_bin_decode_dup_equ_ok_binsz),

	CUTE_REF(dpackut_bin_decode_dup_max_null_dec),
	CUTE_REF(dpackut_bin_decode_dup_max_uninit_dec),
	CUTE_REF(dpackut_bin_decode_dup_max_null_data),
	CUTE_REF(dpackut_bin_decode_dup_max_0),
	CUTE_REF(dpackut_bin_decode_dup_max_binsz),
	CUTE_REF(dpackut_bin_decode_dup_max_ok_1_sup),
	CUTE_REF(dpackut_bin_decode_dup_max_ok_2_equ),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_2_short_equ),
	CUTE_REF(dpackut_bin_decode_dup_max_ok_2_sup),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_2_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_3_inf),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_3_short_inf),
	CUTE_REF(dpackut_bin_decode_dup_max_ok_3_equ),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_3_short_equ),
	CUTE_REF(dpackut_bin_decode_dup_max_ok_3_sup),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_3_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_uint8_inf),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_uint8_short_inf),
	CUTE_REF(dpackut_bin_decode_dup_max_ok_uint8_equ),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_uint8_short_equ),
	CUTE_REF(dpackut_bin_decode_dup_max_ok_uint8_sup),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_uint8_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_uint16_inf),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_uint16_short_inf),
	CUTE_REF(dpackut_bin_decode_dup_max_ok_uint16_equ),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_uint16_short_equ),
	CUTE_REF(dpackut_bin_decode_dup_max_ok_uint16_sup),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_uint16_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_max_ok_binsz_sup),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_binsz_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_binsz_inf),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_binsz_short_inf),
	CUTE_REF(dpackut_bin_decode_dup_max_ok_binsz_equ),
	CUTE_REF(dpackut_bin_decode_dup_max_nok_binsz_short_equ),

	CUTE_REF(dpackut_bin_decode_dup_range_null_dec),
	CUTE_REF(dpackut_bin_decode_dup_range_null_data),
	CUTE_REF(dpackut_bin_decode_dup_range_02),
	CUTE_REF(dpackut_bin_decode_dup_range_10),
	CUTE_REF(dpackut_bin_decode_dup_range_21),
	CUTE_REF(dpackut_bin_decode_dup_range_binsz),
	CUTE_REF(dpackut_bin_decode_dup_range_uninit_dec),
	CUTE_REF(dpackut_bin_decode_dup_range_ok_1),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_1_short),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_1),
	CUTE_REF(dpackut_bin_decode_dup_range_ok_2),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_2_short),
	CUTE_REF(dpackut_bin_decode_dup_range_ok_3),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_3_short),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_4),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_4_short),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_uint8minus2_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_ok_uint8minus_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_uint8minus_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_ok_uint8),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_uint8_short),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_uint8plus_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_uint8plus_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_uint16minus2_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_ok_uint16minus_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_uint16minus_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_ok_uint16),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_uint16_short),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_uint16plus_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_uint16plus_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_binszminus2_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_ok_binszminus_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_binszminus_short_sup),
	CUTE_REF(dpackut_bin_decode_dup_range_ok_binsz),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_binsz_short),
	CUTE_REF(dpackut_bin_decode_dup_range_nok_binszplus_short_sup),

	CUTE_REF(dpackut_bin_decode_cpy_null_dec),
	CUTE_REF(dpackut_bin_decode_cpy_null_data),
	CUTE_REF(dpackut_bin_decode_cpy_zero),
	CUTE_REF(dpackut_bin_decode_cpy_huge),
	CUTE_REF(dpackut_bin_decode_cpy_ok_1),
	CUTE_REF(dpackut_bin_decode_cpy_nok_2_short),
	CUTE_REF(dpackut_bin_decode_cpy_ok_2),
	CUTE_REF(dpackut_bin_decode_cpy_nok_3_short),
	CUTE_REF(dpackut_bin_decode_cpy_ok_uint8),
	CUTE_REF(dpackut_bin_decode_cpy_nok_uint8plus_short),
	CUTE_REF(dpackut_bin_decode_cpy_ok_uint8plus),
	CUTE_REF(dpackut_bin_decode_cpy_ok_uint16),
	CUTE_REF(dpackut_bin_decode_cpy_nok_uint16plus_short),
	CUTE_REF(dpackut_bin_decode_cpy_ok_uint16plus),
	CUTE_REF(dpackut_bin_decode_cpy_ok_max),
	CUTE_REF(dpackut_bin_decode_cpy_nok_max_short),

	CUTE_REF(dpackut_bin_decode_cpy_equ_null_dec),
	CUTE_REF(dpackut_bin_decode_cpy_equ_uninit_dec),
	CUTE_REF(dpackut_bin_decode_cpy_equ_null_data),
	CUTE_REF(dpackut_bin_decode_cpy_equ_0),
	CUTE_REF(dpackut_bin_decode_cpy_equ_binsz),
	CUTE_REF(dpackut_bin_decode_cpy_equ_ok_1),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_1_sup),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_2_short_inf),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_2_short_equ),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_2_short_sup),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_2_inf),
	CUTE_REF(dpackut_bin_decode_cpy_equ_ok_2),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_2_sup),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_uint8_sup),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_uint8_short_inf),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_uint8_short_equ),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_uint8_short_sup),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_uint8_inf),
	CUTE_REF(dpackut_bin_decode_cpy_equ_ok_uint8),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_uint8_sup),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_uint16_sup),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_uint16_short_inf),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_uint16_short_equ),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_uint16_short_sup),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_uint16_inf),
	CUTE_REF(dpackut_bin_decode_cpy_equ_ok_uint16),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_uint16_sup),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_binsz_sup),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_binsz_short_sup),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_binsz_short_inf),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_binsz_short_equ),
	CUTE_REF(dpackut_bin_decode_cpy_equ_nok_binsz_inf),
	CUTE_REF(dpackut_bin_decode_cpy_equ_ok_binsz),

	CUTE_REF(dpackut_bin_decode_cpy_range_null_dec),
	CUTE_REF(dpackut_bin_decode_cpy_range_null_data),
	CUTE_REF(dpackut_bin_decode_cpy_range_02),
	CUTE_REF(dpackut_bin_decode_cpy_range_10),
	CUTE_REF(dpackut_bin_decode_cpy_range_21),
	CUTE_REF(dpackut_bin_decode_cpy_range_binsz),
	CUTE_REF(dpackut_bin_decode_cpy_range_uninit_dec),
	CUTE_REF(dpackut_bin_decode_cpy_range_ok_1),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_1_short),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_1),
	CUTE_REF(dpackut_bin_decode_cpy_range_ok_2),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_2_short),
	CUTE_REF(dpackut_bin_decode_cpy_range_ok_3),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_3_short),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_4),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_4_short),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_uint8minus2_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_ok_uint8minus_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_uint8minus_short_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_ok_uint8),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_uint8_short),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_uint8plus_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_uint8plus_short_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_uint16minus2_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_ok_uint16minus_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_uint16minus_short_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_ok_uint16),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_uint16_short),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_uint16plus_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_uint16plus_short_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_binszminus2_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_ok_binszminus_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_binszminus_short_sup),
	CUTE_REF(dpackut_bin_decode_cpy_range_ok_binsz),
	CUTE_REF(dpackut_bin_decode_cpy_range_nok_binsz_short)
};

CUTE_SUITE_EXTERN(dpackut_bin_suite,
                  dpackut_bin_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
