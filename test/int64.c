#include "utest.h"
#include "dpack/scalar.h"
#include "dpack/codec.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <errno.h>

#define DPACKUT_INT64(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int64 = _value \
	}

static void
dpackut_int64_encode(const struct dpackut_scalar_data * data)
{
	struct dpack_encoder enc = { 0, };
	size_t               sz = data->size;
	char                 buff[sz];

	memset(buff, 0xa5, sz);
	dpack_encoder_init_buffer(&enc, buff, sz);

	cute_check_uint(data->size, greater_equal, DPACK_INT64_SIZE_MIN);
	cute_check_uint(data->size, lower_equal, DPACK_INT64_SIZE_MAX);

	cute_check_sint(dpack_encode_int64(&enc, data->value.int64),
	                equal,
	                data->error);
	cute_check_mem(buff, equal, data->packed, sz);

	cute_check_uint(dpack_encoder_space_used(&enc), equal, sz);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int64_encode_assert)
{
	int64_t              val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_encode_int64(NULL, val));
	cute_expect_assertion(ret = dpack_encode_int64(&enc, val));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int64_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int64_encode_minplus0)
{
	/* -9223372036854775808 */
	DPACKUT_INT64(data,
	              "\xd3\x80\x00\x00\x00\x00\x00\x00\x00",
	              0,
	              INT64_MIN);
	dpackut_int64_encode(&data);
}

CUTE_TEST(dpackut_int64_encode_minplus1)
{
	/* -9223372036854775807 */
	DPACKUT_INT64(data,
	              "\xd3\x80\x00\x00\x00\x00\x00\x00\x01",
	              0,
	              INT64_MIN + 1);
	dpackut_int64_encode(&data);
}

CUTE_TEST(dpackut_int64_encode__1)
{
	/* -1 */
	DPACKUT_INT64(data, "\xff", 0, INT64_C(-1));
	dpackut_int64_encode(&data);
}

CUTE_TEST(dpackut_int64_encode_0)
{
	/* 0 */
	DPACKUT_INT64(data, "\x00", 0, INT64_C(0));
	dpackut_int64_encode(&data);
}

CUTE_TEST(dpackut_int64_encode_1)
{
	/* 1 */
	DPACKUT_INT64(data, "\x01", 0, INT64_C(1));
	dpackut_int64_encode(&data);
}

CUTE_TEST(dpackut_int64_encode_maxplus0)
{
	/* 9223372036854775807 */
	DPACKUT_INT64(data,
	              "\xcf\x7f\xff\xff\xff\xff\xff\xff\xff",
	              0,
	              INT64_MAX);
	dpackut_int64_encode(&data);
}

