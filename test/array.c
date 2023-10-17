/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/array.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <math.h>
#include <errno.h>

#define DPACKUT_ARRAY_ELMSIZE \
	(1U)

#define DPACKUT_ARRAY_FIXED_SIZE(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACKUT_ARRAY_ELMSIZE)

#define DPACKUT_ARRAY_ENABLED(_elm_nr) \
	((DPACK_ARRAY_ELMNR_MAX >= (_elm_nr)) && \
	 (((_elm_nr) * DPACKUT_ARRAY_ELMSIZE) <= _DPACK_ARRAY_DATA_SIZE_MAX))

CUTE_TEST(dpackut_fixarray_sizes)
{
	/* Msgpack fixarray sizes. */
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(1),
	                equal,
	                1 + (1 * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(1, DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                1 + (1 * DPACKUT_ARRAY_ELMSIZE));

	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(2),
	                equal,
	                1 + (2 * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(2, DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                1 + (2 * DPACKUT_ARRAY_ELMSIZE));

#if DPACKUT_ARRAY_ENABLED(14)
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(14),
	                equal,
	                1 + (14 * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(14, DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                1 + (14 * DPACKUT_ARRAY_ELMSIZE));
#endif

#if DPACKUT_ARRAY_ENABLED(15)
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(15),
	                equal,
	                1 + (15 * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(15, DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                1 + (15 * DPACKUT_ARRAY_ELMSIZE));
#endif
}

#if DPACK_ARRAY_ELMNR_MAX > _DPACK_FIXARRAY_ELMNR_MAX

CUTE_TEST(dpackut_array16_sizes)
{
	/* Msgpack fixarray sizes. */
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(16),
	                equal,
	                3U + (16 * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(16, DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                3U + (16 * DPACKUT_ARRAY_ELMSIZE));

#if DPACKUT_ARRAY_ENABLED(17)
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(17),
	                equal,
	                3U + (17 * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(17, DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                3U + (17 * DPACKUT_ARRAY_ELMSIZE));
#endif

#if DPACKUT_ARRAY_ENABLED(UINT16_MAX - 2)
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(UINT16_MAX - 2),
	                equal,
	                3U + ((UINT16_MAX - 2) * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(UINT16_MAX - 2,
	                                       DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                3U + ((UINT16_MAX - 2) * DPACKUT_ARRAY_ELMSIZE));
#endif

#if DPACKUT_ARRAY_ENABLED(UINT16_MAX - 1)
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(UINT16_MAX - 1),
	                equal,
	                3U + ((UINT16_MAX - 1) * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(UINT16_MAX - 1,
	                                       DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                3U + ((UINT16_MAX - 1) * DPACKUT_ARRAY_ELMSIZE));
#endif

#if DPACKUT_ARRAY_ENABLED(UINT16_MAX)
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(UINT16_MAX),
	                equal,
	                3U + (UINT16_MAX * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(UINT16_MAX,
	                                       DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                3U + (UINT16_MAX * DPACKUT_ARRAY_ELMSIZE));
#endif
}

#else   /* !(DPACK_ARRAY_ELMNR_MAX > _DPACK_FIXARRAY_ELMNR_MAX) */

CUTE_TEST(dpackut_array16_sizes)
{
	cute_skip("16-bits MessagePack array support not compiled-in");
}

#endif  /* DPACK_ARRAY_ELMNR_MAX > _DPACK_FIXARRAY_ELMNR_MAX */

#if DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY16_ELMNR_MAX

CUTE_TEST(dpackut_array32_sizes)
{
	/* Msgpack fixarray sizes. */
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(UINT16_MAX + 1),
	                equal,
	                5U + ((UINT16_MAX + 1) * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(UINT16_MAX + 1,
	                                       DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                5U + ((UINT16_MAX + 1) * DPACKUT_ARRAY_ELMSIZE));

#if DPACKUT_ARRAY_ENABLED(UINT16_MAX + 2)
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(UINT16_MAX + 2),
	                equal,
	                5U + ((UINT16_MAX + 2) * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(UINT16_MAX + 2,
	                                       DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                5U + ((UINT16_MAX + 2) * DPACKUT_ARRAY_ELMSIZE));
#endif

#if DPACKUT_ARRAY_ENABLED(UINT32_MAX - 2)
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(UINT32_MAX - 2),
	                equal,
	                5U + ((UINT32_MAX - 2) * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(UINT32_MAX - 2,
	                                       DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                5U + ((UINT32_MAX - 2) * DPACKUT_ARRAY_ELMSIZE));
#endif

#if DPACKUT_ARRAY_ENABLED(UINT32_MAX - 1)
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(UINT32_MAX - 1),
	                equal,
	                5U + ((UINT32_MAX - 1) * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(UINT32_MAX - 1,
	                                       DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                5U + ((UINT32_MAX - 1) * DPACKUT_ARRAY_ELMSIZE));
#endif
#if DPACKUT_ARRAY_ENABLED(UINT32_MAX)
	cute_check_uint(DPACKUT_ARRAY_FIXED_SIZE(UINT32_MAX),
	                equal,
	                5U + (UINT32_MAX * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_fixed_size(UINT32_MAX,
	                                       DPACKUT_ARRAY_ELMSIZE),
	                equal,
	                5U + (UINT32_MAX * DPACKUT_ARRAY_ELMSIZE));
#endif
}

#else   /* !(DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY16_ELMNR_MAX) */

CUTE_TEST(dpackut_array32_sizes)
{
	cute_skip("32-bits MessagePack array support not compiled-in");
}

#endif  /* DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY16_ELMNR_MAX */

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_array_fixed_size_assert)
{
	size_t ret __unused;

	cute_expect_assertion(ret = dpack_array_fixed_size(0, 1));

	cute_expect_assertion(
		ret = dpack_array_fixed_size(DPACK_ARRAY_ELMNR_MAX + 1, 1));

	ret = dpack_array_fixed_size(1, _DPACK_ARRAY_DATA_SIZE_MAX);
	cute_expect_assertion(
		ret = dpack_array_fixed_size(1,
		                             _DPACK_ARRAY_DATA_SIZE_MAX + 1));
}

CUTE_TEST(dpackut_array_encode_begin_null_enc)
{
	struct dpack_encoder enc;
	char                 buff;
	int                  ret __unused;

	dpack_encoder_init_buffer(&enc, &buff, 1);
	cute_expect_assertion(ret = dpack_array_begin_encode(NULL, 1));
	dpack_encoder_fini(&enc, DPACK_ABORT);
}


CUTE_TEST(dpackut_array_encode_begin_zero)
{
	struct dpack_encoder enc;
	char                 buff;
	int                  ret __unused;

	dpack_encoder_init_buffer(&enc, &buff, 1);
	cute_expect_assertion(ret = dpack_array_begin_encode(&enc, 0));
	dpack_encoder_fini(&enc, DPACK_ABORT);
}

CUTE_TEST(dpackut_array_encode_begin_huge)
{
	struct dpack_encoder enc;
	char                 buff;
	int                  ret __unused;

	dpack_encoder_init_buffer(&enc, &buff, 1);
	cute_expect_assertion(
		ret = dpack_array_begin_encode(&enc,
		                               DPACK_ARRAY_ELMNR_MAX + 1));
	dpack_encoder_fini(&enc, DPACK_ABORT);
}

CUTE_TEST(dpackut_array_encode_end_null_enc)
{
	struct dpack_encoder enc;
	char                 buff;

	dpack_encoder_init_buffer(&enc, &buff, 1);
	cute_expect_assertion(dpack_array_end_encode(NULL));
	dpack_encoder_fini(&enc, DPACK_ABORT);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_array_fixed_size_assert)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_array_encode_begin_null_enc)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_array_encode_begin_zero)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_array_encode_begin_huge)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_array_encode_end_null_enc)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_DEBUG)

CUTE_TEST(dpackut_array_encode_begin_uninit_enc)
{
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_array_begin_encode(&enc, 1));
}

CUTE_TEST(dpackut_array_encode_end_uninit_enc)
{
	struct dpack_encoder enc = { 0, };

	cute_expect_assertion(dpack_array_end_encode(&enc));
}

CUTE_TEST(dpackut_array_encode_end_empty)
{
	struct dpack_encoder enc;
	char                 buff[DPACK_ARRAY_BOOL_SIZE(1)] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc, 1), equal, 0);
	cute_expect_assertion(dpack_array_end_encode(&enc));

	dpack_encoder_fini(&enc, DPACK_ABORT);
}

CUTE_TEST(dpackut_array_encode_end_partial)
{
	struct dpack_encoder enc;
	char                 buff[DPACK_ARRAY_BOOL_SIZE(2)] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc, 2), equal, 0);
	cute_check_sint(dpack_encode_bool(&enc, true), equal, 0);
	cute_expect_assertion(dpack_array_end_encode(&enc));

	dpack_encoder_fini(&enc, DPACK_ABORT);
}

CUTE_TEST(dpackut_array_encode_xcess)
{
	struct dpack_encoder enc;
	char                 buff[DPACK_ARRAY_INT8_SIZE_MAX(2)] = { 0, };
	int                  ret __unused;

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc, 1), equal, 0);
	cute_check_sint(dpack_encode_int8(&enc, INT8_MIN), equal, 0);
	cute_expect_assertion(ret = dpack_encode_int8(&enc, 0));

	dpack_encoder_fini(&enc, DPACK_ABORT);
}

#else /* !defined(CONFIG_DPACK_DEBUG) */

CUTE_TEST(dpackut_array_encode_begin_uninit_enc)
{
	cute_skip("debug build disabled");
}

CUTE_TEST(dpackut_array_encode_end_uninit_enc)
{
	cute_skip("debug build disabled");
}

CUTE_TEST(dpackut_array_encode_end_empty)
{
	cute_skip("debug build disabled");
}

CUTE_TEST(dpackut_array_encode_end_partial)
{
	cute_skip("debug build disabled");
}

CUTE_TEST(dpackut_array_encode_xcess)
{
	cute_skip("debug build disabled");
}

#endif /* defined(CONFIG_DPACK_DEBUG) */

CUTE_TEST(dpackut_array_encode_begin_msgsize)
{
	struct dpack_encoder enc;
	char                 buff[1] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc, 512), equal, -EMSGSIZE);

	dpack_encoder_fini(&enc, DPACK_ABORT);
}

