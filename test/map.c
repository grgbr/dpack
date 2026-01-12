/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/map.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include "utest.h"
#include <math.h>
#include <errno.h>

#define DPACKUT_MAP_ENABLED(_fld_nr) \
	(DPACK_MAP_FLDNR_MAX >= (_fld_nr))

CUTE_TEST(dpackut_fixmap_sizes)
{
	/* Msgpack fixmap sizes. */
	cute_check_uint(DPACK_MAP_SIZE(1U, 1U), equal, 1U + 1U);
	cute_check_uint(dpack_map_size(1U, 1U), equal, 1U + 1U);

	cute_check_uint(DPACK_MAP_SIZE(2U, 1U), equal, 1U + 1U);
	cute_check_uint(dpack_map_size(2U, 1U), equal, 1U + 1U);

#if DPACKUT_MAP_ENABLED(14)
	cute_check_uint(DPACK_MAP_SIZE(14U, 1U), equal, 1U + 1U);
	cute_check_uint(dpack_map_size(14U, 1U), equal, 1U + 1U);
#endif

#if DPACKUT_MAP_ENABLED(15)
	cute_check_uint(DPACK_MAP_SIZE(15U, 1U), equal, 1U + 1U);
	cute_check_uint(dpack_map_size(15U, 1U), equal, 1U + 1U);
#endif
}

#if DPACK_MAP_FLDNR_MAX > _DPACK_FIXMAP_FLDNR_MAX

CUTE_TEST(dpackut_map16_sizes)
{
	cute_check_uint(DPACK_MAP_SIZE(16U, 1U), equal, 3U + 1U);
	cute_check_uint(dpack_map_size(16U, 1U), equal, 3U + 1U);

#if DPACKUT_MAP_ENABLED(17)
      cute_check_uint(DPACK_MAP_SIZE(17U, 1U), equal, 3U + 1U);
      cute_check_uint(dpack_map_size(17U, 1U), equal, 3U + 1U);
#endif

#if DPACKUT_MAP_ENABLED(UINT16_MAX - 2)
	cute_check_uint(DPACK_MAP_SIZE(UINT16_MAX - 2, 1U), equal, 3U + 1U);
	cute_check_uint(dpack_map_size(UINT16_MAX - 2, 1U), equal, 3U + 1U);
#endif

#if DPACKUT_MAP_ENABLED(UINT16_MAX - 1)
	cute_check_uint(DPACK_MAP_SIZE(UINT16_MAX - 1, 1U), equal, 3U + 1U);
	cute_check_uint(dpack_map_size(UINT16_MAX - 1, 1U), equal, 3U + 1U);
#endif

#if DPACKUT_MAP_ENABLED(UINT16_MAX)
	cute_check_uint(DPACK_MAP_SIZE(UINT16_MAX, 1U), equal, 3U + 1U);
	cute_check_uint(dpack_map_size(UINT16_MAX, 1U), equal, 3U + 1U);
#endif
}

#else  /* DPACK_MAP_FLDNR_MAX > _DPACK_FIXMAP_FLDNR_MAX */

CUTE_TEST(dpackut_map16_sizes)
{
	cute_skip("16-bits MessagePack map support not compiled-in");
}

#endif /* DPACK_MAP_FLDNR_MAX > _DPACK_FIXMAP_FLDNR_MAX */

#if DPACK_MAP_FLDNR_MAX > _DPACK_MAP16_FLDNR_MAX

CUTE_TEST(dpackut_map32_sizes)
{
	cute_check_uint(DPACK_MAP_SIZE(UINT16_MAX + 1, 1U), equal, 5U + 1U);
	cute_check_uint(dpack_map_size(UINT16_MAX + 1, 1U), equal, 5U + 1U);

#if DPACKUT_MAP_ENABLED(UINT16_MAX + 2)
      cute_check_uint(DPACK_MAP_SIZE(UINT16_MAX + 2, 1U), equal, 5U + 1U);
      cute_check_uint(dpack_map_size(UINT16_MAX + 2, 1U), equal, 5U + 1U);
#endif

#if DPACKUT_MAP_ENABLED((_DPACK_MAP32_FLDNR_MAX / 2) - 2)
	cute_check_uint(DPACK_MAP_SIZE(UINT32_MAX - 2, 1U), equal, 5U + 1U);
	cute_check_uint(dpack_map_size(UINT32_MAX - 2, 1U), equal, 5U + 1U);
#endif

#if DPACKUT_MAP_ENABLED((_DPACK_MAP32_FLDNR_MAX / 2) - 1)
	cute_check_uint(DPACK_MAP_SIZE(UINT32_MAX - 1, 1U), equal, 5U + 1U);
	cute_check_uint(dpack_map_size(UINT32_MAX - 1, 1U), equal, 5U + 1U);
#endif

#if DPACKUT_MAP_ENABLED(_DPACK_MAP32_FLDNR_MAX / 2)
	cute_check_uint(DPACK_MAP_SIZE(UINT32_MAX, 1U), equal, 5U + 1U);
	cute_check_uint(dpack_map_size(UINT32_MAX, 1U), equal, 5U + 1U);
#endif
}

