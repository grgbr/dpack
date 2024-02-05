/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "utest.h"
#include "dpack/codec.h"
#include <stroll/assert.h>
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#if defined(CONFIG_DPACK_VALGRIND)
#include <valgrind/valgrind.h>
#endif

static char dpackut_assert_msg[LINE_MAX];

/*
 * Override libstroll's stroll_assert_fail() and use cute_mock_assert() to
 * validate assertions.
 */
void
stroll_assert_fail(const char * __restrict prefix,
                   const char * __restrict expr,
                   const char * __restrict file,
                   unsigned int            line,
                   const char * __restrict func)
{
	int    ret;
	size_t sz = sizeof(dpackut_assert_msg) - 1;

	/*
	 * cute_mock_assert() does not really "return". It uses a nonlocal goto
	 * logic to restore program / stack state that existed before the code
	 * under test called us. This is the way CUTe allows checking for
	 * assertions.
	 * This means that the code below will never reach the abort() call
	 * below (which is just there to prevent GCC from warning us since
	 * stroll_assert_fail() is declared as a function that cannot return).
	 *
	 * Since cute_mock_assert() does not give control back to us, we MUST
	 * use a statically allocated buffer to store assertion messages. We
	 * would not have the opportunity to free(3) a previously allocated
	 * buffer otherwise.
	 * In other words, Valgrind memory leak checker should be happy with
	 * this...
	 */
	ret = snprintf(dpackut_assert_msg,
	               sz,
	               "{utest assert} %s:%s:%u:%s:\'%s\'",
	               prefix,
	               file,
	               line,
	               func,
	               expr);
	if (ret > 0) {
		if ((size_t)ret >= sz)
			dpackut_assert_msg[sz - 1] = '\0';

		cute_mock_assert(dpackut_assert_msg, file, line, func);
	}
	else
		cute_mock_assert("{utest assert} ??", file, line, func);

	/* Not reached (see comment above)... */
	abort();
}

/*
 * Override libstroll's stroll_assert_fail_msg() and use cute_mock_assert() to
 * validate assertions.
 */
void
stroll_assert_fail_msg(const char * __restrict prefix,
                       const char * __restrict message)
{
	int    ret;
	size_t sz = sizeof(dpackut_assert_msg) - 1;

	/*
	 * cute_mock_assert() does not really "return". It uses a nonlocal goto
	 * logic to restore program / stack state that existed before the code
	 * under test called us. This is the way CUTe allows checking for
	 * assertions.
	 * This means that the code below will never reach the abort() call
	 * below (which is just there to prevent GCC from warning us since
	 * stroll_assert_fail_msg() is declared as a function that cannot
	 * return).
	 *
	 * Since cute_mock_assert() does not give control back to us, we MUST
	 * use a statically allocated buffer to store assertion messages. We
	 * would not have the opportunity to free(3) a previously allocated
	 * buffer otherwise.
	 * In other words, Valgrind memory leak checker should be happy with
	 * this...
	 */
	ret = snprintf(dpackut_assert_msg,
	               sz,
	               "{utest assert} %s:%s",
	               prefix,
	               message);
	if (ret > 0) {
		if ((size_t)ret >= sz)
			dpackut_assert_msg[sz - 1] = '\0';

		cute_mock_assert(dpackut_assert_msg,
		                 __FILE__,
		                 __LINE__,
		                 __func__);
	}
	else
		cute_mock_assert("{utest assert} ??",
		                 __FILE__,
		                 __LINE__,
		                 __func__);

	abort();
}

static bool dpackut_free_wrapped;

/*
 * Mock Glibc's free(3) for verification purposes.
 *
 * Set dpackut_free_wrapped to true from client testing code to enable
 * free(3) argument checking logic.
 */
void
free(void * ptr)
{
	if (dpackut_free_wrapped) {
		/*
		 * Disable checking logic implicitly. Client testing code will
		 * have to re-enable it by setting dpackut_free_wrapped to
		 * true to perform subsequent validation.
		 *
		 * Watch out ! This MUST be done before calling any
		 * cute_mock_...() function is called since they all rely upon a
		 * working free(3). We would otherwise wrap CUTe's internal
		 * calls to free(3) !
		 */
		dpackut_free_wrapped = false;
		/*
		 * free(3) argument checking logic is enabled: do the check
		 * using standard CUTe's cute_mock_ptr_parm() /
		 * cute_mock_mem_parm().
		 * First check pointer value, then content of memory pointed to.
		 */
		cute_mock_ptr_parm(ptr);
		cute_mock_mem_parm(ptr);
	}

	/* Now call the original GLibc core free(3) function. */
#if defined __GLIBC__
	extern void __libc_free(void *);
	__libc_free(ptr);
#else
#error Glibc is the only C library supported for now !
#endif
}

void
dpackut_expect_free(const void * parm, size_t size)
{
#if defined(CONFIG_DPACK_VALGRIND)
	/*
	 * As Valgrind overrides C library's malloc(3) / realloc(3) / free(3)
	 * functions, it bypasses our own free(3) wrapper implemented above.
	 * This breaks our mocked free(3) testing mechanism and leads to test
	 * failures.
	 * Inhibit our mocked free(3) based tests when running testsuite under
	 * Valgrind. We may still run the entire testsuite without Valgrind
	 * anyway.
	 */
	if (RUNNING_ON_VALGRIND)
		return;
#endif
	/* Request checking of pointer value. */
	cute_expect_ptr_parm(free, ptr, equal, parm);
	/* Request checking of pointed to memory content. */
	cute_expect_mem_parm(free, ptr, equal, parm, size);

	/* Instruct free() function above to perform checking of arguments. */
	dpackut_free_wrapped = true;
}

