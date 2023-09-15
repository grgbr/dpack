#include "dpack/array.h"
#include <cute/cute.h>
#include <cute/check.h>

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
	cute_check_uint(DPACKUT_ARRAY_SIZE(2),
	                equal,
	                1 + (2 * DPACKUT_ARRAY_ELMSIZE));
#if DPACKUT_ARRAY_ENABLED(14)
	cute_check_uint(DPACKUT_ARRAY_SIZE(14),
	                equal,
	                1 + (14 * DPACKUT_ARRAY_ELMSIZE));
#endif
#if DPACKUT_ARRAY_ENABLED(15)
	cute_check_uint(DPACKUT_ARRAY_SIZE(15),
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
#if DPACKUT_ARRAY_ENABLED(17)
	cute_check_uint(DPACKUT_ARRAY_SIZE(17),
	                equal,
	                3U + (17 * DPACKUT_ARRAY_ELMSIZE));
#endif
#if DPACKUT_ARRAY_ENABLED(UINT16_MAX - 2)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT16_MAX - 2),
	                equal,
	                3U + ((UINT16_MAX - 2) * DPACKUT_ARRAY_ELMSIZE));
#endif
#if DPACKUT_ARRAY_ENABLED(UINT16_MAX - 1)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT16_MAX - 1),
	                equal,
	                3U + ((UINT16_MAX - 1) * DPACKUT_ARRAY_ELMSIZE));
#endif
#if DPACKUT_ARRAY_ENABLED(UINT16_MAX)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT16_MAX),
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
#if DPACKUT_ARRAY_ENABLED(UINT16_MAX + 2)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT16_MAX + 2),
	                equal,
	                5U + ((UINT16_MAX + 2) * DPACKUT_ARRAY_ELMSIZE));
#endif
#if DPACKUT_ARRAY_ENABLED(UINT32_MAX - 2)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT32_MAX - 2),
	                equal,
	                5U + ((UINT32_MAX - 2) * DPACKUT_ARRAY_ELMSIZE));
#endif
#if DPACKUT_ARRAY_ENABLED(UINT32_MAX - 1)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT32_MAX - 1),
	                equal,
	                5U + ((UINT32_MAX - 1) * DPACKUT_ARRAY_ELMSIZE));
#endif
#if DPACKUT_ARRAY_ENABLED(UINT32_MAX)
	cute_check_uint(DPACKUT_ARRAY_SIZE(UINT32_MAX),
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

CUTE_GROUP(dpackut_array_group) = {
	CUTE_REF(dpackut_fixarray_sizes),
	CUTE_REF(dpackut_array16_sizes),
	CUTE_REF(dpackut_array32_sizes)
};

CUTE_SUITE_EXTERN(dpackut_array_suite,
                  dpackut_array_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