#else  /* DPACK_MAP_FLDNR_MAX > _DPACK_MAP32_FLDNR_MAX */

CUTE_TEST(dpackut_map32_sizes)
{
	cute_skip("32-bits MessagePack map support not compiled-in");
}

#endif /* DPACK_MAP_FLDNR_MAX > _DPACK_MAP32_FLDNR_MAX */

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_map_size_assert)
{
	size_t ret __unused;

	cute_expect_assertion(ret = dpack_map_size(0, 1));

	cute_expect_assertion(
		ret = dpack_map_size(DPACK_MAP_FLDNR_MAX + 1, 1));

	ret = dpack_map_size(1, DPACK_MAP_DATA_SIZE_MAX);

	cute_expect_assertion(
		ret = dpack_map_size(1, DPACK_MAP_DATA_SIZE_MAX + 1));
}

CUTE_TEST(dpackut_map_encode_begin_null_enc)
{
	int ret __unused;

	cute_expect_assertion(ret = dpack_map_begin_encode(NULL, 1));
}

CUTE_TEST(dpackut_map_encode_begin_zero)
{
	struct dpack_encoder_buffer enc = { 0, };
	uint8_t                     buff;
	int                         ret __unused;

	dpack_encoder_init_buffer(&enc, &buff, 1);
	cute_expect_assertion(ret = dpack_map_begin_encode(&enc.base, 0));
	dpack_encoder_fini(&enc.base);
}

CUTE_TEST(dpackut_map_encode_begin_huge)
{
	struct dpack_encoder_buffer enc = { 0, };
	uint8_t                     buff;
	int                         ret __unused;

	dpack_encoder_init_buffer(&enc, &buff, 1);
	cute_expect_assertion(
		ret = dpack_map_begin_encode(&enc.base,
		                             DPACK_MAP_FLDNR_MAX + 1));
	dpack_encoder_fini(&enc.base);
}

CUTE_TEST(dpackut_map_encode_end_null_enc)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff;

	dpack_encoder_init_buffer(&enc, &buff, 1);
	cute_expect_assertion(dpack_map_end_encode(NULL));
	dpack_encoder_fini(&enc.base);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_map_size_assert)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_map_encode_begin_null_enc)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_map_encode_begin_zero)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_map_encode_begin_huge)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_map_encode_end_null_enc)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_DEBUG)

CUTE_TEST(dpackut_map_encode_begin_uninit_enc)
{
	struct dpack_encoder_buffer enc = { 0, };
	int                         ret __unused;

	cute_expect_assertion(ret = dpack_map_begin_encode(&enc.base, 1));
}

CUTE_TEST(dpackut_map_encode_end_uninit_enc)
{
	struct dpack_encoder_buffer enc = { 0, };

	cute_expect_assertion(dpack_map_end_encode(&enc.base));
}

#else /* !defined(CONFIG_DPACK_DEBUG) */

CUTE_TEST(dpackut_map_encode_begin_uninit_enc)
{
	cute_skip("debug build disabled");
}

CUTE_TEST(dpackut_map_encode_end_uninit_enc)
{
	cute_skip("debug build disabled");
}

#endif /* defined(CONFIG_DPACK_DEBUG) */

/* dpack-utest-gen.py "{0: False, 1: True}" */
#define DPACKUT_MAP_BOOL_FLD_NR \
	(2U)
#define DPACKUT_MAP_BOOL_PACK_DATA \
	"\x82\x00\xc2\x01\xc3"
#define DPACKUT_MAP_BOOL_PACK_SIZE \
	(sizeof(DPACKUT_MAP_BOOL_PACK_DATA) - 1)
#define DPACKUT_MAP_BOOL_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_BOOL_FLD_NR, \
	               DPACKUT_MAP_BOOL_FLD_NR * DPACK_MAP_BOOL_SIZE_MAX)

CUTE_TEST(dpackut_map_encode_bool)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_BOOL_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_BOOL_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_bool(&enc.base, 0, false), equal, 0);
	cute_check_sint(dpack_map_encode_bool(&enc.base, 1, true), equal, 0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_BOOL_PACK_DATA,
	               DPACKUT_MAP_BOOL_PACK_SIZE);
}