CUTE_TEST(dpackut_array_encode_goon_msgsize)
{
	struct dpack_encoder enc;
	char                 buff[3] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc, 2), equal, 0);
	cute_check_sint(dpack_encode_int8(&enc, INT8_MIN), equal, 0);
	cute_check_sint(dpack_encode_int8(&enc, 0), equal, -EMSGSIZE);

	dpack_encoder_fini(&enc, DPACK_ABORT);
}

/* dpack-utest-gen.py "[False,True]" */
#define DPACKUT_ARRAY_BOOL_ELM_NR \
	(2U)
#define DPACKUT_ARRAY_BOOL_PACK_DATA \
	"\x92\xc2\xc3"
#define DPACKUT_ARRAY_BOOL_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_BOOL_PACK_DATA) - 1)
#define DPACKUT_ARRAY_BOOL_PACK_SIZE_MAX \
	DPACK_ARRAY_BOOL_SIZE(DPACKUT_ARRAY_BOOL_ELM_NR)

CUTE_TEST(dpackut_array_encode_bool)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_BOOL_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_BOOL_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_bool(&enc, false), equal, 0);
	cute_check_sint(dpack_encode_bool(&enc, true), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_BOOL_PACK_DATA,
	               DPACKUT_ARRAY_BOOL_PACK_SIZE);
}

/* dpack-utest-gen.py "[-128,0,127]" */
#define DPACKUT_ARRAY_INT8_ELM_NR \
	(3U)
#define DPACKUT_ARRAY_INT8_PACK_DATA \
	"\x93\xd0\x80\x00\x7f"
#define DPACKUT_ARRAY_INT8_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_INT8_PACK_DATA) - 1)
#define DPACKUT_ARRAY_INT8_PACK_SIZE_MAX \
	DPACK_ARRAY_INT8_SIZE_MAX(DPACKUT_ARRAY_INT8_ELM_NR)

CUTE_TEST(dpackut_array_encode_int8)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_INT8_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_INT8_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_int8(&enc, INT8_MIN), equal, 0);
	cute_check_sint(dpack_encode_int8(&enc, 0), equal, 0);
	cute_check_sint(dpack_encode_int8(&enc, INT8_MAX), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_INT8_PACK_DATA,
	               DPACKUT_ARRAY_INT8_PACK_SIZE);
}

/* dpack-utest-gen.py "[0,255]" */
#define DPACKUT_ARRAY_UINT8_ELM_NR \
	(2U)
#define DPACKUT_ARRAY_UINT8_PACK_DATA \
	"\x92\x00\xcc\xff"
#define DPACKUT_ARRAY_UINT8_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_UINT8_PACK_DATA) - 1)
#define DPACKUT_ARRAY_UINT8_PACK_SIZE_MAX \
	DPACK_ARRAY_UINT8_SIZE_MAX(DPACKUT_ARRAY_UINT8_ELM_NR)