static bool dpackut_malloc_wrapped;

/*
 * Mock Glibc's malloc(3) for verification purposes.
 *
 * Set dpackut_malloc_wrapped to true from client testing code to enable
 * malloc(3) argument checking logic and simulate allocation failure.
 */
void *
malloc(size_t size)
{
	if (dpackut_malloc_wrapped) {
		/*
		 * Disable checking logic implicitly. Client testing code will
		 * have to re-enable it by setting dpackut_malloc_wrapped to
		 * true to perform subsequent validation.
		 *
		 * Watch out ! This MUST be done before calling any
		 * cute_mock_...() function is called since they all rely
		 * upon a working malloc(3). We would otherwise wrap CUTe's
		 * internal calls to malloc(3) !
		 */
		dpackut_malloc_wrapped = false;
		/*
		 * malloc(3) argument checking logic is enabled: do the check
		 * using standard CUTe cute_mock_uint_parm().
		 */
		cute_mock_uint_parm(size);

		/* Now simulate a malloc() failure */
		errno = ENOMEM;
		return NULL;
	}

	/* Now call the GLibc core malloc(3) function. */
#if defined __GLIBC__
	extern void * __libc_malloc(size_t);
	return __libc_malloc(size);
#else
#error Glibc is the only C library supported for now !
#endif
}

int
dpackut_expect_malloc(void)
{
#if defined(CONFIG_DPACK_VALGRIND)
	/*
	 * As Valgrind overrides C library's malloc(3) / realloc(3) / free(3)
	 * functions, it bypasses our own malloc(3) wrapper implemented above.
	 * This breaks our mocked malloc(3) testing mechanism and leads to test
	 * failures.
	 * Inhibit our mocked malloc(3) based tests when running testsuite under
	 * Valgrind. We may still run the entire testsuite without Valgrind
	 * anyway.
	 */
	if (RUNNING_ON_VALGRIND)
		/*
		 * Tell the caller we cannot intercept malloc() calls since
		 * Valgrind has already overridden malloc().
		 */
		return -ECANCELED;
#endif

	/* Request checking of malloc(3) argument value to ensure it is != 0. */
	cute_expect_uint_parm(malloc, size, unequal, 0);
	/*
	 * Instruct malloc() function above to check argument and simulate a
	 * failure.
	 */
	dpackut_malloc_wrapped = true;

	return 0;
}

#if defined(CONFIG_DPACK_ARRAY)
extern CUTE_SUITE_DECL(dpackut_array_suite);
#endif
#if defined(CONFIG_DPACK_BIN)
extern CUTE_SUITE_DECL(dpackut_bin_suite);
#endif
#if defined(CONFIG_DPACK_DOUBLE)
extern CUTE_SUITE_DECL(dpackut_double_suite);
#endif
#if defined(CONFIG_DPACK_FLOAT)
extern CUTE_SUITE_DECL(dpackut_float_suite);
#endif
#if defined(CONFIG_DPACK_SCALAR)
extern CUTE_SUITE_DECL(dpackut_bool_suite);
extern CUTE_SUITE_DECL(dpackut_uint8_suite);
extern CUTE_SUITE_DECL(dpackut_int8_suite);
extern CUTE_SUITE_DECL(dpackut_uint16_suite);
extern CUTE_SUITE_DECL(dpackut_int16_suite);
extern CUTE_SUITE_DECL(dpackut_uint32_suite);
extern CUTE_SUITE_DECL(dpackut_int32_suite);
extern CUTE_SUITE_DECL(dpackut_uint64_suite);
extern CUTE_SUITE_DECL(dpackut_int64_suite);
#endif
#if defined(CONFIG_DPACK_STRING)
extern CUTE_SUITE_DECL(dpackut_str_suite);
#endif
#if defined(CONFIG_DPACK_LVSTR)
extern CUTE_SUITE_DECL(dpackut_lvstr_suite);
#endif
#if defined(CONFIG_DPACK_MAP)
extern CUTE_SUITE_DECL(dpackut_map_suite);
#endif

CUTE_GROUP(dpackut_group) = {
#if defined(CONFIG_DPACK_ARRAY)
	CUTE_REF(dpackut_array_suite),
#endif
#if defined(CONFIG_DPACK_BIN)
	CUTE_REF(dpackut_bin_suite),
#endif
#if defined(CONFIG_DPACK_DOUBLE)
	CUTE_REF(dpackut_double_suite),
#endif
#if defined(CONFIG_DPACK_FLOAT)
	CUTE_REF(dpackut_float_suite),
#endif
#if defined(CONFIG_DPACK_SCALAR)
	CUTE_REF(dpackut_bool_suite),
	CUTE_REF(dpackut_uint8_suite),
	CUTE_REF(dpackut_int8_suite),
	CUTE_REF(dpackut_uint16_suite),
	CUTE_REF(dpackut_int16_suite),
	CUTE_REF(dpackut_uint32_suite),
	CUTE_REF(dpackut_int32_suite),
	CUTE_REF(dpackut_uint64_suite),
	CUTE_REF(dpackut_int64_suite),
#endif
#if defined(CONFIG_DPACK_STRING)
	CUTE_REF(dpackut_str_suite),
#endif
#if defined(CONFIG_DPACK_LVSTR)
	CUTE_REF(dpackut_lvstr_suite),
#endif
#if defined(CONFIG_DPACK_MAP)
	CUTE_REF(dpackut_map_suite),
#endif
};

CUTE_SUITE(dpackut_suite, dpackut_group);

CUTE_MAIN(dpackut_suite, "DPack", DPACK_VERSION_STRING)
