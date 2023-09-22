#include "utest.h"
#include "dpack/scalar.h"
#include "dpack/codec.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <errno.h>

#define DPACKUT_INT8(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.int8 = _value \
	}

static void
dpackut_int8_encode(const struct dpackut_scalar_data * data)
{
	struct dpack_encoder enc = { 0, };
	size_t               sz = data->size;
	char                 buff[sz];

	memset(buff, 0xa5, sz);
	dpack_encoder_init_buffer(&enc, buff, sz);

	cute_check_uint(data->size, greater_equal, DPACK_INT8_SIZE_MIN);
	cute_check_uint(data->size, lower_equal, DPACK_INT8_SIZE_MAX);

	cute_check_sint(dpack_encode_int8(&enc, data->value.int8),
	                equal,
	                data->error);
	cute_check_mem(buff, equal, data->packed, sz);

	cute_check_uint(dpack_encoder_space_used(&enc), equal, sz);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int8_encode_assert)
{
	int8_t               val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_encode_int8(NULL, val));
	cute_expect_assertion(ret = dpack_encode_int8(&enc, val));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_encode__128)
{
	/* -128 */
	DPACKUT_INT8(data, "\xd0\x80", 0, INT8_C(-128));
	dpackut_int8_encode(&data);
}

CUTE_TEST(dpackut_int8_encode__127)
{
	/* -127 */
	DPACKUT_INT8(data, "\xd0\x81", 0, INT8_C(-127));
	dpackut_int8_encode(&data);
}

CUTE_TEST(dpackut_int8_encode__1)
{
	/* -1 */
	DPACKUT_INT8(data, "\xff", 0, INT8_C(-1));
	dpackut_int8_encode(&data);
}

CUTE_TEST(dpackut_int8_encode_0)
{
	/* 0 */
	DPACKUT_INT8(data, "\x00", 0, INT8_C(0));
	dpackut_int8_encode(&data);
}

CUTE_TEST(dpackut_int8_encode_1)
{
	/* 1 */
	DPACKUT_INT8(data, "\x01", 0, INT8_C(1));
	dpackut_int8_encode(&data);
}

CUTE_TEST(dpackut_int8_encode_127)
{
	/* 127 */
	DPACKUT_INT8(data, "\x7f", 0, INT8_C(127));
	dpackut_int8_encode(&data);
}