/* dpack-utest-gen.py "{ 0: -128, 1: 0, 2: 127 }" */
#define DPACKUT_MAP_INT8_FLD_NR \
	(3U)
#define DPACKUT_MAP_INT8_PACK_DATA \
	"\x83\x00\xd0\x80\x01\x00\x02\x7f"
#define DPACKUT_MAP_INT8_PACK_SIZE \
	(sizeof(DPACKUT_MAP_INT8_PACK_DATA) - 1)
#define DPACKUT_MAP_INT8_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_INT8_FLD_NR, \
	               DPACKUT_MAP_INT8_FLD_NR * DPACK_MAP_INT8_SIZE_MAX)

CUTE_TEST(dpackut_map_encode_int8)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_INT8_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_INT8_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_int8(&enc.base, 0, INT8_MIN), equal, 0);
	cute_check_sint(dpack_map_encode_int8(&enc.base, 1, 0), equal, 0);
	cute_check_sint(dpack_map_encode_int8(&enc.base, 2, INT8_MAX), equal, 0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_INT8_PACK_DATA,
	               DPACKUT_MAP_INT8_PACK_SIZE);
}

/*
 * Cannot generate map with duplicate keys, hence using pack-utest-gen.py is
 * not an option.
 * Test a generated map containing 3 8-bits unsigned values indexed in a Msgpack
 * map as following:
 * - 0: 127,
 * - 1: 0,
 * - 0: 255
 * Note the duplicate `0' indices.
 */
#define DPACKUT_MAP_UINT8_FLD_NR \
	(3U)
#define DPACKUT_MAP_UINT8_PACK_DATA \
	"\x83\x00\x7f\x01\x00\x00\xcc\xff"
#define DPACKUT_MAP_UINT8_PACK_SIZE \
	(sizeof(DPACKUT_MAP_UINT8_PACK_DATA) - 1)
#define DPACKUT_MAP_UINT8_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_UINT8_FLD_NR, \
	               DPACKUT_MAP_UINT8_FLD_NR * DPACK_MAP_UINT8_SIZE_MAX)

CUTE_TEST(dpackut_map_encode_uint8)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_UINT8_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_UINT8_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_uint8(&enc.base, 0, 127), equal, 0);
	cute_check_sint(dpack_map_encode_uint8(&enc.base, 1, 0), equal, 0);
	cute_check_sint(dpack_map_encode_uint8(&enc.base, 0, UINT8_MAX),
	                equal,
	                0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_UINT8_PACK_DATA,
	               DPACKUT_MAP_UINT8_PACK_SIZE);
}

/* dpack-utest-gen.py "{ 0: -32768, 1: 0, 2: 32767 }" */
#define DPACKUT_MAP_INT16_FLD_NR \
	(3U)
#define DPACKUT_MAP_INT16_PACK_DATA \
	"\x83\x00\xd1\x80\x00\x01\x00\x02\xd1\x7f\xff"
#define DPACKUT_MAP_INT16_PACK_SIZE \
	(sizeof(DPACKUT_MAP_INT16_PACK_DATA) - 1)
#define DPACKUT_MAP_INT16_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_INT16_FLD_NR, \
	               DPACKUT_MAP_INT16_FLD_NR * DPACK_MAP_INT16_SIZE_MAX)

CUTE_TEST(dpackut_map_encode_int16)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_INT16_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_INT16_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_int16(&enc.base, 0, INT16_MIN),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_int16(&enc.base, 1, 0),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_int16(&enc.base, 2, INT16_MAX),
	                equal,
	                0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_INT16_PACK_DATA,
	               DPACKUT_MAP_INT16_PACK_SIZE);
}

/* dpack-utest-gen.py "{ 0: 0, 2: 65535 }" */
#define DPACKUT_MAP_UINT16_FLD_NR \
	(2U)
#define DPACKUT_MAP_UINT16_PACK_DATA \
	"\x82\x00\x00\x02\xcd\xff\xff"
#define DPACKUT_MAP_UINT16_PACK_SIZE \
	(sizeof(DPACKUT_MAP_UINT16_PACK_DATA) - 1)
#define DPACKUT_MAP_UINT16_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_UINT16_FLD_NR, \
	               DPACKUT_MAP_UINT16_FLD_NR * DPACK_MAP_UINT16_SIZE_MAX)

CUTE_TEST(dpackut_map_encode_uint16)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_UINT16_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_UINT16_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_uint16(&enc.base, 0, 0), equal, 0);
	cute_check_sint(dpack_map_encode_uint16(&enc.base, 2, UINT16_MAX),
	                equal,
	                0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_UINT16_PACK_DATA,
	               DPACKUT_MAP_UINT16_PACK_SIZE);
}

