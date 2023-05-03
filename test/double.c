#include "utest.h"
#include "dpack/scalar.h"
#include "dpack/codec.h"
#include <math.h>
#include <errno.h>

#define DPACK_UTEST_DOUBLE(_packed, _error, _value) \
	{ \
		.packed    = _packed, \
		.size      = sizeof(_packed) - 1, \
		.error     = _error, \
		.value.f64 = _value \
	}

static int
dpack_scalar_utest_pack_double(struct dpack_encoder *                 encoder,
                               const struct dpack_scalar_utest_data * data)
{
	assert_int_equal(data->size, DPACK_DOUBLE_SIZE);

	return dpack_encode_double(encoder, data->value.f64);
}

static void
dpack_scalar_utest_encode_double(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -INFINITY: dpack-utest-gen.py "float('-inf')" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\xff\xf0\x00\x00"
		                   "\x00\x00\x00\x00",     0, -INFINITY),
		/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\xff\xef\xff\xff"
		                   "\xff\xff\xff\xff",     0, -MAXDOUBLE),
		/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\x80\x10\x00\x00"
		                   "\x00\x00\x00\x00",     0, -MINDOUBLE),
		/* -0: dpack-utest-gen.py "- 0.0" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\x80\x00\x00\x00"
		                   "\x00\x00\x00\x00",     0, -0.0),
		/* +0: dpack-utest-gen.py "0.0" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\x00\x00\x00\x00"
		                   "\x00\x00\x00\x00",     0, 0.0),
		/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\x00\x10\x00\x00"
		                   "\x00\x00\x00\x00",     0, MINDOUBLE),
		/* +MAXDOUBLE: dpack-utest-gen.py "1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\x7f\xef\xff\xff"
		                   "\xff\xff\xff\xff",     0, MAXDOUBLE),
		/* +INFINITY: dpack-utest-gen.py "float('inf')" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\x7f\xf0\x00\x00"
		                   "\x00\x00\x00\x00",     0, INFINITY)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	double                val = NAN;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;
	char                 buff[DPACK_DOUBLE_SIZE];

	expect_assert_failure(ret = dpack_encode_double(NULL, val));
	expect_assert_failure(ret = dpack_encode_double(&enc, val));

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));
	expect_assert_failure(ret = dpack_encode_double(&enc, NAN));
	dpack_encoder_fini(&enc);
#endif

	dpack_scalar_utest_encode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_pack_double);
}

static void
dpack_scalar_utest_unpack_double(struct dpack_decoder *                 decoder,
                                const struct dpack_scalar_utest_data * data)
{
	double val;

	assert_int_equal(dpack_decode_double(decoder, &val), data->error);
	if (!data->error) {
		assert_double_equal(val, data->value.f64, 0.0);
		assert_int_equal(data->size, DPACK_DOUBLE_SIZE);
	}
}

static void
dpack_scalar_utest_decode_double(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -Not A Number: dpack-utest-gen.py "float('-nan')" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\xff\xf8\x00\x00"
		                   "\x00\x00\x00\x00",     -ENOMSG, -NAN),
		/* -INFINITY: dpack-utest-gen.py "float('-inf')" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\xff\xf0\x00\x00"
		                   "\x00\x00\x00\x00",     0,       -INFINITY),
		/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\xff\xef\xff\xff"
		                   "\xff\xff\xff\xff",     0,       -MAXDOUBLE),
		/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\x80\x10\x00\x00"
		                   "\x00\x00\x00\x00",     0,       -MINDOUBLE),
		/* -0: dpack-utest-gen.py "- 0.0" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\x80\x00\x00\x00"
		                   "\x00\x00\x00\x00",     0,       -0.0),
		/* +0: dpack-utest-gen.py "0.0" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\x00\x00\x00\x00"
		                   "\x00\x00\x00\x00",     0,       0.0),
		/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\x00\x10\x00\x00"
		                   "\x00\x00\x00\x00",     0,       MINDOUBLE),
		/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\x7f\xef\xff\xff"
		                   "\xff\xff\xff\xff",     0,       MAXDOUBLE),
		/* +INFINITY: dpack-utest-gen.py "float('inf')" */
		DPACK_UTEST_DOUBLE("\xcb"
		                   "\x7f\xf0\x00\x00"
		                   "\x00\x00\x00\x00",     0,       INFINITY),
		/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
		DPACK_UTEST_DOUBLE("\xca\x7f\xc0\x00\x00", -ENOMSG, NAN),
		/* +0 (float): dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_DOUBLE("\xca\x00\x00\x00\x00", -ENOMSG, 0)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	double                val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_double(NULL, &val));
	expect_assert_failure(ret = dpack_decode_double(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_double(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_double);
}

#define DPACK_UTEST_DOUBLE_MIN(_packed, _error, _value, _low) \
	{ \
		.packed    = _packed, \
		.size      = sizeof(_packed) - 1, \
		.error     = _error, \
		.value.f64 = _value, \
		.low.f64   = _low \
	}

static void
dpack_scalar_utest_unpack_double_min(
	struct dpack_decoder *                 decoder,
        const struct dpack_scalar_utest_data * data)
{
	double val;

	assert_int_equal(dpack_decode_double_min(decoder, data->low.f64, &val),
	                 data->error);
	if (!data->error) {
		assert_double_equal(val, data->value.f64, 0.0);
		assert_int_equal(data->size, DPACK_DOUBLE_SIZE);
	}
}

static void
dpack_scalar_utest_decode_double_min(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -Not A Number: dpack-utest-gen.py "float('-nan')" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\xff\xf8\x00\x00"
		                       "\x00\x00\x00\x00",     -ENOMSG, -NAN,      0.0),

		/* -INFINITY: dpack-utest-gen.py "float('-inf')" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\xff\xf0\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, -INFINITY, -MAXDOUBLE),
		/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\xff\xef\xff\xff"
		                       "\xff\xff\xff\xff",     0,       -MAXDOUBLE, -MAXDOUBLE),
		/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x80\x10\x00\x00"
		                       "\x00\x00\x00\x00",     0,       -MINDOUBLE, -MAXDOUBLE),
		/* -0: dpack-utest-gen.py "- 0.0" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x80\x00\x00\x00"
		                       "\x00\x00\x00\x00",     0,       -0.0,     -MAXDOUBLE),
		/* +0: dpack-utest-gen.py "0.0" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x00\x00\x00\x00"
		                       "\x00\x00\x00\x00",     0,       0.0,      -MAXDOUBLE),
		/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x00\x10\x00\x00"
		                       "\x00\x00\x00\x00",     0,       MINDOUBLE,  -MAXDOUBLE),
		/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x7f\xef\xff\xff"
		                       "\xff\xff\xff\xff",     0,       MAXDOUBLE,  -MAXDOUBLE),
		/* +INFINITY: dpack-utest-gen.py "float('inf')" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x7f\xf0\x00\x00"
		                       "\x00\x00\x00\x00",     0,       INFINITY,  -MAXDOUBLE),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\xff\xf0\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, -INFINITY, 0.0),
		/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\xff\xef\xff\xff"
		                       "\xff\xff\xff\xff",     -ERANGE, -MAXDOUBLE, 0.0),
		/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x80\x10\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, -MINDOUBLE, 0.0),
		/* -0: dpack-utest-gen.py "- 0.0" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x80\x00\x00\x00"
		                       "\x00\x00\x00\x00",     0,       -0.0,     0.0),
		/* +0: dpack-utest-gen.py "0.0" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x00\x00\x00\x00"
		                       "\x00\x00\x00\x00",     0,       0.0,      0.0),
		/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x00\x10\x00\x00"
		                       "\x00\x00\x00\x00",     0,       MINDOUBLE,  0.0),
		/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x7f\xef\xff\xff"
		                       "\xff\xff\xff\xff",     0,       MAXDOUBLE,  0.0),
		/* +INFINITY: dpack-utest-gen.py "float('inf')" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x7f\xf0\x00\x00"
		                       "\x00\x00\x00\x00",     0,       INFINITY,  0.0),
		/* +Not A Number: dpack-utest-gen.py "float('nan')" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x7f\xf8\x00\x00"
		                       "\x00\x00\x00\x00",     -ENOMSG, NAN,       0.0),

		/* -INFINITY: dpack-utest-gen.py "float('-inf')" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\xff\xf0\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, -INFINITY, MAXDOUBLE),
		/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\xff\xef\xff\xff"
		                       "\xff\xff\xff\xff",     -ERANGE, -MAXDOUBLE, MAXDOUBLE),
		/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x80\x10\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, -MINDOUBLE, MAXDOUBLE),
		/* -0: dpack-utest-gen.py "- 0.0" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x80\x00\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, -0.0,     MAXDOUBLE),
		/* +0: dpack-utest-gen.py "0.0" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x00\x00\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, 0.0,      MAXDOUBLE),
		/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x00\x10\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, MINDOUBLE,  MAXDOUBLE),
		/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x7f\xef\xff\xff"
		                       "\xff\xff\xff\xff",     0,       MAXDOUBLE,  MAXDOUBLE),
		/* +INFINITY: dpack-utest-gen.py "float('inf')" */
		DPACK_UTEST_DOUBLE_MIN("\xcb"
		                       "\x7f\xf0\x00\x00"
		                       "\x00\x00\x00\x00",     0,       INFINITY,  MAXDOUBLE),

		/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
		DPACK_UTEST_DOUBLE_MIN("\xca\x7f\xc0\x00\x00", -ENOMSG, NAN,       0.0),
		/* +0 (float): dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_DOUBLE_MIN("\xca\x00\x00\x00\x00", -ENOMSG, 0.0,       0.0)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	double                val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_double_min(NULL, 1.0, &val));
	expect_assert_failure(ret = dpack_decode_double_min(&dec, 1.0, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_double_min(&dec, -INFINITY, &val));
	expect_assert_failure(ret = dpack_decode_double_min(&dec, INFINITY, &val));
	expect_assert_failure(ret = dpack_decode_double_min(&dec, NAN, &val));
	expect_assert_failure(ret = dpack_decode_double_min(&dec, -NAN, &val));
	expect_assert_failure(ret = dpack_decode_double_min(&dec, 1.0, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_double_min);
}

#define DPACK_UTEST_DOUBLE_MAX(_packed, _error, _value, _high) \
	{ \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.f64  = _value, \
		.high.f64   = _high \
	}

static void
dpack_scalar_utest_unpack_double_max(
	struct dpack_decoder *                 decoder,
        const struct dpack_scalar_utest_data * data)
{
	double val;

	assert_int_equal(dpack_decode_double_max(decoder, data->high.f64, &val),
	                 data->error);
	if (!data->error) {
		assert_double_equal(val, data->value.f64, 0.0);
		assert_int_equal(data->size, DPACK_DOUBLE_SIZE);
	}
}

static void
dpack_scalar_utest_decode_double_max(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -Not A Number: dpack-utest-gen.py "float('-nan')" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\xff\xf8\x00\x00"
		                       "\x00\x00\x00\x00",     -ENOMSG, -NAN,      0.0),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\xff\xf0\x00\x00"
		                       "\x00\x00\x00\x00",     0,       -INFINITY, -MAXDOUBLE),
		/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\xff\xef\xff\xff"
		                       "\xff\xff\xff\xff",     0,       -MAXDOUBLE, -MAXDOUBLE),
		/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x80\x10\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, -MINDOUBLE, -MAXDOUBLE),
		/* -0: dpack-utest-gen.py "- 0.0" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x80\x00\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, -0.0,     -MAXDOUBLE),
		/* +0: dpack-utest-gen.py "0.0" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x00\x00\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, 0.0,      -MAXDOUBLE),
		/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x00\x10\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, MINDOUBLE,  -MAXDOUBLE),
		/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x7f\xef\xff\xff"
		                       "\xff\xff\xff\xff",     -ERANGE, MAXDOUBLE,  -MAXDOUBLE),
		/* +INFINITY: dpack-utest-gen.py "float('inf')" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x7f\xf0\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, INFINITY,  -MAXDOUBLE),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\xff\xf0\x00\x00"
		                       "\x00\x00\x00\x00",     0,       -INFINITY, 0.0),
		/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\xff\xef\xff\xff"
		                       "\xff\xff\xff\xff",     0,       -MAXDOUBLE, 0.0),
		/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x80\x10\x00\x00"
		                       "\x00\x00\x00\x00",     0,       -MINDOUBLE, 0.0),
		/* -0: dpack-utest-gen.py "- 0.0" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x80\x00\x00\x00"
		                       "\x00\x00\x00\x00",     0,       -0.0,     0.0),
		/* +0: dpack-utest-gen.py "0.0" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x00\x00\x00\x00"
		                       "\x00\x00\x00\x00",     0,       0.0,      0.0),
		/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x00\x10\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, MINDOUBLE,  0.0),
		/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x7f\xef\xff\xff"
		                       "\xff\xff\xff\xff",     -ERANGE, MAXDOUBLE,  0.0),
		/* +INFINITY: dpack-utest-gen.py "float('inf')" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x7f\xf0\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, INFINITY,  0.0),
		/* +Not A Number: dpack-utest-gen.py "float('nan')" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x7f\xf8\x00\x00"
		                       "\x00\x00\x00\x00",     -ENOMSG, NAN,       0.0),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\xff\xf0\x00\x00"
		                       "\x00\x00\x00\x00",     0,       -INFINITY, MAXDOUBLE),
		/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\xff\xef\xff\xff"
		                       "\xff\xff\xff\xff",     0,       -MAXDOUBLE, MAXDOUBLE),
		/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x80\x10\x00\x00"
		                       "\x00\x00\x00\x00",     0,       -MINDOUBLE, MAXDOUBLE),
		/* -0: dpack-utest-gen.py "- 0.0" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x80\x00\x00\x00"
		                       "\x00\x00\x00\x00",     0,       -0.0,     MAXDOUBLE),
		/* +0: dpack-utest-gen.py "0.0" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x00\x00\x00\x00"
		                       "\x00\x00\x00\x00",     0,       0.0,      MAXDOUBLE),
		/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x00\x10\x00\x00"
		                       "\x00\x00\x00\x00",     0,       MINDOUBLE,  MAXDOUBLE),
		/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x7f\xef\xff\xff"
		                       "\xff\xff\xff\xff",     0,       MAXDOUBLE,  MAXDOUBLE),
		/* +INFINITY: dpack-utest-gen.py "float('inf')" */
		DPACK_UTEST_DOUBLE_MAX("\xcb"
		                       "\x7f\xf0\x00\x00"
		                       "\x00\x00\x00\x00",     -ERANGE, INFINITY,  MAXDOUBLE),

		/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
		DPACK_UTEST_DOUBLE_MAX("\xca\x7f\xc0\x00\x00", -ENOMSG, NAN,       0.0),
		/* +0 (float): dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_DOUBLE_MAX("\xca\x00\x00\x00\x00", -ENOMSG, 0.0,       0.0)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	double                val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_double_max(NULL, 1.0, &val));
	expect_assert_failure(ret = dpack_decode_double_max(&dec, 1.0, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_double_max(&dec, -INFINITY, &val));
	expect_assert_failure(ret = dpack_decode_double_max(&dec, INFINITY, &val));
	expect_assert_failure(ret = dpack_decode_double_max(&dec, NAN, &val));
	expect_assert_failure(ret = dpack_decode_double_max(&dec, -NAN, &val));
	expect_assert_failure(ret = dpack_decode_double_max(&dec, 1.0, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_double_max);
}

#define DPACK_UTEST_DOUBLE_RANGE(_packed, _error, _value, _low, _high) \
	{ \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.f64  = _value, \
		.low.f64    = _low, \
		.high.f64   = _high \
	}

static void
dpack_scalar_utest_unpack_double_range(
	struct dpack_decoder *                 decoder,
        const struct dpack_scalar_utest_data * data)
{
	double val;

	assert_int_equal(dpack_decode_double_range(decoder,
	                                          data->low.f64,
	                                          data->high.f64,
	                                          &val),
	                 data->error);
	if (!data->error) {
		assert_double_equal(val, data->value.f64, 0.0);
		assert_int_equal(data->size, DPACK_DOUBLE_SIZE);
	}
}

static void
dpack_scalar_utest_decode_double_range(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -Not A Number: dpack-utest-gen.py "float('-nan')" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\xff\xf8\x00\x00"
		                         "\x00\x00\x00\x00",     -ENOMSG, -NAN,      0.0, 1.0),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\xff\xf0\x00\x00"
		                         "\x00\x00\x00\x00",     -ERANGE, -INFINITY, -MAXDOUBLE, 0.0),
		/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\xff\xef\xff\xff"
		                         "\xff\xff\xff\xff",     0,       -MAXDOUBLE, -MAXDOUBLE, 0.0),
		/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x80\x10\x00\x00"
		                         "\x00\x00\x00\x00",     0,       -MINDOUBLE, -MAXDOUBLE, 0.0),
		/* -0: dpack-utest-gen.py "- 0.0" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x80\x00\x00\x00"
		                         "\x00\x00\x00\x00",     0,       -0.0,     -MAXDOUBLE, 0.0),
		/* +0: dpack-utest-gen.py "0.0" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x00\x00\x00\x00"
		                         "\x00\x00\x00\x00",     0,       0.0,      -MAXDOUBLE, 0.0),
		/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x00\x10\x00\x00"
		                         "\x00\x00\x00\x00",     -ERANGE, MINDOUBLE,  -MAXDOUBLE, 0.0),
		/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x7f\xef\xff\xff"
		                         "\xff\xff\xff\xff",     -ERANGE, MAXDOUBLE,  -MAXDOUBLE, 0.0),
		/* +INFINITY: dpack-utest-gen.py "float('inf')" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x7f\xf0\x00\x00"
		                         "\x00\x00\x00\x00",     -ERANGE, INFINITY,  -MAXDOUBLE, 0.0),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\xff\xf0\x00\x00"
		                         "\x00\x00\x00\x00",     -ERANGE, -INFINITY, 0.0,      MAXDOUBLE),
		/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\xff\xef\xff\xff"
		                         "\xff\xff\xff\xff",     -ERANGE, -MAXDOUBLE, 0.0,      MAXDOUBLE),
		/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x80\x10\x00\x00"
		                         "\x00\x00\x00\x00",     -ERANGE, -MINDOUBLE, 0.0,      MAXDOUBLE),
		/* -0: dpack-utest-gen.py "- 0.0" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x80\x00\x00\x00"
		                         "\x00\x00\x00\x00",     0,       -0.0,     0.0,      MAXDOUBLE),
		/* +0: dpack-utest-gen.py "0.0" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x00\x00\x00\x00"
		                         "\x00\x00\x00\x00",     0,       0.0,      0.0,      MAXDOUBLE),
		/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x00\x10\x00\x00"
		                         "\x00\x00\x00\x00",     0,       MINDOUBLE,  0.0,      MAXDOUBLE),
		/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x7f\xef\xff\xff"
		                         "\xff\xff\xff\xff",     0,       MAXDOUBLE,  0.0,      MAXDOUBLE),
		/* +INFINITY: dpack-utest-gen.py "float('inf')" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x7f\xf0\x00\x00"
		                         "\x00\x00\x00\x00",     -ERANGE, INFINITY,  0.0,      MAXDOUBLE),
		/* +Not A Number: dpack-utest-gen.py "float('nan')" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x7f\xf8\x00\x00"
		                         "\x00\x00\x00\x00",     -ENOMSG, NAN,       0.0,      MAXDOUBLE),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\xff\xf0\x00\x00"
		                         "\x00\x00\x00\x00",     -ERANGE, -INFINITY, 1.0,      2.0),
		/* -MAXDOUBLE: dpack-utest-gen.py -s "- 1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\xff\xef\xff\xff"
		                         "\xff\xff\xff\xff",     -ERANGE, -MAXDOUBLE, 1.0,      2.0),
		/* -MINDOUBLE: dpack-utest-gen.py "- 2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x80\x10\x00\x00"
		                         "\x00\x00\x00\x00",     -ERANGE, -MINDOUBLE, 1.0,      2.0),
		/* -0: dpack-utest-gen.py "- 0.0" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x80\x00\x00\x00"
		                         "\x00\x00\x00\x00",     -ERANGE, -0.0,      1.0,      2.0),
		/* +0: dpack-utest-gen.py "0.0" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x00\x00\x00\x00"
		                         "\x00\x00\x00\x00",     -ERANGE, 0.0,       1.0,      2.0),
		/* +1.0: dpack-utest-gen.py "1.0" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x3f\xf0\x00\x00"
		                         "\x00\x00\x00\x00",     0,       1.0,       1.0,      2.0),
		/* +1.5: dpack-utest-gen.py "1.5" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x3f\xf8\x00\x00"
		                         "\x00\x00\x00\x00",     0,       1.5f,      1.0,      2.0),
		/* +2.0: dpack-utest-gen.py "2.0" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x40\x00\x00\x00"
		                         "\x00\x00\x00\x00", 0,       2.0,       1.0,      2.0),
		/* +MINDOUBLE: dpack-utest-gen.py "2.22507385850720138309023271733240406e-308" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x00\x10\x00\x00"
		                         "\x00\x00\x00\x00",     -ERANGE, MINDOUBLE, 1.0,      2.0),
		/* +MAXDOUBLE: dpack-utest-gen.py  "1.79769313486231570814527423731704357e+308" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x7f\xef\xff\xff"
		                         "\xff\xff\xff\xff",     -ERANGE, MAXDOUBLE, 1.0,      2.0),
		/* +INFINITY: dpack-utest-gen.py "float('inf')" */
		DPACK_UTEST_DOUBLE_RANGE("\xcb"
		                         "\x7f\xf0\x00\x00"
		                         "\x00\x00\x00\x00",     -ERANGE, INFINITY,  1.0,      2.0),

		/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
		DPACK_UTEST_DOUBLE_RANGE("\xca\x7f\xc0\x00\x00", -ENOMSG, NAN,       0.0,      1.0),
		/* +0 (float): dpack-utest-gen.py "0.0" */
		DPACK_UTEST_DOUBLE_RANGE("\xca\x00\x00\x00\x00", -ENOMSG, 0.0,       0.0,      1.0)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	double                val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_double_range(NULL,
	                                                      1.0,
	                                                      2.0,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_double_range(&dec,
	                                                      1.0,
	                                                      2.0,
	                                                      &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_double_range(&dec,
	                                                      -INFINITY,
	                                                      2.0,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_double_range(&dec,
	                                                      1.0,
	                                                      INFINITY,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_double_range(&dec,
	                                                      NAN,
	                                                      2.0,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_double_range(&dec,
	                                                      -NAN,
	                                                      2.0,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_double_range(&dec,
	                                                      1.0,
	                                                      NAN,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_double_range(&dec,
	                                                      1.0,
	                                                      -NAN,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_double_range(&dec,
	                                                      1.0,
	                                                      2.0,
	                                                      NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_double_range);
}

static const struct CMUnitTest dpack_double_utests[] = {
	cmocka_unit_test(dpack_scalar_utest_encode_double),
	cmocka_unit_test(dpack_scalar_utest_decode_double),
	cmocka_unit_test(dpack_scalar_utest_decode_double_min),
	cmocka_unit_test(dpack_scalar_utest_decode_double_max),
	cmocka_unit_test(dpack_scalar_utest_decode_double_range)
};

int
main(void)
{
	return cmocka_run_group_tests_name(
		"Double precision doubleing point numbers",
		dpack_double_utests,
		NULL,
		NULL);
}
