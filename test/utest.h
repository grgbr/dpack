#ifndef _DPACK_UTEST_H
#define _DPACK_UTEST_H

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <cmocka.h>

extern bool dpack_utest_free_wrapped;
extern void free(void * ptr);
extern void dpack_utest_expect_free_arg(const void * data, size_t size);

#endif /* _DPACK_UTEST_H */
