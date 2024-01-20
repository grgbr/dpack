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
	cute_check_uint(DPACK_STR_SIZE(30), equal, 31);
	cute_check_uint(dpack_lvstr_size(30), equal, 31);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 30) */

CUTE_TEST(dpackut_fixlvstr_sizes_30)
{
	cute_skip("MsgPack string length >= 30 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 30 */

#if DPACK_LVSTRLEN_MAX >= 31

CUTE_TEST(dpackut_fixlvstr_sizes_31)
{
	cute_check_uint(DPACK_STR_SIZE(31), equal, 32);
	cute_check_uint(dpack_lvstr_size(31), equal, 32);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 31) */

CUTE_TEST(dpackut_fixlvstr_sizes_31)
{
	cute_skip("MsgPack string length >= 31 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 31 */

#if DPACK_LVSTRLEN_MAX > DPACK_FIXSTR_LEN_MAX

CUTE_TEST(dpackut_lvstr8_sizes)
{
	/* Msgpack 8 bits string sizes. */
	cute_check_uint(DPACK_STR_SIZE(32), equal, 34);
	cute_check_uint(dpack_lvstr_size(32), equal, 34);
}

#else  /* !(DPACK_LVSTRLEN_MAX > DPACK_FIXSTR_LEN_MAX) */

CUTE_TEST(dpackut_lvstr8_sizes)
{
	cute_skip("string length >= MsgPack fixstr length "
	          " support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX > DPACK_FIXSTR_LEN_MAX */

#if DPACK_LVSTRLEN_MAX >= 33

CUTE_TEST(dpackut_lvstr8_sizes_33)
{
	cute_check_uint(DPACK_STR_SIZE(33), equal, 35);
	cute_check_uint(dpack_lvstr_size(33), equal, 35);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= 33) */

CUTE_TEST(dpackut_lvstr8_sizes_33)
{
	cute_skip("MsgPack string length >= 33 support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= 33 */

#if DPACK_LVSTRLEN_MAX >= (UINT8_MAX - 2)

CUTE_TEST(dpackut_lvstr8_sizes_253)
{
	cute_check_uint(DPACK_STR_SIZE(UINT8_MAX - 2), equal, UINT8_MAX);
	cute_check_uint(dpack_lvstr_size(UINT8_MAX - 2), equal, UINT8_MAX);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT8_MAX - 2)) */

CUTE_TEST(dpackut_lvstr8_sizes_253)
{
	cute_skip("MsgPack string length >= (UINT8_MAX - 2) support "
	          "not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT8_MAX - 2) */

#if DPACK_LVSTRLEN_MAX >= (UINT8_MAX - 1)

CUTE_TEST(dpackut_lvstr8_sizes_254)
{
	cute_check_uint(DPACK_STR_SIZE(UINT8_MAX - 1), equal, UINT8_MAX + 1);
	cute_check_uint(dpack_lvstr_size(UINT8_MAX - 1), equal, UINT8_MAX + 1);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT8_MAX - 1)) */

CUTE_TEST(dpackut_lvstr8_sizes_254)
{
	cute_skip("MsgPack string length >= (UINT8_MAX - 1) support "
	          "not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT8_MAX - 1) */

#if DPACK_LVSTRLEN_MAX >= UINT8_MAX

CUTE_TEST(dpackut_lvstr8_sizes_255)
{
	cute_check_uint(DPACK_STR_SIZE(UINT8_MAX), equal, UINT8_MAX + 2);
	cute_check_uint(dpack_lvstr_size(UINT8_MAX), equal, UINT8_MAX + 2);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= UINT8_MAX) */

CUTE_TEST(dpackut_lvstr8_sizes_255)
{
	cute_skip("MsgPack string length >= UINT8_MAX support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= UINT8_MAX */

#if DPACK_LVSTRLEN_MAX > DPACK_STR8_LEN_MAX

CUTE_TEST(dpackut_lvstr16_sizes)
{
	/* Msgpack 16 bits string sizes. */
	cute_check_uint(DPACK_STR_SIZE(UINT8_MAX + 1),
	                equal,
	                UINT8_MAX + 1 + 3);
	cute_check_uint(dpack_lvstr_size(UINT8_MAX + 1),
	                equal,
	                UINT8_MAX + 1 + 3);
}

#else  /* !(DPACK_LVSTRLEN_MAX > DPACK_STR8_LEN_MAX) */

CUTE_TEST(dpackut_lvstr16_sizes)
{
	cute_skip("string length >= MsgPack str8 length "
	          " support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX > DPACK_STR8_LEN_MAX */

#if DPACK_LVSTRLEN_MAX >= (UINT8_MAX + 2)

CUTE_TEST(dpackut_lvstr16_sizes_257)
{
	cute_check_uint(DPACK_STR_SIZE(UINT8_MAX + 2),
	                equal,
	                UINT8_MAX + 2 + 3);
	cute_check_uint(dpack_lvstr_size(UINT8_MAX + 2),
	                equal,
	                UINT8_MAX + 2 + 3);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT8_MAX + 2)) */

CUTE_TEST(dpackut_lvstr16_sizes_257)
{
	cute_skip("MsgPack string length >= (UINT8_MAX + 2) support "
	          "not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT8_MAX + 2) */

#if DPACK_LVSTRLEN_MAX >= (UINT16_MAX - 2)

CUTE_TEST(dpackut_lvstr16_sizes_65533)
{
	cute_check_uint(DPACK_STR_SIZE(UINT16_MAX - 2),
	                equal,
	                UINT16_MAX - 2 + 3);
	cute_check_uint(dpack_lvstr_size(UINT16_MAX - 2),
	                equal,
	                UINT16_MAX - 2 + 3);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT16_MAX - 2)) */

CUTE_TEST(dpackut_lvstr16_sizes_65533)
{
	cute_skip("MsgPack string length >= (UINT16_MAX - 2) support "
	          "not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT16_MAX - 2) */

#if DPACK_LVSTRLEN_MAX >= (UINT16_MAX - 1)

CUTE_TEST(dpackut_lvstr16_sizes_65534)
{
	cute_check_uint(DPACK_STR_SIZE(UINT16_MAX - 1),
	                equal,
	                UINT16_MAX - 1 + 3);
	cute_check_uint(dpack_lvstr_size(UINT16_MAX - 1),
	                equal,
	                UINT16_MAX - 1 + 3);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT16_MAX - 1)) */

CUTE_TEST(dpackut_lvstr16_sizes_65534)
{
	cute_skip("MsgPack string length >= (UINT16_MAX - 1) support "
	          "not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT16_MAX - 1) */

#if DPACK_LVSTRLEN_MAX >= UINT16_MAX

CUTE_TEST(dpackut_lvstr16_sizes_65535)
{
	cute_check_uint(DPACK_STR_SIZE(UINT16_MAX), equal, UINT16_MAX + 3);
	cute_check_uint(dpack_lvstr_size(UINT16_MAX), equal, UINT16_MAX + 3);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= UINT16_MAX) */

CUTE_TEST(dpackut_lvstr16_sizes_65535)
{
	cute_skip("MsgPack string length >= UINT16_MAX support "
	          "not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= UINT16_MAX */

#if DPACK_LVSTRLEN_MAX > DPACK_STR16_LEN_MAX

CUTE_TEST(dpackut_lvstr32_sizes)
{
	/* Msgpack 32 bits string sizes. */
	cute_check_uint(DPACK_STR_SIZE(UINT16_MAX + 1),
	                equal,
	                UINT16_MAX + 1 + 5);
	cute_check_uint(dpack_lvstr_size(UINT16_MAX + 1),
	                equal,
	                UINT16_MAX + 1 + 5);
}

#else  /* !(DPACK_LVSTRLEN_MAX > DPACK_STR16_LEN_MAX) */

CUTE_TEST(dpackut_lvstr32_sizes)
{
	cute_skip("string length >= MsgPack str16 length "
	          " support not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX > DPACK_STR16_LEN_MAX */

#if DPACK_LVSTRLEN_MAX >= (UINT16_MAX + 2)

CUTE_TEST(dpackut_lvstr32_sizes_65537)
{
	cute_check_uint(DPACK_STR_SIZE(UINT16_MAX + 2),
	                equal,
	                UINT16_MAX + 2 + 5);
	cute_check_uint(dpack_lvstr_size(UINT16_MAX + 2),
	                equal,
	                UINT16_MAX + 2 + 5);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT16_MAX + 2)) */

CUTE_TEST(dpackut_lvstr32_sizes_65537)
{
	cute_skip("MsgPack string length >= (UINT16_MAX + 2) support "
	          "not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT16_MAX + 2) */

#if DPACK_LVSTRLEN_MAX >= (UINT32_MAX - 2)

CUTE_TEST(dpackut_lvstr32_sizes_maxminus2)
{
	cute_check_uint(DPACK_STR_SIZE(UINT32_MAX - 2),
	                equal,
	                (uintmax_t)UINT32_MAX - 2 + 5);
	cute_check_uint(dpack_lvstr_size(UINT32_MAX - 2),
	                equal,
	                (uintmax_t)UINT32_MAX - 2 + 5);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT32_MAX - 2)) */

CUTE_TEST(dpackut_lvstr32_sizes_maxminus2)
{
	cute_skip("MsgPack string length >= (UINT32_MAX - 2) support "
	          "not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT32_MAX - 2) */

#if DPACK_LVSTRLEN_MAX >= (UINT32_MAX - 1)

CUTE_TEST(dpackut_lvstr32_sizes_maxminus1)
{
	cute_check_uint(DPACK_STR_SIZE(UINT32_MAX - 1),
	                equal,
	                (uintmax_t)UINT32_MAX - 1 + 5);
	cute_check_uint(dpack_lvstr_size(UINT32_MAX - 1),
	                equal,
	                (uintmax_t)UINT32_MAX - 1 + 5);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= (UINT32_MAX - 1)) */

CUTE_TEST(dpackut_lvstr32_sizes_maxminus1)
{
	cute_skip("MsgPack string length >= (UINT32_MAX - 1) support "
	          "not compiled-in");
}

#endif /* DPACK_LVSTRLEN_MAX >= (UINT32_MAX - 1) */

#if DPACK_LVSTRLEN_MAX >= UINT32_MAX

CUTE_TEST(dpackut_lvstr32_sizes_max)
{
	cute_check_uint(DPACK_STR_SIZE(UINT32_MAX),
	                equal,
	                (uintmax_t)UINT32_MAX + 5);
	cute_check_uint(dpack_lvstr_size(UINT32_MAX),
	                equal,
	                (uintmax_t)UINT32_MAX + 5);
}

#else  /* !(DPACK_LVSTRLEN_MAX >= UINT32_MAX) */

CUTE_TEST(dpackut_lvstr32_sizes_max)
{
	cute_skip("MsgPack string length >= UINT32_MAX support "
	          "not compiled-in");
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

};

CUTE_SUITE_EXTERN(dpackut_lvstr_suite,
                  dpackut_lvstr_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