/* dpack-utest-gen.py "{ 0: -2147483648, 1: 0, 2: 2147483647 }" */
#define DPACKUT_MAP_INT32_FLD_NR \
	(3U)
#define DPACKUT_MAP_INT32_PACK_DATA \
	"\x83\x00\xd2\x80\x00\x00\x00\x01\x00\x02\xd2\x7f\xff\xff\xff"
#define DPACKUT_MAP_INT32_PACK_SIZE \
	(sizeof(DPACKUT_MAP_INT32_PACK_DATA) - 1)
#define DPACKUT_MAP_INT32_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_INT32_FLD_NR, \
	               DPACKUT_MAP_INT32_FLD_NR * DPACK_MAP_INT32_SIZE_MAX)

CUTE_TEST(dpackut_map_encode_int32)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_INT32_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_map_begin_encode(&enc.base, DPACKUT_MAP_INT32_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_int32(&enc.base, 0, INT32_MIN), equal, 0);
	cute_check_sint(dpack_map_encode_int32(&enc.base, 1, 0), equal, 0);
	cute_check_sint(dpack_map_encode_int32(&enc.base, 2, INT32_MAX), equal, 0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_INT32_PACK_DATA,
	               DPACKUT_MAP_INT32_PACK_SIZE);
}

/* dpack-utest-gen.py "{ 0: 0, 2: 4294967295 }" */
#define DPACKUT_MAP_UINT32_FLD_NR \
	(2U)
#define DPACKUT_MAP_UINT32_PACK_DATA \
	"\x82\x00\x00\x02\xce\xff\xff\xff\xff"
#define DPACKUT_MAP_UINT32_PACK_SIZE \
	(sizeof(DPACKUT_MAP_UINT32_PACK_DATA) - 1)
#define DPACKUT_MAP_UINT32_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_UINT32_FLD_NR, \
	               DPACKUT_MAP_UINT32_FLD_NR * DPACK_MAP_UINT32_SIZE_MAX)

CUTE_TEST(dpackut_map_encode_uint32)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_UINT32_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_UINT32_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_uint32(&enc.base, 0, 0), equal, 0);
	cute_check_sint(dpack_map_encode_uint32(&enc.base, 2, UINT32_MAX),
	                equal,
	                0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_UINT32_PACK_DATA,
	               DPACKUT_MAP_UINT32_PACK_SIZE);
}

/*
 * dpack-utest-gen.py \
 * "{ 0: -9223372036854775808, 1: 0, 2: 9223372036854775807 }"
 */
#define DPACKUT_MAP_INT64_FLD_NR \
	(3U)
#define DPACKUT_MAP_INT64_PACK_DATA \
	"\x83" \
	"\x00\xd3\x80\x00\x00\x00\x00\x00\x00" \
	"\x00" \
	"\x01\x00\x02\xd3\x7f\xff\xff\xff\xff\xff\xff\xff"
#define DPACKUT_MAP_INT64_PACK_SIZE \
	(sizeof(DPACKUT_MAP_INT64_PACK_DATA) - 1)
#define DPACKUT_MAP_INT64_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_INT64_FLD_NR, \
	               DPACKUT_MAP_INT64_FLD_NR * DPACK_MAP_INT64_SIZE_MAX)

CUTE_TEST(dpackut_map_encode_int64)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_INT64_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_map_begin_encode(&enc.base, DPACKUT_MAP_INT64_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_int64(&enc.base, 0, INT64_MIN),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_int64(&enc.base, 1, 0), equal, 0);
	cute_check_sint(dpack_map_encode_int64(&enc.base, 2, INT64_MAX),
	                equal,
	                0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_INT64_PACK_DATA,
	               DPACKUT_MAP_INT64_PACK_SIZE);
}

/* dpack-utest-gen.py "{ 0: 0, 2: 18446744073709551615 }" */
#define DPACKUT_MAP_UINT64_FLD_NR \
	(2U)
#define DPACKUT_MAP_UINT64_PACK_DATA \
	"\x82\x00\x00\x02\xcf\xff\xff\xff\xff\xff\xff\xff\xff"
#define DPACKUT_MAP_UINT64_PACK_SIZE \
	(sizeof(DPACKUT_MAP_UINT64_PACK_DATA) - 1)
#define DPACKUT_MAP_UINT64_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_UINT64_FLD_NR, \
	               DPACKUT_MAP_UINT64_FLD_NR * DPACK_MAP_UINT64_SIZE_MAX)

