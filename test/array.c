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

#define DPACKUT_ARRAY_SIZE(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACKUT_ARRAY_ELMSIZE, _elm_nr)

#define DPACKUT_ARRAY_ENABLED(_elm_nr) \
	((DPACK_ARRAY_ELMNR_MAX >= (_elm_nr)) && \
	 (((_elm_nr) * DPACKUT_ARRAY_ELMSIZE) <= DPACK_ARRAY_DATA_SIZE_MAX))

CUTE_TEST(dpackut_fixarray_sizes)
{
	/* Msgpack fixarray sizes. */
	cute_check_uint(DPACKUT_ARRAY_SIZE(1),
	                equal,
	                1 + (1 * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, 1),
	                equal,
	                1 + (1 * DPACKUT_ARRAY_ELMSIZE));

	cute_check_uint(DPACKUT_ARRAY_SIZE(2),
	                equal,
	                1 + (2 * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, 2),
	                equal,
	                1 + (2 * DPACKUT_ARRAY_ELMSIZE));

#if DPACKUT_ARRAY_ENABLED(14)
	cute_check_uint(DPACKUT_ARRAY_SIZE(14),
	                equal,
	                1 + (14 * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, 14),
	                equal,
	                1 + (14 * DPACKUT_ARRAY_ELMSIZE));
#endif

#if DPACKUT_ARRAY_ENABLED(15)
	cute_check_uint(DPACKUT_ARRAY_SIZE(15),
	                equal,
	                1 + (15 * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, 15),
	                equal,
	                1 + (15 * DPACKUT_ARRAY_ELMSIZE));
#endif
}

#if DPACK_ARRAY_ELMNR_MAX > DPACK_FIXARRAY_ELMNR_MAX

CUTE_TEST(dpackut_array16_sizes)
{
	/* Msgpack fixarray sizes. */
	cute_check_uint(DPACKUT_ARRAY_SIZE(16),
	                equal,
	                3U + (16 * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, 16),
	                equal,
	                3U + (16 * DPACKUT_ARRAY_ELMSIZE));

#if DPACKUT_ARRAY_ENABLED(17)
	cute_check_uint(DPACKUT_ARRAY_SIZE(17),
	                equal,
	                3U + (17 * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, 17),
	                equal,
	                3U + (17 * DPACKUT_ARRAY_ELMSIZE));
#endif

#if DPACKUT_ARRAY_ENABLED(UINT16_MAX - 2)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT16_MAX - 2),
	                equal,
	                3U + ((UINT16_MAX - 2) * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, UINT16_MAX - 2),
	                equal,
	                3U + ((UINT16_MAX - 2) * DPACKUT_ARRAY_ELMSIZE));
#endif

#if DPACKUT_ARRAY_ENABLED(UINT16_MAX - 1)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT16_MAX - 1),
	                equal,
	                3U + ((UINT16_MAX - 1) * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, UINT16_MAX - 1),
	                equal,
	                3U + ((UINT16_MAX - 1) * DPACKUT_ARRAY_ELMSIZE));
#endif

#if DPACKUT_ARRAY_ENABLED(UINT16_MAX)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT16_MAX),
	                equal,
	                3U + (UINT16_MAX * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, UINT16_MAX),
	                equal,
	                3U + (UINT16_MAX * DPACKUT_ARRAY_ELMSIZE));
#endif
}

#else   /* !(DPACK_ARRAY_ELMNR_MAX > DPACK_FIXARRAY_ELMNR_MAX) */

CUTE_TEST(dpackut_array16_sizes)
{
	cute_skip("16-bits MessagePack array support not compiled-in");
}

#endif  /* DPACK_ARRAY_ELMNR_MAX > DPACK_FIXARRAY_ELMNR_MAX */

#if DPACK_ARRAY_ELMNR_MAX > DPACK_ARRAY16_ELMNR_MAX

CUTE_TEST(dpackut_array32_sizes)
{
	/* Msgpack fixarray sizes. */
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT16_MAX + 1),
	                equal,
	                5U + ((UINT16_MAX + 1) * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, UINT16_MAX + 1),
	                equal,
	                5U + ((UINT16_MAX + 1) * DPACKUT_ARRAY_ELMSIZE));

#if DPACKUT_ARRAY_ENABLED(UINT16_MAX + 2)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT16_MAX + 2),
	                equal,
	                5U + ((UINT16_MAX + 2) * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, UINT16_MAX + 2),
	                equal,
	                5U + ((UINT16_MAX + 2) * DPACKUT_ARRAY_ELMSIZE));
#endif

#if DPACKUT_ARRAY_ENABLED(UINT32_MAX - 2)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT32_MAX - 2),
	                equal,
	                5U + ((UINT32_MAX - 2) * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, UINT32_MAX - 2),
	                equal,
	                5U + ((UINT32_MAX - 2) * DPACKUT_ARRAY_ELMSIZE));
#endif

#if DPACKUT_ARRAY_ENABLED(UINT32_MAX - 1)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT32_MAX - 1),
	                equal,
	                5U + ((UINT32_MAX - 1) * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, UINT32_MAX - 1),
	                equal,
	                5U + ((UINT32_MAX - 1) * DPACKUT_ARRAY_ELMSIZE));
#endif
#if DPACKUT_ARRAY_ENABLED(UINT32_MAX)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT32_MAX),
	                equal,
	                5U + (UINT32_MAX * DPACKUT_ARRAY_ELMSIZE));
	cute_check_uint(dpack_array_size(DPACKUT_ARRAY_ELMSIZE, UINT32_MAX),
	                equal,
	                5U + (UINT32_MAX * DPACKUT_ARRAY_ELMSIZE));
#endif
}

#else   /* !(DPACK_ARRAY_ELMNR_MAX > DPACK_ARRAY16_ELMNR_MAX) */

CUTE_TEST(dpackut_array32_sizes)
{
	cute_skip("32-bits MessagePack array support not compiled-in");
}

#endif  /* DPACK_ARRAY_ELMNR_MAX > DPACK_ARRAY16_ELMNR_MAX */

#if defined(CONFIG_DPACK_ASSERT_API)

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
#define DPACKUT_ARRAY_MULTI_ELM_SIZE \
	STROLL_CONST_MAX(DPACK_BOOL_SIZE, \
		STROLL_CONST_MAX(DPACK_DOUBLE_SIZE, \
			STROLL_CONST_MAX(DPACK_STR_SIZE(4), \
				STROLL_CONST_MAX(DPACK_INT16_SIZE_MAX, \
					STROLL_CONST_MAX(DPACK_BIN_SIZE(3), \
						STROLL_CONST_MAX(DPACK_UINT32_SIZE_MAX, \
						                 DPACK_NIL_SIZE))))))
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
	DPACK_ARRAY_SIZE(DPACKUT_ARRAY_MULTI_ELM_SIZE, \
	                 DPACKUT_ARRAY_MULTI_ELM_NR)

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
#define DPACKUT_ARRAY_NEST_PACK_SIZE_MAX (128U)

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

CUTE_GROUP(dpackut_array_group) = {
	CUTE_REF(dpackut_fixarray_sizes),
	CUTE_REF(dpackut_array16_sizes),
	CUTE_REF(dpackut_array32_sizes),

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
};

CUTE_SUITE_EXTERN(dpackut_array_suite,
                  dpackut_array_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
