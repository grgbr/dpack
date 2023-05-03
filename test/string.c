#include "dpack/string.h"

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#if defined(ZERO_STR_SIZE_UTEST)

static void
dpack_zero_str_size_utest(void ** state __unused)
{
	/* Should fail to compile since 0 length string are not allowed. */
	int sz = DPACK_STR_SIZE(0);
}

#endif /* defined(ZERO_STR_SIZE_UTEST) */

#if defined(NONCONST_STR_SIZE_UTEST)

static void
dpack_nonconst_str_size_utest(void ** state __unused)
{
	unsigned int len;

	/* Should fail to compile since len is not constant. */
	assert_int_equal(DPACK_STR_SIZE(len), 0);
}

#endif /* defined(NONCONST_STR_SIZE_UTEST) */

#if defined(MAXLEN_STR_SIZE_UTEST)

static void
dpack_maxlen_str_size_utest(void ** state __unused)
{
	/*
	 * Should fail to compile since requested length > maximum string length
	 * allowed.
	 */
	assert_int_equal(DPACK_STR_SIZE(DPACK_STRLEN_MAX + 1), 0);
}

#endif /* defined(MAXLEN_STR_SIZE_UTEST) */

static void
dpack_fixstr_sizes_utest(void ** state __unused)
{
	/* Msgpack fixstr sizes. */
	assert_int_equal(DPACK_STR_SIZE(1),  2);
	assert_int_equal(dpack_str_size(1),  2);
	assert_int_equal(DPACK_STR_SIZE(2),  3);
	assert_int_equal(dpack_str_size(2),  3);
#if DPACK_STRLEN_MAX >= 30
	assert_int_equal(DPACK_STR_SIZE(30), 31);
	assert_int_equal(dpack_str_size(30), 31);
#endif
#if DPACK_STRLEN_MAX >= 31
	assert_int_equal(DPACK_STR_SIZE(31), 32);
	assert_int_equal(dpack_str_size(31), 32);
#endif
}

#if DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX

static void
dpack_str8_sizes_utest(void ** state __unused)
{
	/* Msgpack 8 bits string sizes. */
	assert_int_equal(DPACK_STR_SIZE(32),            34);
	assert_int_equal(dpack_str_size(32),            34);
#if DPACK_STRLEN_MAX >= 33
	assert_int_equal(DPACK_STR_SIZE(33),            35);
	assert_int_equal(dpack_str_size(33),            35);
#endif
#if DPACK_STRLEN_MAX >= (UINT8_MAX - 2)
	assert_int_equal(DPACK_STR_SIZE(UINT8_MAX - 2), UINT8_MAX);
	assert_int_equal(dpack_str_size(UINT8_MAX - 2), UINT8_MAX);
#endif
#if DPACK_STRLEN_MAX >= (UINT8_MAX - 1)
	assert_int_equal(DPACK_STR_SIZE(UINT8_MAX - 1), UINT8_MAX + 1);
	assert_int_equal(dpack_str_size(UINT8_MAX - 1), UINT8_MAX + 1);
#endif
#if DPACK_STRLEN_MAX >= UINT8_MAX
	assert_int_equal(DPACK_STR_SIZE(UINT8_MAX),     UINT8_MAX + 2);
	assert_int_equal(dpack_str_size(UINT8_MAX),     UINT8_MAX + 2);
#endif
}

#else  /* !(DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX) */

static void
dpack_str8_sizes_utest(void ** state __unused)
{
	skip();
}

#endif /* DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX */

#if DPACK_STRLEN_MAX > DPACK_STR8_LEN_MAX