CUTE_TEST(dpackut_map_encode_uint64)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_UINT64_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_UINT64_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_uint64(&enc.base, 0, 0), equal, 0);
	cute_check_sint(dpack_map_encode_uint64(&enc.base, 2, UINT64_MAX),
	                equal,
	                0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_UINT64_PACK_DATA,
	               DPACKUT_MAP_UINT64_PACK_SIZE);
}

#if defined(CONFIG_DPACK_FLOAT)

/* dpack-utest-gen.py -s "{ 2: -1.005, 0: 10.0 }" */
#define DPACKUT_MAP_FLOAT_FLD_NR \
	(2U)
#define DPACKUT_MAP_FLOAT_PACK_DATA \
	"\x82\x02\xca\xbf\x80\xa3\xd7\x00\xca\x41\x20\x00\x00"
#define DPACKUT_MAP_FLOAT_PACK_SIZE \
	(sizeof(DPACKUT_MAP_FLOAT_PACK_DATA) - 1)
#define DPACKUT_MAP_FLOAT_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_FLOAT_FLD_NR, \
	               DPACKUT_MAP_FLOAT_FLD_NR * DPACK_MAP_FLOAT_SIZE_MAX)

CUTE_TEST(dpackut_map_encode_float)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_FLOAT_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_FLOAT_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_float(&enc.base, 2, -1.005f),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_float(&enc.base, 0, 10.0f),
	                equal,
	                0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_FLOAT_PACK_DATA,
	               DPACKUT_MAP_FLOAT_PACK_SIZE);
}

#else  /* !defined(CONFIG_DPACK_FLOAT) */

CUTE_TEST(dpackut_map_encode_float)
{
	cute_skip("MessagePack float support not compiled-in");
}

#endif /* defined(CONFIG_DPACK_FLOAT) */

#if defined(CONFIG_DPACK_DOUBLE)

/* dpack-utest-gen.py "{ 2: -1.005, 0: float('inf') }" */
#define DPACKUT_MAP_DOUBLE_FLD_NR \
	(2U)
#define DPACKUT_MAP_DOUBLE_PACK_DATA \
	"\x82" \
	"\x02\xcb\xbf\xf0\x14\x7a\xe1\x47\xae\x14" \
	"\x00\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00"
#define DPACKUT_MAP_DOUBLE_PACK_SIZE \
	(sizeof(DPACKUT_MAP_DOUBLE_PACK_DATA) - 1)
#define DPACKUT_MAP_DOUBLE_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_DOUBLE_FLD_NR, \
	               DPACKUT_MAP_DOUBLE_FLD_NR * DPACK_MAP_DOUBLE_SIZE_MAX)

CUTE_TEST(dpackut_map_encode_double)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_DOUBLE_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_DOUBLE_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_double(&enc.base, 2, -1.005),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_double(&enc.base, 0, INFINITY),
	                equal,
	                0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_DOUBLE_PACK_DATA,
	               DPACKUT_MAP_DOUBLE_PACK_SIZE);
}

#else  /* !defined(CONFIG_DPACK_DOUBLE) */

CUTE_TEST(dpackut_map_encode_double)
{
	cute_skip("MessagePack double support not compiled-in");
}

#endif /* defined(CONFIG_DPACK_DOUBLE) */

#if defined(CONFIG_DPACK_STRING) || \
    defined(CONFIG_DPACK_LVSTR) || \
    defined(CONFIG_DPACK_BIN)

static uint8_t * dpackut_map_buff;

static void dpackut_map_teardown(void)
{
	cute_check_ptr(dpackut_map_buff, unequal, NULL);
	free(dpackut_map_buff);
}

#endif  /* defined(CONFIG_DPACK_STRING) || \
           defined(CONFIG_DPACK_LVSTR) || \
           defined(CONFIG_DPACK_BIN) */

#if defined(CONFIG_DPACK_STRING)

/* dpack-utest-gen.py '{ 0: "a", 1: "list", 2: "of strings" }' */
#define DPACKUT_MAP_STR_FLD_NR \
	(3U)
#define DPACKUT_MAP_STR_PACK_DATA \
	"\x83" \
	"\x00\xa1\x61" \
	"\x01\xa4\x6c\x69\x73\x74" \
	"\x02\xaa\x6f\x66\x20\x73\x74\x72\x69\x6e\x67\x73"
#define DPACKUT_MAP_STR_PACK_SIZE \
	(sizeof(DPACKUT_MAP_STR_PACK_DATA) - 1)
#define DPACKUT_MAP_STR_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_STR_FLD_NR, \
	               DPACKUT_MAP_STR_FLD_NR * DPACK_MAP_STR_SIZE(10))

static void dpackut_map_encode_str_setup(void)
{
	dpackut_map_buff = malloc(DPACKUT_MAP_STR_PACK_SIZE_MAX);
	cute_check_ptr(dpackut_map_buff, unequal, NULL);

	memset(dpackut_map_buff, 0, DPACKUT_MAP_STR_PACK_SIZE_MAX);
}

