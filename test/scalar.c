#include "utest.h"
#include "dpack/scalar.h"
#include "dpack/codec.h"
#include <errno.h>

struct dpack_scalar_utest_data;

typedef void (dpack_utest_unpack_fn)(struct dpack_decoder *,
                                     const struct dpack_scalar_utest_data *);

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
};

static void
dpack_scalar_utest(const struct dpack_scalar_utest_data * data,
                   unsigned int                           nr,
                   dpack_utest_unpack_fn *                unpack)

{
	struct dpack_decoder dec = { 0, };
	unsigned int         d;

	for (d = 0; d < nr; d++) {
		dpack_decoder_init_buffer(&dec, data[d].packed, data[d].size);

		unpack(&dec, &data[d]);

		dpack_decoder_fini(&dec);
	}
}

#define DPACK_UTEST_BOOL(_packed, _error, _value) \
	{ \
		.packed        = _packed, \
		.size          = sizeof(_packed) - 1, \
		.error         = _error, \
		.value.boolean = _value \
	}

static void
dpack_scalar_utest_unpack_bool(struct dpack_decoder *                 decoder,
                               const struct dpack_scalar_utest_data * data)
{
	bool val;

	assert_int_equal(dpack_decode_bool(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->value.boolean);
}

static void
dpack_scalar_utest_bool(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* 1 */
		DPACK_UTEST_BOOL("\x01", -ENOMSG, 0),
		/* 0 */
		DPACK_UTEST_BOOL("\x00", -ENOMSG, 0),
		/* True */
		DPACK_UTEST_BOOL("\xc3", 0,       true),
		/* False */
		DPACK_UTEST_BOOL("\xc2", 0,       false)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	bool                 val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_bool(NULL, &val));
	expect_assert_failure(ret = dpack_decode_bool(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_bool(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest(data,
	                   array_nr(data),
	                   dpack_scalar_utest_unpack_bool);
}

#define DPACK_UTEST_UINT8(_packed, _error, _value) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.uint8 = _value \
	}

static void
dpack_scalar_utest_unpack_uint8(struct dpack_decoder *                 decoder,
                                const struct dpack_scalar_utest_data * data)
{
	uint8_t val;

	assert_int_equal(dpack_decode_uint8(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->value.uint8);
}

static void
dpack_scalar_utest_uint8(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT8("\xff",         -ENOMSG, 0),
		/* 0 */
		DPACK_UTEST_UINT8("\x00",         0,       UINT8_C(0)),
		/* 1 */
		DPACK_UTEST_UINT8("\x01",         0,       UINT8_C(1)),
		/* 255 */
		DPACK_UTEST_UINT8("\xcc\xff",     0,       UINT8_C(255)),
		/* 256 */
		DPACK_UTEST_UINT8("\xcd\x01\x00", -ERANGE, 0)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint8_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint8(NULL, &val));
	expect_assert_failure(ret = dpack_decode_uint8(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint8(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest(data,
	                   array_nr(data),
	                   dpack_scalar_utest_unpack_uint8);
}

#define DPACK_UTEST_UINT8_MIN(_packed, _error, _value, _low) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.uint8 = _value, \
		.low.uint8   = _low \
	}

static void
dpack_scalar_utest_unpack_uint8_min(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint8_t val;

	assert_int_equal(dpack_decode_uint8_min(decoder, data->low.uint8, &val),
	                 data->error);
	if (!data->error)
		assert_int_equal(val, data->value.uint8);
}

static void
dpack_scalar_utest_uint8_min(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT8_MIN("\xff",         -ENOMSG, 0,            UINT8_C(1)),
		/* 0 */
		DPACK_UTEST_UINT8_MIN("\x00",         -ERANGE, 0,            UINT8_C(1)),
		/* 1 */
		DPACK_UTEST_UINT8_MIN("\x01",         0,       UINT8_C(1),   UINT8_C(1)),
		/* 255 */
		DPACK_UTEST_UINT8_MIN("\xcc\xff",     0,       UINT8_C(255), UINT8_C(1)),
		/* 256 */
		DPACK_UTEST_UINT8_MIN("\xcd\x01\x00", -ERANGE, 0,            UINT8_C(1)),

		/* 1 */
		DPACK_UTEST_UINT8_MIN("\x01",         -ERANGE, 0,            UINT8_C(64)),
		/* 63 */
		DPACK_UTEST_UINT8_MIN("\x3f",         -ERANGE, 0,            UINT8_C(64)),
		/* 64 */
		DPACK_UTEST_UINT8_MIN("\x40",         0,       UINT8_C(64),  UINT8_C(64)),
		/* 65 */
		DPACK_UTEST_UINT8_MIN("\x41",         0,       UINT8_C(65),  UINT8_C(64)),
		/* 255 */
		DPACK_UTEST_UINT8_MIN("\xcc\xff",     0,       UINT8_C(255), UINT8_C(64)),

		/* 253 */
		DPACK_UTEST_UINT8_MIN("\xcc\xfd",     -ERANGE, 0,            UINT8_C(254)),
		/* 254 */
		DPACK_UTEST_UINT8_MIN("\xcc\xfe",     0,       UINT8_C(254), UINT8_C(254)),
		/* 255 */
		DPACK_UTEST_UINT8_MIN("\xcc\xff",     0,       UINT8_C(255), UINT8_C(254)),
		/* 256 */
		DPACK_UTEST_UINT8_MIN("\xcd\x01\x00", -ERANGE, 0,            UINT8_C(254))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint8_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint8_min(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_uint8_min(&dec, 1, &val));
	expect_assert_failure(ret = dpack_decode_uint8_min(NULL, 0, &val));
	expect_assert_failure(ret = dpack_decode_uint8_min(NULL,
	                                                   UINT8_MAX,
	                                                   &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint8_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest(data,
	                   array_nr(data),
	                   dpack_scalar_utest_unpack_uint8_min);
}

#define DPACK_UTEST_INT8(_packed, _error, _value) \
	{ \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.int8 = _value \
	}

static void
dpack_scalar_utest_unpack_int8(struct dpack_decoder *                 decoder,
                               const struct dpack_scalar_utest_data * data)
{
	int8_t val;

	assert_int_equal(dpack_decode_int8(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->value.int8);
}

static void
dpack_scalar_utest_int8(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -129 */
		DPACK_UTEST_INT8("\xd1\xff\x7f", -ERANGE, 0),
		/* -128 */
		DPACK_UTEST_INT8("\xd0\x80",     0,       INT8_C(-128)),
		/* -127 */
		DPACK_UTEST_INT8("\xd0\x81",     0,       INT8_C(-127)),
		/* -1 */
		DPACK_UTEST_INT8("\xff",         0,       INT8_C(-1)),
		/* 0 */
		DPACK_UTEST_INT8("\x00",         0,       INT8_C(0)),
		/* 1 */
		DPACK_UTEST_INT8("\x01",         0,       INT8_C(1)),
		/* 127 */
		DPACK_UTEST_INT8("\x7f",         0,       INT8_C(127)),
		/* 128 */
		DPACK_UTEST_INT8("\xcc\x80",     -ERANGE, 0)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int8_t               val;
	struct dpack_decoder dec = { 0, };

	expect_assert_failure(dpack_decode_int8(NULL, &val));
	expect_assert_failure(dpack_decode_int8(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(dpack_decode_int8(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest(data,
	                   array_nr(data),
	                   dpack_scalar_utest_unpack_int8);
}

#define DPACK_UTEST_INT8_MIN(_packed, _error, _value, _low) \
	{ \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.int8 = _value, \
		.low.int8   = _low \
	}

static void
dpack_scalar_utest_unpack_int8_min(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	int8_t val;

	assert_int_equal(dpack_decode_int8_min(decoder, data->low.int8, &val),
	                 data->error);
	if (!data->error)
		assert_int_equal(val, data->value.int8);
}

static void
dpack_scalar_utest_int8_min(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -129 */
		DPACK_UTEST_INT8_MIN("\xd1\xff\x7f", -ERANGE, 0,            INT8_C(0)),
		/* -128 */
		DPACK_UTEST_INT8_MIN("\xd0\x80",     -ERANGE, 0,            INT8_C(0)),
		/* -127 */
		DPACK_UTEST_INT8_MIN("\xd0\x81",     0,       INT8_C(-127), INT8_C(-127)),
		/* -126 */
		DPACK_UTEST_INT8_MIN("\xd0\x82",     0,       INT8_C(-126), INT8_C(-127)),

		/* -1 */
		DPACK_UTEST_INT8_MIN("\xff",         -ERANGE, 0,            INT8_C(0)),
		/* 0 */
		DPACK_UTEST_INT8_MIN("\x00",         0,       INT8_C(0),    INT8_C(0)),
		/* 1 */
		DPACK_UTEST_INT8_MIN("\x01",         0,       INT8_C(1),    INT8_C(0)),
		/* 127 */
		DPACK_UTEST_INT8_MIN("\x7f",         0,       INT8_C(127),  INT8_C(0)),
		/* 128 */
		DPACK_UTEST_INT8_MIN("\xcc\x80",     -ERANGE, 0,            INT8_C(0)),

		/* 125 */
		DPACK_UTEST_INT8_MIN("\x7d",         -ERANGE, 0,            INT8_C(126)),
		/* 126 */
		DPACK_UTEST_INT8_MIN("\x7e",         0,       INT8_C(126),  INT8_C(126)),
		/* 127 */
		DPACK_UTEST_INT8_MIN("\x7f",         0,       INT8_C(127),  INT8_C(126)),
		/* 128 */
		DPACK_UTEST_INT8_MIN("\xcc\x80",     -ERANGE, 0,            INT8_C(126))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int8_t              val;
	struct dpack_decoder dec = { 0, };

	expect_assert_failure(dpack_decode_int8_min(NULL, 1, &val));
	expect_assert_failure(dpack_decode_int8_min(&dec, 1, &val));
	expect_assert_failure(dpack_decode_int8_min(NULL, INT8_MIN, &val));
	expect_assert_failure(dpack_decode_int8_min(NULL, INT8_MAX, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(dpack_decode_int8_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest(data,
	                   array_nr(data),
	                   dpack_scalar_utest_unpack_int8_min);
}

#define DPACK_UTEST_UINT16(_packed, _error, _value) \
	{ \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint16 = _value \
	}

static void
dpack_scalar_utest_unpack_uint16(struct dpack_decoder *                 decoder,
                                 const struct dpack_scalar_utest_data * data)
{
	uint16_t val;

	assert_int_equal(dpack_decode_uint16(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->value.uint16);
}

static void
dpack_scalar_utest_uint16(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT16("\xff",                 -ENOMSG, UINT16_C(0)),
		/* 0 */
		DPACK_UTEST_UINT16("\x00",                 0,       UINT16_C(0)),
		/* 1 */
		DPACK_UTEST_UINT16("\x01",                 0,       UINT16_C(1)),
		/* 65535 */
		DPACK_UTEST_UINT16("\xcd\xff\xff",         0,       UINT16_MAX),
		/* 65536 */
		DPACK_UTEST_UINT16("\xce\x00\x01\x00\x00", -ERANGE, UINT16_C(0))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint16_t             val;
	struct dpack_decoder dec = { 0, };

	expect_assert_failure(dpack_decode_uint16(NULL, &val));
	expect_assert_failure(dpack_decode_uint16(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(dpack_decode_uint16(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest(data,
	                   array_nr(data),
	                   dpack_scalar_utest_unpack_uint16);
}

#define DPACK_UTEST_INT16(_packed, _error, _value) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int16 = _value \
	}

static void
dpack_scalar_utest_unpack_int16(struct dpack_decoder *                 decoder,
                                const struct dpack_scalar_utest_data * data)
{
	int16_t val;

	assert_int_equal(dpack_decode_int16(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->value.int16);
}

static void
dpack_scalar_utest_int16(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -32769 */
		DPACK_UTEST_INT16("\xd2"
		                  "\xff\xff\x7f\xff", -ERANGE, INT16_C(0)),
		/* -32768 */
		DPACK_UTEST_INT16("\xd1\x80\x00",     0,       INT16_MIN),
		/* -32767 */
		DPACK_UTEST_INT16("\xd1\x80\x01",     0,       INT16_MIN + 1),
		/* -1 */
		DPACK_UTEST_INT16("\xff",             0,       INT16_C(-1)),
		/* 0 */
		DPACK_UTEST_INT16("\x00",             0,       INT16_C(0)),
		/* 1 */
		DPACK_UTEST_INT16("\x01",             0,       INT16_C(1)),
		/* 32767 */
		DPACK_UTEST_INT16("\xcd\x7f\xff",     0,       INT16_MAX),
		/* 32768 */
		DPACK_UTEST_INT16("\xcd\x80\x00",     -ERANGE, INT16_C(0))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int16_t               val;
	struct dpack_decoder dec = { 0, };

	expect_assert_failure(dpack_decode_int16(NULL, &val));
	expect_assert_failure(dpack_decode_int16(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(dpack_decode_int16(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest(data,
	                   array_nr(data),
	                   dpack_scalar_utest_unpack_int16);
}

#define DPACK_UTEST_UINT32(_packed, _error, _value) \
	{ \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint32 = _value \
	}

static void
dpack_scalar_utest_unpack_uint32(struct dpack_decoder *                 decoder,
                                 const struct dpack_scalar_utest_data * data)
{
	uint32_t val;

	assert_int_equal(dpack_decode_uint32(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->value.uint32);
}

static void
dpack_scalar_utest_uint32(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT32("\xff",             -ENOMSG, UINT32_C(0)),
		/* 0 */
		DPACK_UTEST_UINT32("\x00",             0,       UINT32_C(0)),
		/* 1 */
		DPACK_UTEST_UINT32("\x01",             0,       UINT32_C(1)),
		/* 4294967295 */
		DPACK_UTEST_UINT32("\xce"
		                   "\xff\xff\xff\xff", 0,       UINT32_MAX),
		/* 4294967296 */
		DPACK_UTEST_UINT32("\xcf"
		                   "\x00\x00\x00\x01"
		                   "\x00\x00\x00\x00", -ERANGE, UINT32_C(0))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint32_t             val;
	struct dpack_decoder dec = { 0, };

	expect_assert_failure(dpack_decode_uint32(NULL, &val));
	expect_assert_failure(dpack_decode_uint32(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(dpack_decode_uint32(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest(data,
	                   array_nr(data),
	                   dpack_scalar_utest_unpack_uint32);
}

#define DPACK_UTEST_INT32(_packed, _error, _value) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int32 = _value \
	}

static void
dpack_scalar_utest_unpack_int32(struct dpack_decoder *                 decoder,
                                const struct dpack_scalar_utest_data * data)
{
	int32_t val;

	assert_int_equal(dpack_decode_int32(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->value.int32);
}

static void
dpack_scalar_utest_int32(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -32769 */
		DPACK_UTEST_INT32("\xd3"
		                  "\xff\xff\xff\xff"
		                  "\x7f\xff\xff\xff", -ERANGE, INT32_C(0)),
		/* -32768 */
		DPACK_UTEST_INT32("\xd2"
		                  "\x80\x00\x00\x00", 0,       INT32_MIN),
		/* -32767 */
		DPACK_UTEST_INT32("\xd2"
		                  "\x80\x00\x00\x01", 0,       INT32_MIN + 1),
		/* -1 */
		DPACK_UTEST_INT32("\xff",             0,       INT32_C(-1)),
		/* 0 */
		DPACK_UTEST_INT32("\x00",             0,       INT32_C(0)),
		/* 1 */
		DPACK_UTEST_INT32("\x01",             0,       INT32_C(1)),
		/* 32767 */
		DPACK_UTEST_INT32("\xce"
		                  "\x7f\xff\xff\xff", 0,       INT32_MAX),
		/* 32768 */
		DPACK_UTEST_INT32("\xce"
		                  "\x80\x00\x00\x00", -ERANGE, INT32_C(0))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int32_t               val;
	struct dpack_decoder dec = { 0, };

	expect_assert_failure(dpack_decode_int32(NULL, &val));
	expect_assert_failure(dpack_decode_int32(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(dpack_decode_int32(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest(data,
	                   array_nr(data),
	                   dpack_scalar_utest_unpack_int32);
}

#define DPACK_UTEST_UINT64(_packed, _error, _value) \
	{ \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint64 = _value \
	}

static void
dpack_scalar_utest_unpack_uint64(struct dpack_decoder *                 decoder,
                                 const struct dpack_scalar_utest_data * data)
{
	uint64_t val;

	assert_int_equal(dpack_decode_uint64(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->value.uint64);
}

static void
dpack_scalar_utest_uint64(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT64("\xff",             -ENOMSG, UINT64_C(0)),
		/* 0 */
		DPACK_UTEST_UINT64("\x00",             0,       UINT64_C(0)),
		/* 1 */
		DPACK_UTEST_UINT64("\x01",             0,       UINT64_C(1)),
		/* 18446744073709551615 */
		DPACK_UTEST_UINT64("\xcf"
		                   "\xff\xff\xff\xff"
		                   "\xff\xff\xff\xff", 0,       UINT64_MAX)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint64_t             val;
	struct dpack_decoder dec = { 0, };

	expect_assert_failure(dpack_decode_uint64(NULL, &val));
	expect_assert_failure(dpack_decode_uint64(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(dpack_decode_uint64(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest(data,
	                   array_nr(data),
	                   dpack_scalar_utest_unpack_uint64);
}

#define DPACK_UTEST_INT64(_packed, _error, _value) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int64 = _value \
	}

static void
dpack_scalar_utest_unpack_int64(struct dpack_decoder *                 decoder,
                                const struct dpack_scalar_utest_data * data)
{
	int64_t val;

	assert_int_equal(dpack_decode_int64(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->value.int64);
}

static void
dpack_scalar_utest_int64(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* 9223372036854775808 */
		DPACK_UTEST_INT64("\xcf"
		                  "\x80\x00\x00\x00"
		                  "\x00\x00\x00\x00", -ERANGE, INT64_C(0)),
		/* -9223372036854775808 */
		DPACK_UTEST_INT64("\xd3"
		                  "\x80\x00\x00\x00"
		                  "\x00\x00\x00\x00", 0,       INT64_MIN),
		/* -9223372036854775807 */
		DPACK_UTEST_INT64("\xd3"
		                  "\x80\x00\x00\x00"
		                  "\x00\x00\x00\x01", 0,       INT64_MIN + 1),
		/* -1 */
		DPACK_UTEST_INT64("\xff",             0,       INT64_C(-1)),
		/* 0 */
		DPACK_UTEST_INT64("\x00",             0,       INT64_C(0)),
		/* 1 */
		DPACK_UTEST_INT64("\x01",             0,       INT64_C(1)),
		/* 9223372036854775807 */
		DPACK_UTEST_INT64("\xcf"
		                  "\x7f\xff\xff\xff"
		                  "\xff\xff\xff\xff", 0,       INT64_MAX),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int64_t               val;
	struct dpack_decoder dec = { 0, };

	expect_assert_failure(dpack_decode_int64(NULL, &val));
	expect_assert_failure(dpack_decode_int64(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(dpack_decode_int64(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest(data,
	                   array_nr(data),
	                   dpack_scalar_utest_unpack_int64);
}

static const struct CMUnitTest dpack_stdint_utests[] = {
	cmocka_unit_test(dpack_scalar_utest_bool),
	cmocka_unit_test(dpack_scalar_utest_uint8),
	cmocka_unit_test(dpack_scalar_utest_uint8_min),
	cmocka_unit_test(dpack_scalar_utest_int8),
	cmocka_unit_test(dpack_scalar_utest_int8_min),
	cmocka_unit_test(dpack_scalar_utest_uint16),
	cmocka_unit_test(dpack_scalar_utest_int16),
	cmocka_unit_test(dpack_scalar_utest_uint32),
	cmocka_unit_test(dpack_scalar_utest_int32),
	cmocka_unit_test(dpack_scalar_utest_uint64),
	cmocka_unit_test(dpack_scalar_utest_int64)
};

int
main(void)
{
	return cmocka_run_group_tests_name("integers",
	                                   dpack_stdint_utests,
	                                   NULL,
	                                   NULL);
}