CUTE_TEST(dpackut_array_encode_uint8)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_UINT8_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_UINT8_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_uint8(&enc, 0), equal, 0);
	cute_check_sint(dpack_encode_uint8(&enc, UINT8_MAX), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_UINT8_PACK_DATA,
	               DPACKUT_ARRAY_UINT8_PACK_SIZE);
}

/* dpack-utest-gen.py "[-32768,0,32767]"*/
#define DPACKUT_ARRAY_INT16_ELM_NR \
	(3U)
#define DPACKUT_ARRAY_INT16_PACK_DATA \
	"\x93\xd1\x80\x00\x00\xcd\x7f\xff"
#define DPACKUT_ARRAY_INT16_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_INT16_PACK_DATA) - 1)
#define DPACKUT_ARRAY_INT16_PACK_SIZE_MAX \
	DPACK_ARRAY_INT16_SIZE_MAX(DPACKUT_ARRAY_INT16_ELM_NR)

CUTE_TEST(dpackut_array_encode_int16)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_INT16_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_INT16_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_int16(&enc, INT16_MIN), equal, 0);
	cute_check_sint(dpack_encode_int16(&enc, 0), equal, 0);
	cute_check_sint(dpack_encode_int16(&enc, INT16_MAX), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_INT16_PACK_DATA,
	               DPACKUT_ARRAY_INT16_PACK_SIZE);
}

/* dpack-utest-gen.py "[0,255]" */
#define DPACKUT_ARRAY_UINT16_ELM_NR \
	(2U)
#define DPACKUT_ARRAY_UINT16_PACK_DATA \
	"\x92\x00\xcd\xff\xff"
#define DPACKUT_ARRAY_UINT16_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_UINT16_PACK_DATA) - 1)
#define DPACKUT_ARRAY_UINT16_PACK_SIZE_MAX \
	DPACK_ARRAY_UINT16_SIZE_MAX(DPACKUT_ARRAY_UINT16_ELM_NR)

CUTE_TEST(dpackut_array_encode_uint16)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_UINT16_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_UINT16_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_uint16(&enc, 0), equal, 0);
	cute_check_sint(dpack_encode_uint16(&enc, UINT16_MAX), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_UINT16_PACK_DATA,
	               DPACKUT_ARRAY_UINT16_PACK_SIZE);
}

/* dpack-utest-gen.py "[-2147483648,0,2147483647]" */
#define DPACKUT_ARRAY_INT32_ELM_NR \
	(3U)
#define DPACKUT_ARRAY_INT32_PACK_DATA \
	"\x93\xd2\x80\x00\x00\x00\x00\xce\x7f\xff\xff\xff"
#define DPACKUT_ARRAY_INT32_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_INT32_PACK_DATA) - 1)
#define DPACKUT_ARRAY_INT32_PACK_SIZE_MAX \
	DPACK_ARRAY_INT32_SIZE_MAX(DPACKUT_ARRAY_INT32_ELM_NR)

CUTE_TEST(dpackut_array_encode_int32)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_INT32_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_INT32_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_int32(&enc, INT32_MIN), equal, 0);
	cute_check_sint(dpack_encode_int32(&enc, 0), equal, 0);
	cute_check_sint(dpack_encode_int32(&enc, INT32_MAX), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_INT32_PACK_DATA,
	               DPACKUT_ARRAY_INT32_PACK_SIZE);
}

/* dpack-utest-gen.py "[0,4294967295]" */
#define DPACKUT_ARRAY_UINT32_ELM_NR \
	(2U)
#define DPACKUT_ARRAY_UINT32_PACK_DATA \
	"\x92\x00\xce\xff\xff\xff\xff"
#define DPACKUT_ARRAY_UINT32_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_UINT32_PACK_DATA) - 1)
#define DPACKUT_ARRAY_UINT32_PACK_SIZE_MAX \
	DPACK_ARRAY_UINT32_SIZE_MAX(DPACKUT_ARRAY_UINT32_ELM_NR)

CUTE_TEST(dpackut_array_encode_uint32)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_UINT32_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_UINT32_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_uint32(&enc, 0), equal, 0);
	cute_check_sint(dpack_encode_uint32(&enc, UINT32_MAX), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_UINT32_PACK_DATA,
	               DPACKUT_ARRAY_UINT32_PACK_SIZE);
}

/* dpack-utest-gen.py "[-9223372036854775808,0,9223372036854775807]" */
#define DPACKUT_ARRAY_INT64_ELM_NR \
	(3U)
#define DPACKUT_ARRAY_INT64_PACK_DATA \
	"\x93" \
	"\xd3\x80\x00\x00\x00\x00\x00\x00\x00" \
	"\x00" \
	"\xcf\x7f\xff\xff\xff\xff\xff\xff\xff"
#define DPACKUT_ARRAY_INT64_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_INT64_PACK_DATA) - 1)
#define DPACKUT_ARRAY_INT64_PACK_SIZE_MAX \
	DPACK_ARRAY_INT64_SIZE_MAX(DPACKUT_ARRAY_INT64_ELM_NR)

CUTE_TEST(dpackut_array_encode_int64)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_INT64_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_INT64_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_int64(&enc, INT64_MIN), equal, 0);
	cute_check_sint(dpack_encode_int64(&enc, 0), equal, 0);
	cute_check_sint(dpack_encode_int64(&enc, INT64_MAX), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_INT64_PACK_DATA,
	               DPACKUT_ARRAY_INT64_PACK_SIZE);
}

/* dpack-utest-gen.py "[0,18446744073709551615]" */
#define DPACKUT_ARRAY_UINT64_ELM_NR \
	(2U)
#define DPACKUT_ARRAY_UINT64_PACK_DATA \
	"\x92\x00\xcf\xff\xff\xff\xff\xff\xff\xff\xff"
#define DPACKUT_ARRAY_UINT64_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_UINT64_PACK_DATA) - 1)
#define DPACKUT_ARRAY_UINT64_PACK_SIZE_MAX \
	DPACK_ARRAY_UINT64_SIZE_MAX(DPACKUT_ARRAY_UINT64_ELM_NR)

CUTE_TEST(dpackut_array_encode_uint64)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_UINT64_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_UINT64_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_uint64(&enc, 0), equal, 0);
	cute_check_sint(dpack_encode_uint64(&enc, UINT64_MAX), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_UINT64_PACK_DATA,
	               DPACKUT_ARRAY_UINT64_PACK_SIZE);
}

#if defined(CONFIG_DPACK_FLOAT)