CUTE_TEST_STATIC(dpackut_map_encode_str,
                 dpackut_map_encode_str_setup,
                 dpackut_map_teardown,
                 CUTE_DFLT_TMOUT)
{
	struct dpack_encoder_buffer enc;

	dpack_encoder_init_buffer(&enc,
	                          dpackut_map_buff,
	                          DPACKUT_MAP_STR_PACK_SIZE_MAX);

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_STR_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_str(&enc.base, 0, "a"), equal, 0);
	cute_check_sint(dpack_map_encode_str(&enc.base, 1, "list"), equal, 0);
	cute_check_sint(dpack_map_encode_str(&enc.base, 2, "of strings"),
	                equal,
	                0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(dpackut_map_buff,
	               equal,
	               DPACKUT_MAP_STR_PACK_DATA,
	               DPACKUT_MAP_STR_PACK_SIZE);
}

#else  /* !defined(CONFIG_DPACK_STRING) */

CUTE_TEST(dpackut_map_encode_str)
{
	cute_skip("MessagePack string support not compiled-in");
}

#endif /* defined(CONFIG_DPACK_STRING) */

#if defined(CONFIG_DPACK_LVSTR)

CUTE_TEST_STATIC(dpackut_map_encode_lvstr,
                 dpackut_map_encode_str_setup,
                 dpackut_map_teardown,
                 CUTE_DFLT_TMOUT)
{
	struct dpack_encoder_buffer enc;
	struct stroll_lvstr         str0 = STROLL_LVSTR_INIT_LEND("a");
	struct stroll_lvstr         str1 = STROLL_LVSTR_INIT_LEND("list");
	struct stroll_lvstr         str2 = STROLL_LVSTR_INIT_LEND("of strings");

	dpack_encoder_init_buffer(&enc,
	                          dpackut_map_buff,
	                          DPACKUT_MAP_STR_PACK_SIZE_MAX);

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_STR_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_lvstr(&enc.base, 0, &str0), equal, 0);
	cute_check_sint(dpack_map_encode_lvstr(&enc.base, 1, &str1), equal, 0);
	cute_check_sint(dpack_map_encode_lvstr(&enc.base, 2, &str2), equal, 0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(dpackut_map_buff,
	               equal,
	               DPACKUT_MAP_STR_PACK_DATA,
	               DPACKUT_MAP_STR_PACK_SIZE);

	stroll_lvstr_fini(&str0);
	stroll_lvstr_fini(&str1);
	stroll_lvstr_fini(&str2);
}

#else  /* !defined(CONFIG_DPACK_STRING) */

CUTE_TEST(dpackut_map_encode_lvstr)
{
	cute_skip("MessagePack lvstr support not compiled-in");
}

#endif /* defined(CONFIG_DPACK_STRING) */

#if defined(CONFIG_DPACK_BIN)

/* dpack/test/dpack-utest-gen.py "{ 2: b'\x00\x01\x03', 0: b'\xff\xfe\xfd' }" */
#define DPACKUT_MAP_BIN_FLD_NR \
	(2U)
#define DPACKUT_MAP_BIN_PACK_DATA \
	"\x82\x02\xc4\x03\x00\x01\x03\x00\xc4\x03\xff\xfe\xfd"
#define DPACKUT_MAP_BIN_PACK_SIZE \
	(sizeof(DPACKUT_MAP_BIN_PACK_DATA) - 1)
#define DPACKUT_MAP_BIN_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_BIN_FLD_NR, \
	               DPACKUT_MAP_BIN_FLD_NR * DPACK_MAP_BIN_SIZE(3))

static void dpackut_map_encode_bin_setup(void)
{
	dpackut_map_buff = malloc(DPACKUT_MAP_BIN_PACK_SIZE_MAX);
	cute_check_ptr(dpackut_map_buff, unequal, NULL);

	memset(dpackut_map_buff, 0, DPACKUT_MAP_BIN_PACK_SIZE_MAX);
}

CUTE_TEST_STATIC(dpackut_map_encode_bin,
                 dpackut_map_encode_bin_setup,
                 dpackut_map_teardown,
                 CUTE_DFLT_TMOUT)
{
	struct dpack_encoder_buffer enc;
	const uint8_t               blob0[] = "\x00\x01\x03";
	const uint8_t               blob1[] = "\xff\xfe\xfd";

	dpack_encoder_init_buffer(&enc,
	                          dpackut_map_buff,
	                          DPACKUT_MAP_BIN_PACK_SIZE_MAX);

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_BIN_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_bin(&enc.base,
	                                     2,
	                                     blob0,
	                                     sizeof(blob0) - 1),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_bin(&enc.base,
	                                     0,
	                                     blob1,
	                                     sizeof(blob1) - 1),
	                equal,
	                0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(dpackut_map_buff,
	               equal,
	               DPACKUT_MAP_BIN_PACK_DATA,
	               DPACKUT_MAP_BIN_PACK_SIZE);
}

