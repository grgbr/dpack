#include "utest.h"
#include "dpack/scalar.h"
#include "dpack/codec.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <errno.h>

#define DPACKUT_UINT64(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint64 = _value \
	}

static void
dpackut_uint64_encode(const struct dpackut_scalar_data * data)
{
	struct dpack_encoder enc = { 0, };
	size_t               sz = data->size;
	char                 buff[sz];

	memset(buff, 0xa5, sz);
	dpack_encoder_init_buffer(&enc, buff, sz);

	cute_check_uint(data->size, greater_equal, DPACK_UINT64_SIZE_MIN);
	cute_check_uint(data->size, lower_equal, DPACK_UINT64_SIZE_MAX);

	cute_check_sint(dpack_encode_uint64(&enc, data->value.uint64),
	                equal,
	                data->error);
	cute_check_mem(buff, equal, data->packed, sz);

	cute_check_uint(dpack_encoder_space_used(&enc), equal, sz);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint64_encode_assert)
{
	uint64_t             val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_encode_uint64(NULL, val));
	cute_expect_assertion(ret = dpack_encode_uint64(&enc, val));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint64_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint64_encode_0)
{
	/* 0 */
	DPACKUT_UINT64(data, "\x00", 0, UINT64_C(0));
	dpackut_uint64_encode(&data);
}

CUTE_TEST(dpackut_uint64_encode_1)
{
	/* 1 */
	DPACKUT_UINT64(data, "\x01", 0, UINT64_C(1));
	dpackut_uint64_encode(&data);
}

CUTE_TEST(dpackut_uint64_encode_maxplus0)
{
	/* 18446744073709551615 */
	DPACKUT_UINT64(data,
	               "\xcf\xff\xff\xff\xff\xff\xff\xff\xff",
	               0,
	               UINT64_MAX);
	dpackut_uint64_encode(&data);
}