/* dpack-utest-gen.py -s "[-1.005, 10.0]" */
#define DPACKUT_ARRAY_FLOAT_ELM_NR \
	(2U)
#define DPACKUT_ARRAY_FLOAT_PACK_DATA \
	"\x92\xca\xbf\x80\xa3\xd7\xca\x41\x20\x00\x00"
#define DPACKUT_ARRAY_FLOAT_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_FLOAT_PACK_DATA) - 1)
#define DPACKUT_ARRAY_FLOAT_PACK_SIZE_MAX \
	DPACK_ARRAY_FLOAT_SIZE(DPACKUT_ARRAY_FLOAT_ELM_NR)

CUTE_TEST(dpackut_array_encode_float)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_FLOAT_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_FLOAT_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_float(&enc, -1.005f), equal, 0);
	cute_check_sint(dpack_encode_float(&enc, 10.0f), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_FLOAT_PACK_DATA,
	               DPACKUT_ARRAY_FLOAT_PACK_SIZE);
}

#else  /* !defined(CONFIG_DPACK_FLOAT) */

CUTE_TEST(dpackut_array_encode_float)
{
	cute_skip("MessagePack float support not compiled-in");
}

#endif /* defined(CONFIG_DPACK_FLOAT) */

#if defined(CONFIG_DPACK_DOUBLE)

/* dpack-utest-gen.py "[-1.005, float('inf')]" */
#define DPACKUT_ARRAY_DOUBLE_ELM_NR \
	(2U)
#define DPACKUT_ARRAY_DOUBLE_PACK_DATA \
	"\x92" \
	"\xcb\xbf\xf0\x14\x7a\xe1\x47\xae\x14" \
	"\xcb\x7f\xf0\x00\x00\x00\x00\x00\x00"
#define DPACKUT_ARRAY_DOUBLE_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_DOUBLE_PACK_DATA) - 1)
#define DPACKUT_ARRAY_DOUBLE_PACK_SIZE_MAX \
	DPACK_ARRAY_DOUBLE_SIZE(DPACKUT_ARRAY_DOUBLE_ELM_NR)

CUTE_TEST(dpackut_array_encode_double)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_DOUBLE_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_DOUBLE_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_double(&enc, -1.005), equal, 0);
	cute_check_sint(dpack_encode_double(&enc, INFINITY), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_DOUBLE_PACK_DATA,
	               DPACKUT_ARRAY_DOUBLE_PACK_SIZE);
}

#else  /* !defined(CONFIG_DPACK_DOUBLE) */

CUTE_TEST(dpackut_array_encode_double)
{
	cute_skip("MessagePack double support not compiled-in");
}

#endif /* defined(CONFIG_DPACK_DOUBLE) */

#if defined(CONFIG_DPACK_STRING)

/* dpack-utest-gen.py '["a", "list", "of strings"]' */
#define DPACKUT_ARRAY_STR_ELM_NR \
	(3U)
#define DPACKUT_ARRAY_STR_PACK_DATA \
	"\x93" \
	"\xa1\x61" \
	"\xa4\x6c\x69\x73\x74" \
	"\xaa\x6f\x66\x20\x73\x74\x72\x69\x6e\x67\x73"
#define DPACKUT_ARRAY_STR_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_STR_PACK_DATA) - 1)
#define DPACKUT_ARRAY_STR_PACK_SIZE_MAX \
	DPACK_ARRAY_STR_SIZE_MAX(DPACKUT_ARRAY_STR_ELM_NR)

CUTE_TEST(dpackut_array_encode_str)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_STR_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_STR_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_str(&enc, "a"), equal, 0);
	cute_check_sint(dpack_encode_str(&enc, "list"), equal, 0);
	cute_check_sint(dpack_encode_str(&enc, "of strings"), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_STR_PACK_DATA,
	               DPACKUT_ARRAY_STR_PACK_SIZE);
}

#else  /* !defined(CONFIG_DPACK_STRING) */

CUTE_TEST(dpackut_array_encode_str)
{
	cute_skip("MessagePack string support not compiled-in");
}

#endif  /* !defined(CONFIG_DPACK_STRING) */

#if defined(CONFIG_ARRAY_BIN)

/* dpack/test/dpack-utest-gen.py "[b'\x00\x01\x03', b'\xff\xfe\xfd']" */
#define DPACKUT_ARRAY_BIN_ELM_NR \
	(2U)
#define DPACKUT_ARRAY_BIN_PACK_DATA \
	"\x92\xc4\x03\x00\x01\x03\xc4\x03\xff\xfe\xfd"
#define DPACKUT_ARRAY_BIN_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_BIN_PACK_DATA) - 1)
#define DPACKUT_ARRAY_BIN_PACK_SIZE_MAX \
	DPACK_ARRAY_BIN_SIZE_MAX(DPACKUT_ARRAY_BIN_ELM_NR)

CUTE_TEST(dpackut_array_encode_bin)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_BIN_PACK_SIZE_MAX] = { 0, };
	const char           blob0[] = "\x00\x01\x03";
	const char           blob1[] = "\xff\xfe\xfd";

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_BIN_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_bin(&enc, blob0, sizeof(blob0) - 1),
	                equal,
	                0);
	cute_check_sint(dpack_encode_bin(&enc, blob1, sizeof(blob1) - 1),
	                equal,
	                0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_BIN_PACK_DATA,
	               DPACKUT_ARRAY_BIN_PACK_SIZE);
}

#else  /* !defined(CONFIG_ARRAY_BIN) */

CUTE_TEST(dpackut_array_encode_bin)
{
	cute_skip("MessagePack bin support not compiled-in");
}

#endif /* defined(CONFIG_ARRAY_BIN) */


#if defined(CONFIG_DPACK_SCALAR) && \
    defined(CONFIG_DPACK_DOUBLE) && \
    defined(CONFIG_DPACK_STRING) && \
    defined(CONFIG_DPACK_BIN)

/*
 * dpack/test/dpack-utest-gen.py "[True,
 *                                 1.005,
 *                                 'test',
 *                                 -32768,
 *                                 b'\x00\x01\x03',
 *                                 4294967295,
 *                                 None]"
 */
#define DPACKUT_ARRAY_MULTI_ELM_NR \
	(7U)
#define DPACKUT_ARRAY_MULTI_DATA_SIZE \
	(DPACK_BOOL_SIZE + \
	 DPACK_DOUBLE_SIZE + \
	 DPACK_STR_SIZE(4) + \
	 DPACK_INT16_SIZE_MAX + \
	 DPACK_BIN_SIZE(3) + \
	 DPACK_UINT32_SIZE_MAX + \
	 DPACK_NIL_SIZE)