#else  /* !defined(CONFIG_DPACK_BIN) */

CUTE_TEST(dpackut_map_encode_bin)
{
	cute_skip("MessagePack bin support not compiled-in");
}

#endif /* defined(CONFIG_DPACK_BIN) */

#if defined(CONFIG_DPACK_SCALAR) && \
    defined(CONFIG_DPACK_DOUBLE) && \
    defined(CONFIG_DPACK_STRING) && \
    defined(CONFIG_DPACK_BIN)

/*
 * dpack/test/dpack-utest-gen.py "{ 0: True,
 *                                  5: 1.005,
 *                                  1: 'test',
 *                                  3: -32768,
 *                                  2: b'\x00\x01\x03',
 *                                  9: 4294967295,
 *                                  6: None }"
 */
#define DPACKUT_MAP_MULTI_FLD_NR \
	(7U)
#define DPACKUT_MAP_MULTI_DATA_SIZE \
	(DPACK_MAP_BOOL_SIZE_MAX + \
	 DPACK_MAP_DOUBLE_SIZE_MAX + \
	 DPACK_MAP_STR_SIZE(4) + \
	 DPACK_MAP_INT16_SIZE_MAX + \
	 DPACK_MAP_BIN_SIZE(3) + \
	 DPACK_MAP_UINT32_SIZE_MAX + \
	 DPACK_MAP_NIL_SIZE_MAX)
#define DPACKUT_MAP_MULTI_PACK_DATA \
	"\x87" \
	"\x00\xc3" \
	"\05\xcb\x3f\xf0\x14\x7a\xe1\x47\xae\x14" \
	"\x01\xa4\x74\x65\x73\x74" \
	"\x03\xd1\x80\x00" \
	"\x02\xc4\x03\x00\x01\x03" \
	"\x09\xce\xff\xff\xff\xff" \
	"\x06\xc0"
#define DPACKUT_MAP_MULTI_PACK_SIZE \
	(sizeof(DPACKUT_MAP_MULTI_PACK_DATA) - 1)
#define DPACKUT_MAP_MULTI_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_MULTI_FLD_NR, DPACKUT_MAP_MULTI_DATA_SIZE)

CUTE_TEST(dpackut_map_encode_multi)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_MULTI_PACK_SIZE_MAX] = { 0, };
	const uint8_t               blob[] = "\x00\x01\x03";

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_map_begin_encode(&enc.base,
	                                       DPACKUT_MAP_MULTI_FLD_NR),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_bool(&enc.base, 0, true), equal, 0);
	cute_check_sint(dpack_map_encode_double(&enc.base, 5, 1.005), equal, 0);
	cute_check_sint(dpack_map_encode_str(&enc.base, 1, "test"), equal, 0);
	cute_check_sint(dpack_map_encode_int16(&enc.base, 3, INT16_MIN),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_bin(&enc.base,
	                                     2,
	                                     blob,
	                                     sizeof(blob) - 1),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_uint32(&enc.base, 9, UINT32_MAX),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_nil(&enc.base, 6), equal, 0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_MULTI_PACK_DATA,
	               DPACKUT_MAP_MULTI_PACK_SIZE);
}

#else  /* !(defined(CONFIG_DPACK_SCALAR) && \
            defined(CONFIG_DPACK_DOUBLE) && \
            defined(CONFIG_DPACK_STRING) && \
            defined(CONFIG_DPACK_BIN)) */

CUTE_TEST(dpackut_map_encode_multi)
{
	cute_skip("MessagePack multi map test not compiled-in");
}

#endif /* defined(CONFIG_DPACK_SCALAR) && \
          defined(CONFIG_DPACK_DOUBLE) && \
          defined(CONFIG_DPACK_STRING) && \
          defined(CONFIG_DPACK_BIN) */

#if defined(CONFIG_DPACK_SCALAR) && \
    defined(CONFIG_DPACK_DOUBLE) && \
    defined(CONFIG_DPACK_STRING)

