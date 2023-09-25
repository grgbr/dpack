#include "utest.h"
#include "dpack/scalar.h"
#include "dpack/codec.h"
#include <errno.h>

#define DPACKUT_UINT64(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint64 = _value \
	}

static int
dpack_scalar_utest_pack_uint64(struct dpack_encoder *                 encoder,
                               const struct dpack_scalar_utest_data * data)
{
	assert_true(DPACK_UINT64_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT64_SIZE_MAX >= data->size);

	return dpack_encode_uint64(encoder, data->value.uint64);
}

static void
dpack_scalar_utest_encode_uint64(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* 0 */
		DPACKUT_UINT64("\x00",             0, UINT64_C(0)),
		/* 1 */
		DPACKUT_UINT64("\x01",             0, UINT64_C(1)),
		/* 18446744073709551615 */
		DPACKUT_UINT64("\xcf"
		                   "\xff\xff\xff\xff"
		                   "\xff\xff\xff\xff", 0, UINT64_MAX)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint64_t             val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_encode_uint64(NULL, val));
	cute_expect_assertion(ret = dpack_encode_uint64(&enc, val));
#endif

	dpack_scalar_utest_encode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_pack_uint64);
}

static void
dpack_scalar_utest_unpack_uint64(struct dpack_decoder *                 decoder,
                                 const struct dpack_scalar_utest_data * data)
{
	uint64_t val;

	assert_int_equal(dpack_decode_uint64(decoder, &val), data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT64_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT64_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint64);
}