#define DPACKUT_ARRAY_MULTI_PACK_DATA \
	"\x97" \
	"\xc3" \
	"\xcb\x3f\xf0\x14\x7a\xe1\x47\xae\x14" \
	"\xa4\x74\x65\x73\x74" \
	"\xd1\x80\x00" \
	"\xc4\x03\x00\x01\x03" \
	"\xce\xff\xff\xff\xff" \
	"\xc0"
#define DPACKUT_ARRAY_MULTI_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_MULTI_PACK_DATA) - 1)
#define DPACKUT_ARRAY_MULTI_PACK_SIZE_MAX \
	DPACK_ARRAY_MIXED_SIZE(DPACKUT_ARRAY_MULTI_ELM_NR, \
	                       DPACKUT_ARRAY_MULTI_DATA_SIZE)

CUTE_TEST(dpackut_array_encode_multi)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_MULTI_PACK_SIZE_MAX] = { 0, };
	const char           blob[] = "\x00\x01\x03";

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(dpack_array_begin_encode(&enc,
	                                         DPACKUT_ARRAY_MULTI_ELM_NR),
	                equal,
	                0);
	cute_check_sint(dpack_encode_bool(&enc, true), equal, 0);
	cute_check_sint(dpack_encode_double(&enc, 1.005), equal, 0);
	cute_check_sint(dpack_encode_str(&enc, "test"), equal, 0);
	cute_check_sint(dpack_encode_int16(&enc, INT16_MIN), equal, 0);
	cute_check_sint(dpack_encode_bin(&enc, blob, sizeof(blob) - 1),
	                equal,
	                0);
	cute_check_sint(dpack_encode_uint32(&enc, UINT32_MAX), equal, 0);
	cute_check_sint(dpack_encode_nil(&enc), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_MULTI_PACK_DATA,
	               DPACKUT_ARRAY_MULTI_PACK_SIZE);
}

#else  /* !(defined(CONFIG_DPACK_SCALAR) && \
            defined(CONFIG_DPACK_DOUBLE) && \
            defined(CONFIG_DPACK_STRING) && \
            defined(CONFIG_DPACK_BIN)) */

CUTE_TEST(dpackut_array_encode_multi)
{
	cute_skip("MessagePack multi array test not compiled-in");
}

#endif /* defined(CONFIG_DPACK_SCALAR) && \
          defined(CONFIG_DPACK_DOUBLE) && \
          defined(CONFIG_DPACK_STRING) && \
          defined(CONFIG_DPACK_BIN) */

#if defined(CONFIG_DPACK_SCALAR) && \
    defined(CONFIG_DPACK_DOUBLE) && \
    defined(CONFIG_DPACK_STRING)

/* dpack-utest-gen.py '[True, [-32768, 1.005], "test"]' */
#define DPACKUT_ARRAY_NEST_LVL0_ELM_NR (3U)
#define DPACKUT_ARRAY_NEST_LVL1_ELM_NR (2U)
#define DPACKUT_ARRAY_NEST_PACK_DATA \
	"\x93" \
		"\xc3" \
		"\x92" \
			"\xd1\x80\x00" \
			"\xcb\x3f\xf0\x14\x7a\xe1\x47\xae\x14" \
		"\xa4\x74\x65\x73\x74"
#define DPACKUT_ARRAY_NEST_PACK_SIZE \
	(sizeof(DPACKUT_ARRAY_NEST_PACK_DATA) - 1)
#define DPACKUT_ARRAY_NEST_LVL1_SIZE_MAX \
	DPACK_ARRAY_MIXED_SIZE(DPACKUT_ARRAY_NEST_LVL1_ELM_NR, \
	                       DPACK_INT16_SIZE_MAX + DPACK_DOUBLE_SIZE)
#define DPACKUT_ARRAY_NEST_PACK_SIZE_MAX \
	DPACK_ARRAY_MIXED_SIZE(DPACKUT_ARRAY_NEST_LVL0_ELM_NR, \
	                       DPACK_BOOL_SIZE + \
	                       DPACKUT_ARRAY_NEST_LVL1_SIZE_MAX + \
	                       DPACK_STR_SIZE(4))

CUTE_TEST(dpackut_array_encode_nest)
{
	struct dpack_encoder enc;
	char                 buff[DPACKUT_ARRAY_NEST_PACK_SIZE_MAX] = { 0, };

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));

	cute_check_sint(
		dpack_array_begin_encode(&enc, DPACKUT_ARRAY_NEST_LVL0_ELM_NR),
		equal,
		0);
	cute_check_sint(dpack_encode_bool(&enc, true), equal, 0);
	cute_check_sint(
		dpack_array_begin_encode(&enc, DPACKUT_ARRAY_NEST_LVL1_ELM_NR),
		equal,
		0);
	cute_check_sint(dpack_encode_int16(&enc, INT16_MIN), equal, 0);
	cute_check_sint(dpack_encode_double(&enc, 1.005), equal, 0);
	dpack_array_end_encode(&enc);
	cute_check_sint(dpack_encode_str(&enc, "test"), equal, 0);
	dpack_array_end_encode(&enc);

	dpack_encoder_fini(&enc, DPACK_DONE);

	cute_check_mem(buff,
	               equal,
	               DPACKUT_ARRAY_NEST_PACK_DATA,
	               DPACKUT_ARRAY_NEST_PACK_SIZE);
}

#else  /* !(defined(CONFIG_DPACK_SCALAR) && \
            defined(CONFIG_DPACK_DOUBLE) && \
            defined(CONFIG_DPACK_STRING)) */

CUTE_TEST(dpackut_array_encode_nest)
{
	cute_skip("MessagePack nested array test not compiled-in");
}

#endif /* defined(CONFIG_DPACK_SCALAR) && \
          defined(CONFIG_DPACK_DOUBLE) && \
          defined(CONFIG_DPACK_STRING) */


#if defined(CONFIG_DPACK_ASSERT_API)

static int
dpackut_array_xtract_data(struct dpack_decoder * decoder __unused,
                          unsigned int           id __unused,
                          void * __restrict      data __unused)
{
	return 0;
}

CUTE_TEST(dpackut_array_decode_null_dec)
{
	struct dpack_decoder dec;
	char                 buff = buff;
	char                 data = data;
	int                  ret __unused;

	dpack_decoder_init_buffer(&dec, &buff, 1);
	cute_expect_assertion(
		ret = dpack_array_decode(NULL,
		                         dpackut_array_xtract_data,
		                         &data));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_array_decode_null_func)
{
	struct dpack_decoder dec;
	char                 buff = buff;
	char                 data = data;
	int                  ret __unused;

	dpack_decoder_init_buffer(&dec, &buff, 1);
	cute_expect_assertion(ret = dpack_array_decode(&dec, NULL, &data));
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_array_decode_null_data)
{
	struct dpack_decoder dec;

	cute_expect_assertion(dpack_decoder_init_buffer(&dec, NULL, 1));
}