static void
dpack_str16_sizes_utest(void ** state __unused)
{
	/* Msgpack 16 bits string sizes. */
	assert_int_equal(DPACK_STR_SIZE(UINT8_MAX + 1),  UINT8_MAX + 1 + 3);
	assert_int_equal(dpack_str_size(UINT8_MAX + 1),  UINT8_MAX + 1 + 3);
#if DPACK_STRLEN_MAX >= (UINT8_MAX + 2)
	assert_int_equal(DPACK_STR_SIZE(UINT8_MAX + 2),  UINT8_MAX + 2 + 3);
	assert_int_equal(dpack_str_size(UINT8_MAX + 2),  UINT8_MAX + 2 + 3);
#endif
#if DPACK_STRLEN_MAX >= (UINT16_MAX - 2)
	assert_int_equal(DPACK_STR_SIZE(UINT16_MAX - 2), UINT16_MAX - 2 + 3);
	assert_int_equal(dpack_str_size(UINT16_MAX - 2), UINT16_MAX - 2 + 3);
#endif
#if DPACK_STRLEN_MAX >= (UINT16_MAX - 1)
	assert_int_equal(DPACK_STR_SIZE(UINT16_MAX - 1), UINT16_MAX - 1 + 3);
	assert_int_equal(dpack_str_size(UINT16_MAX - 1), UINT16_MAX - 1 + 3);
#endif
#if DPACK_STRLEN_MAX >= UINT16_MAX
	assert_int_equal(DPACK_STR_SIZE(UINT16_MAX),     UINT16_MAX + 3);
	assert_int_equal(dpack_str_size(UINT16_MAX),     UINT16_MAX + 3);
#endif
}

#else  /* !(DPACK_STRLEN_MAX > DPACK_STR8_LEN_MAX) */

static void
dpack_str16_sizes_utest(void ** state __unused)
{
	skip();
}

#endif /* DPACK_STRLEN_MAX > DPACK_STR8_LEN_MAX */

#if DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX

static void
dpack_str32_sizes_utest(void ** state __unused)
{
	/* Msgpack 32 bits string sizes. */
	assert_int_equal(DPACK_STR_SIZE(UINT16_MAX + 1), UINT16_MAX + 1 + 5);
	assert_int_equal(dpack_str_size(UINT16_MAX + 1), UINT16_MAX + 1 + 5);
#if DPACK_STRLEN_MAX >= (UINT16_MAX + 2)
	assert_int_equal(DPACK_STR_SIZE(UINT16_MAX + 2), UINT16_MAX + 2 + 5);
	assert_int_equal(dpack_str_size(UINT16_MAX + 2), UINT16_MAX + 2 + 5);
#endif
#if DPACK_STRLEN_MAX >= (UINT32_MAX - 2)
	assert_int_equal(DPACK_STR_SIZE(UINT32_MAX - 2), UINT32_MAX - 2 + 5);
	assert_int_equal(dpack_str_size(UINT32_MAX - 2), UINT32_MAX - 2 + 5);
#endif
#if DPACK_STRLEN_MAX >= (UINT32_MAX - 1)
	assert_int_equal(DPACK_STR_SIZE(UINT32_MAX - 1), UINT32_MAX - 1 + 5);
	assert_int_equal(dpack_str_size(UINT32_MAX - 1), UINT32_MAX - 1 + 5);
#endif
#if DPACK_STRLEN_MAX >= UINT32_MAX
	assert_int_equal(DPACK_STR_SIZE(UINT32_MAX),     UINT32_MAX + 5);
	assert_int_equal(dpack_str_size(UINT32_MAX),     UINT32_MAX + 5);
#endif
}

#else  /* !(DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX) */

static void
dpack_str32_sizes_utest(void ** state __unused)
{
	skip();
}

#endif /* DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX */

#if defined(CONFIG_DPACK_ASSERT_API)

static void
dpack_assert_sizes_utest(void ** state __unused)
{
	size_t sz __unused;

	expect_assert_failure(sz = dpack_str_size(0));
	expect_assert_failure(sz = dpack_str_size(DPACK_STRLEN_MAX + 1));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

static void
dpack_assert_sizes_utest(void ** state __unused)
{
	skip();
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

static const struct CMUnitTest dpack_str_utests[] = {
	cmocka_unit_test(dpack_fixstr_sizes_utest),
	cmocka_unit_test(dpack_str8_sizes_utest),
	cmocka_unit_test(dpack_str16_sizes_utest),
	cmocka_unit_test(dpack_str32_sizes_utest),
	cmocka_unit_test(dpack_assert_sizes_utest),
};

int
main(void)
{
	return cmocka_run_group_tests_name("string size",
	                                   dpack_str_utests,
	                                   NULL,
	                                   NULL);
}