static void
dpackut_int8_decode(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int8_t               val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int8(&dec, &val), equal, data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int8);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT8_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT8_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int8_decode_assert)
{
	int8_t               val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT8_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int8(NULL, &val));
	cute_expect_assertion(ret = dpack_decode_int8(&dec, &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int8(&dec, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_decode__129)
{
	/* -129 */
	DPACKUT_INT8(data, "\xd1\xff\x7f", -ERANGE, 0);
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode__128)
{
	/* -128 */
	DPACKUT_INT8(data, "\xd0\x80", 0, INT8_C(-128));
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode__127)
{
	/* -127 */
	DPACKUT_INT8(data, "\xd0\x81", 0, INT8_C(-127));
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode__1)
{
	/* -1 */
	DPACKUT_INT8(data, "\xff", 0, INT8_C(-1));
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode_0)
{
	/* 0 */
	DPACKUT_INT8(data, "\x00", 0, INT8_C(0));
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode_1)
{
	/* 1 */
	DPACKUT_INT8(data, "\x01", 0, INT8_C(1));
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode_127)
{
	/* 127 */
	DPACKUT_INT8(data, "\x7f", 0, INT8_C(127));
	dpackut_int8_decode(&data);
}

CUTE_TEST(dpackut_int8_decode_128)
{
	/* 128 */
	DPACKUT_INT8(data, "\xcc\x80", -ERANGE, 0);
	dpackut_int8_decode(&data);
}

#define DPACKUT_INT8_MIN(_var, _packed, _error, _value, _low) \
	const struct dpackut_scalar_data _var = { \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.int8 = _value, \
		.low.int8   = _low \
	}

static void
dpackut_int8_decode_min(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int8_t               val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int8_min(&dec, data->low.int8, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int8);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT8_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT8_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if  defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int8_decode_min_assert)
{
	int8_t               val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT8_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int8_min(NULL, 1, &val));
	cute_expect_assertion(ret = dpack_decode_int8_min(&dec, 1, &val));
	cute_expect_assertion(ret = dpack_decode_int8_min(&dec,
	                                                  INT8_MIN,
	                                                  &val));
	cute_expect_assertion(ret = dpack_decode_int8_min(&dec,
	                                                  INT8_MAX,
	                                                  &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int8_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_decode_min_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int8_decode_min__129_0)
{
	/* -129 */
	DPACKUT_INT8_MIN(data, "\xd1\xff\x7f", -ERANGE, 0, INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min__128_0)
{
	/* -128 */
	DPACKUT_INT8_MIN(data, "\xd0\x80", -ERANGE, 0, INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min__127__127)
{
	/* -127 */
	DPACKUT_INT8_MIN(data, "\xd0\x81", 0, INT8_C(-127), INT8_C(-127));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min__126__127)
{
	/* -126 */
	DPACKUT_INT8_MIN(data, "\xd0\x82", 0, INT8_C(-126), INT8_C(-127));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min__1_0)
{
	/* -1 */
	DPACKUT_INT8_MIN(data, "\xff", -ERANGE, 0, INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_0_0)
{
	/* 0 */
	DPACKUT_INT8_MIN(data, "\x00", 0, INT8_C(0), INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_1_0)
{
	/* 1 */
	DPACKUT_INT8_MIN(data, "\x01", 0, INT8_C(1), INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_127_0)
{
	/* 127 */
	DPACKUT_INT8_MIN(data, "\x7f", 0, INT8_C(127), INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_128_0)
{
	/* 128 */
	DPACKUT_INT8_MIN(data, "\xcc\x80", -ERANGE, 0, INT8_C(0));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_125_126)
{
	/* 125 */
	DPACKUT_INT8_MIN(data, "\x7d", -ERANGE, 0, INT8_C(126));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_126_126)
{
	/* 126 */
	DPACKUT_INT8_MIN(data, "\x7e", 0, INT8_C(126), INT8_C(126));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_127_126)
{
	/* 127 */
	DPACKUT_INT8_MIN(data, "\x7f", 0, INT8_C(127), INT8_C(126));
	dpackut_int8_decode_min(&data);
}

CUTE_TEST(dpackut_int8_decode_min_128_128)
{
	/* 128 */
	DPACKUT_INT8_MIN(data, "\xcc\x80", -ERANGE, 0, INT8_C(126));
	dpackut_int8_decode_min(&data);
}

#if 0
#define DPACKUT_INT8_MAX(_var, _packed, _error, _value, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.int8 = _value, \
		.high.int8  = _high \
	}

static void
dpack_scalar_utest_unpack_int8_max(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	int8_t val;

	assert_int_equal(dpack_decode_int8_max(decoder, data->high.int8, &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT8_SIZE_MIN <= data->size);
	assert_true(DPACK_INT8_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int8);
}

static void
dpack_scalar_utest_decode_int8_max(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -129 */
		DPACKUT_INT8_MAX("\xd1\xff\x7f", -ERANGE, 0,            INT8_C(0)),
		/* -128 */
		DPACKUT_INT8_MAX("\xd0\x80",     0,       INT8_C(-128), INT8_C(0)),
		/* -127 */
		DPACKUT_INT8_MAX("\xd0\x81",     0,       INT8_C(-127), INT8_C(-127)),
		/* -126 */
		DPACKUT_INT8_MAX("\xd0\x82",     -ERANGE, 0,            INT8_C(-127)),

		/* -1 */
		DPACKUT_INT8_MAX("\xff",         0,       INT8_C(-1),   INT8_C(0)),
		/* 0 */
		DPACKUT_INT8_MAX("\x00",         0,       INT8_C(0),    INT8_C(0)),
		/* 1 */
		DPACKUT_INT8_MAX("\x01",         -ERANGE, 0,            INT8_C(0)),
		/* 127 */
		DPACKUT_INT8_MAX("\x7f",         -ERANGE, 0,            INT8_C(0)),
		/* 128 */
		DPACKUT_INT8_MAX("\xcc\x80",     -ERANGE, 0,            INT8_C(0)),

		/* 125 */
		DPACKUT_INT8_MAX("\x7d",         0,       INT8_C(125),  INT8_C(126)),
		/* 126 */
		DPACKUT_INT8_MAX("\x7e",         0,       INT8_C(126),  INT8_C(126)),
		/* 127 */
		DPACKUT_INT8_MAX("\x7f",         -ERANGE, 0,            INT8_C(126)),
		/* 128 */
		DPACKUT_INT8_MAX("\xcc\x80",     -ERANGE, 0,            INT8_C(126))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int8_t               val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int8_max(NULL, 1, &val));
	cute_expect_assertion(ret = dpack_decode_int8_max(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	cute_expect_assertion(ret = dpack_decode_int8_max(&dec,
	                                                  INT8_MIN,
	                                                  &val));
	cute_expect_assertion(ret = dpack_decode_int8_max(&dec,
	                                                  INT8_MAX,
	                                                  &val));
	cute_expect_assertion(ret = dpack_decode_int8_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int8_max);
}

#define DPACKUT_INT8_RANGE(_var, _packed, _error, _value, _low, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.int8 = _value, \
		.low.int8   = _low, \
		.high.int8  = _high \
	}

static void
dpack_scalar_utest_unpack_int8_range(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	int8_t val;

	assert_int_equal(dpack_decode_int8_range(decoder,
	                                         data->low.int8,
	                                         data->high.int8,
	                                         &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT8_SIZE_MIN <= data->size);
	assert_true(DPACK_INT8_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int8);
}

static void
dpack_scalar_utest_decode_int8_range(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -129 */
		DPACKUT_INT8_RANGE("\xd1\xff\x7f", -ERANGE, 0,            INT8_MIN + 1, INT8_MIN + 2),
		/* -128 */
		DPACKUT_INT8_RANGE("\xd0\x80",     -ERANGE, 0,            INT8_MIN + 1, INT8_MIN + 2),
		/* -127 */
		DPACKUT_INT8_RANGE("\xd0\x81",     0,       INT8_MIN + 1, INT8_MIN + 1, INT8_MIN + 2),
		/* -126 */
		DPACKUT_INT8_RANGE("\xd0\x82",     0,       INT8_MIN + 2, INT8_MIN + 1, INT8_MIN + 2),
		/* -125 */
		DPACKUT_INT8_RANGE("\xd0\x83",     -ERANGE, 0,            INT8_MIN + 1, INT8_MIN + 2),

		/* -1 */
		DPACKUT_INT8_RANGE("\xff",         -ERANGE, 0,            INT8_C(0), INT8_C(1)),
		/* 0 */
		DPACKUT_INT8_RANGE("\x00",         0,       INT8_C(0),    INT8_C(0), INT8_C(1)),
		/* 1 */
		DPACKUT_INT8_RANGE("\x01",         0,       INT8_C(1),    INT8_C(0), INT8_C(1)),
		/* 2 */
		DPACKUT_INT8_RANGE("\x02",         -ERANGE, 0,            INT8_C(0), INT8_C(1)),

		/* 124 */
		DPACKUT_INT8_RANGE("\x7c",         -ERANGE, 0,            INT8_MAX - 2, INT8_MAX - 1),
		/* 125 */
		DPACKUT_INT8_RANGE("\x7d",         0,       INT8_MAX - 2, INT8_MAX - 2, INT8_MAX - 1),
		/* 126 */
		DPACKUT_INT8_RANGE("\x7e",         0,       INT8_MAX - 1, INT8_MAX - 2, INT8_MAX - 1),
		/* 127 */
		DPACKUT_INT8_RANGE("\x7f",         -ERANGE, 0,            INT8_MAX - 2, INT8_MAX - 1),
		/* 128 */
		DPACKUT_INT8_RANGE("\xcc\x80",     -ERANGE, 0,            INT8_MAX - 2, INT8_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int8_t               val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int8_range(NULL, 1, 2, &val));
	cute_expect_assertion(ret = dpack_decode_int8_range(&dec, 1, 2, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	cute_expect_assertion(ret = dpack_decode_int8_range(&dec,
	                                                    INT8_MIN,
	                                                    2,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_int8_range(&dec,
	                                                    1,
	                                                    INT8_MAX,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_int8_range(&dec, 2, 2, &val));
	cute_expect_assertion(ret = dpack_decode_int8_range(&dec, 1, 2, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int8_range);
}

#endif

CUTE_GROUP(dpackut_int8_group) = {
	CUTE_REF(dpackut_int8_encode_assert),
	CUTE_REF(dpackut_int8_encode__128),
	CUTE_REF(dpackut_int8_encode__127),
	CUTE_REF(dpackut_int8_encode__1),
	CUTE_REF(dpackut_int8_encode_0),
	CUTE_REF(dpackut_int8_encode_1),
	CUTE_REF(dpackut_int8_encode_127),

	CUTE_REF(dpackut_int8_decode_assert),
	CUTE_REF(dpackut_int8_decode__129),
	CUTE_REF(dpackut_int8_decode__128),
	CUTE_REF(dpackut_int8_decode__127),
	CUTE_REF(dpackut_int8_decode__1),
	CUTE_REF(dpackut_int8_decode_0),
	CUTE_REF(dpackut_int8_decode_1),
	CUTE_REF(dpackut_int8_decode_127),
	CUTE_REF(dpackut_int8_decode_128),

	CUTE_REF(dpackut_int8_decode_min_assert),
	CUTE_REF(dpackut_int8_decode_min__129_0),
	CUTE_REF(dpackut_int8_decode_min__128_0),
	CUTE_REF(dpackut_int8_decode_min__127__127),
	CUTE_REF(dpackut_int8_decode_min__126__127),
	CUTE_REF(dpackut_int8_decode_min__1_0),
	CUTE_REF(dpackut_int8_decode_min_0_0),
	CUTE_REF(dpackut_int8_decode_min_1_0),
	CUTE_REF(dpackut_int8_decode_min_127_0),
	CUTE_REF(dpackut_int8_decode_min_128_0),
	CUTE_REF(dpackut_int8_decode_min_125_126),
	CUTE_REF(dpackut_int8_decode_min_126_126),
	CUTE_REF(dpackut_int8_decode_min_127_126),
	CUTE_REF(dpackut_int8_decode_min_128_128),
};

CUTE_SUITE_EXTERN(dpackut_int8_suite,
                  dpackut_int8_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
