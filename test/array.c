#include "dpack/array.h"

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#define DPACK_ARRAY_UTEST_ELMSIZE \
	(1U)

#define DPACK_ARRAY_UTEST_SIZE(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_ARRAY_UTEST_ELMSIZE, _elm_nr)

#define DPACK_ARRAY_UTEST_ENABLED(_elm_nr) \
	((DPACK_ARRAY_ELMNR_MAX >= (_elm_nr)) && \
	 (((_elm_nr) * DPACK_ARRAY_UTEST_ELMSIZE) <= DPACK_ARRAY_DATA_SIZE_MAX))

static void
dpack_fixarray_sizes_utest(void ** state __unused)
{
	/* Msgpack fixarray sizes. */
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(1),
	                 1 + (1 * DPACK_ARRAY_UTEST_ELMSIZE));
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(2),
	                 1 + (2 * DPACK_ARRAY_UTEST_ELMSIZE));
#if DPACK_ARRAY_UTEST_ENABLED(14)
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(14),
	                 1 + (14 * DPACK_ARRAY_UTEST_ELMSIZE));
#endif
#if DPACK_ARRAY_UTEST_ENABLED(15)
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(15),
	                 1 + (15 * DPACK_ARRAY_UTEST_ELMSIZE));
#endif
}

#if DPACK_ARRAY_ELMNR_MAX > DPACK_FIXARRAY_ELMNR_MAX

static void
dpack_array16_sizes_utest(void ** state __unused)
{
	/* Msgpack fixarray sizes. */
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(16),
	                 3U + (16 * DPACK_ARRAY_UTEST_ELMSIZE));
#if DPACK_ARRAY_UTEST_ENABLED(17)
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(17),
	                 3U + (17 * DPACK_ARRAY_UTEST_ELMSIZE));
#endif
#if DPACK_ARRAY_UTEST_ENABLED(UINT16_MAX - 2)
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(UINT16_MAX - 2),
	                 3U + ((UINT16_MAX - 2) * DPACK_ARRAY_UTEST_ELMSIZE));
#endif
#if DPACK_ARRAY_UTEST_ENABLED(UINT16_MAX - 1)
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(UINT16_MAX - 1),
	                 3U + ((UINT16_MAX - 1) * DPACK_ARRAY_UTEST_ELMSIZE));
#endif
#if DPACK_ARRAY_UTEST_ENABLED(UINT16_MAX)
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(UINT16_MAX),
	                 3U + (UINT16_MAX * DPACK_ARRAY_UTEST_ELMSIZE));
#endif
}

#else   /* !(DPACK_ARRAY_ELMNR_MAX > DPACK_FIXARRAY_ELMNR_MAX) */

static void
dpack_array16_sizes_utest(void ** state __unused)
{
	skip();
}

#endif  /* DPACK_ARRAY_ELMNR_MAX > DPACK_FIXARRAY_ELMNR_MAX */

#if DPACK_ARRAY_ELMNR_MAX > DPACK_ARRAY16_ELMNR_MAX

static void
dpack_array32_sizes_utest(void ** state __unused)
{
	/* Msgpack fixarray sizes. */
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(UINT16_MAX + 1),
	                 5U + ((UINT16_MAX + 1) * DPACK_ARRAY_UTEST_ELMSIZE));
#if DPACK_ARRAY_UTEST_ENABLED(UINT16_MAX + 2)
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(UINT16_MAX + 2),
	                 5U + ((UINT16_MAX + 2) * DPACK_ARRAY_UTEST_ELMSIZE));
#endif
#if DPACK_ARRAY_UTEST_ENABLED(UINT32_MAX - 2)
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(UINT32_MAX - 2),
	                 5U + ((UINT32_MAX - 2) * DPACK_ARRAY_UTEST_ELMSIZE));
#endif
#if DPACK_ARRAY_UTEST_ENABLED(UINT32_MAX - 1)
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(UINT32_MAX - 1),
	                 5U + ((UINT32_MAX - 1) * DPACK_ARRAY_UTEST_ELMSIZE));
#endif
#if DPACK_ARRAY_UTEST_ENABLED(UINT32_MAX)
	assert_int_equal(DPACK_ARRAY_UTEST_SIZE(UINT32_MAX),
	                 5U + (UINT32_MAX * DPACK_ARRAY_UTEST_ELMSIZE));
#endif
}

#else   /* !(DPACK_ARRAY_ELMNR_MAX > DPACK_ARRAY16_ELMNR_MAX) */

static void
dpack_array32_sizes_utest(void ** state __unused)
{
	skip();
}

#endif  /* DPACK_ARRAY_ELMNR_MAX > DPACK_ARRAY16_ELMNR_MAX */

static const struct CMUnitTest dpack_array_utests[] = {
	cmocka_unit_test(dpack_fixarray_sizes_utest),
	cmocka_unit_test(dpack_array16_sizes_utest),
	cmocka_unit_test(dpack_array32_sizes_utest)
};

int
main(void)
{
	return cmocka_run_group_tests(dpack_array_utests, NULL, NULL);
}