static void
dpackut_int64_decode(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int64_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int64(&dec, &val), equal, data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int64);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT64_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT64_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int64_decode_assert)
{
	int64_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT64_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int64(NULL, &val));
	cute_expect_assertion(ret = dpack_decode_int64(&dec, &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int64(&dec, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int64_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int64_decode_maxplus1)
{
	/* 9223372036854775808 */
	DPACKUT_INT64(data,
	              "\xcf\x80\x00\x00\x00\x00\x00\x00\x00",
	              -ERANGE,
	              INT64_C(0));
	dpackut_int64_decode(&data);
}

CUTE_TEST(dpackut_int64_decode_minplus0)
{
	/* -9223372036854775808 */
	DPACKUT_INT64(data,
	              "\xd3\x80\x00\x00\x00\x00\x00\x00\x00",
	              0,
	              INT64_MIN);
	dpackut_int64_decode(&data);
}

CUTE_TEST(dpackut_int64_decode_minplus1)
{
	/* -9223372036854775807 */
	DPACKUT_INT64(data,
	              "\xd3\x80\x00\x00\x00\x00\x00\x00\x01",
	              0,
	              INT64_MIN + 1);
	dpackut_int64_decode(&data);
}

CUTE_TEST(dpackut_int64_decode__1)
{
	/* -1 */
	DPACKUT_INT64(data, "\xff", 0, INT64_C(-1));
	dpackut_int64_decode(&data);
}

CUTE_TEST(dpackut_int64_decode_0)
{
	/* 0 */
	DPACKUT_INT64(data, "\x00", 0, INT64_C(0));
	dpackut_int64_decode(&data);
}

CUTE_TEST(dpackut_int64_decode_1)
{
	/* 1 */
	DPACKUT_INT64(data, "\x01", 0, INT64_C(1));
	dpackut_int64_decode(&data);
}

CUTE_TEST(dpackut_int64_decode_maxplus0)
{
	/* 9223372036854775807 */
	DPACKUT_INT64(data,
	              "\xcf\x7f\xff\xff\xff\xff\xff\xff\xff",
	              0,
	              INT64_MAX);
	dpackut_int64_decode(&data);
}

#if 0
#define DPACKUT_INT64_MIN(_var, _packed, _error, _value, _low) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int64 = _value, \
		.low.int64   = _low \
	}

static void
dpack_scalar_utest_unpack_int64_min(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	int64_t val;

	assert_int_equal(dpack_decode_int64_min(decoder, data->low.int64, &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT64_SIZE_MIN <= data->size);
	assert_true(DPACK_INT64_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int64);
}

static void
dpack_scalar_utest_decode_int64_min(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -9223372036854775808 */
		DPACKUT_INT64_MIN("\xd3"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x00", -ERANGE, 0,             INT64_MIN + 1),
		/* -9223372036854775807 */
		DPACKUT_INT64_MIN("\xd3"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x01", 0,       INT64_MIN + 1, INT64_MIN + 1),
		/* -9223372036854775806 */
		DPACKUT_INT64_MIN("\xd3"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x02", 0,       INT64_MIN + 2, INT64_MIN + 1),

		/* -1 */
		DPACKUT_INT64_MIN("\xff",             -ERANGE, 0,             INT64_C(0)),
		/* 0 */
		DPACKUT_INT64_MIN("\x00",             0,       INT64_C(0),    INT64_C(0)),
		/* 1 */
		DPACKUT_INT64_MIN("\x01",             0,       INT64_C(1),    INT64_C(0)),

		/* 9223372036854775807 */
		DPACKUT_INT64_MIN("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xff", 0,       INT64_MAX,     INT64_C(0)),
		/* 9223372036854775808 */
		DPACKUT_INT64_MIN("\xcf"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x00", -ERANGE, 0,             INT64_C(0)),

		/* 9223372036854775805 */
		DPACKUT_INT64_MIN("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xfd", -ERANGE, 0,             INT64_MAX - 1),
		/* 9223372036854775806 */
		DPACKUT_INT64_MIN("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xfe", 0,       INT64_MAX - 1, INT64_MAX - 1),
		/* 9223372036854775807 */
		DPACKUT_INT64_MIN("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xff", 0,       INT64_MAX,     INT64_MAX - 1),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int64_t               val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int64_min(NULL, 1, &val));
	cute_expect_assertion(ret = dpack_decode_int64_min(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	cute_expect_assertion(ret = dpack_decode_int64_min(&dec,
	                                                   INT64_MIN,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_int64_min(&dec,
	                                                   INT64_MAX,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_int64_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int64_min);
}

#define DPACKUT_INT64_MAX(_var, _packed, _error, _value, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int64 = _value, \
		.high.int64  = _high \
	}

static void
dpack_scalar_utest_unpack_int64_max(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	int64_t val;

	assert_int_equal(dpack_decode_int64_max(decoder,
	                                        data->high.int64,
	                                        &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT64_SIZE_MIN <= data->size);
	assert_true(DPACK_INT64_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int64);
}

static void
dpack_scalar_utest_decode_int64_max(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -9223372036854775808 */
		DPACKUT_INT64_MAX("\xd3"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x00", 0,       INT64_MIN,     INT64_MIN + 1),
		/* -9223372036854775807 */
		DPACKUT_INT64_MAX("\xd3"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x01", 0,       INT64_MIN + 1, INT64_MIN + 1),
		/* -9223372036854775806 */
		DPACKUT_INT64_MAX("\xd3"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x02", -ERANGE, 0,             INT64_MIN + 1),

		/* -1 */
		DPACKUT_INT64_MAX("\xff",             0,       INT64_C(-1),   INT64_C(0)),
		/* 0 */
		DPACKUT_INT64_MAX("\x00",             0,       INT64_C(0),    INT64_C(0)),
		/* 1 */
		DPACKUT_INT64_MAX("\x01",             -ERANGE, 0,             INT64_C(0)),

		/* 9223372036854775807 */
		DPACKUT_INT64_MAX("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xff", -ERANGE, 0,             INT64_C(0)),
		/* 9223372036854775808 */
		DPACKUT_INT64_MAX("\xcf"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x00", -ERANGE, 0,             INT64_C(0)),

		/* 9223372036854775805 */
		DPACKUT_INT64_MAX("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xfd", 0,       INT64_MAX - 2, INT64_MAX - 1),
		/* 9223372036854775806 */
		DPACKUT_INT64_MAX("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xfe", 0,       INT64_MAX - 1, INT64_MAX - 1),
		/* 9223372036854775807 */
		DPACKUT_INT64_MAX("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xff", -ERANGE, 0,             INT64_MAX - 1),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int64_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int64_max(NULL, 1, &val));
	cute_expect_assertion(ret = dpack_decode_int64_max(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	cute_expect_assertion(ret = dpack_decode_int64_max(&dec,
	                                                   INT64_MIN,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_int64_max(&dec,
	                                                   INT64_MAX,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_int64_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int64_max);
}

#define DPACKUT_INT64_RANGE(_var, _packed, _error, _value, _low, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int64 = _value, \
		.low.int64   = _low, \
		.high.int64  = _high \
	}

static void
dpack_scalar_utest_unpack_int64_range(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	int64_t val;

	assert_int_equal(dpack_decode_int64_range(decoder,
	                                          data->low.int64,
	                                          data->high.int64,
	                                          &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT64_SIZE_MIN <= data->size);
	assert_true(DPACK_INT64_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int64);
}

static void
dpack_scalar_utest_decode_int64_range(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -9223372036854775808 */
		DPACKUT_INT64_RANGE("\xd3"
		                        "\x80\x00\x00\x00"
		                        "\x00\x00\x00\x00", -ERANGE, 0,             INT64_MIN + 1, INT64_MIN + 2),
		/* -9223372036854775807 */
		DPACKUT_INT64_RANGE("\xd3"
		                        "\x80\x00\x00\x00"
		                        "\x00\x00\x00\x01", 0,       INT64_MIN + 1, INT64_MIN + 1, INT64_MIN + 2),
		/* -9223372036854775806 */
		DPACKUT_INT64_RANGE("\xd3"
		                        "\x80\x00\x00\x00"
		                        "\x00\x00\x00\x02", 0,       INT64_MIN + 2, INT64_MIN + 1, INT64_MIN + 2),
		/* -9223372036854775805 */
		DPACKUT_INT64_RANGE("\xd3"
		                        "\x80\x00\x00\x00"
		                        "\x00\x00\x00\x03", -ERANGE, 0,             INT64_MIN + 1, INT64_MIN + 2),

		/* -1 */
		DPACKUT_INT64_RANGE("\xff",             -ERANGE, 0,             INT64_C(0),    INT64_C(1)),
		/* 0 */
		DPACKUT_INT64_RANGE("\x00",             0,       INT64_C(0),    INT64_C(0),    INT64_C(1)),
		/* 1 */
		DPACKUT_INT64_RANGE("\x01",             0,       INT64_C(1),    INT64_C(0),    INT64_C(1)),
		/* 2 */
		DPACKUT_INT64_RANGE("\x02",             -ERANGE, 0,             INT64_C(0),    INT64_C(1)),

		/* 9223372036854775804 */
		DPACKUT_INT64_RANGE("\xcf"
		                        "\x7f\xff\xff\xff"
		                        "\xff\xff\xff\xfc", -ERANGE, 0,             INT64_MAX - 2, INT64_MAX - 1),
		/* 9223372036854775805 */
		DPACKUT_INT64_RANGE("\xcf"
		                        "\x7f\xff\xff\xff"
		                        "\xff\xff\xff\xfd", 0,       INT64_MAX - 2, INT64_MAX - 2, INT64_MAX - 1),
		/* 9223372036854775806 */
		DPACKUT_INT64_RANGE("\xcf"
		                        "\x7f\xff\xff\xff"
		                        "\xff\xff\xff\xfe", 0,       INT64_MAX - 1, INT64_MAX - 2, INT64_MAX - 1),
		/* 9223372036854775807 */
		DPACKUT_INT64_RANGE("\xcf"
		                        "\x7f\xff\xff\xff"
		                        "\xff\xff\xff\xff", -ERANGE, 0,             INT64_MAX - 2, INT64_MAX - 1),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int64_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int64_range(NULL, 1, 2, &val));
	cute_expect_assertion(ret = dpack_decode_int64_range(&dec, 1, 2, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	cute_expect_assertion(ret = dpack_decode_int64_range(&dec,
	                                                     INT64_MIN,
	                                                     2,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_int64_range(&dec,
	                                                     1,
	                                                     INT64_MAX,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_int64_range(&dec, 2, 2, &val));
	cute_expect_assertion(ret = dpack_decode_int64_range(&dec, 1, 2, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int64_range);
}

#endif

CUTE_GROUP(dpackut_int64_group) = {
	CUTE_REF(dpackut_int64_encode_assert),
	CUTE_REF(dpackut_int64_encode_minplus0),
	CUTE_REF(dpackut_int64_encode_minplus1),
	CUTE_REF(dpackut_int64_encode__1),
	CUTE_REF(dpackut_int64_encode_0),
	CUTE_REF(dpackut_int64_encode_1),
	CUTE_REF(dpackut_int64_encode_maxplus0),

	CUTE_REF(dpackut_int64_decode_assert),
	CUTE_REF(dpackut_int64_decode_maxplus1),
	CUTE_REF(dpackut_int64_decode_minplus0),
	CUTE_REF(dpackut_int64_decode_minplus1),
	CUTE_REF(dpackut_int64_decode__1),
	CUTE_REF(dpackut_int64_decode_0),
	CUTE_REF(dpackut_int64_decode_1),
	CUTE_REF(dpackut_int64_decode_maxplus0),
};

CUTE_SUITE_EXTERN(dpackut_int64_suite,
                  dpackut_int64_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
