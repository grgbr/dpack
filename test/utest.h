#ifndef _DPACK_UTEST_H
#define _DPACK_UTEST_H

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <cmocka.h>

union dpack_scalar_utest_value {
	bool     boolean;
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

struct dpack_scalar_utest_data {
	const char *                   packed;
	size_t                         size;
	int                            error;
	union dpack_scalar_utest_value value;
	union dpack_scalar_utest_value low;
	union dpack_scalar_utest_value high;
};

struct dpack_decoder;

typedef void (dpack_utest_unpack_fn)(struct dpack_decoder *,
                                     const struct dpack_scalar_utest_data *);

struct dpack_encoder;

typedef int (dpack_utest_pack_fn)(struct dpack_encoder *,
                                  const struct dpack_scalar_utest_data *);

extern void
dpack_scalar_utest_decode(const struct dpack_scalar_utest_data * data,
                          unsigned int                           nr,
                          dpack_utest_unpack_fn *                unpack);

extern void
dpack_scalar_utest_encode(const struct dpack_scalar_utest_data * data,
                          unsigned int                           nr,
                          dpack_utest_pack_fn *                  pack);

extern bool dpack_utest_free_wrapped;
extern void free(void * ptr);
extern void dpack_utest_expect_free_arg(const void * data, size_t size);

#endif /* _DPACK_UTEST_H */