static void
dpackut_uint64_decode(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	uint64_t             val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint64(&dec, &val), equal, data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint64);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT64_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT64_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint64_decode_assert)
{
	uint64_t             val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_UINT64_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint64(NULL, &val));
	cute_expect_assertion(ret = dpack_decode_uint64(&dec, &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint64(&dec, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint64_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint64_decode__1)
{
	/* -1 */
	DPACKUT_UINT64(data, "\xff", -ENOMSG, UINT64_C(0));
	dpackut_uint64_decode(&data);
}

CUTE_TEST(dpackut_uint64_decode_0)
{
	/* 0 */
	DPACKUT_UINT64(data, "\x00", 0, UINT64_C(0));
	dpackut_uint64_decode(&data);
}

CUTE_TEST(dpackut_uint64_decode_1)
{
	/* 1 */
	DPACKUT_UINT64(data, "\x01", 0, UINT64_C(1));
	dpackut_uint64_decode(&data);
}

CUTE_TEST(dpackut_uint64_decode_maxplus0)
{
	/* 18446744073709551615 */
	DPACKUT_UINT64(data,
	               "\xcf\xff\xff\xff\xff\xff\xff\xff\xff",
	               0,
	               UINT64_MAX);
	dpackut_uint64_decode(&data);
}

#define DPACKUT_UINT64_MIN(_var, _packed, _error, _value, _low) \
	const struct dpackut_scalar_data _var = { \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint64 = _value, \
		.low.uint64   = _low \
	}

static void
dpackut_uint64_decode_min(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	uint64_t             val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint64_min(&dec, data->low.uint64, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint64);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT64_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT64_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint64_decode_min_assert)
{
	uint64_t             val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_UINT64_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint64_min(NULL, 1, &val));
	cute_expect_assertion(ret = dpack_decode_uint64_min(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint64_min(&dec, 0, &val));
	cute_expect_assertion(ret = dpack_decode_uint64_min(&dec,
	                                                    UINT64_MAX,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_uint64_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint64_decode_min_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint64_decode_min__1_1)
{
	/* -1 */
	DPACKUT_UINT64_MIN(data, "\xff", -ENOMSG, 0, UINT64_C(1));
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min_0_1)
{
	/* 0 */
	DPACKUT_UINT64_MIN(data, "\x00", -ERANGE, 0, UINT64_C(1));
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min_1_1)
{
	/* 1 */
	DPACKUT_UINT64_MIN(data, "\x01", 0, UINT64_C(1), UINT64_C(1));
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min_halfmax_1)
{
	/* 9223372036854775807 */
	DPACKUT_UINT64_MIN(data, "\xcf\x7f\xff\xff\xff\xff\xff\xff\xff",
	                   0,
	                   UINT64_MAX / 2,
	                   UINT64_C(1));
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min_max_1)
{
	/* 18446744073709551615 */
	DPACKUT_UINT64_MIN(data, "\xcf\xff\xff\xff\xff\xff\xff\xff\xff",
	                   0,
	                   UINT64_MAX,
	                   UINT64_C(1));
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min__1_halfmax)
{
	/* -1 */
	DPACKUT_UINT64_MIN(data, "\xff", -ENOMSG, 0, UINT64_MAX / 2);
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min__0_halfmax)
{
	/* 0 */
	DPACKUT_UINT64_MIN(data, "\x00", -ERANGE, 0, UINT64_MAX / 2);
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min_1_halfmax)
{
	/* 1 */
	DPACKUT_UINT64_MIN(data, "\x01", -ERANGE, 0, UINT64_MAX / 2);
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min_halfmaxminus1_halfmax)
{
	/* 9223372036854775806 */
	DPACKUT_UINT64_MIN(data,
	                   "\xcf\x7f\xff\xff\xff\xff\xff\xff\xfe",
	                   -ERANGE,
	                   0,
	                   UINT64_MAX / 2);
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min_halfmax_halfmax)
{
	/* 9223372036854775807 */
	DPACKUT_UINT64_MIN(data,
	                   "\xcf\x7f\xff\xff\xff\xff\xff\xff\xff",
	                   0,
	                   UINT64_MAX / 2,
	                   UINT64_MAX / 2);
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min_max_halfmax)
{
	/* 18446744073709551615 */
	DPACKUT_UINT64_MIN(data,
	                   "\xcf\xff\xff\xff\xff\xff\xff\xff\xff",
	                   0,
	                   UINT64_MAX,
	                   UINT64_MAX / 2);
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min_maxminus2_maxminus1)
{
	/* 18446744073709551613 */
	DPACKUT_UINT64_MIN(data,
	                   "\xcf\xff\xff\xff\xff\xff\xff\xff\xfd",
	                   -ERANGE,
	                   0,
	                   UINT64_MAX - 1);
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min_maxminus1_maxminus1)
{
	/* 18446744073709551614 */
	DPACKUT_UINT64_MIN(data,
	                   "\xcf\xff\xff\xff\xff\xff\xff\xff\xfe",
	                   0,
	                   UINT64_MAX - 1,
	                   UINT64_MAX - 1);
	dpackut_uint64_decode_min(&data);
}

CUTE_TEST(dpackut_uint64_decode_min_max_maxminus1)
{
	/* 18446744073709551615 */
	DPACKUT_UINT64_MIN(data,
	                   "\xcf\xff\xff\xff\xff\xff\xff\xff\xff",
	                   0,
	                   UINT64_MAX,
	                   UINT64_MAX - 1);
	dpackut_uint64_decode_min(&data);
}

#define DPACKUT_UINT64_MAX(_var, _packed, _error, _value, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint64 = _value, \
		.high.uint64  = _high \
	}

static void
dpackut_uint64_decode_max(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	uint64_t             val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint64_max(&dec, data->high.uint64, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint64);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT64_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT64_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint64_decode_max_assert)
{
	uint64_t             val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_UINT64_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint64_max(NULL, 1, &val));
	cute_expect_assertion(ret = dpack_decode_uint64_max(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint64_max(&dec, 0, &val));
	cute_expect_assertion(ret = dpack_decode_uint64_max(&dec,
	                                                    UINT64_MAX,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_uint64_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint64_decode_max_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint64_decode_max__1_1)
{
	/* -1 */
	DPACKUT_UINT64_MAX(data, "\xff", -ENOMSG, 0, UINT64_C(1));
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max_0_1)
{
	/* 0 */
	DPACKUT_UINT64_MAX(data,
	                   "\x00",
	                   0,
	                   UINT64_C(0),
	                   UINT64_C(1));
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max_1_1)
{
	/* 1 */
	DPACKUT_UINT64_MAX(data,
	                   "\x01",
	                   0,
	                   UINT64_C(1),
	                   UINT64_C(1));
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max_halfmax_1)
{
	/* 9223372036854775807 */
	DPACKUT_UINT64_MAX(data,
	                   "\xcf\x7f\xff\xff\xff\xff\xff\xff\xff",
	                   -ERANGE,
	                   0,
	                   UINT64_C(1));
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max_max_1)
{
	/* 18446744073709551615 */
	DPACKUT_UINT64_MAX(data,
	                   "\xcf\xff\xff\xff\xff\xff\xff\xff\xff",
	                   -ERANGE,
	                   0,
	                   UINT64_C(1));
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max__1_halfmax)
{
	/* -1 */
	DPACKUT_UINT64_MAX(data,
	                   "\xff",
	                   -ENOMSG,
	                   0,
	                   UINT64_MAX / 2);
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max_0_halfmax)
{
	/* 0 */
	DPACKUT_UINT64_MAX(data,
	                   "\x00",
	                   0,
	                   UINT64_C(0),
	                   UINT64_MAX / 2);
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max_1_halfmax)
{
	/* 1 */
	DPACKUT_UINT64_MAX(data,
	                   "\x01",
	                   0,
	                   UINT64_C(1),
	                   UINT64_MAX / 2);
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max_halfmaxminus1_halfmax)
{
	/* 9223372036854775806 */
	DPACKUT_UINT64_MAX(data,
	                   "\xcf\x7f\xff\xff\xff\xff\xff\xff\xfe",
	                   0,
	                   (UINT64_MAX / 2) - 1,
	                   UINT64_MAX / 2);
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max_halfmax_halfmax)
{
	/* 9223372036854775807 */
	DPACKUT_UINT64_MAX(data,
	                   "\xcf\x7f\xff\xff\xff\xff\xff\xff\xff",
	                   0,
	                   UINT64_MAX / 2,
	                   UINT64_MAX / 2);
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max_max_halfmax)
{
	/* 18446744073709551615 */
	DPACKUT_UINT64_MAX(data,
	                   "\xcf\xff\xff\xff\xff\xff\xff\xff\xff",
	                   -ERANGE,
	                   0,
	                   UINT64_MAX / 2);
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max_maxminus2_maxminus1)
{
	/* 18446744073709551613 */
	DPACKUT_UINT64_MAX(data,
	                   "\xcf\xff\xff\xff\xff\xff\xff\xff\xfd",
	                   0,
	                   UINT64_MAX - 2,
	                   UINT64_MAX - 1);
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max_maxminus1_maxminus1)
{
	/* 18446744073709551614 */
	DPACKUT_UINT64_MAX(data,
	                   "\xcf\xff\xff\xff\xff\xff\xff\xff\xfe",
	                   0,
	                   UINT64_MAX - 1,
	                   UINT64_MAX - 1);
	dpackut_uint64_decode_max(&data);
}

CUTE_TEST(dpackut_uint64_decode_max_max_maxminus1)
{
	/* 18446744073709551615 */
	DPACKUT_UINT64_MAX(data,
	                   "\xcf\xff\xff\xff\xff\xff\xff\xff\xff",
	                   -ERANGE,
	                   0,
	                   UINT64_MAX - 1);
	dpackut_uint64_decode_max(&data);
}

#define DPACKUT_UINT64_RANGE(_var, _packed, _error, _value, _low, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint64 = _value, \
		.low.uint64   = _low, \
		.high.uint64  = _high \
	}

static void
dpackut_uint64_decode_range(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	uint64_t             val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_uint64_range(&dec,
	                                         data->low.uint64,
	                                         data->high.uint64,
	                                         &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_uint(val, equal, data->value.uint64);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_UINT64_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_UINT64_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_uint64_decode_range_assert)
{
	uint64_t             val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_UINT64_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint64_range(NULL,
	                                                      1,
	                                                      2,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint64_range(&dec,
	                                                      1,
	                                                      2,
	                                                      &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_uint64_range(&dec,
	                                                      0,
	                                                      2,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint64_range(&dec,
	                                                      1,
	                                                      UINT64_MAX,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint64_range(&dec,
	                                                      2,
	                                                      2,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint64_range(&dec,
	                                                      1,
	                                                      2,
	                                                      NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint64_decode_range_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_uint64_decode_range__1_1_2)
{
	/* -1 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xff",
	                     -ENOMSG,
	                     0,
	                     UINT64_C(1),
	                     UINT64_C(2));
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_0_1_2)
{
	/* 0 */
	DPACKUT_UINT64_RANGE(data,
	                     "\x00",
	                     -ERANGE,
	                     0,
	                     UINT64_C(1),
	                     UINT64_C(2));
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_1_1_2)
{
	/* 1 */
	DPACKUT_UINT64_RANGE(data,
	                     "\x01",
	                     0,
	                     UINT64_C(1),
	                     UINT64_C(1),
	                     UINT64_C(2));
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_2_1_2)
{
	/* 2 */
	DPACKUT_UINT64_RANGE(data,
	                     "\x02",
	                     0,
	                     UINT64_C(2),
	                     UINT64_C(1),
	                     UINT64_C(2));
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_3_1_2)
{
	/* 3 */
	DPACKUT_UINT64_RANGE(data,
	                     "\x03",
	                     -ERANGE,
	                     0,
	                     UINT64_C(1),
	                     UINT64_C(2));
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_halfmax_1_2)
{
	/* 9223372036854775807 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xcf\x7f\xff\xff\xff\xff\xff\xff\xff",
	                     -ERANGE,
	                     0,
	                     UINT64_C(1),
	                     UINT64_C(2));
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_max_halfmaxminus1_halfmax)
{
	/* 18446744073709551615 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xcf\xff\xff\xff\xff\xff\xff\xff\xff",
	                     -ERANGE,
	                     0,
	                     (UINT64_MAX / 2) - 1,
	                     UINT64_MAX / 2);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range__1_halfmaxminus1_halfmax)
{
	/* -1 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xff",
	                     -ENOMSG,
	                     0,
	                     (UINT64_MAX / 2) - 1,
	                     UINT64_MAX / 2);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_0_halfmaxminus1_halfmax)
{
	/* 0 */
	DPACKUT_UINT64_RANGE(data,
	                     "\x00",
	                     -ERANGE,
	                     0,
	                     (UINT64_MAX / 2) - 1,
	                     UINT64_MAX / 2);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_halfmaxminus2_halfmaxminus1_halfmax)
{
	/* 9223372036854775805 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xcf\x7f\xff\xff\xff\xff\xff\xff\xfd",
	                     -ERANGE,
	                     0,
	                     (UINT64_MAX / 2) - 1,
	                     UINT64_MAX / 2);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_halfmaxminus1_halfmaxminus1_halfmax)
{
	/* 9223372036854775806 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xcf\x7f\xff\xff\xff\xff\xff\xff\xfe",
	                     0,
	                     (UINT64_MAX / 2) - 1,
	                     (UINT64_MAX / 2) - 1,
	                     UINT64_MAX / 2);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_halfmax_halfmaxminus1_halfmax)
{
	/* 9223372036854775807 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xcf\x7f\xff\xff\xff\xff\xff\xff\xff",
	                     0,
	                     UINT64_MAX / 2,
	                     (UINT64_MAX / 2) - 1,
	                     UINT64_MAX / 2);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_halfmaxplus1_halfmaxminus1_halfmax)
{
	/* 9223372036854775808 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xcf\x80\x00\x00\x00\x00\x00\x00\x00",
	                     -ERANGE,
	                     0,
	                     (UINT64_MAX / 2) - 1,
	                     UINT64_MAX / 2);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_max_halfmaxminus1_maxminus1)
{
	/* 18446744073709551615 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xcf\xff\xff\xff\xff\xff\xff\xff\xff",
	                     -ERANGE,
	                     0,
	                     (UINT64_MAX / 2) - 1,
	                     UINT64_MAX - 1);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range__1_halfmaxminus1_maxminus1)
{
	/* -1 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xff",
	                     -ENOMSG,
	                     0,
	                     (UINT64_MAX / 2) - 1,
	                     UINT64_MAX - 1);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_0_halfmaxminus1_maxminus1)
{
	/* 0 */
	DPACKUT_UINT64_RANGE(data,
	                     "\x00",
	                     -ERANGE,
	                     0,
	                     (UINT64_MAX / 2) - 1,
	                     UINT64_MAX - 1);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_maxminus3_maxminus2_maxminus1)
{
	/* 18446744073709551612 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xcf\xff\xff\xff\xff\xff\xff\xff\xfc",
	                     -ERANGE,
	                     0,
	                     UINT64_MAX - 2,
	                     UINT64_MAX - 1);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_maxminus2_maxminus2_maxminus1)
{
	/* 18446744073709551613 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xcf\xff\xff\xff\xff\xff\xff\xff\xfd",
	                     0,
	                     UINT64_MAX - 2,
	                     UINT64_MAX - 2,
	                     UINT64_MAX - 1);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_maxminus1_maxminus2_maxminus1)
{
	/* 18446744073709551614 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xcf\xff\xff\xff\xff\xff\xff\xff\xfe",
	                     0,
	                     UINT64_MAX - 1,
	                     UINT64_MAX - 2,
	                     UINT64_MAX - 1);
	dpackut_uint64_decode_range(&data);
}

CUTE_TEST(dpackut_uint64_decode_range_max_maxminus2_maxminus1)
{
	/* 18446744073709551615 */
	DPACKUT_UINT64_RANGE(data,
	                     "\xcf\xff\xff\xff\xff\xff\xff\xff\xff",
	                     -ERANGE,
	                     0,
	                     UINT64_MAX - 2,
	                     UINT64_MAX - 1);
	dpackut_uint64_decode_range(&data);
}

#if 0
};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint64_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint64_range(NULL,
	                                                      1,
	                                                      2,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint64_range(&dec,
	                                                      1,
	                                                      2,
	                                                      &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	cute_expect_assertion(ret = dpack_decode_uint64_range(&dec,
	                                                      0,
	                                                      2,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint64_range(&dec,
	                                                      1,
	                                                      UINT64_MAX,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint64_range(&dec,
	                                                      2,
	                                                      2,
	                                                      &val));
	cute_expect_assertion(ret = dpack_decode_uint64_range(&dec,
	                                                      1,
	                                                      2,
	                                                      NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint64_range);
}

#endif

CUTE_GROUP(dpackut_uint64_group) = {
	CUTE_REF(dpackut_uint64_encode_assert),
	CUTE_REF(dpackut_uint64_encode_0),
	CUTE_REF(dpackut_uint64_encode_1),
	CUTE_REF(dpackut_uint64_encode_maxplus0),

	CUTE_REF(dpackut_uint64_decode_assert),
	CUTE_REF(dpackut_uint64_decode__1),
	CUTE_REF(dpackut_uint64_decode_0),
	CUTE_REF(dpackut_uint64_decode_1),
	CUTE_REF(dpackut_uint64_decode_maxplus0),

	CUTE_REF(dpackut_uint64_decode_min_assert),
	CUTE_REF(dpackut_uint64_decode_min__1_1),
	CUTE_REF(dpackut_uint64_decode_min_assert),
	CUTE_REF(dpackut_uint64_decode_min__1_1),
	CUTE_REF(dpackut_uint64_decode_min_0_1),
	CUTE_REF(dpackut_uint64_decode_min_1_1),
	CUTE_REF(dpackut_uint64_decode_min_halfmax_1),
	CUTE_REF(dpackut_uint64_decode_min_max_1),
	CUTE_REF(dpackut_uint64_decode_min__1_halfmax),
	CUTE_REF(dpackut_uint64_decode_min__0_halfmax),
	CUTE_REF(dpackut_uint64_decode_min_1_halfmax),
	CUTE_REF(dpackut_uint64_decode_min_halfmaxminus1_halfmax),
	CUTE_REF(dpackut_uint64_decode_min_halfmax_halfmax),
	CUTE_REF(dpackut_uint64_decode_min_max_halfmax),
	CUTE_REF(dpackut_uint64_decode_min_maxminus2_maxminus1),
	CUTE_REF(dpackut_uint64_decode_min_maxminus1_maxminus1),
	CUTE_REF(dpackut_uint64_decode_min_max_maxminus1),

	CUTE_REF(dpackut_uint64_decode_max_assert),
	CUTE_REF(dpackut_uint64_decode_max__1_1),
	CUTE_REF(dpackut_uint64_decode_max_0_1),
	CUTE_REF(dpackut_uint64_decode_max_1_1),
	CUTE_REF(dpackut_uint64_decode_max_halfmax_1),
	CUTE_REF(dpackut_uint64_decode_max_max_1),
	CUTE_REF(dpackut_uint64_decode_max__1_halfmax),
	CUTE_REF(dpackut_uint64_decode_max_0_halfmax),
	CUTE_REF(dpackut_uint64_decode_max_1_halfmax),
	CUTE_REF(dpackut_uint64_decode_max_halfmaxminus1_halfmax),
	CUTE_REF(dpackut_uint64_decode_max_halfmax_halfmax),
	CUTE_REF(dpackut_uint64_decode_max_max_halfmax),
	CUTE_REF(dpackut_uint64_decode_max_maxminus2_maxminus1),
	CUTE_REF(dpackut_uint64_decode_max_maxminus1_maxminus1),
	CUTE_REF(dpackut_uint64_decode_max_max_maxminus1),

	CUTE_REF(dpackut_uint64_decode_range_assert),
	CUTE_REF(dpackut_uint64_decode_range__1_1_2),
	CUTE_REF(dpackut_uint64_decode_range_0_1_2),
	CUTE_REF(dpackut_uint64_decode_range_1_1_2),
	CUTE_REF(dpackut_uint64_decode_range_2_1_2),
	CUTE_REF(dpackut_uint64_decode_range_3_1_2),
	CUTE_REF(dpackut_uint64_decode_range_halfmax_1_2),
	CUTE_REF(dpackut_uint64_decode_range_max_halfmaxminus1_halfmax),
	CUTE_REF(dpackut_uint64_decode_range__1_halfmaxminus1_halfmax),
	CUTE_REF(dpackut_uint64_decode_range_0_halfmaxminus1_halfmax),
	CUTE_REF(dpackut_uint64_decode_range_halfmaxminus2_halfmaxminus1_halfmax),
	CUTE_REF(dpackut_uint64_decode_range_halfmaxminus1_halfmaxminus1_halfmax),
	CUTE_REF(dpackut_uint64_decode_range_halfmax_halfmaxminus1_halfmax),
	CUTE_REF(dpackut_uint64_decode_range_halfmaxplus1_halfmaxminus1_halfmax),
	CUTE_REF(dpackut_uint64_decode_range_max_halfmaxminus1_maxminus1),
	CUTE_REF(dpackut_uint64_decode_range__1_halfmaxminus1_maxminus1),
	CUTE_REF(dpackut_uint64_decode_range_0_halfmaxminus1_maxminus1),
	CUTE_REF(dpackut_uint64_decode_range_maxminus3_maxminus2_maxminus1),
	CUTE_REF(dpackut_uint64_decode_range_maxminus2_maxminus2_maxminus1),
	CUTE_REF(dpackut_uint64_decode_range_maxminus1_maxminus2_maxminus1),
	CUTE_REF(dpackut_uint64_decode_range_max_maxminus2_maxminus1)
};

CUTE_SUITE_EXTERN(dpackut_uint64_suite,
                  dpackut_uint64_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