static void
dpack_scalar_utest_decode_uint64(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACKUT_UINT64("\xff",             -ENOMSG, UINT64_C(0)),
		/* 0 */
		DPACKUT_UINT64("\x00",             0,       UINT64_C(0)),
		/* 1 */
		DPACKUT_UINT64("\x01",             0,       UINT64_C(1)),
		/* 18446744073709551615 */
		DPACKUT_UINT64("\xcf"
		                   "\xff\xff\xff\xff"
		                   "\xff\xff\xff\xff", 0,       UINT64_MAX)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint64_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint64(NULL, &val));
	cute_expect_assertion(ret = dpack_decode_uint64(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	cute_expect_assertion(ret = dpack_decode_uint64(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint64);
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
dpack_scalar_utest_unpack_uint64_min(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint64_t val;

	assert_int_equal(dpack_decode_uint64_min(decoder,
	                                         data->low.uint64,
	                                         &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT64_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT64_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint64);
}

static void
dpack_scalar_utest_decode_uint64_min(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACKUT_UINT64_MIN("\xff",             -ENOMSG, 0,              UINT64_C(1)),
		/* 0 */
		DPACKUT_UINT64_MIN("\x00",             -ERANGE, 0,              UINT64_C(1)),
		/* 1 */
		DPACKUT_UINT64_MIN("\x01",             0,       UINT64_C(1),    UINT64_C(1)),
		/* 9223372036854775807 */
		DPACKUT_UINT64_MIN("\xcf"
		                       "\x7f\xff\xff\xff"
		                       "\xff\xff\xff\xff", 0,       UINT64_MAX / 2, UINT64_C(1)),
		/* 18446744073709551615 */
		DPACKUT_UINT64_MIN("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xff", 0,       UINT64_MAX,     UINT64_C(1)),

		/* -1 */
		DPACKUT_UINT64_MIN("\xff",             -ENOMSG, 0,              UINT64_MAX / 2),
		/* 0 */
		DPACKUT_UINT64_MIN("\x00",             -ERANGE, 0,              UINT64_MAX / 2),
		/* 1 */
		DPACKUT_UINT64_MIN("\x01",             -ERANGE, 0,              UINT64_MAX / 2),

		/* 9223372036854775806 */
		DPACKUT_UINT64_MIN("\xcf"
		                       "\x7f\xff\xff\xff"
		                       "\xff\xff\xff\xfe", -ERANGE, 0,              UINT64_MAX / 2),
		/* 9223372036854775807 */
		DPACKUT_UINT64_MIN("\xcf"
		                       "\x7f\xff\xff\xff"
		                       "\xff\xff\xff\xff", 0,       UINT64_MAX / 2, UINT64_MAX / 2),
		/* 18446744073709551615 */
		DPACKUT_UINT64_MIN("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xff", 0,       UINT64_MAX,     UINT64_MAX / 2),

		/* 18446744073709551613 */
		DPACKUT_UINT64_MIN("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xfd", -ERANGE, 0,              UINT64_MAX - 1),
		/* 18446744073709551614 */
		DPACKUT_UINT64_MIN("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xfe", 0,       UINT64_MAX - 1, UINT64_MAX - 1),
		/* 18446744073709551615 */
		DPACKUT_UINT64_MIN("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xff", 0,       UINT64_MAX,     UINT64_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint64_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint64_min(NULL, 1, &val));
	cute_expect_assertion(ret = dpack_decode_uint64_min(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	cute_expect_assertion(ret = dpack_decode_uint64_min(&dec, 0, &val));
	cute_expect_assertion(ret = dpack_decode_uint64_min(&dec,
	                                                    UINT64_MAX,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_uint64_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint64_min);
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
dpack_scalar_utest_unpack_uint64_max(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint64_t val;

	assert_int_equal(dpack_decode_uint64_max(decoder,
	                                         data->high.uint64,
	                                         &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT64_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT64_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint64);
}

static void
dpack_scalar_utest_decode_uint64_max(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACKUT_UINT64_MAX("\xff",             -ENOMSG, 0,                    UINT64_C(1)),
		/* 0 */
		DPACKUT_UINT64_MAX("\x00",             0,       UINT64_C(0),          UINT64_C(1)),
		/* 1 */
		DPACKUT_UINT64_MAX("\x01",             0,       UINT64_C(1),          UINT64_C(1)),
		/* 9223372036854775807 */
		DPACKUT_UINT64_MAX("\xcf"
		                       "\x7f\xff\xff\xff"
		                       "\xff\xff\xff\xff", -ERANGE, 0,                    UINT64_C(1)),
		/* 18446744073709551615 */
		DPACKUT_UINT64_MAX("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xff", -ERANGE, 0,                    UINT64_C(1)),

		/* -1 */
		DPACKUT_UINT64_MAX("\xff",             -ENOMSG, 0,                    UINT64_MAX / 2),
		/* 0 */
		DPACKUT_UINT64_MAX("\x00",             0,       UINT64_C(0),          UINT64_MAX / 2),
		/* 1 */
		DPACKUT_UINT64_MAX("\x01",             0,       UINT64_C(1),          UINT64_MAX / 2),

		/* 9223372036854775806 */
		DPACKUT_UINT64_MAX("\xcf"
		                       "\x7f\xff\xff\xff"
		                       "\xff\xff\xff\xfe", 0,       (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 9223372036854775807 */
		DPACKUT_UINT64_MAX("\xcf"
		                       "\x7f\xff\xff\xff"
		                       "\xff\xff\xff\xff", 0,       UINT64_MAX / 2,       UINT64_MAX / 2),
		/* 18446744073709551615 */
		DPACKUT_UINT64_MAX("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xff", -ERANGE, 0,                    UINT64_MAX / 2),

		/* 18446744073709551613 */
		DPACKUT_UINT64_MAX("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xfd", 0,       UINT64_MAX - 2,       UINT64_MAX - 1),
		/* 18446744073709551614 */
		DPACKUT_UINT64_MAX("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xfe", 0,       UINT64_MAX - 1,       UINT64_MAX - 1),
		/* 18446744073709551615 */
		DPACKUT_UINT64_MAX("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xff", -ERANGE, 0,                    UINT64_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint64_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_uint64_max(NULL, 1, &val));
	cute_expect_assertion(ret = dpack_decode_uint64_max(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	cute_expect_assertion(ret = dpack_decode_uint64_max(&dec, 0, &val));
	cute_expect_assertion(ret = dpack_decode_uint64_max(&dec,
	                                                    UINT64_MAX,
	                                                    &val));
	cute_expect_assertion(ret = dpack_decode_uint64_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint64_max);
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
dpack_scalar_utest_unpack_uint64_range(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint64_t val;

	assert_int_equal(dpack_decode_uint64_range(decoder,
	                                           data->low.uint64,
	                                           data->high.uint64,
	                                           &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT64_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT64_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint64);
}

static void
dpack_scalar_utest_decode_uint64_range(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACKUT_UINT64_RANGE("\xff",             -ENOMSG, 0,                    UINT64_C(1), UINT64_C(2)),
		/* 0 */
		DPACKUT_UINT64_RANGE("\x00",             -ERANGE, 0,                    UINT64_C(1), UINT64_C(2)),
		/* 1 */
		DPACKUT_UINT64_RANGE("\x01",             0,       UINT64_C(1),          UINT64_C(1), UINT64_C(2)),
		/* 2 */
		DPACKUT_UINT64_RANGE("\x02",             0,       UINT64_C(2),          UINT64_C(1), UINT64_C(2)),
		/* 3 */
		DPACKUT_UINT64_RANGE("\x03",             -ERANGE, 0,                    UINT64_C(1), UINT64_C(2)),
		/* 9223372036854775807 */
		DPACKUT_UINT64_RANGE("\xcf"
		                         "\x7f\xff\xff\xff"
		                         "\xff\xff\xff\xff", -ERANGE, 0,                    UINT64_C(1), UINT64_C(2)),
		/* 18446744073709551615 */
		DPACKUT_UINT64_RANGE("\xcf"
		                         "\xff\xff\xff\xff"
		                         "\xff\xff\xff\xff", -ERANGE, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX / 2),

		/* -1 */
		DPACKUT_UINT64_RANGE("\xff",             -ENOMSG, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 0 */
		DPACKUT_UINT64_RANGE("\x00",             -ERANGE, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 9223372036854775805 */
		DPACKUT_UINT64_RANGE("\xcf"
		                         "\x7f\xff\xff\xff"
		                         "\xff\xff\xff\xfd", -ERANGE, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 9223372036854775806 */
		DPACKUT_UINT64_RANGE("\xcf"
		                         "\x7f\xff\xff\xff"
		                         "\xff\xff\xff\xfe", 0,       (UINT64_MAX / 2) - 1, (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 9223372036854775807 */
		DPACKUT_UINT64_RANGE("\xcf"
		                         "\x7f\xff\xff\xff"
		                         "\xff\xff\xff\xff", 0,       UINT64_MAX / 2,       (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 9223372036854775808 */
		DPACKUT_UINT64_RANGE("\xcf"
		                         "\x80\x00\x00\x00"
		                         "\x00\x00\x00\x00", -ERANGE, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 18446744073709551615 */
		DPACKUT_UINT64_RANGE("\xcf"
		                         "\xff\xff\xff\xff"
		                         "\xff\xff\xff\xff", -ERANGE, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX - 1),

		/* -1 */
		DPACKUT_UINT64_RANGE("\xff",             -ENOMSG, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX - 1),
		/* 0 */
		DPACKUT_UINT64_RANGE("\x00",             -ERANGE, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX - 1),
		/* 18446744073709551612 */
		DPACKUT_UINT64_RANGE("\xcf"
		                         "\xff\xff\xff\xff"
		                         "\xff\xff\xff\xfc", -ERANGE, 0,                    UINT64_MAX - 2,       UINT64_MAX - 1),
		/* 18446744073709551613 */
		DPACKUT_UINT64_RANGE("\xcf"
		                         "\xff\xff\xff\xff"
		                         "\xff\xff\xff\xfd", 0,       UINT64_MAX - 2,       UINT64_MAX - 2,       UINT64_MAX - 1),
		/* 18446744073709551614 */
		DPACKUT_UINT64_RANGE("\xcf"
		                         "\xff\xff\xff\xff"
		                         "\xff\xff\xff\xfe", 0,       UINT64_MAX - 1,       UINT64_MAX - 2,       UINT64_MAX - 1),
		/* 18446744073709551615 */
		DPACKUT_UINT64_RANGE("\xcf"
		                         "\xff\xff\xff\xff"
		                         "\xff\xff\xff\xff", -ERANGE, 0,                    UINT64_MAX - 2,       UINT64_MAX - 1),
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

#define DPACKUT_INT64(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int64 = _value \
	}

static int
dpack_scalar_utest_pack_int64(struct dpack_encoder *                 encoder,
                              const struct dpack_scalar_utest_data * data)
{
	assert_true(DPACK_INT64_SIZE_MIN <= data->size);
	assert_true(DPACK_INT64_SIZE_MAX >= data->size);

	return dpack_encode_int64(encoder, data->value.int64);
}

static void
dpack_scalar_utest_encode_int64(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -9223372036854775808 */
		DPACKUT_INT64("\xd3"
		                  "\x80\x00\x00\x00"
		                  "\x00\x00\x00\x00", 0, INT64_MIN),
		/* -9223372036854775807 */
		DPACKUT_INT64("\xd3"
		                  "\x80\x00\x00\x00"
		                  "\x00\x00\x00\x01", 0, INT64_MIN + 1),
		/* -1 */
		DPACKUT_INT64("\xff",             0, INT64_C(-1)),
		/* 0 */
		DPACKUT_INT64("\x00",             0, INT64_C(0)),
		/* 1 */
		DPACKUT_INT64("\x01",             0, INT64_C(1)),
		/* 9223372036854775807 */
		DPACKUT_INT64("\xcf"
		                  "\x7f\xff\xff\xff"
		                  "\xff\xff\xff\xff", 0, INT64_MAX),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int64_t              val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_encode_int64(NULL, val));
	cute_expect_assertion(ret = dpack_encode_int64(&enc, val));
#endif

	dpack_scalar_utest_encode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_pack_int64);
}

static void
dpack_scalar_utest_unpack_int64(struct dpack_decoder *                 decoder,
                                const struct dpack_scalar_utest_data * data)
{
	int64_t val;

	assert_int_equal(dpack_decode_int64(decoder, &val), data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT64_SIZE_MIN <= data->size);
	assert_true(DPACK_INT64_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int64);
}

static void
dpack_scalar_utest_decode_int64(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* 9223372036854775808 */
		DPACKUT_INT64("\xcf"
		                  "\x80\x00\x00\x00"
		                  "\x00\x00\x00\x00", -ERANGE, INT64_C(0)),
		/* -9223372036854775808 */
		DPACKUT_INT64("\xd3"
		                  "\x80\x00\x00\x00"
		                  "\x00\x00\x00\x00", 0,       INT64_MIN),
		/* -9223372036854775807 */
		DPACKUT_INT64("\xd3"
		                  "\x80\x00\x00\x00"
		                  "\x00\x00\x00\x01", 0,       INT64_MIN + 1),
		/* -1 */
		DPACKUT_INT64("\xff",             0,       INT64_C(-1)),
		/* 0 */
		DPACKUT_INT64("\x00",             0,       INT64_C(0)),
		/* 1 */
		DPACKUT_INT64("\x01",             0,       INT64_C(1)),
		/* 9223372036854775807 */
		DPACKUT_INT64("\xcf"
		                  "\x7f\xff\xff\xff"
		                  "\xff\xff\xff\xff", 0,       INT64_MAX),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int64_t               val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int64(NULL, &val));
	cute_expect_assertion(ret = dpack_decode_int64(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	cute_expect_assertion(ret = dpack_decode_int64(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int64);
}

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

static const struct CMUnitTest dpack_stdint_utests[] = {
	cmocka_unit_test(dpack_scalar_utest_encode_int8),
	cmocka_unit_test(dpack_scalar_utest_decode_int8),
	cmocka_unit_test(dpack_scalar_utest_decode_int8_min),
	cmocka_unit_test(dpack_scalar_utest_decode_int8_max),
	cmocka_unit_test(dpack_scalar_utest_decode_int8_range),

	cmocka_unit_test(dpack_scalar_utest_encode_uint16),
	cmocka_unit_test(dpack_scalar_utest_decode_uint16),
	cmocka_unit_test(dpack_scalar_utest_decode_uint16_min),
	cmocka_unit_test(dpack_scalar_utest_decode_uint16_max),
	cmocka_unit_test(dpack_scalar_utest_decode_uint16_range),

	cmocka_unit_test(dpack_scalar_utest_encode_int16),
	cmocka_unit_test(dpack_scalar_utest_decode_int16),
	cmocka_unit_test(dpack_scalar_utest_decode_int16_min),
	cmocka_unit_test(dpack_scalar_utest_decode_int16_max),
	cmocka_unit_test(dpack_scalar_utest_decode_int16_range),

	cmocka_unit_test(dpack_scalar_utest_encode_uint32),
	cmocka_unit_test(dpack_scalar_utest_decode_uint32),
	cmocka_unit_test(dpack_scalar_utest_decode_uint32_min),
	cmocka_unit_test(dpack_scalar_utest_decode_uint32_max),
	cmocka_unit_test(dpack_scalar_utest_decode_uint32_range),

	cmocka_unit_test(dpack_scalar_utest_encode_int32),
	cmocka_unit_test(dpack_scalar_utest_decode_int32),
	cmocka_unit_test(dpack_scalar_utest_decode_int32_min),
	cmocka_unit_test(dpack_scalar_utest_decode_int32_max),
	cmocka_unit_test(dpack_scalar_utest_decode_int32_range),

	cmocka_unit_test(dpack_scalar_utest_encode_uint64),
	cmocka_unit_test(dpack_scalar_utest_decode_uint64),
	cmocka_unit_test(dpack_scalar_utest_decode_uint64_min),
	cmocka_unit_test(dpack_scalar_utest_decode_uint64_max),
	cmocka_unit_test(dpack_scalar_utest_decode_uint64_range),

	cmocka_unit_test(dpack_scalar_utest_encode_int64),
	cmocka_unit_test(dpack_scalar_utest_decode_int64),
	cmocka_unit_test(dpack_scalar_utest_decode_int64_min),
	cmocka_unit_test(dpack_scalar_utest_decode_int64_max),
	cmocka_unit_test(dpack_scalar_utest_decode_int64_range)
};

int
main(void)
{
	return cmocka_run_group_tests_name("integers",
	                                   dpack_stdint_utests,
	                                   NULL,
	                                   NULL);
}
