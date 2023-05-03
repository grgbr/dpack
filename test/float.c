#include "utest.h"
#include "dpack/scalar.h"
#include "dpack/codec.h"
#include <math.h>
#include <errno.h>

#define DPACK_UTEST_FLOAT(_packed, _error, _value) \
	{ \
		.packed    = _packed, \
		.size      = sizeof(_packed) - 1, \
		.error     = _error, \
		.value.f32 = _value \
	}

static int
dpack_scalar_utest_pack_float(struct dpack_encoder *                 encoder,
                              const struct dpack_scalar_utest_data * data)
{
	assert_int_equal(data->size, DPACK_FLOAT_SIZE);

	return dpack_encode_float(encoder, data->value.f32);
}

static void
dpack_scalar_utest_encode_float(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_FLOAT("\xca\xff\x80\x00\x00", 0, -INFINITY),
		/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT("\xca\xff\x7f\xff\xff", 0, -MAXFLOAT),
		/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT("\xca\x80\x80\x00\x00", 0, -MINFLOAT),
		/* -0: dpack-utest-gen.py -s "- 0.0" */
		DPACK_UTEST_FLOAT("\xca\x80\x00\x00\x00", 0, -0.0f),
		/* +0: dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_FLOAT("\xca\x00\x00\x00\x00", 0, 0.0f),
		/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT("\xca\x00\x80\x00\x00", 0, MINFLOAT),
		/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT("\xca\x7f\x7f\xff\xff", 0, MAXFLOAT),
		/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
		DPACK_UTEST_FLOAT("\xca\x7f\x80\x00\x00", 0, INFINITY)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	float                val = NAN;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;
	char                 buff[DPACK_FLOAT_SIZE];

	expect_assert_failure(ret = dpack_encode_float(NULL, val));
	expect_assert_failure(ret = dpack_encode_float(&enc, val));

	dpack_encoder_init_buffer(&enc, buff, sizeof(buff));
	expect_assert_failure(ret = dpack_encode_float(&enc, NAN));
	dpack_encoder_fini(&enc);
#endif

	dpack_scalar_utest_encode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_pack_float);
}

static void
dpack_scalar_utest_unpack_float(struct dpack_decoder *                 decoder,
                                const struct dpack_scalar_utest_data * data)
{
	float val;

	assert_int_equal(dpack_decode_float(decoder, &val), data->error);
	if (!data->error) {
		assert_float_equal(val, data->value.f32, 0.0);
		assert_int_equal(data->size, DPACK_FLOAT_SIZE);
	}
}

static void
dpack_scalar_utest_decode_float(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -Not A Number: dpack-utest-gen.py -s "float('-nan')" */
		DPACK_UTEST_FLOAT("\xca\xff\xc0\x00\x00", -ENOMSG, -NAN),
		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_FLOAT("\xca\xff\x80\x00\x00", 0,       -INFINITY),
		/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT("\xca\xff\x7f\xff\xff", 0,       -MAXFLOAT),
		/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT("\xca\x80\x80\x00\x00", 0,       -MINFLOAT),
		/* -0: dpack-utest-gen.py -s "- 0.0" */
		DPACK_UTEST_FLOAT("\xca\x80\x00\x00\x00", 0,       -0.0f),
		/* +0: dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_FLOAT("\xca\x00\x00\x00\x00", 0,       0.0f),
		/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT("\xca\x00\x80\x00\x00", 0,       MINFLOAT),
		/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT("\xca\x7f\x7f\xff\xff", 0,       MAXFLOAT),
		/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
		DPACK_UTEST_FLOAT("\xca\x7f\x80\x00\x00", 0,       INFINITY),
		/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
		DPACK_UTEST_FLOAT("\xca\x7f\xc0\x00\x00", -ENOMSG, NAN),
		/* +0 (double): dpack-utest-gen.py "float(0.0)" */
		DPACK_UTEST_FLOAT("\xcb"
		                  "\x00\x00\x00\x00"
		                  "\x00\x00\x00\x00",     -ENOMSG, 0)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	float                val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_float(NULL, &val));
	expect_assert_failure(ret = dpack_decode_float(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_float(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_float);
}

#define DPACK_UTEST_FLOAT_MIN(_packed, _error, _value, _low) \
	{ \
		.packed    = _packed, \
		.size      = sizeof(_packed) - 1, \
		.error     = _error, \
		.value.f32 = _value, \
		.low.f32   = _low \
	}

static void
dpack_scalar_utest_unpack_float_min(
	struct dpack_decoder *                 decoder,
        const struct dpack_scalar_utest_data * data)
{
	float val;

	assert_int_equal(dpack_decode_float_min(decoder, data->low.f32, &val),
	                 data->error);
	if (!data->error) {
		assert_float_equal(val, data->value.f32, 0.0);
		assert_int_equal(data->size, DPACK_FLOAT_SIZE);
	}
}

static void
dpack_scalar_utest_decode_float_min(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -Not A Number: dpack-utest-gen.py -s "float('-nan')" */
		DPACK_UTEST_FLOAT_MIN("\xca\xff\xc0\x00\x00", -ENOMSG, -NAN,      0.0f),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_FLOAT_MIN("\xca\xff\x80\x00\x00", -ERANGE, -INFINITY, -MAXFLOAT),
		/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_MIN("\xca\xff\x7f\xff\xff", 0,       -MAXFLOAT, -MAXFLOAT),
		/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_MIN("\xca\x80\x80\x00\x00", 0,       -MINFLOAT, -MAXFLOAT),
		/* -0: dpack-utest-gen.py -s "- 0.0" */
		DPACK_UTEST_FLOAT_MIN("\xca\x80\x00\x00\x00", 0,       -0.0f,     -MAXFLOAT),
		/* +0: dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_FLOAT_MIN("\xca\x00\x00\x00\x00", 0,       0.0f,      -MAXFLOAT),
		/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_MIN("\xca\x00\x80\x00\x00", 0,       MINFLOAT,  -MAXFLOAT),
		/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_MIN("\xca\x7f\x7f\xff\xff", 0,       MAXFLOAT,  -MAXFLOAT),
		/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
		DPACK_UTEST_FLOAT_MIN("\xca\x7f\x80\x00\x00", 0,       INFINITY,  -MAXFLOAT),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_FLOAT_MIN("\xca\xff\x80\x00\x00", -ERANGE, -INFINITY, 0.0f),
		/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_MIN("\xca\xff\x7f\xff\xff", -ERANGE, -MAXFLOAT, 0.0f),
		/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_MIN("\xca\x80\x80\x00\x00", -ERANGE, -MINFLOAT, 0.0f),
		/* -0: dpack-utest-gen.py -s "- 0.0" */
		DPACK_UTEST_FLOAT_MIN("\xca\x80\x00\x00\x00", 0,       -0.0f,     0.0f),
		/* +0: dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_FLOAT_MIN("\xca\x00\x00\x00\x00", 0,       0.0f,      0.0f),
		/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_MIN("\xca\x00\x80\x00\x00", 0,       MINFLOAT,  0.0f),
		/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_MIN("\xca\x7f\x7f\xff\xff", 0,       MAXFLOAT,  0.0f),
		/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
		DPACK_UTEST_FLOAT_MIN("\xca\x7f\x80\x00\x00", 0,       INFINITY,  0.0f),
		/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
		DPACK_UTEST_FLOAT_MIN("\xca\x7f\xc0\x00\x00", -ENOMSG, NAN,       0.0f),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_FLOAT_MIN("\xca\xff\x80\x00\x00", -ERANGE, -INFINITY, MAXFLOAT),
		/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_MIN("\xca\xff\x7f\xff\xff", -ERANGE, -MAXFLOAT, MAXFLOAT),
		/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_MIN("\xca\x80\x80\x00\x00", -ERANGE, -MINFLOAT, MAXFLOAT),
		/* -0: dpack-utest-gen.py -s "- 0.0" */
		DPACK_UTEST_FLOAT_MIN("\xca\x80\x00\x00\x00", -ERANGE, -0.0f,     MAXFLOAT),
		/* +0: dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_FLOAT_MIN("\xca\x00\x00\x00\x00", -ERANGE, 0.0f,      MAXFLOAT),
		/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_MIN("\xca\x00\x80\x00\x00", -ERANGE, MINFLOAT,  MAXFLOAT),
		/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_MIN("\xca\x7f\x7f\xff\xff", 0,       MAXFLOAT,  MAXFLOAT),
		/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
		DPACK_UTEST_FLOAT_MIN("\xca\x7f\x80\x00\x00", 0,       INFINITY,  MAXFLOAT),

		/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
		DPACK_UTEST_FLOAT_MIN("\xca\x7f\xc0\x00\x00", -ENOMSG, NAN,       0.0f),
		/* +0 (double): dpack-utest-gen.py "float(0.0)" */
		DPACK_UTEST_FLOAT_MIN("\xcb"
		                      "\x00\x00\x00\x00"
		                      "\x00\x00\x00\x00",     -ENOMSG, 0.0,       0.0f)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	float                val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_float_min(NULL, 1.0f, &val));
	expect_assert_failure(ret = dpack_decode_float_min(&dec, 1.0f, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_float_min(&dec, -INFINITY, &val));
	expect_assert_failure(ret = dpack_decode_float_min(&dec, INFINITY, &val));
	expect_assert_failure(ret = dpack_decode_float_min(&dec, NAN, &val));
	expect_assert_failure(ret = dpack_decode_float_min(&dec, -NAN, &val));
	expect_assert_failure(ret = dpack_decode_float_min(&dec, 1.0f, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_float_min);
}

#define DPACK_UTEST_FLOAT_MAX(_packed, _error, _value, _high) \
	{ \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.f32  = _value, \
		.high.f32   = _high \
	}

static void
dpack_scalar_utest_unpack_float_max(
	struct dpack_decoder *                 decoder,
        const struct dpack_scalar_utest_data * data)
{
	float val;

	assert_int_equal(dpack_decode_float_max(decoder, data->high.f32, &val),
	                 data->error);
	if (!data->error) {
		assert_float_equal(val, data->value.f32, 0.0);
		assert_int_equal(data->size, DPACK_FLOAT_SIZE);
	}
}

static void
dpack_scalar_utest_decode_float_max(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -Not A Number: dpack-utest-gen.py -s "float('-nan')" */
		DPACK_UTEST_FLOAT_MAX("\xca\xff\xc0\x00\x00", -ENOMSG, -NAN,      0.0f),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_FLOAT_MAX("\xca\xff\x80\x00\x00", 0,       -INFINITY, -MAXFLOAT),
		/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_MAX("\xca\xff\x7f\xff\xff", 0,       -MAXFLOAT, -MAXFLOAT),
		/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_MAX("\xca\x80\x80\x00\x00", -ERANGE, -MINFLOAT, -MAXFLOAT),
		/* -0: dpack-utest-gen.py -s "- 0.0" */
		DPACK_UTEST_FLOAT_MAX("\xca\x80\x00\x00\x00", -ERANGE, -0.0f,     -MAXFLOAT),
		/* +0: dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_FLOAT_MAX("\xca\x00\x00\x00\x00", -ERANGE, 0.0f,      -MAXFLOAT),
		/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_MAX("\xca\x00\x80\x00\x00", -ERANGE, MINFLOAT,  -MAXFLOAT),
		/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_MAX("\xca\x7f\x7f\xff\xff", -ERANGE, MAXFLOAT,  -MAXFLOAT),
		/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
		DPACK_UTEST_FLOAT_MAX("\xca\x7f\x80\x00\x00", -ERANGE, INFINITY,  -MAXFLOAT),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_FLOAT_MAX("\xca\xff\x80\x00\x00", 0,       -INFINITY, 0.0f),
		/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_MAX("\xca\xff\x7f\xff\xff", 0,       -MAXFLOAT, 0.0f),
		/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_MAX("\xca\x80\x80\x00\x00", 0,       -MINFLOAT, 0.0f),
		/* -0: dpack-utest-gen.py -s "- 0.0" */
		DPACK_UTEST_FLOAT_MAX("\xca\x80\x00\x00\x00", 0,       -0.0f,     0.0f),
		/* +0: dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_FLOAT_MAX("\xca\x00\x00\x00\x00", 0,       0.0f,      0.0f),
		/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_MAX("\xca\x00\x80\x00\x00", -ERANGE, MINFLOAT,  0.0f),
		/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_MAX("\xca\x7f\x7f\xff\xff", -ERANGE, MAXFLOAT,  0.0f),
		/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
		DPACK_UTEST_FLOAT_MAX("\xca\x7f\x80\x00\x00", -ERANGE, INFINITY,  0.0f),
		/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
		DPACK_UTEST_FLOAT_MAX("\xca\x7f\xc0\x00\x00", -ENOMSG, NAN,       0.0f),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_FLOAT_MAX("\xca\xff\x80\x00\x00", 0,       -INFINITY, MAXFLOAT),
		/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_MAX("\xca\xff\x7f\xff\xff", 0,       -MAXFLOAT, MAXFLOAT),
		/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_MAX("\xca\x80\x80\x00\x00", 0,       -MINFLOAT, MAXFLOAT),
		/* -0: dpack-utest-gen.py -s "- 0.0" */
		DPACK_UTEST_FLOAT_MAX("\xca\x80\x00\x00\x00", 0,       -0.0f,     MAXFLOAT),
		/* +0: dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_FLOAT_MAX("\xca\x00\x00\x00\x00", 0,       0.0f,      MAXFLOAT),
		/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_MAX("\xca\x00\x80\x00\x00", 0,       MINFLOAT,  MAXFLOAT),
		/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_MAX("\xca\x7f\x7f\xff\xff", 0,       MAXFLOAT,  MAXFLOAT),
		/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
		DPACK_UTEST_FLOAT_MAX("\xca\x7f\x80\x00\x00", -ERANGE, INFINITY,  MAXFLOAT),

		/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
		DPACK_UTEST_FLOAT_MAX("\xca\x7f\xc0\x00\x00", -ENOMSG, NAN,       0.0f),
		/* +0 (double): dpack-utest-gen.py "float(0.0)" */
		DPACK_UTEST_FLOAT_MAX("\xcb"
		                      "\x00\x00\x00\x00"
		                      "\x00\x00\x00\x00",     -ENOMSG, 0.0,       0.0f)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	float                val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_float_max(NULL, 1.0f, &val));
	expect_assert_failure(ret = dpack_decode_float_max(&dec, 1.0f, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_float_max(&dec, -INFINITY, &val));
	expect_assert_failure(ret = dpack_decode_float_max(&dec, INFINITY, &val));
	expect_assert_failure(ret = dpack_decode_float_max(&dec, NAN, &val));
	expect_assert_failure(ret = dpack_decode_float_max(&dec, -NAN, &val));
	expect_assert_failure(ret = dpack_decode_float_max(&dec, 1.0f, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_float_max);
}

#define DPACK_UTEST_FLOAT_RANGE(_packed, _error, _value, _low, _high) \
	{ \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.f32  = _value, \
		.low.f32    = _low, \
		.high.f32   = _high \
	}

static void
dpack_scalar_utest_unpack_float_range(
	struct dpack_decoder *                 decoder,
        const struct dpack_scalar_utest_data * data)
{
	float val;

	assert_int_equal(dpack_decode_float_range(decoder,
	                                          data->low.f32,
	                                          data->high.f32,
	                                          &val),
	                 data->error);
	if (!data->error) {
		assert_float_equal(val, data->value.f32, 0.0);
		assert_int_equal(data->size, DPACK_FLOAT_SIZE);
	}
}

static void
dpack_scalar_utest_decode_float_range(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -Not A Number: dpack-utest-gen.py -s "float('-nan')" */
		DPACK_UTEST_FLOAT_RANGE("\xca\xff\xc0\x00\x00", -ENOMSG, -NAN,      0.0f, 1.0f),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_FLOAT_RANGE("\xca\xff\x80\x00\x00", -ERANGE, -INFINITY, -MAXFLOAT, 0.0f),
		/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_RANGE("\xca\xff\x7f\xff\xff", 0,       -MAXFLOAT, -MAXFLOAT, 0.0f),
		/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x80\x80\x00\x00", 0,       -MINFLOAT, -MAXFLOAT, 0.0f),
		/* -0: dpack-utest-gen.py -s "- 0.0" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x80\x00\x00\x00", 0,       -0.0f,     -MAXFLOAT, 0.0f),
		/* +0: dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x00\x00\x00\x00", 0,       0.0f,      -MAXFLOAT, 0.0f),
		/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x00\x80\x00\x00", -ERANGE, MINFLOAT,  -MAXFLOAT, 0.0f),
		/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x7f\x7f\xff\xff", -ERANGE, MAXFLOAT,  -MAXFLOAT, 0.0f),
		/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x7f\x80\x00\x00", -ERANGE, INFINITY,  -MAXFLOAT, 0.0f),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_FLOAT_RANGE("\xca\xff\x80\x00\x00", -ERANGE, -INFINITY, 0.0f,      MAXFLOAT),
		/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_RANGE("\xca\xff\x7f\xff\xff", -ERANGE, -MAXFLOAT, 0.0f,      MAXFLOAT),
		/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x80\x80\x00\x00", -ERANGE, -MINFLOAT, 0.0f,      MAXFLOAT),
		/* -0: dpack-utest-gen.py -s "- 0.0" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x80\x00\x00\x00", 0,       -0.0f,     0.0f,      MAXFLOAT),
		/* +0: dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x00\x00\x00\x00", 0,       0.0f,      0.0f,      MAXFLOAT),
		/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x00\x80\x00\x00", 0,       MINFLOAT,  0.0f,      MAXFLOAT),
		/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x7f\x7f\xff\xff", 0,       MAXFLOAT,  0.0f,      MAXFLOAT),
		/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x7f\x80\x00\x00", -ERANGE, INFINITY,  0.0f,      MAXFLOAT),
		/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x7f\xc0\x00\x00", -ENOMSG, NAN,       0.0f,      MAXFLOAT),

		/* -INFINITY: dpack-utest-gen.py -s "float('-inf')" */
		DPACK_UTEST_FLOAT_RANGE("\xca\xff\x80\x00\x00", -ERANGE, -INFINITY, 1.0f,      2.0f),
		/* -MAXFLOAT: dpack-utest-gen.py -s "- 3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_RANGE("\xca\xff\x7f\xff\xff", -ERANGE, -MAXFLOAT, 1.0f,      2.0f),
		/* -MINFLOAT: dpack-utest-gen.py -s "- 1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x80\x80\x00\x00", -ERANGE, -MINFLOAT, 1.0f,      2.0f),
		/* -0: dpack-utest-gen.py -s "- 0.0" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x80\x00\x00\x00", -ERANGE, -0.0f,     1.0f,      2.0f),
		/* +0: dpack-utest-gen.py -s "0.0" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x00\x00\x00\x00", -ERANGE, 0.0f,      1.0f,      2.0f),
		/* +1.0: dpack-utest-gen.py -s "1.0" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x3f\x80\x00\x00", 0,       1.0f,      1.0f,      2.0f),
		/* +1.5: dpack-utest-gen.py -s "1.5" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x3f\xc0\x00\x00", 0,       1.5f,      1.0f,      2.0f),
		/* +2.0: dpack-utest-gen.py -s "2.0" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x40\x00\x00\x00", 0,       2.0f,      1.0f,      2.0f),
		/* +MINFLOAT: dpack-utest-gen.py -s "1.17549435082228750796873653722224568e-38" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x00\x80\x00\x00", -ERANGE, MINFLOAT,  1.0f,      2.0f),
		/* +MAXFLOAT: dpack-utest-gen.py -s "3.40282346638528859811704183484516925e+38" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x7f\x7f\xff\xff", -ERANGE, MAXFLOAT,  1.0f,      2.0f),
		/* +INFINITY: dpack-utest-gen.py -s "float('inf')" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x7f\x80\x00\x00", -ERANGE, INFINITY,  1.0f,      2.0f),

		/* +Not A Number: dpack-utest-gen.py -s "float('nan')" */
		DPACK_UTEST_FLOAT_RANGE("\xca\x7f\xc0\x00\x00", -ENOMSG, NAN,       0.0f,      1.0f),
		/* +0 (double): dpack-utest-gen.py "float(0.0)" */
		DPACK_UTEST_FLOAT_RANGE("\xcb"
		                        "\x00\x00\x00\x00"
		                        "\x00\x00\x00\x00",     -ENOMSG, 0.0,       0.0f,      1.0f)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	float                val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_float_range(NULL,
	                                                     1.0f,
	                                                     2.0f,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_float_range(&dec,
	                                                     1.0f,
	                                                     2.0f,
	                                                     &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_float_range(&dec,
	                                                     -INFINITY,
	                                                     2.0f,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_float_range(&dec,
	                                                     1.0f,
	                                                     INFINITY,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_float_range(&dec,
	                                                     NAN,
	                                                     2.0f,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_float_range(&dec,
	                                                     -NAN,
	                                                     2.0f,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_float_range(&dec,
	                                                     1.0f,
	                                                     NAN,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_float_range(&dec,
	                                                     1.0f,
	                                                     -NAN,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_float_range(&dec,
	                                                     1.0f,
	                                                     2.0f,
	                                                     NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_float_range);
}

static const struct CMUnitTest dpack_float_utests[] = {
	cmocka_unit_test(dpack_scalar_utest_encode_float),
	cmocka_unit_test(dpack_scalar_utest_decode_float),
	cmocka_unit_test(dpack_scalar_utest_decode_float_min),
	cmocka_unit_test(dpack_scalar_utest_decode_float_max),
	cmocka_unit_test(dpack_scalar_utest_decode_float_range)
};

int
main(void)
{
	return cmocka_run_group_tests_name(
		"Single precision floating point numbers",
		dpack_float_utests,
		NULL,
		NULL);
}