CUTE_TEST(dpackut_array_decode_zero_data)
{
	struct dpack_decoder dec;
	char                 buff = buff;

	cute_expect_assertion(dpack_decoder_init_buffer(&dec, &buff, 0));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_array_decode_null_dec)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_array_decode_null_func)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_array_decode_null_data)
{
	cute_skip("assertion unsupported");
}

CUTE_TEST(dpackut_array_decode_zero_data)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_DEBUG)

CUTE_TEST(dpackut_array_decode_uninit_dec)
{
	struct dpack_decoder dec = { 0, };
	char                 data = data;
	int                  ret __unused;

	cute_expect_assertion(
		ret = dpack_array_decode(&dec,
		                         dpackut_array_xtract_data,
		                         &data));
}

#else /* !defined(CONFIG_DPACK_DEBUG) */

CUTE_TEST(dpackut_array_decode_uninit_dec)
{
	cute_skip("debug build disabled");
}

#endif /* defined(CONFIG_DPACK_DEBUG) */

static int
dpackut_array_xtract_some(struct dpack_decoder * decoder,
                          unsigned int           id,
                          void * __restrict      data)
{
	bool * values = (bool *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	return dpack_decode_bool(decoder, &values[id]);
}

CUTE_TEST(dpackut_array_decode_empty)
{
	struct dpack_decoder dec;
	const char           buff[] = "";

	dpack_decoder_init_buffer(&dec, buff, 1);
	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_some,
	                                   NULL),
	                equal,
	                -ENOMSG);
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_array_decode_nodata)
{
	struct dpack_decoder dec;
	const char           buff[] = "\x90";

	dpack_decoder_init_buffer(&dec, buff, 1);
	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_some,
	                                   NULL),
	                equal,
	                -ENOMSG);
	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_array_decode_starve)
{
	struct dpack_decoder dec;
	const char           buff[] = "\x91";
	bool                 value;

	dpack_decoder_init_buffer(&dec, buff, 1);

	cute_expect_uint_parm(dpackut_array_xtract_some, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_some, data, equal, &value);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_some,
	                                   &value),
	                equal,
	                -EPROTO);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	dpack_decoder_fini(&dec);
}

CUTE_TEST(dpackut_array_decode_short)
{
	struct dpack_decoder dec;
	const char           buff[] = "\x92\xc3\xc2";
	bool                 value = false;

	dpack_decoder_init_buffer(&dec, buff, 2);

	cute_expect_uint_parm(dpackut_array_xtract_some, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_some, data, equal, &value);

	cute_expect_uint_parm(dpackut_array_xtract_some, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_some, data, equal, &value);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_some,
	                                   &value),
	                equal,
	                -EPROTO);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	cute_check_bool(value, is, true);

	dpack_decoder_fini(&dec);
}

static int
dpackut_array_xtract_bool(struct dpack_decoder * decoder,
                          unsigned int           id,
                          void * __restrict      data)
{
	bool * values = (bool *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_bool(decoder, &values[id]), equal, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_bool)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_BOOL_PACK_DATA;
	bool                 values[] = { true, false };
	const bool           xpct[] = { false, true };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_bool, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_bool, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_bool, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_bool, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_bool,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++)
		cute_check_bool(values[v], is, xpct[v]);

	dpack_decoder_fini(&dec);
}

static int
dpackut_array_xtract_int8(struct dpack_decoder * decoder,
                          unsigned int           id,
                          void * __restrict      data)
{
	int8_t * values = (int8_t *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_int8(decoder, &values[id]), equal, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_int8)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_INT8_PACK_DATA;
	int8_t               values[] = { 1, 1, 1 };
	const int8_t         xpct[] = { INT8_MIN, 0, INT8_MAX };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_int8, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_int8, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_int8, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_int8, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_int8, id, equal, 2);
	cute_expect_ptr_parm(dpackut_array_xtract_int8, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_int8,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++)
		cute_check_sint(values[v], equal, xpct[v]);

	dpack_decoder_fini(&dec);
}

static int
dpackut_array_xtract_uint8(struct dpack_decoder * decoder,
                           unsigned int           id,
                           void * __restrict      data)
{
	uint8_t * values = (uint8_t *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_uint8(decoder, &values[id]), equal, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_uint8)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_UINT8_PACK_DATA;
	uint8_t              values[] = { 1, 1 };
	const uint8_t        xpct[] = { 0, UINT8_MAX };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_uint8, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_uint8, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_uint8, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_uint8, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_uint8,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++)
		cute_check_uint(values[v], equal, xpct[v]);

	dpack_decoder_fini(&dec);
}

static int
dpackut_array_xtract_int16(struct dpack_decoder * decoder,
                           unsigned int           id,
                           void * __restrict      data)
{
	int16_t * values = (int16_t *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_int16(decoder, &values[id]), equal, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_int16)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_INT16_PACK_DATA;
	int16_t              values[] = { 1, 1, 1 };
	const int16_t        xpct[] = { INT16_MIN, 0, INT16_MAX };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_int16, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_int16, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_int16, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_int16, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_int16, id, equal, 2);
	cute_expect_ptr_parm(dpackut_array_xtract_int16, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_int16,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++)
		cute_check_sint(values[v], equal, xpct[v]);

	dpack_decoder_fini(&dec);
}

static int
dpackut_array_xtract_uint16(struct dpack_decoder * decoder,
                            unsigned int           id,
                            void * __restrict      data)
{
	uint16_t * values = (uint16_t *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_uint16(decoder, &values[id]), equal, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_uint16)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_UINT16_PACK_DATA;
	uint16_t             values[] = { 1, 1 };
	const uint16_t       xpct[] = { 0, UINT16_MAX };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_uint16, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_uint16, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_uint16, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_uint16, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_uint16,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++)
		cute_check_uint(values[v], equal, xpct[v]);

	dpack_decoder_fini(&dec);
}

static int
dpackut_array_xtract_int32(struct dpack_decoder * decoder,
                           unsigned int           id,
                           void * __restrict      data)
{
	int32_t * values = (int32_t *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_int32(decoder, &values[id]), equal, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_int32)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_INT32_PACK_DATA;
	int32_t              values[] = { 1, 1, 1 };
	const int32_t        xpct[] = { INT32_MIN, 0, INT32_MAX };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_int32, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_int32, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_int32, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_int32, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_int32, id, equal, 2);
	cute_expect_ptr_parm(dpackut_array_xtract_int32, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_int32,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++)
		cute_check_sint(values[v], equal, xpct[v]);

	dpack_decoder_fini(&dec);
}

