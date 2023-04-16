#ifndef _DPACK_UTEST_H
#define _DPACK_UTEST_H

#include "dpack/codec.h"

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <cmocka.h>

struct dpack_scalar_utest_data;

typedef void (dpack_utest_unpack_fn)(struct dpack_decoder *,
                                     const struct dpack_scalar_utest_data *);

struct dpack_scalar_utest_data {
	const char *     packed;
	size_t           size;
	int              error;
	union {
		uint8_t  uint8;
		int8_t   int8;
		uint16_t uint16;
		int16_t  int16;
		uint32_t uint32;
		int32_t  int32;
		uint64_t uint64;
		int64_t  int64;
		float    f32;
		double   f64;
	};
};

struct dpack_scalar_utest {
	dpack_utest_unpack_fn *              unpack;
	const struct dpack_scalar_utest_data data[];
};

extern bool dpack_utest_free_wrapped;
extern void free(void * ptr);
extern void dpack_utest_expect_free_arg(const void * data, size_t size);

#endif /* _DPACK_UTEST_H */
