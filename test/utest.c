#include "utest.h"
#include "dpack/codec.h"
#include <stroll/assert.h>
#include <stdio.h>
#include <stdlib.h>

static char dpack_utest_assert_msg[LINE_MAX];

/*
 * Override libstroll's stroll_asser_fail() and use mock_assert() to validate
 * assertions.
 */
void
stroll_assert_fail(const char * __restrict prefix,
                   const char * __restrict expr,
                   const char * __restrict file,
                   unsigned int            line,
                   const char * __restrict func __unused)
{
	int    ret;
	size_t sz = sizeof(dpack_utest_assert_msg) - 1;

	/*
	 * mock_assert() does not really "return". It uses a nonlocal goto logic
	 * to restore program / stack state that existed before the code under
	 * test called us. This is the way cmocka allows checking for
	 * assertions.
	 * This means that the code below will never reach the abort() call
	 * below (which is just there to prevent GCC from warning us since
	 * stroll_assert_fail() is declared as a function that cannot return).
	 *
	 * Since mock_assert() does not give control back to us, we MUST use a
	 * statically allocated buffer to store assertion messages. We would not
	 * have the opportunity to free(3) a previously allocated buffer
	 * otherwise.
	 * In other words, valgrind memory leak checker should be happy with
	 * this...
	 */
	ret = snprintf(dpack_utest_assert_msg,
	               sz,
	               "{utest assert} %s:%s:%u:%s:\'%s\'",
	               prefix,
	               file,
	               line,
	               func,
	               expr);
	if (ret > 0) {
		if ((size_t)ret >= sz)
			dpack_utest_assert_msg[sz - 1] = '\0';

		mock_assert(0, dpack_utest_assert_msg, file, (int)line);
	}
	else
		mock_assert(0, "{utest assert} ??", file, (int)line);

	abort();
}

/*
 * Override libstroll's stroll_asser_fail_msg() and use mock_assert() to
 * validate assertions.
 */
void
stroll_assert_fail_msg(const char * __restrict prefix,
                       const char * __restrict message)
{
	int    ret;
	size_t sz = sizeof(dpack_utest_assert_msg) - 1;

	/*
	 * mock_assert() does not really "return". It uses a nonlocal goto logic
	 * to restore program / stack state that existed before the code under
	 * test called us. This is the way cmocka allows checking for
	 * assertions.
	 * This means that the code below will never reach the abort() call
	 * below (which is just there to prevent GCC from warning us since
	 * stroll_assert_fail() is declared as a function that cannot return).
	 *
	 * Since mock_assert() does not give control back to us, we MUST use a
	 * statically allocated buffer to store assertion messages. We would not
	 * have the opportunity to free(3) a previously allocated buffer
	 * otherwise.
	 * In other words, valgrind memory leak checker should be happy with
	 * this...
	 */
	ret = snprintf(dpack_utest_assert_msg,
	               sz,
	               "{utest assert} %s:%s",
	               prefix,
	               message);
	if (ret > 0) {
		if ((size_t)ret >= sz)
			dpack_utest_assert_msg[sz - 1] = '\0';

		mock_assert(0, dpack_utest_assert_msg, __FILE__, __LINE__);
	}
	else
		mock_assert(0, "{utest assert} ??", __FILE__, __LINE__);

	abort();
}

bool dpack_utest_free_wrapped;

/*
 * Mock Glibc's free(3) for verification purposes.
 *
 * Set dpack_utest_free_wrapped to true from client testing code to enable
 * free(3) argument checking logic.
 */
void
free(void * ptr)
{
	if (dpack_utest_free_wrapped) {
		/*
		 * Disable checking logic implicitly. Client testing code will
		 * have to re-enable it by setting dpack_utest_free_wrapped to
		 * true to perform subsequent validation.
		 *
		 * Watch out ! This MUST be done before calling any
		 * check_expected...() function is called since they all rely
		 * upon a working free(3). We would otherwise wrap cmocka
		 * internal calls to free(3) !
		 */
		dpack_utest_free_wrapped = false;
		/*
		 * free(3) argument checking logic is enabled: do the check
		 * using standard cmocka check_expected_ptr().
		 * First check pointer value, then content of memory pointed to.
		 */
		check_expected_ptr(ptr);
		check_expected_ptr(ptr);
	}

	/* Now call the original free(3) function. */
#if defined __GLIBC__
	extern void __libc_free(void *);
	__libc_free(ptr);
#else
#error Glibc is the only C library supported for now !
#endif
}

#if defined(CONFIG_DPACK_VALGRIND)
#include <valgrind/valgrind.h>
#endif

void
dpack_utest_expect_free_arg(const void * arg, size_t size)
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
	expect_value(free, ptr, (uintptr_t)arg);
	/* Request checking of pointed to memory content. */
	expect_memory(free, ptr, arg, size);
	/* Instruct free() function above to perform checking of arguments. */
	dpack_utest_free_wrapped = true;
}

void
dpack_scalar_utest_decode(const struct dpack_scalar_utest_data * data,
                          unsigned int                           nr,
                          dpack_utest_unpack_fn *                unpack)
{
	struct dpack_decoder dec = { 0, };
	unsigned int         d;

	for (d = 0; d < nr; d++) {
		dpack_decoder_init_buffer(&dec, data[d].packed, data[d].size);

		unpack(&dec, &data[d]);
		if (!data[d].error)
			assert_int_equal(dpack_decoder_data_left(&dec), 0);

		dpack_decoder_fini(&dec);
	}
}

void
dpack_scalar_utest_encode(const struct dpack_scalar_utest_data * data,
                          unsigned int                           nr,
                          dpack_utest_pack_fn *                  pack)
{
	struct dpack_encoder enc = { 0, };
	unsigned int         d;

	for (d = 0; d < nr; d++) {
		size_t sz = data[d].size;
		char   buff[sz];
		int    err;

		memset(buff, 0xa5, sz);
		dpack_encoder_init_buffer(&enc, buff, sz);

		err = pack(&enc, &data[d]);
		assert_int_equal(err, data[d].error);
		assert_memory_equal(buff, data[d].packed, sz);

		assert_int_equal(dpack_encoder_space_used(&enc), sz);
		assert_int_equal(dpack_encoder_space_left(&enc), 0);

		dpack_encoder_fini(&enc);
	}
}