static int
dpackut_array_xtract_uint32(struct dpack_decoder * decoder,
                            unsigned int           id,
                            void * __restrict      data)
{
	uint32_t * values = (uint32_t *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_uint32(decoder, &values[id]), equal, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_uint32)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_UINT32_PACK_DATA;
	uint32_t             values[] = { 1, 1 };
	const uint32_t       xpct[] = { 0, UINT32_MAX };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_uint32, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_uint32, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_uint32, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_uint32, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_uint32,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++)
		cute_check_uint(values[v], equal, xpct[v]);

	dpack_decoder_fini(&dec);
}

static int
dpackut_array_xtract_int64(struct dpack_decoder * decoder,
                           unsigned int           id,
                           void * __restrict      data)
{
	int64_t * values = (int64_t *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_int64(decoder, &values[id]), equal, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_int64)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_INT64_PACK_DATA;
	int64_t              values[] = { 1, 1, 1 };
	const int64_t        xpct[] = { INT64_MIN, 0, INT64_MAX };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_int64, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_int64, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_int64, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_int64, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_int64, id, equal, 2);
	cute_expect_ptr_parm(dpackut_array_xtract_int64, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_int64,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++)
		cute_check_sint(values[v], equal, xpct[v]);

	dpack_decoder_fini(&dec);
}

static int
dpackut_array_xtract_uint64(struct dpack_decoder * decoder,
                            unsigned int           id,
                            void * __restrict      data)
{
	uint64_t * values = (uint64_t *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_uint64(decoder, &values[id]), equal, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_uint64)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_UINT64_PACK_DATA;
	uint64_t             values[] = { 1, 1 };
	const uint64_t       xpct[] = { 0, UINT64_MAX };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_uint64, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_uint64, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_uint64, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_uint64, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_uint64,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++)
		cute_check_uint(values[v], equal, xpct[v]);

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_FLOAT)

static int
dpackut_array_xtract_float(struct dpack_decoder * decoder,
                           unsigned int           id,
                           void * __restrict      data)
{
	float * values = (float *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_float(decoder, &values[id]), equal, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_float)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_FLOAT_PACK_DATA;
	float                values[] = { 1, 1 };
	const float          xpct[] = { -1.005f, 10.0f };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_float, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_float, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_float, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_float, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_float,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++)
		cute_check_flt(values[v], equal, xpct[v]);

	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_FLOAT) */

CUTE_TEST(dpackut_array_decode_float)
{
	cute_skip("MessagePack float support not compiled-in");
}

#endif /* defined(CONFIG_DPACK_FLOAT) */

#if defined(CONFIG_DPACK_DOUBLE)

static int
dpackut_array_xtract_double(struct dpack_decoder * decoder,
                            unsigned int           id,
                            void * __restrict      data)
{
	double * values = (double *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_double(decoder, &values[id]), equal, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_double)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_DOUBLE_PACK_DATA;
	double               values[] = { 1, 1 };
	const double         xpct[] = { -1.005, INFINITY };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_double, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_double, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_double, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_double, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_double,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++)
		cute_check_flt(values[v], equal, xpct[v]);

	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_DOUBLE) */

CUTE_TEST(dpackut_array_decode_double)
{
	cute_skip("MessagePack double support not compiled-in");
}

#endif /* defined(CONFIG_DPACK_DOUBLE) */

#if defined(CONFIG_DPACK_STRING)

static int
dpackut_array_xtract_str(struct dpack_decoder * decoder,
                         unsigned int           id,
                         void * __restrict      data)
{
	char ** values = (char **)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_strdup(decoder, &values[id]), greater, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_str)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_STR_PACK_DATA;
	char *               values[] = { NULL, NULL, NULL };
	const char *         xpct[] = { "a", "list", "of strings" };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_str, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_str, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_str, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_str, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_str, id, equal, 2);
	cute_expect_ptr_parm(dpackut_array_xtract_str, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_str,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++) {
		cute_check_str(values[v], equal, xpct[v]);
		free(values[v]);
	}

	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_STRING) */

CUTE_TEST(dpackut_array_decode_str)
{
	cute_skip("MessagePack string support not compiled-in");
}

#endif  /* !defined(CONFIG_DPACK_STRING) */

#if defined(CONFIG_ARRAY_BIN)

static int
dpackut_array_xtract_bin(struct dpack_decoder * decoder,
                         unsigned int           id,
                         void * __restrict      data)
{
	char ** values = (char **)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	cute_check_sint(dpack_decode_bindup(decoder, &values[id]), greater, 0);

	return 0;
}

CUTE_TEST(dpackut_array_decode_bin)
{
	struct dpack_decoder dec = { 0, };
	const char           buff[] = DPACKUT_ARRAY_BIN_PACK_DATA;
	char *               values[] = { NULL, NULL, NULL };
	const char *         xpct[] = { "\x00\x01\x03", "\xff\xfe\xfd" };
	unsigned int         v;

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	cute_expect_uint_parm(dpackut_array_xtract_bin, id, equal, 0);
	cute_expect_ptr_parm(dpackut_array_xtract_bin, data, equal, values);

	cute_expect_uint_parm(dpackut_array_xtract_bin, id, equal, 1);
	cute_expect_ptr_parm(dpackut_array_xtract_bin, data, equal, values);

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_bin,
	                                   values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	for (v = 0; v < array_nr(xpct); v++) {
		cute_check_mem(values[v], equal, xpct[v], 3);
		free(values[v]);
	}

	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_ARRAY_BIN) */

CUTE_TEST(dpackut_array_decode_bin)
{
	cute_skip("MessagePack bin support not compiled-in");
}

#endif /* defined(CONFIG_ARRAY_BIN) */

#if defined(CONFIG_DPACK_SCALAR) && \
    defined(CONFIG_DPACK_DOUBLE) && \
    defined(CONFIG_DPACK_STRING) && \
    defined(CONFIG_DPACK_BIN)

struct dpackut_array_decode_multi_data {
	bool     abool;
	double   adbl;
	char *   astr;
	int16_t  ashrt;
	char *   abin;
	uint32_t aword;
	void *   aptr;
};