/* dpack-utest-gen.py '{ 3: True, 5: { 0: -32768, 1: 1.005 }, 0: "test" }' */
#define DPACKUT_MAP_NEST_LVL0_FLD_NR (3U)
#define DPACKUT_MAP_NEST_LVL1_FLD_NR (2U)
#define DPACKUT_MAP_NEST_PACK_DATA \
	"\x83" \
		"\x03\xc3" \
		"\x05\x82" \
			"\x00\xd1\x80\x00" \
			"\x01\xcb\x3f\xf0\x14\x7a\xe1\x47\xae\x14" \
		"\x00\xa4\x74\x65\x73\x74"
#define DPACKUT_MAP_NEST_PACK_SIZE \
	(sizeof(DPACKUT_MAP_NEST_PACK_DATA) - 1)
#define DPACKUT_MAP_NEST_LVL1_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_NEST_LVL1_FLD_NR, \
	               DPACK_MAP_INT16_SIZE_MAX + DPACK_MAP_DOUBLE_SIZE_MAX)
#define DPACKUT_MAP_NEST_PACK_SIZE_MAX \
	DPACK_MAP_SIZE(DPACKUT_MAP_NEST_LVL0_FLD_NR, \
	               DPACK_MAP_BOOL_SIZE_MAX + \
	               DPACK_MAP_NEST_SIZE_MAX(DPACKUT_MAP_NEST_LVL1_SIZE_MAX) + \
	               DPACK_MAP_STR_SIZE(4))

CUTE_TEST(dpackut_map_encode_nest)
{
	struct dpack_encoder_buffer enc;
	uint8_t                     buff[DPACKUT_MAP_NEST_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(
		dpack_map_begin_encode(&enc.base, DPACKUT_MAP_NEST_LVL0_FLD_NR),
		equal,
		0);
	cute_check_sint(dpack_map_encode_bool(&enc.base, 3, true), equal, 0);
	cute_check_sint(
		dpack_map_begin_encode_nest_map(&enc.base,
		                                5,
		                                DPACKUT_MAP_NEST_LVL1_FLD_NR),
		equal,
		0);
	cute_check_sint(dpack_map_encode_int16(&enc.base, 0, INT16_MIN),
	                equal,
	                0);
	cute_check_sint(dpack_map_encode_double(&enc.base, 1, 1.005), equal, 0);
	dpack_map_end_encode(&enc.base);
	cute_check_sint(dpack_map_encode_str(&enc.base, 0, "test"), equal, 0);
	dpack_map_end_encode(&enc.base);

	dpack_encoder_fini(&enc.base);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_MAP_NEST_PACK_DATA,
	               DPACKUT_MAP_NEST_PACK_SIZE);
}

#else  /* !(defined(CONFIG_DPACK_SCALAR) && \
            defined(CONFIG_DPACK_DOUBLE) && \
            defined(CONFIG_DPACK_STRING)) */

CUTE_TEST(dpackut_map_encode_nest)
{
	cute_skip("MessagePack nested map test not compiled-in");
}

#endif /* defined(CONFIG_DPACK_SCALAR) && \
          defined(CONFIG_DPACK_DOUBLE) && \
          defined(CONFIG_DPACK_STRING) */

CUTE_GROUP(dpackut_map_group) = {
	CUTE_REF(dpackut_fixmap_sizes),
	CUTE_REF(dpackut_map16_sizes),
	CUTE_REF(dpackut_map32_sizes),
	CUTE_REF(dpackut_map_size_assert),

	CUTE_REF(dpackut_map_encode_begin_null_enc),
	CUTE_REF(dpackut_map_encode_begin_zero),
	CUTE_REF(dpackut_map_encode_begin_huge),
	CUTE_REF(dpackut_map_encode_end_null_enc),
	CUTE_REF(dpackut_map_encode_begin_uninit_enc),
	CUTE_REF(dpackut_map_encode_end_uninit_enc),

	CUTE_REF(dpackut_map_encode_bool),
	CUTE_REF(dpackut_map_encode_int8),
	CUTE_REF(dpackut_map_encode_uint8),
	CUTE_REF(dpackut_map_encode_int16),
	CUTE_REF(dpackut_map_encode_uint16),
	CUTE_REF(dpackut_map_encode_int32),
	CUTE_REF(dpackut_map_encode_uint32),
	CUTE_REF(dpackut_map_encode_int64),
	CUTE_REF(dpackut_map_encode_uint64),
	CUTE_REF(dpackut_map_encode_float),
	CUTE_REF(dpackut_map_encode_double),
	CUTE_REF(dpackut_map_encode_str),
	CUTE_REF(dpackut_map_encode_lvstr),
	CUTE_REF(dpackut_map_encode_bin),
	CUTE_REF(dpackut_map_encode_multi),
	CUTE_REF(dpackut_map_encode_nest),
};

CUTE_SUITE_EXTERN(dpackut_map_suite,
                  dpackut_map_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