static int
dpackut_array_xtract_multi(struct dpack_decoder * decoder,
                           unsigned int           id,
                           void * __restrict      data)
{
	struct dpackut_array_decode_multi_data * values =
		(struct dpackut_array_decode_multi_data *)data;

	cute_mock_uint_parm(id);
	cute_mock_ptr_parm(data);

	switch (id) {
	case 0:
		cute_check_sint(dpack_decode_bool(decoder, &values->abool),
		                equal,
		                0);
		break;
	case 1:
		cute_check_sint(dpack_decode_double(decoder, &values->adbl),
		                equal,
		                0);
		break;
	case 2:
		cute_check_sint(dpack_decode_strdup(decoder, &values->astr),
		                equal,
		                4);
		break;
	case 3:
		cute_check_sint(dpack_decode_int16(decoder, &values->ashrt),
		                equal,
		                0);
		break;
	case 4:
		cute_check_sint(dpack_decode_bindup(decoder, &values->abin),
		                equal,
		                3);
		break;
	case 5:
		cute_check_sint(dpack_decode_uint32(decoder, &values->aword),
		                equal,
		                0);
		break;
	case 6:
		cute_check_sint(dpack_decode_nil(decoder), equal, 0);
		values->aptr = NULL;
		break;
	default:
		cute_fail("invalid array index");
	}

	return 0;
}

CUTE_TEST(dpackut_array_decode_multi)
{
	struct dpack_decoder                   dec = { 0, };
	const char                             buff[] = \
		DPACKUT_ARRAY_MULTI_PACK_DATA;
	const char                             blob[]  = "\x00\x01\x03";
	unsigned int                           v;
	struct dpackut_array_decode_multi_data values = {
		.abool = false,
		.adbl  = 0,
		.astr  = "",
		.ashrt = 0,
		.abin  = NULL,
		.aword = 0,
		.aptr  = (void *)0xdeadbeef
	};

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff) - 1);

	for (v = 0; v < 7; v++) {
		cute_expect_uint_parm(dpackut_array_xtract_multi, id, equal, v);
		cute_expect_ptr_parm(dpackut_array_xtract_multi,
		                     data,
		                     equal,
		                     &values);
	}

	cute_check_sint(dpack_array_decode(&dec,
	                                   dpackut_array_xtract_multi,
	                                   &values),
	                equal,
	                0);
	cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);

	cute_check_bool(values.abool, is, true);
	cute_check_flt(values.adbl, equal, 1.005);
	cute_check_str(values.astr, equal, "test");
	free(values.astr);
	cute_check_sint(values.ashrt, equal, INT16_MIN);
	cute_check_mem(values.abin, equal, blob, sizeof(blob) - 1);
	free(values.abin);
	cute_check_uint(values.aword, equal, UINT32_MAX);
	cute_check_ptr(values.aptr, equal, NULL);

	dpack_decoder_fini(&dec);
}

#else  /* !(defined(CONFIG_DPACK_SCALAR) && \
            defined(CONFIG_DPACK_DOUBLE) && \
            defined(CONFIG_DPACK_STRING) && \
            defined(CONFIG_DPACK_BIN)) */

CUTE_TEST(dpackut_array_decode_multi)
{
	cute_skip("MessagePack multi array test not compiled-in");
}

#endif /* defined(CONFIG_DPACK_SCALAR) && \
          defined(CONFIG_DPACK_DOUBLE) && \
          defined(CONFIG_DPACK_STRING) && \
          defined(CONFIG_DPACK_BIN) */

CUTE_GROUP(dpackut_array_group) = {
	CUTE_REF(dpackut_fixarray_sizes),
	CUTE_REF(dpackut_array16_sizes),
	CUTE_REF(dpackut_array32_sizes),
	CUTE_REF(dpackut_array_fixed_size_assert),

	CUTE_REF(dpackut_array_encode_begin_null_enc),
	CUTE_REF(dpackut_array_encode_begin_zero),
	CUTE_REF(dpackut_array_encode_begin_huge),
	CUTE_REF(dpackut_array_encode_begin_uninit_enc),
	CUTE_REF(dpackut_array_encode_end_null_enc),
	CUTE_REF(dpackut_array_encode_end_uninit_enc),
	CUTE_REF(dpackut_array_encode_end_empty),
	CUTE_REF(dpackut_array_encode_end_partial),
	CUTE_REF(dpackut_array_encode_begin_msgsize),
	CUTE_REF(dpackut_array_encode_goon_msgsize),
	CUTE_REF(dpackut_array_encode_xcess),

	CUTE_REF(dpackut_array_encode_bool),
	CUTE_REF(dpackut_array_encode_int8),
	CUTE_REF(dpackut_array_encode_uint8),
	CUTE_REF(dpackut_array_encode_int16),
	CUTE_REF(dpackut_array_encode_uint16),
	CUTE_REF(dpackut_array_encode_int32),
	CUTE_REF(dpackut_array_encode_uint32),
	CUTE_REF(dpackut_array_encode_int64),
	CUTE_REF(dpackut_array_encode_uint64),
	CUTE_REF(dpackut_array_encode_float),
	CUTE_REF(dpackut_array_encode_double),
	CUTE_REF(dpackut_array_encode_str),
	CUTE_REF(dpackut_array_encode_bin),
	CUTE_REF(dpackut_array_encode_multi),
	CUTE_REF(dpackut_array_encode_nest),

	CUTE_REF(dpackut_array_decode_null_dec),
	CUTE_REF(dpackut_array_decode_null_func),
	CUTE_REF(dpackut_array_decode_null_data),
	CUTE_REF(dpackut_array_decode_zero_data),
	CUTE_REF(dpackut_array_decode_uninit_dec),
	CUTE_REF(dpackut_array_decode_empty),
	CUTE_REF(dpackut_array_decode_nodata),
	CUTE_REF(dpackut_array_decode_starve),
	CUTE_REF(dpackut_array_decode_short),

	CUTE_REF(dpackut_array_decode_bool),
	CUTE_REF(dpackut_array_decode_int8),
	CUTE_REF(dpackut_array_decode_uint8),
	CUTE_REF(dpackut_array_decode_int16),
	CUTE_REF(dpackut_array_decode_uint16),
	CUTE_REF(dpackut_array_decode_int32),
	CUTE_REF(dpackut_array_decode_uint32),
	CUTE_REF(dpackut_array_decode_int64),
	CUTE_REF(dpackut_array_decode_uint64),
	CUTE_REF(dpackut_array_decode_float),
	CUTE_REF(dpackut_array_decode_double),
	CUTE_REF(dpackut_array_decode_str),
	CUTE_REF(dpackut_array_decode_bin),
	CUTE_REF(dpackut_array_decode_multi)
};

CUTE_SUITE_EXTERN(dpackut_array_suite,
                  dpackut_array_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
