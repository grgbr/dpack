#include "utest.h"
#include "dpack/scalar.h"
#include "dpack/codec.h"
#include <errno.h>

struct dpack_scalar_utest_data;

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

typedef void (dpack_utest_unpack_fn)(struct dpack_decoder *,
                                     const struct dpack_scalar_utest_data *);

static void
dpack_scalar_utest_decode(const struct dpack_scalar_utest_data * data,
                          unsigned int                           nr,
                          dpack_utest_unpack_fn *                unpack)

{
	struct dpack_decoder dec = { 0, };
	unsigned int         d;

	for (d = 0; d < nr; d++) {
		dpack_decoder_init_buffer(&dec, data[d].packed, data[d].size);

		unpack(&dec, &data[d]);
		if (!data[d].error)
			assert_int_equal(dpack_decoder_data_left(&dec), 0);

		dpack_decoder_fini(&dec);
	}
}

typedef int (dpack_utest_pack_fn)(struct dpack_encoder *,
                                  const struct dpack_scalar_utest_data *);

static void
dpack_scalar_utest_encode(const struct dpack_scalar_utest_data * data,
                          unsigned int                           nr,
                          dpack_utest_pack_fn *                  pack)

{
	struct dpack_encoder enc = { 0, };
	unsigned int         d;

	for (d = 0; d < nr; d++) {
		size_t sz = data[d].size;
		char   buff[sz];
		int    err;

		memset(buff, 0xa5, sz);
		dpack_encoder_init_buffer(&enc, buff, sz);

		err = pack(&enc, &data[d]);
		assert_int_equal(err, data[d].error);
		assert_memory_equal(buff, data[d].packed, sz);

		assert_int_equal(dpack_encoder_space_used(&enc), sz);
		assert_int_equal(dpack_encoder_space_left(&enc), 0);

		dpack_encoder_fini(&enc);
	}
}

#define DPACK_UTEST_BOOL(_packed, _error, _value) \
	{ \
		.packed        = _packed, \
		.size          = sizeof(_packed) - 1, \
		.error         = _error, \
		.value.boolean = _value \
	}

static int
dpack_scalar_utest_pack_bool(struct dpack_encoder *                 encoder,
                             const struct dpack_scalar_utest_data * data)
{
	assert_int_equal(data->size, DPACK_BOOL_SIZE);

	return dpack_encode_bool(encoder, data->value.boolean);
}

static void
dpack_scalar_utest_encode_bool(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* True */
		DPACK_UTEST_BOOL("\xc3", 0, true),
		/* False */
		DPACK_UTEST_BOOL("\xc2", 0, false)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	bool                 val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_encode_bool(NULL, val));
	expect_assert_failure(ret = dpack_encode_bool(&enc, val));
#endif

	dpack_scalar_utest_encode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_pack_bool);
}

static void
dpack_scalar_utest_unpack_bool(struct dpack_decoder *                 decoder,
                               const struct dpack_scalar_utest_data * data)
{
	bool val;

	assert_int_equal(data->size, DPACK_BOOL_SIZE);

	assert_int_equal(dpack_decode_bool(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->value.boolean);
}

static void
dpack_scalar_utest_decode_bool(void ** state __unused)
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

	dpack_scalar_utest_decode(data,
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

static int
dpack_scalar_utest_pack_uint8(struct dpack_encoder *                 encoder,
                              const struct dpack_scalar_utest_data * data)
{
	assert_true(DPACK_UINT8_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT8_SIZE_MAX >= data->size);

	return dpack_encode_uint8(encoder, data->value.uint8);
}

static void
dpack_scalar_utest_encode_uint8(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* 0 */
		DPACK_UTEST_UINT8("\x00",         0,       UINT8_C(0)),
		/* 1 */
		DPACK_UTEST_UINT8("\x01",         0,       UINT8_C(1)),
		/* 255 */
		DPACK_UTEST_UINT8("\xcc\xff",     0,       UINT8_C(255))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint8_t              val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_encode_uint8(NULL, val));
	expect_assert_failure(ret = dpack_encode_uint8(&enc, val));
#endif

	dpack_scalar_utest_encode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_pack_uint8);
}

static void
dpack_scalar_utest_unpack_uint8(struct dpack_decoder *                 decoder,
                                const struct dpack_scalar_utest_data * data)
{
	uint8_t val;

	assert_int_equal(dpack_decode_uint8(decoder, &val), data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT8_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT8_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint8);
}

static void
dpack_scalar_utest_decode_uint8(void ** state __unused)
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

	dpack_scalar_utest_decode(data,
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
	if (data->error)
		return;

	assert_true(DPACK_UINT8_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT8_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint8);
}

static void
dpack_scalar_utest_decode_uint8_min(void ** state __unused)
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

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint8_min(&dec, 0, &val));
	expect_assert_failure(ret = dpack_decode_uint8_min(&dec,
	                                                   UINT8_MAX,
	                                                   &val));
	expect_assert_failure(ret = dpack_decode_uint8_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint8_min);
}

#define DPACK_UTEST_UINT8_MAX(_packed, _error, _value, _high) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.uint8 = _value, \
		.high.uint8  = _high \
	}

static void
dpack_scalar_utest_unpack_uint8_max(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint8_t val;

	assert_int_equal(dpack_decode_uint8_max(decoder,
	                                        data->high.uint8,
	                                        &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT8_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT8_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint8);
}

static void
dpack_scalar_utest_decode_uint8_max(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT8_MAX("\xff",         -ENOMSG, 0,            UINT8_C(1)),
		/* 0 */
		DPACK_UTEST_UINT8_MAX("\x00",         0,       UINT8_C(0),   UINT8_C(1)),
		/* 1 */
		DPACK_UTEST_UINT8_MAX("\x01",         0,       UINT8_C(1),   UINT8_C(1)),
		/* 255 */
		DPACK_UTEST_UINT8_MAX("\xcc\xff",     -ERANGE, 0,            UINT8_C(1)),
		/* 256 */
		DPACK_UTEST_UINT8_MAX("\xcd\x01\x00", -ERANGE, 0,            UINT8_C(1)),

		/* 1 */
		DPACK_UTEST_UINT8_MAX("\x01",         0,       UINT8_C(1),   UINT8_C(64)),
		/* 63 */
		DPACK_UTEST_UINT8_MAX("\x3f",         0,       UINT8_C(63),  UINT8_C(64)),
		/* 64 */
		DPACK_UTEST_UINT8_MAX("\x40",         0,       UINT8_C(64),  UINT8_C(64)),
		/* 65 */
		DPACK_UTEST_UINT8_MAX("\x41",         -ERANGE, 0,            UINT8_C(64)),
		/* 255 */
		DPACK_UTEST_UINT8_MAX("\xcc\xff",     -ERANGE, 0,            UINT8_C(64)),

		/* 253 */
		DPACK_UTEST_UINT8_MAX("\xcc\xfd",     0,       UINT8_C(253), UINT8_C(254)),
		/* 254 */
		DPACK_UTEST_UINT8_MAX("\xcc\xfe",     0,       UINT8_C(254), UINT8_C(254)),
		/* 255 */
		DPACK_UTEST_UINT8_MAX("\xcc\xff",     -ERANGE, 0,            UINT8_C(254)),
		/* 256 */
		DPACK_UTEST_UINT8_MAX("\xcd\x01\x00", -ERANGE, 0,            UINT8_C(254))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint8_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint8_max(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_uint8_max(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint8_max(&dec, 0, &val));
	expect_assert_failure(ret = dpack_decode_uint8_max(&dec,
	                                                   UINT8_MAX,
	                                                   &val));
	expect_assert_failure(ret = dpack_decode_uint8_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint8_max);
}

#define DPACK_UTEST_UINT8_RANGE(_packed, _error, _value, _low, _high) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.uint8 = _value, \
		.low.uint8   = _low, \
		.high.uint8  = _high \
	}

static void
dpack_scalar_utest_unpack_uint8_range(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint8_t val;

	assert_int_equal(dpack_decode_uint8_range(decoder,
	                                          data->low.uint8,
	                                          data->high.uint8,
	                                          &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT8_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT8_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint8);
}

static void
dpack_scalar_utest_decode_uint8_range(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT8_RANGE("\xff",         -ENOMSG, 0,            UINT8_C(1),   UINT8_C(2)),
		/* 0 */
		DPACK_UTEST_UINT8_RANGE("\x00",         -ERANGE, 0,            UINT8_C(1),   UINT8_C(2)),
		/* 1 */
		DPACK_UTEST_UINT8_RANGE("\x01",         0,       UINT8_C(1),   UINT8_C(1),   UINT8_C(2)),
		/* 2 */
		DPACK_UTEST_UINT8_RANGE("\x02",         0,       UINT8_C(2),   UINT8_C(1),   UINT8_C(2)),
		/* 3 */
		DPACK_UTEST_UINT8_RANGE("\x03",         -ERANGE, 0,            UINT8_C(1),   UINT8_C(2)),
		/* 255 */
		DPACK_UTEST_UINT8_RANGE("\xcc\xff",     -ERANGE, 0,            UINT8_C(1),   UINT8_C(2)),
		/* 256 */
		DPACK_UTEST_UINT8_RANGE("\xcd\x01\x00", -ERANGE, 0,            UINT8_C(1),   UINT8_C(2)),

		/* 252 */
		DPACK_UTEST_UINT8_RANGE("\xcc\xfc",     -ERANGE, 0,            UINT8_C(253), UINT8_C(254)),
		/* 253 */
		DPACK_UTEST_UINT8_RANGE("\xcc\xfd",     0,       UINT8_C(253), UINT8_C(253), UINT8_C(254)),
		/* 254 */
		DPACK_UTEST_UINT8_RANGE("\xcc\xfe",     0,       UINT8_C(254), UINT8_C(253), UINT8_C(254)),
		/* 255 */
		DPACK_UTEST_UINT8_RANGE("\xcc\xff",     -ERANGE, 0,            UINT8_C(253), UINT8_C(254)),
		/* 256 */
		DPACK_UTEST_UINT8_RANGE("\xcd\x01\x00", -ERANGE, 0,            UINT8_C(253), UINT8_C(254))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint8_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint8_range(NULL, 1, 2, &val));
	expect_assert_failure(ret = dpack_decode_uint8_range(&dec, 1, 2, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint8_range(&dec, 0, 2, &val));
	expect_assert_failure(ret = dpack_decode_uint8_range(&dec,
	                                                     1,
	                                                     UINT8_MAX,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_uint8_range(&dec, 2, 2, &val));
	expect_assert_failure(ret = dpack_decode_uint8_range(&dec, 1, 2, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint8_range);
}

#define DPACK_UTEST_INT8(_packed, _error, _value) \
	{ \
		.packed     = _packed, \
		.size       = sizeof(_packed) - 1, \
		.error      = _error, \
		.value.int8 = _value \
	}

static int
dpack_scalar_utest_pack_int8(struct dpack_encoder *                 encoder,
                             const struct dpack_scalar_utest_data * data)
{
	assert_true(DPACK_INT8_SIZE_MIN <= data->size);
	assert_true(DPACK_INT8_SIZE_MAX >= data->size);

	return dpack_encode_int8(encoder, data->value.int8);
}

static void
dpack_scalar_utest_encode_int8(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
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
		DPACK_UTEST_INT8("\x7f",         0,       INT8_C(127))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int8_t               val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_encode_int8(NULL, val));
	expect_assert_failure(ret = dpack_encode_int8(&enc, val));
#endif

	dpack_scalar_utest_encode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_pack_int8);
}

static void
dpack_scalar_utest_unpack_int8(struct dpack_decoder *                 decoder,
                               const struct dpack_scalar_utest_data * data)
{
	int8_t val;

	assert_int_equal(dpack_decode_int8(decoder, &val), data->error);

	if (data->error)
		return;

	assert_true(DPACK_INT8_SIZE_MIN <= data->size);
	assert_true(DPACK_INT8_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int8);
}

static void
dpack_scalar_utest_decode_int8(void ** state __unused)
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
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int8(NULL, &val));
	expect_assert_failure(ret = dpack_decode_int8(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int8(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
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
	if (data->error)
		return;

	assert_true(DPACK_INT8_SIZE_MIN <= data->size);
	assert_true(DPACK_INT8_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int8);
}

static void
dpack_scalar_utest_decode_int8_min(void ** state __unused)
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
	int8_t               val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int8_min(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_int8_min(&dec, 1, &val));
	expect_assert_failure(ret = dpack_decode_int8_min(&dec,
	                                                  INT8_MIN,
	                                                  &val));
	expect_assert_failure(ret = dpack_decode_int8_min(&dec,
	                                                  INT8_MAX,
	                                                  &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int8_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int8_min);
}

#define DPACK_UTEST_INT8_MAX(_packed, _error, _value, _high) \
	{ \
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
		DPACK_UTEST_INT8_MAX("\xd1\xff\x7f", -ERANGE, 0,            INT8_C(0)),
		/* -128 */
		DPACK_UTEST_INT8_MAX("\xd0\x80",     0,       INT8_C(-128), INT8_C(0)),
		/* -127 */
		DPACK_UTEST_INT8_MAX("\xd0\x81",     0,       INT8_C(-127), INT8_C(-127)),
		/* -126 */
		DPACK_UTEST_INT8_MAX("\xd0\x82",     -ERANGE, 0,            INT8_C(-127)),

		/* -1 */
		DPACK_UTEST_INT8_MAX("\xff",         0,       INT8_C(-1),   INT8_C(0)),
		/* 0 */
		DPACK_UTEST_INT8_MAX("\x00",         0,       INT8_C(0),    INT8_C(0)),
		/* 1 */
		DPACK_UTEST_INT8_MAX("\x01",         -ERANGE, 0,            INT8_C(0)),
		/* 127 */
		DPACK_UTEST_INT8_MAX("\x7f",         -ERANGE, 0,            INT8_C(0)),
		/* 128 */
		DPACK_UTEST_INT8_MAX("\xcc\x80",     -ERANGE, 0,            INT8_C(0)),

		/* 125 */
		DPACK_UTEST_INT8_MAX("\x7d",         0,       INT8_C(125),  INT8_C(126)),
		/* 126 */
		DPACK_UTEST_INT8_MAX("\x7e",         0,       INT8_C(126),  INT8_C(126)),
		/* 127 */
		DPACK_UTEST_INT8_MAX("\x7f",         -ERANGE, 0,            INT8_C(126)),
		/* 128 */
		DPACK_UTEST_INT8_MAX("\xcc\x80",     -ERANGE, 0,            INT8_C(126))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int8_t               val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int8_max(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_int8_max(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int8_max(&dec,
	                                                  INT8_MIN,
	                                                  &val));
	expect_assert_failure(ret = dpack_decode_int8_max(&dec,
	                                                  INT8_MAX,
	                                                  &val));
	expect_assert_failure(ret = dpack_decode_int8_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int8_max);
}

#define DPACK_UTEST_INT8_RANGE(_packed, _error, _value, _low, _high) \
	{ \
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
		DPACK_UTEST_INT8_RANGE("\xd1\xff\x7f", -ERANGE, 0,            INT8_MIN + 1, INT8_MIN + 2),
		/* -128 */
		DPACK_UTEST_INT8_RANGE("\xd0\x80",     -ERANGE, 0,            INT8_MIN + 1, INT8_MIN + 2),
		/* -127 */
		DPACK_UTEST_INT8_RANGE("\xd0\x81",     0,       INT8_MIN + 1, INT8_MIN + 1, INT8_MIN + 2),
		/* -126 */
		DPACK_UTEST_INT8_RANGE("\xd0\x82",     0,       INT8_MIN + 2, INT8_MIN + 1, INT8_MIN + 2),
		/* -125 */
		DPACK_UTEST_INT8_RANGE("\xd0\x83",     -ERANGE, 0,            INT8_MIN + 1, INT8_MIN + 2),

		/* -1 */
		DPACK_UTEST_INT8_RANGE("\xff",         -ERANGE, 0,            INT8_C(0), INT8_C(1)),
		/* 0 */
		DPACK_UTEST_INT8_RANGE("\x00",         0,       INT8_C(0),    INT8_C(0), INT8_C(1)),
		/* 1 */
		DPACK_UTEST_INT8_RANGE("\x01",         0,       INT8_C(1),    INT8_C(0), INT8_C(1)),
		/* 2 */
		DPACK_UTEST_INT8_RANGE("\x02",         -ERANGE, 0,            INT8_C(0), INT8_C(1)),

		/* 124 */
		DPACK_UTEST_INT8_RANGE("\x7c",         -ERANGE, 0,            INT8_MAX - 2, INT8_MAX - 1),
		/* 125 */
		DPACK_UTEST_INT8_RANGE("\x7d",         0,       INT8_MAX - 2, INT8_MAX - 2, INT8_MAX - 1),
		/* 126 */
		DPACK_UTEST_INT8_RANGE("\x7e",         0,       INT8_MAX - 1, INT8_MAX - 2, INT8_MAX - 1),
		/* 127 */
		DPACK_UTEST_INT8_RANGE("\x7f",         -ERANGE, 0,            INT8_MAX - 2, INT8_MAX - 1),
		/* 128 */
		DPACK_UTEST_INT8_RANGE("\xcc\x80",     -ERANGE, 0,            INT8_MAX - 2, INT8_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int8_t               val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int8_range(NULL, 1, 2, &val));
	expect_assert_failure(ret = dpack_decode_int8_range(&dec, 1, 2, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int8_range(&dec,
	                                                    INT8_MIN,
	                                                    2,
	                                                    &val));
	expect_assert_failure(ret = dpack_decode_int8_range(&dec,
	                                                    1,
	                                                    INT8_MAX,
	                                                    &val));
	expect_assert_failure(ret = dpack_decode_int8_range(&dec, 2, 2, &val));
	expect_assert_failure(ret = dpack_decode_int8_range(&dec, 1, 2, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int8_range);
}

#define DPACK_UTEST_UINT16(_packed, _error, _value) \
	{ \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint16 = _value \
	}

static int
dpack_scalar_utest_pack_uint16(struct dpack_encoder *                 encoder,
                               const struct dpack_scalar_utest_data * data)
{
	assert_true(DPACK_UINT16_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT16_SIZE_MAX >= data->size);

	return dpack_encode_uint16(encoder, data->value.uint16);
}

static void
dpack_scalar_utest_encode_uint16(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* 0 */
		DPACK_UTEST_UINT16("\x00",         0, UINT16_C(0)),
		/* 1 */
		DPACK_UTEST_UINT16("\x01",         0, UINT16_C(1)),
		/* 65535 */
		DPACK_UTEST_UINT16("\xcd\xff\xff", 0, UINT16_MAX),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint16_t             val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_encode_uint16(NULL, val));
	expect_assert_failure(ret = dpack_encode_uint16(&enc, val));
#endif

	dpack_scalar_utest_encode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_pack_uint16);
}

static void
dpack_scalar_utest_unpack_uint16(struct dpack_decoder *                 decoder,
                                 const struct dpack_scalar_utest_data * data)
{
	uint16_t val;

	assert_int_equal(dpack_decode_uint16(decoder, &val), data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT16_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT16_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint16);

}

static void
dpack_scalar_utest_decode_uint16(void ** state __unused)
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
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint16(NULL, &val));
	expect_assert_failure(ret = dpack_decode_uint16(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint16(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint16);
}

#define DPACK_UTEST_UINT16_MIN(_packed, _error, _value, _low) \
	{ \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint16 = _value, \
		.low.uint16   = _low \
	}

static void
dpack_scalar_utest_unpack_uint16_min(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint16_t val;

	assert_int_equal(dpack_decode_uint16_min(decoder,
	                                         data->low.uint16,
	                                         &val),
	                 data->error);

	if (data->error)
		return;

	assert_true(DPACK_UINT16_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT16_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint16);
}

static void
dpack_scalar_utest_decode_uint16_min(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT16_MIN("\xff",             -ENOMSG, 0,              UINT16_C(1)),
		/* 0 */
		DPACK_UTEST_UINT16_MIN("\x00",             -ERANGE, 0,              UINT16_C(1)),
		/* 1 */
		DPACK_UTEST_UINT16_MIN("\x01",             0,       UINT16_C(1),    UINT16_C(1)),
		/* 32767 */
		DPACK_UTEST_UINT16_MIN("\xcd\x7f\xff",     0,       UINT16_MAX / 2, UINT16_C(1)),
		/* 65535 */
		DPACK_UTEST_UINT16_MIN("\xcd\xff\xff",     0,       UINT16_MAX,     UINT16_C(1)),
		/* 65536 */
		DPACK_UTEST_UINT16_MIN("\xce"
		                       "\x00\x01\x00\x00", -ERANGE, 0,              UINT16_C(1)),

		/* -1 */
		DPACK_UTEST_UINT16_MIN("\xff",             -ENOMSG, 0,              UINT16_MAX / 2),
		/* 0 */
		DPACK_UTEST_UINT16_MIN("\x00",             -ERANGE, 0,              UINT16_MAX / 2),
		/* 1 */
		DPACK_UTEST_UINT16_MIN("\x01",             -ERANGE, 0,              UINT16_MAX / 2),
		/* 32766 */
		DPACK_UTEST_UINT16_MIN("\xcd\x7f\xfe",     -ERANGE, 0,              UINT16_MAX / 2),
		/* 32767 */
		DPACK_UTEST_UINT16_MIN("\xcd\x7f\xff",     0,       UINT16_MAX / 2, UINT16_MAX / 2),
		/* 65535 */
		DPACK_UTEST_UINT16_MIN("\xcd\xff\xff",     0,       UINT16_MAX,     UINT16_MAX / 2),
		/* 65536 */
		DPACK_UTEST_UINT16_MIN("\xce"
		                       "\x00\x01\x00\x00", -ERANGE, 0,              UINT16_MAX / 2),

		/* 65533 */
		DPACK_UTEST_UINT16_MIN("\xcd\xff\xfd",     -ERANGE, 0,              UINT16_MAX - 1),
		/* 65534 */
		DPACK_UTEST_UINT16_MIN("\xcd\xff\xfe",     0,       UINT16_MAX - 1, UINT16_MAX - 1),
		/* 65535 */
		DPACK_UTEST_UINT16_MIN("\xcd\xff\xff",     0,       UINT16_MAX,     UINT16_MAX - 1),
		/* 65536 */
		DPACK_UTEST_UINT16_MIN("\xce"
		                       "\x00\x01\x00\x00", -ERANGE, 0,              UINT16_MAX - 1),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint16_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint16_min(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_uint16_min(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint16_min(&dec, 0, &val));
	expect_assert_failure(ret = dpack_decode_uint16_min(&dec,
	                                                    UINT16_MAX,
	                                                    &val));
	expect_assert_failure(ret = dpack_decode_uint16_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint16_min);
}

#define DPACK_UTEST_UINT16_MAX(_packed, _error, _value, _high) \
	{ \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint16 = _value, \
		.high.uint16  = _high \
	}

static void
dpack_scalar_utest_unpack_uint16_max(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint16_t val;

	assert_int_equal(dpack_decode_uint16_max(decoder,
	                                         data->high.uint16,
	                                         &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT16_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT16_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint16);
}

static void
dpack_scalar_utest_decode_uint16_max(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT16_MAX("\xff",              -ENOMSG, 0,                    UINT16_C(1)),
		/* 0 */
		DPACK_UTEST_UINT16_MAX("\x00",              0,       UINT16_C(0),          UINT16_C(1)),
		/* 1 */
		DPACK_UTEST_UINT16_MAX("\x01",              0,       UINT16_C(1),          UINT16_C(1)),
		/* 255 */
		DPACK_UTEST_UINT16_MAX("\xcc\xff",          -ERANGE, 0,                    UINT16_C(1)),
		/* 256 */
		DPACK_UTEST_UINT16_MAX("\xcd\x01\x00",      -ERANGE, 0,                    UINT16_C(1)),

		/* 1 */
		DPACK_UTEST_UINT16_MAX("\x01",              0,       UINT16_C(1),          UINT16_MAX / 2),
		/* 32766 */
		DPACK_UTEST_UINT16_MAX("\xcd\x7f\xfe",      0,       (UINT16_MAX / 2) - 1, UINT16_MAX / 2),
		/* 32767 */
		DPACK_UTEST_UINT16_MAX("\xcd\x7f\xff",      0,       UINT16_MAX / 2,       UINT16_MAX / 2),
		/* 32768 */
		DPACK_UTEST_UINT16_MAX("\xcd\x80\x00",      -ERANGE, 0,                    UINT16_MAX / 2),
		/* 65535 */
		DPACK_UTEST_UINT16_MAX("\xcd\xff\xff",      -ERANGE, 0,                    UINT16_MAX / 2),

		/* 65533 */
		DPACK_UTEST_UINT16_MAX("\xcd\xff\xfd",      0,       UINT16_MAX - 2,       UINT16_MAX - 1),
		/* 65534 */
		DPACK_UTEST_UINT16_MAX("\xcd\xff\xfe",      0,       UINT16_MAX - 1,       UINT16_MAX - 1),
		/* 65535 */
		DPACK_UTEST_UINT16_MAX("\xcd\xff\xff",      -ERANGE, 0,                    UINT16_MAX - 1),
		/* 65536 */
		DPACK_UTEST_UINT16_MAX("\xce"
		                       "\x00\x01\x00\x00" , -ERANGE, 0,                    UINT16_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint16_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint16_max(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_uint16_max(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint16_max(&dec, 0, &val));
	expect_assert_failure(ret = dpack_decode_uint16_max(&dec,
	                                                    UINT16_MAX,
	                                                    &val));
	expect_assert_failure(ret = dpack_decode_uint16_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint16_max);
}

#define DPACK_UTEST_UINT16_RANGE(_packed, _error, _value, _low, _high) \
	{ \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint16 = _value, \
		.low.uint16   = _low, \
		.high.uint16  = _high \
	}

static void
dpack_scalar_utest_unpack_uint16_range(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint16_t val;

	assert_int_equal(dpack_decode_uint16_range(decoder,
	                                           data->low.uint16,
	                                           data->high.uint16,
	                                           &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT16_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT16_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint16);
}

static void
dpack_scalar_utest_decode_uint16_range(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT16_RANGE("\xff",              -ENOMSG, 0,                    UINT16_C(1),    UINT16_C(2)),
		/* 0 */
		DPACK_UTEST_UINT16_RANGE("\x00",              -ERANGE, 0,                    UINT16_C(1),    UINT16_C(2)),
		/* 1 */
		DPACK_UTEST_UINT16_RANGE("\x01",              0,       UINT16_C(1),          UINT16_C(1),    UINT16_C(2)),
		/* 2 */
		DPACK_UTEST_UINT16_RANGE("\x02",              0,       UINT16_C(2),          UINT16_C(1),    UINT16_C(2)),
		/* 3 */
		DPACK_UTEST_UINT16_RANGE("\x03",              -ERANGE, 0,                    UINT16_C(1),    UINT16_C(2)),

		/* 1 */
		DPACK_UTEST_UINT16_RANGE("\x01",              -ERANGE, 0,                    UINT16_MAX / 2, (UINT16_MAX / 2) + 1),
		/* 32766 */
		DPACK_UTEST_UINT16_RANGE("\xcd\x7f\xfe",      -ERANGE, 0,                    UINT16_MAX / 2, (UINT16_MAX / 2) + 1),
		/* 32767 */
		DPACK_UTEST_UINT16_RANGE("\xcd\x7f\xff",      0,       UINT16_MAX / 2,       UINT16_MAX / 2, (UINT16_MAX / 2) + 1),
		/* 32768 */
		DPACK_UTEST_UINT16_RANGE("\xcd\x80\x00",      0,       (UINT16_MAX / 2) + 1, UINT16_MAX / 2, (UINT16_MAX / 2) + 1),
		/* 32769 */
		DPACK_UTEST_UINT16_RANGE("\xcd\x80\x01",      -ERANGE, 0,                    UINT16_MAX / 2, (UINT16_MAX / 2) + 1),

		/* 65532 */
		DPACK_UTEST_UINT16_RANGE("\xcd\xff\xfc",      -ERANGE, 0,                    UINT16_MAX - 2, UINT16_MAX - 1),
		/* 65533 */
		DPACK_UTEST_UINT16_RANGE("\xcd\xff\xfd",      0,       UINT16_MAX - 2,       UINT16_MAX - 2, UINT16_MAX - 1),
		/* 65534 */
		DPACK_UTEST_UINT16_RANGE("\xcd\xff\xfe",      0,       UINT16_MAX - 1,       UINT16_MAX - 2, UINT16_MAX - 1),
		/* 65535 */
		DPACK_UTEST_UINT16_RANGE("\xcd\xff\xff",      -ERANGE, 0,                    UINT16_MAX - 2, UINT16_MAX - 1),

		/* 65536 */
		DPACK_UTEST_UINT16_RANGE("\xce"
		                         "\x00\x01\x00\x00" , -ERANGE, 0,                    UINT16_MAX - 2, UINT16_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint16_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint16_range(NULL,
	                                                      1,
	                                                      2,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_uint16_range(&dec,
	                                                      1,
	                                                      2,
	                                                      &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint16_range(&dec,
	                                                      0,
	                                                      2,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_uint16_range(&dec,
	                                                      1,
	                                                      UINT16_MAX,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_uint16_range(&dec,
	                                                      2,
	                                                      2,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_uint16_range(&dec,
	                                                      1,
	                                                      2,
	                                                      NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint16_range);
}

#define DPACK_UTEST_INT16(_packed, _error, _value) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int16 = _value \
	}

static int
dpack_scalar_utest_pack_int16(struct dpack_encoder *                 encoder,
                              const struct dpack_scalar_utest_data * data)
{
	assert_true(DPACK_INT16_SIZE_MIN <= data->size);
	assert_true(DPACK_INT16_SIZE_MAX >= data->size);

	return dpack_encode_int16(encoder, data->value.int16);
}

static void
dpack_scalar_utest_encode_int16(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -32768 */
		DPACK_UTEST_INT16("\xd1\x80\x00", 0, INT16_MIN),
		/* -32767 */
		DPACK_UTEST_INT16("\xd1\x80\x01", 0, INT16_MIN + 1),
		/* -1 */
		DPACK_UTEST_INT16("\xff",         0, INT16_C(-1)),
		/* 0 */
		DPACK_UTEST_INT16("\x00",         0, INT16_C(0)),
		/* 1 */
		DPACK_UTEST_INT16("\x01",         0, INT16_C(1)),
		/* 32767 */
		DPACK_UTEST_INT16("\xcd\x7f\xff", 0, INT16_MAX)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int16_t              val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_encode_int16(NULL, val));
	expect_assert_failure(ret = dpack_encode_int16(&enc, val));
#endif

	dpack_scalar_utest_encode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_pack_int16);
}

static void
dpack_scalar_utest_unpack_int16(struct dpack_decoder *                 decoder,
                                const struct dpack_scalar_utest_data * data)
{
	int16_t val;

	assert_int_equal(dpack_decode_int16(decoder, &val), data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT16_SIZE_MIN <= data->size);
	assert_true(DPACK_INT16_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int16);
}

static void
dpack_scalar_utest_decode_int16(void ** state __unused)
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
	int16_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int16(NULL, &val));
	expect_assert_failure(ret = dpack_decode_int16(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int16(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int16);
}

#define DPACK_UTEST_INT16_MIN(_packed, _error, _value, _low) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int16 = _value, \
		.low.int16   = _low \
	}

static void
dpack_scalar_utest_unpack_int16_min(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	int16_t val;

	assert_int_equal(dpack_decode_int16_min(decoder, data->low.int16, &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT16_SIZE_MIN <= data->size);
	assert_true(DPACK_INT16_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int16);
}

static void
dpack_scalar_utest_decode_int16_min(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -32769 */
		DPACK_UTEST_INT16_MIN("\xd2"
		                      "\xff\xff\x7f\xff", -ERANGE, 0,             INT16_MIN + 1),
		/* -32768 */
		DPACK_UTEST_INT16_MIN("\xd1\x80\x00",     -ERANGE, 0,             INT16_MIN + 1),
		/* -32767 */
		DPACK_UTEST_INT16_MIN("\xd1\x80\x01",     0,       INT16_MIN + 1, INT16_MIN + 1),
		/* -32766 */
		DPACK_UTEST_INT16_MIN("\xd1\x80\x02",     0,       INT16_MIN + 2, INT16_MIN + 1),

		/* -1 */
		DPACK_UTEST_INT16_MIN("\xff",             -ERANGE, 0,             INT16_C(0)),
		/* 0 */
		DPACK_UTEST_INT16_MIN("\x00",             0,       INT16_C(0),    INT16_C(0)),
		/* 1 */
		DPACK_UTEST_INT16_MIN("\x01",             0,       INT16_C(1),    INT16_C(0)),
		/* 32767 */
		DPACK_UTEST_INT16_MIN("\xcd\x7f\xff",     0,       INT16_MAX,     INT16_C(0)),
		/* 32768 */
		DPACK_UTEST_INT16_MIN("\xcd\x80\x00",     -ERANGE, 0,             INT16_C(0)),

		/* 32765 */
		DPACK_UTEST_INT16_MIN("\xcd\x7f\xfd",     -ERANGE, 0,             INT16_MAX - 1),
		/* 32766 */
		DPACK_UTEST_INT16_MIN("\xcd\x7f\xfe",     0,       INT16_MAX - 1, INT16_MAX - 1),
		/* 32767 */
		DPACK_UTEST_INT16_MIN("\xcd\x7f\xff",     0,       INT16_MAX,     INT16_MAX - 1),
		/* 32768 */
		DPACK_UTEST_INT16_MIN("\xcd\x80\x00",     -ERANGE, 0,             INT16_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int16_t               val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int16_min(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_int16_min(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int16_min(&dec,
	                                                   INT16_MIN,
	                                                   &val));
	expect_assert_failure(ret = dpack_decode_int16_min(&dec,
	                                                   INT16_MAX,
	                                                   &val));
	expect_assert_failure(ret = dpack_decode_int16_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int16_min);
}

#define DPACK_UTEST_INT16_MAX(_packed, _error, _value, _high) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int16 = _value, \
		.high.int16  = _high \
	}

static void
dpack_scalar_utest_unpack_int16_max(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	int16_t val;

	assert_int_equal(dpack_decode_int16_max(decoder,
	                                        data->high.int16,
	                                        &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT16_SIZE_MIN <= data->size);
	assert_true(DPACK_INT16_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int16);
}

static void
dpack_scalar_utest_decode_int16_max(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -32769 */
		DPACK_UTEST_INT16_MAX("\xd2"
		                      "\xff\xff\x7f\xff", -ERANGE, 0,             INT16_MIN + 1),
		/* -32768 */
		DPACK_UTEST_INT16_MAX("\xd1\x80\x00",     0,       INT16_MIN,     INT16_MIN + 1),
		/* -32767 */
		DPACK_UTEST_INT16_MAX("\xd1\x80\x01",     0,       INT16_MIN + 1, INT16_MIN + 1),
		/* -32766 */
		DPACK_UTEST_INT16_MAX("\xd1\x80\x02",     -ERANGE, 0,             INT16_MIN + 1),

		/* -1 */
		DPACK_UTEST_INT16_MAX("\xff",             0,       INT16_C(-1),   INT16_C(0)),
		/* 0 */
		DPACK_UTEST_INT16_MAX("\x00",             0,       INT16_C(0),    INT16_C(0)),
		/* 1 */
		DPACK_UTEST_INT16_MAX("\x01",             -ERANGE, 0,             INT16_C(0)),
		/* 32767 */
		DPACK_UTEST_INT16_MAX("\xcd\x7f\xff",     -ERANGE, 0,             INT16_C(0)),
		/* 32768 */
		DPACK_UTEST_INT16_MAX("\xcd\x80\x00",     -ERANGE, 0,             INT16_C(0)),

		/* 32765 */
		DPACK_UTEST_INT16_MAX("\xcd\x7f\xfd",     0,       INT16_MAX - 2, INT16_MAX - 1),
		/* 32766 */
		DPACK_UTEST_INT16_MAX("\xcd\x7f\xfe",     0,       INT16_MAX - 1, INT16_MAX - 1),
		/* 32767 */
		DPACK_UTEST_INT16_MAX("\xcd\x7f\xff",     -ERANGE, 0,             INT16_MAX - 1),
		/* 32768 */
		DPACK_UTEST_INT16_MAX("\xcd\x80\x00",     -ERANGE, 0,             INT16_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int16_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int16_max(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_int16_max(&dec, 1, &val));
	expect_assert_failure(ret = dpack_decode_int16_max(&dec, 0, &val));
	expect_assert_failure(ret = dpack_decode_int16_max(&dec,
	                                                   INT16_MAX,
	                                                   &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int16_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int16_max);
}

#define DPACK_UTEST_INT16_RANGE(_packed, _error, _value, _low, _high) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int16 = _value, \
		.low.int16   = _low, \
		.high.int16  = _high \
	}

static void
dpack_scalar_utest_unpack_int16_range(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	int16_t val;

	assert_int_equal(dpack_decode_int16_range(decoder,
	                                          data->low.int16,
	                                          data->high.int16,
	                                          &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT16_SIZE_MIN <= data->size);
	assert_true(DPACK_INT16_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int16);
}

static void
dpack_scalar_utest_decode_int16_range(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -32769 */
		DPACK_UTEST_INT16_RANGE("\xd2"
		                        "\xff\xff\x7f\xff", -ERANGE, 0,             INT16_MIN + 1, INT16_MIN + 2),
		/* -32768 */
		DPACK_UTEST_INT16_RANGE("\xd1\x80\x00",     -ERANGE, 0,             INT16_MIN + 1, INT16_MIN + 2),
		/* -32767 */
		DPACK_UTEST_INT16_RANGE("\xd1\x80\x01",     0,       INT16_MIN + 1, INT16_MIN + 1, INT16_MIN + 2),
		/* -32766 */
		DPACK_UTEST_INT16_RANGE("\xd1\x80\x02",     0,       INT16_MIN + 2, INT16_MIN + 1, INT16_MIN + 2),
		/* -32765 */
		DPACK_UTEST_INT16_RANGE("\xd1\x80\x03",     -ERANGE, 0,             INT16_MIN + 1, INT16_MIN + 2),

		/* -1 */
		DPACK_UTEST_INT16_RANGE("\xff",             -ERANGE, 0,             INT16_C(0),    INT16_C(1)),
		/* 0 */
		DPACK_UTEST_INT16_RANGE("\x00",             0,       INT16_C(0),    INT16_C(0),    INT16_C(1)),
		/* 1 */
		DPACK_UTEST_INT16_RANGE("\x01",             0,       INT16_C(1),    INT16_C(0),    INT16_C(1)),
		/* 2 */
		DPACK_UTEST_INT16_RANGE("\x02",             -ERANGE, 0,             INT16_C(0),    INT16_C(1)),

		/* 32764 */
		DPACK_UTEST_INT16_RANGE("\xcd\x7f\xfc",     -ERANGE, 0,             INT16_MAX - 2, INT16_MAX - 1),
		/* 32765 */
		DPACK_UTEST_INT16_RANGE("\xcd\x7f\xfd",     0,       INT16_MAX - 2, INT16_MAX - 2, INT16_MAX - 1),
		/* 32766 */
		DPACK_UTEST_INT16_RANGE("\xcd\x7f\xfe",     0,       INT16_MAX - 1, INT16_MAX - 2, INT16_MAX - 1),
		/* 32767 */
		DPACK_UTEST_INT16_RANGE("\xcd\x7f\xff",     -ERANGE, 0,             INT16_MAX - 2, INT16_MAX - 1),
		/* 32768 */
		DPACK_UTEST_INT16_RANGE("\xcd\x80\x00",     -ERANGE, 0,             INT16_MAX - 2, INT16_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int16_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int16_range(NULL, 1, 2, &val));
	expect_assert_failure(ret = dpack_decode_int16_range(&dec, 1, 2, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int16_range(&dec,
	                                                     INT16_MIN,
	                                                     2,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_int16_range(&dec,
	                                                     1,
	                                                     INT16_MAX,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_int16_range(&dec, 2, 2, &val));
	expect_assert_failure(ret = dpack_decode_int16_range(&dec, 1, 2, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int16_range);
}

#define DPACK_UTEST_UINT32(_packed, _error, _value) \
	{ \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint32 = _value \
	}

static int
dpack_scalar_utest_pack_uint32(struct dpack_encoder *                 encoder,
                               const struct dpack_scalar_utest_data * data)
{
	assert_true(DPACK_UINT32_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT32_SIZE_MAX >= data->size);

	return dpack_encode_uint32(encoder, data->value.uint32);
}

static void
dpack_scalar_utest_encode_uint32(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* 0 */
		DPACK_UTEST_UINT32("\x00",             0,       UINT32_C(0)),
		/* 1 */
		DPACK_UTEST_UINT32("\x01",             0,       UINT32_C(1)),
		/* 4294967295 */
		DPACK_UTEST_UINT32("\xce"
		                   "\xff\xff\xff\xff", 0,       UINT32_MAX),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint32_t             val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_encode_uint32(NULL, val));
	expect_assert_failure(ret = dpack_encode_uint32(&enc, val));
#endif

	dpack_scalar_utest_encode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_pack_uint32);
}

static void
dpack_scalar_utest_unpack_uint32(struct dpack_decoder *                 decoder,
                                 const struct dpack_scalar_utest_data * data)
{
	uint32_t val;

	assert_int_equal(dpack_decode_uint32(decoder, &val), data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT32_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT32_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint32);
}

static void
dpack_scalar_utest_decode_uint32(void ** state __unused)
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
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint32(NULL, &val));
	expect_assert_failure(ret = dpack_decode_uint32(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint32(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint32);
}

#define DPACK_UTEST_UINT32_MIN(_packed, _error, _value, _low) \
	{ \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint32 = _value, \
		.low.uint32   = _low \
	}

static void
dpack_scalar_utest_unpack_uint32_min(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint32_t val;

	assert_int_equal(dpack_decode_uint32_min(decoder,
	                                         data->low.uint32,
	                                         &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT32_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT32_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint32);
}

static void
dpack_scalar_utest_decode_uint32_min(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT32_MIN("\xff",             -ENOMSG, 0,              UINT32_C(1)),
		/* 0 */
		DPACK_UTEST_UINT32_MIN("\x00",             -ERANGE, 0,              UINT32_C(1)),
		/* 1 */
		DPACK_UTEST_UINT32_MIN("\x01",             0,       UINT32_C(1),    UINT32_C(1)),
		/* 2147483647 */
		DPACK_UTEST_UINT32_MIN("\xce"
		                       "\x7f\xff\xff\xff", 0,       UINT32_MAX / 2, UINT32_C(1)),
		/* 4294967295 */
		DPACK_UTEST_UINT32_MIN("\xce"
		                       "\xff\xff\xff\xff", 0,       UINT32_MAX,     UINT32_C(1)),
		/* 4294967296 */
		DPACK_UTEST_UINT32_MIN("\xcf"
		                       "\x00\x00\x00\x01"
		                       "\x00\x00\x00\x00", -ERANGE, 0,              UINT32_C(1)),
		/* -1 */
		DPACK_UTEST_UINT32_MIN("\xff",             -ENOMSG, 0,              UINT32_MAX / 2),
		/* 0 */
		DPACK_UTEST_UINT32_MIN("\x00",             -ERANGE, 0,              UINT32_MAX / 2),
		/* 1 */
		DPACK_UTEST_UINT32_MIN("\x01",             -ERANGE, 0,              UINT32_MAX / 2),

		/* 2147483646 */
		DPACK_UTEST_UINT32_MIN("\xce"
		                       "\x7f\xff\xff\xfe", -ERANGE, 0,              UINT32_MAX / 2),
		/* 2147483647 */
		DPACK_UTEST_UINT32_MIN("\xce"
		                       "\x7f\xff\xff\xff", 0,       UINT32_MAX / 2, UINT32_MAX / 2),
		/* 4294967295 */
		DPACK_UTEST_UINT32_MIN("\xce"
		                       "\xff\xff\xff\xff", 0,       UINT32_MAX,     UINT32_MAX / 2),
		/* 4294967296 */
		DPACK_UTEST_UINT32_MIN("\xcf"
		                       "\x00\x00\x00\x01"
		                       "\x00\x00\x00\x00", -ERANGE, 0,              UINT32_MAX / 2),
		/* 4294967293 */
		DPACK_UTEST_UINT32_MIN("\xce"
		                       "\xff\xff\xff\xfd", -ERANGE, 0,              UINT32_MAX - 1),
		/* 4294967294 */
		DPACK_UTEST_UINT32_MIN("\xce"
		                       "\xff\xff\xff\xfe", 0,       UINT32_MAX - 1, UINT32_MAX - 1),
		/* 4294967295 */
		DPACK_UTEST_UINT32_MIN("\xce"
		                       "\xff\xff\xff\xff", 0,       UINT32_MAX,     UINT32_MAX - 1),
		/* 4294967296 */
		DPACK_UTEST_UINT32_MIN("\xcf"
		                       "\x00\x00\x00\x01"
		                       "\x00\x00\x00\x00", -ERANGE, 0,              UINT32_MAX - 1),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint32_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint32_min(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_uint32_min(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint32_min(&dec, 0, &val));
	expect_assert_failure(ret = dpack_decode_uint32_min(&dec,
	                                                    UINT32_MAX,
	                                                    &val));
	expect_assert_failure(ret = dpack_decode_uint32_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint32_min);
}

#define DPACK_UTEST_UINT32_MAX(_packed, _error, _value, _high) \
	{ \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint32 = _value, \
		.high.uint32  = _high \
	}

static void
dpack_scalar_utest_unpack_uint32_max(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint32_t val;

	assert_int_equal(dpack_decode_uint32_max(decoder,
	                                         data->high.uint32,
	                                         &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT32_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT32_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint32);
}

static void
dpack_scalar_utest_decode_uint32_max(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT32_MAX("\xff",             -ENOMSG, 0,                    UINT32_C(1)),
		/* 0 */
		DPACK_UTEST_UINT32_MAX("\x00",             0,       UINT32_C(0),          UINT32_C(1)),
		/* 1 */
		DPACK_UTEST_UINT32_MAX("\x01",             0,       UINT32_C(1),          UINT32_C(1)),
		/* 2147483647 */
		DPACK_UTEST_UINT32_MAX("\xce"
		                       "\x7f\xff\xff\xff", -ERANGE, 0,                    UINT32_C(1)),
		/* 4294967295 */
		DPACK_UTEST_UINT32_MAX("\xce"
		                       "\xff\xff\xff\xff", -ERANGE, 0,                    UINT32_C(1)),
		/* 4294967296 */
		DPACK_UTEST_UINT32_MAX("\xcf"
		                       "\x00\x00\x00\x01"
		                       "\x00\x00\x00\x00", -ERANGE, 0,                    UINT32_C(1)),
		/* -1 */
		DPACK_UTEST_UINT32_MAX("\xff",             -ENOMSG, 0,                    UINT32_MAX / 2),
		/* 0 */
		DPACK_UTEST_UINT32_MAX("\x00",             0,       UINT32_C(0),          UINT32_MAX / 2),
		/* 1 */
		DPACK_UTEST_UINT32_MAX("\x01",             0,       UINT32_C(1),          UINT32_MAX / 2),

		/* 2147483646 */
		DPACK_UTEST_UINT32_MAX("\xce"
		                       "\x7f\xff\xff\xfe", 0,       (UINT32_MAX / 2) - 1, UINT32_MAX / 2),
		/* 2147483647 */
		DPACK_UTEST_UINT32_MAX("\xce"
		                       "\x7f\xff\xff\xff", 0,       UINT32_MAX / 2,       UINT32_MAX / 2),
		/* 4294967295 */
		DPACK_UTEST_UINT32_MAX("\xce"
		                       "\xff\xff\xff\xff", -ERANGE, 0,                    UINT32_MAX / 2),
		/* 4294967296 */
		DPACK_UTEST_UINT32_MAX("\xcf"
		                       "\x00\x00\x00\x01"
		                       "\x00\x00\x00\x00", -ERANGE, 0,                    UINT32_MAX / 2),
		/* 4294967293 */
		DPACK_UTEST_UINT32_MAX("\xce"
		                       "\xff\xff\xff\xfd", 0,       UINT32_MAX -2,        UINT32_MAX - 1),
		/* 4294967294 */
		DPACK_UTEST_UINT32_MAX("\xce"
		                       "\xff\xff\xff\xfe", 0,       UINT32_MAX - 1,       UINT32_MAX - 1),
		/* 4294967295 */
		DPACK_UTEST_UINT32_MAX("\xce"
		                       "\xff\xff\xff\xff", -ERANGE, 0,                    UINT32_MAX - 1),
		/* 4294967296 */
		DPACK_UTEST_UINT32_MAX("\xcf"
		                       "\x00\x00\x00\x01"
		                       "\x00\x00\x00\x00", -ERANGE, 0,                    UINT32_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint32_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint32_max(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_uint32_max(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint32_max(&dec, 0, &val));
	expect_assert_failure(ret = dpack_decode_uint32_max(&dec,
	                                                    UINT32_MAX,
	                                                    &val));
	expect_assert_failure(ret = dpack_decode_uint32_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint32_max);
}

#define DPACK_UTEST_UINT32_RANGE(_packed, _error, _value, _low, _high) \
	{ \
		.packed       = _packed, \
		.size         = sizeof(_packed) - 1, \
		.error        = _error, \
		.value.uint32 = _value, \
		.low.uint32   = _low, \
		.high.uint32  = _high \
	}

static void
dpack_scalar_utest_unpack_uint32_range(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint32_t val;

	assert_int_equal(dpack_decode_uint32_range(decoder,
	                                           data->low.uint32,
	                                           data->high.uint32,
	                                           &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_UINT32_SIZE_MIN <= data->size);
	assert_true(DPACK_UINT32_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.uint32);
}

static void
dpack_scalar_utest_decode_uint32_range(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -1 */
		DPACK_UTEST_UINT32_RANGE("\xff",             -ENOMSG, 0,                    UINT32_C(1),    UINT32_C(2)),
		/* 0 */
		DPACK_UTEST_UINT32_RANGE("\x00",             -ERANGE, 0,                    UINT32_C(1),    UINT32_C(2)),
		/* 1 */
		DPACK_UTEST_UINT32_RANGE("\x01",             0,       UINT32_C(1),          UINT32_C(1),    UINT32_C(2)),
		/* 2 */
		DPACK_UTEST_UINT32_RANGE("\x02",             0,       UINT32_C(2),          UINT32_C(1),    UINT32_C(2)),
		/* 3 */
		DPACK_UTEST_UINT32_RANGE("\x03",             -ERANGE, 0,                    UINT32_C(1),    UINT32_C(2)),

		/* 1 */
		DPACK_UTEST_UINT32_RANGE("\x01",             -ERANGE, 0,                    UINT32_MAX / 2, (UINT32_MAX / 2) + 1),

		/* 2147483646 */
		DPACK_UTEST_UINT32_RANGE("\xce"
		                         "\x7f\xff\xff\xfe", -ERANGE, 0,                    UINT32_MAX / 2, (UINT32_MAX / 2) + 1),
		/* 2147483647 */
		DPACK_UTEST_UINT32_RANGE("\xce"
		                         "\x7f\xff\xff\xff", 0,       UINT32_MAX / 2,       UINT32_MAX / 2, (UINT32_MAX / 2) + 1),
		/* 2147483648 */
		DPACK_UTEST_UINT32_RANGE("\xce"
		                         "\x80\x00\x00\x00", 0,       (UINT32_MAX / 2) + 1, UINT32_MAX / 2, (UINT32_MAX / 2) + 1),
		/* 2147483649 */
		DPACK_UTEST_UINT32_RANGE("\xce"
		                         "\x80\x00\x00\x01", -ERANGE, 0,                    UINT32_MAX / 2, (UINT32_MAX / 2) + 1),

		/* 4294967292*/
		DPACK_UTEST_UINT32_RANGE("\xce"
		                         "\xff\xff\xff\xfc", -ERANGE, 0,                    UINT32_MAX - 2, UINT32_MAX - 1),
		/* 4294967293*/
		DPACK_UTEST_UINT32_RANGE("\xce"
		                         "\xff\xff\xff\xfd", 0,       UINT32_MAX - 2,       UINT32_MAX - 2, UINT32_MAX - 1),
		/* 4294967294*/
		DPACK_UTEST_UINT32_RANGE("\xce"
		                         "\xff\xff\xff\xfe", 0,       UINT32_MAX - 1,       UINT32_MAX - 2, UINT32_MAX - 1),
		/* 4294967295*/
		DPACK_UTEST_UINT32_RANGE("\xce"
		                         "\xff\xff\xff\xff", -ERANGE, 0,                    UINT32_MAX - 2, UINT32_MAX - 1),
		/* 4294967296*/
		DPACK_UTEST_UINT32_RANGE("\xcf"
		                         "\x00\x00\x00\x01"
		                         "\x00\x00\x00\x00", -ERANGE, 0,                    UINT32_MAX - 2, UINT32_MAX - 1),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint32_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint32_range(NULL,
	                                                      1,
	                                                      2,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_uint32_range(&dec,
	                                                      1,
	                                                      2,
	                                                      &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint32_range(&dec,
	                                                      0,
	                                                      2,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_uint32_range(&dec,
	                                                      1,
	                                                      UINT32_MAX,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_uint32_range(&dec,
	                                                      2,
	                                                      2,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_uint32_range(&dec,
	                                                      1,
	                                                      2,
	                                                      NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint32_range);
}

#define DPACK_UTEST_INT32(_packed, _error, _value) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int32 = _value \
	}

static int
dpack_scalar_utest_pack_int32(struct dpack_encoder *                 encoder,
                              const struct dpack_scalar_utest_data * data)
{
	assert_true(DPACK_INT32_SIZE_MIN <= data->size);
	assert_true(DPACK_INT32_SIZE_MAX >= data->size);

	return dpack_encode_int32(encoder, data->value.int32);
}

static void
dpack_scalar_utest_encode_int32(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
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
		/* 2147483647 */
		DPACK_UTEST_INT32("\xce"
		                  "\x7f\xff\xff\xff", 0,       INT32_MAX)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int32_t              val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_encode_int32(NULL, val));
	expect_assert_failure(ret = dpack_encode_int32(&enc, val));
#endif

	dpack_scalar_utest_encode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_pack_int32);
}

static void
dpack_scalar_utest_unpack_int32(struct dpack_decoder *                 decoder,
                                const struct dpack_scalar_utest_data * data)
{
	int32_t val;

	assert_int_equal(dpack_decode_int32(decoder, &val), data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT32_SIZE_MIN <= data->size);
	assert_true(DPACK_INT32_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int32);
}

static void
dpack_scalar_utest_decode_int32(void ** state __unused)
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
		/* 2147483647 */
		DPACK_UTEST_INT32("\xce"
		                  "\x7f\xff\xff\xff", 0,       INT32_MAX),
		/* 2147483648 */
		DPACK_UTEST_INT32("\xce"
		                  "\x80\x00\x00\x00", -ERANGE, INT32_C(0))
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int32_t               val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int32(NULL, &val));
	expect_assert_failure(ret = dpack_decode_int32(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int32(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int32);
}

#define DPACK_UTEST_INT32_MIN(_packed, _error, _value, _low) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int32 = _value, \
		.low.int32   = _low \
	}

static void
dpack_scalar_utest_unpack_int32_min(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	int32_t val;

	assert_int_equal(dpack_decode_int32_min(decoder, data->low.int32, &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT32_SIZE_MIN <= data->size);
	assert_true(DPACK_INT32_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int32);
}

static void
dpack_scalar_utest_decode_int32_min(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -2147483649 */
		DPACK_UTEST_INT32_MIN("\xd3"
		                      "\xff\xff\xff\xff"
		                      "\x7f\xff\xff\xff", -ERANGE, 0,             INT32_MIN + 1),
		/* -2147483648 */
		DPACK_UTEST_INT32_MIN("\xd2"
		                      "\x80\x00\x00\x00", -ERANGE, 0,             INT32_MIN + 1),
		/* -2147483647 */
		DPACK_UTEST_INT32_MIN("\xd2"
		                      "\x80\x00\x00\x01", 0,       INT32_MIN + 1, INT32_MIN + 1),
		/* -2147483646 */
		DPACK_UTEST_INT32_MIN("\xd2"
		                      "\x80\x00\x00\x02", 0,       INT32_MIN + 2, INT32_MIN + 1),
		/* -1 */
		DPACK_UTEST_INT32_MIN("\xff",             -ERANGE, 0,             INT32_C(0)),
		/* 0 */
		DPACK_UTEST_INT32_MIN("\x00",             0,       INT32_C(0),    INT32_C(0)),
		/* 1 */
		DPACK_UTEST_INT32_MIN("\x01",             0,       INT32_C(1),    INT32_C(0)),
		/* 2147483647 */
		DPACK_UTEST_INT32_MIN("\xce"
		                      "\x7f\xff\xff\xff", 0,       INT32_MAX,     INT32_C(0)),
		/* 2147483648 */
		DPACK_UTEST_INT32_MIN("\xce"
		                      "\x80\x00\x00\x00", -ERANGE, 0,             INT32_C(0)),
		/* 2147483645 */
		DPACK_UTEST_INT32_MIN("\xce"
		                      "\x7f\xff\xff\xfd", -ERANGE, 0,             INT32_MAX - 1),
		/* 2147483646 */
		DPACK_UTEST_INT32_MIN("\xce"
		                      "\x7f\xff\xff\xfe", 0,       INT32_MAX - 1, INT32_MAX - 1),
		/* 2147483647 */
		DPACK_UTEST_INT32_MIN("\xce"
		                      "\x7f\xff\xff\xff", 0,       INT32_MAX,     INT32_MAX - 1),
		/* 2147483648 */
		DPACK_UTEST_INT32_MIN("\xce"
		                      "\x80\x00\x00\x00", -ERANGE, 0,             INT32_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int32_t               val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int32_min(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_int32_min(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int32_min(&dec,
	                                                   INT32_MIN,
	                                                   &val));
	expect_assert_failure(ret = dpack_decode_int32_min(&dec,
	                                                   INT32_MAX,
	                                                   &val));
	expect_assert_failure(ret = dpack_decode_int32_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int32_min);
}

#define DPACK_UTEST_INT32_MAX(_packed, _error, _value, _high) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int32 = _value, \
		.high.int32  = _high \
	}

static void
dpack_scalar_utest_unpack_int32_max(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	int32_t val;

	assert_int_equal(dpack_decode_int32_max(decoder,
	                                        data->high.int32,
	                                        &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT32_SIZE_MIN <= data->size);
	assert_true(DPACK_INT32_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int32);
}

static void
dpack_scalar_utest_decode_int32_max(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -2147483649 */
		DPACK_UTEST_INT32_MAX("\xd3"
		                      "\xff\xff\xff\xff"
		                      "\x7f\xff\xff\xff", -ERANGE, 0,             INT32_MIN + 1),
		/* -2147483648 */
		DPACK_UTEST_INT32_MAX("\xd2"
		                      "\x80\x00\x00\x00", 0,       INT32_MIN,     INT32_MIN + 1),
		/* -2147483647 */
		DPACK_UTEST_INT32_MAX("\xd2"
		                      "\x80\x00\x00\x01", 0,       INT32_MIN + 1, INT32_MIN + 1),
		/* -2147483646 */
		DPACK_UTEST_INT32_MAX("\xd2"
		                      "\x80\x00\x00\x02", -ERANGE, 0,             INT32_MIN + 1),
		/* -1 */
		DPACK_UTEST_INT32_MAX("\xff",             0,       INT32_C(-1),   INT32_C(0)),
		/* 0 */
		DPACK_UTEST_INT32_MAX("\x00",             0,       INT32_C(0),    INT32_C(0)),
		/* 1 */
		DPACK_UTEST_INT32_MAX("\x01",             -ERANGE, 0,             INT32_C(0)),
		/* 2147483647 */
		DPACK_UTEST_INT32_MAX("\xce"
		                      "\x7f\xff\xff\xff", -ERANGE, 0,             INT32_C(0)),
		/* 2147483648 */
		DPACK_UTEST_INT32_MAX("\xce"
		                      "\x80\x00\x00\x00", -ERANGE, 0,             INT32_C(0)),
		/* 2147483645 */
		DPACK_UTEST_INT32_MAX("\xce"
		                      "\x7f\xff\xff\xfd", 0,       INT32_MAX - 2, INT32_MAX - 1),
		/* 2147483646 */
		DPACK_UTEST_INT32_MAX("\xce"
		                      "\x7f\xff\xff\xfe", 0,       INT32_MAX - 1, INT32_MAX - 1),
		/* 2147483647 */
		DPACK_UTEST_INT32_MAX("\xce"
		                      "\x7f\xff\xff\xff", -ERANGE, 0,             INT32_MAX - 1),
		/* 2147483648 */
		DPACK_UTEST_INT32_MAX("\xce"
		                      "\x80\x00\x00\x00", -ERANGE, 0,             INT32_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int32_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int32_max(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_int32_max(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int32_max(&dec,
	                                                   INT32_MIN,
	                                                   &val));
	expect_assert_failure(ret = dpack_decode_int32_max(&dec,
	                                                   INT32_MAX,
	                                                   &val));
	expect_assert_failure(ret = dpack_decode_int32_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int32_max);
}

#define DPACK_UTEST_INT32_RANGE(_packed, _error, _value, _low, _high) \
	{ \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int32 = _value, \
		.low.int32   = _low, \
		.high.int32  = _high \
	}

static void
dpack_scalar_utest_unpack_int32_range(
	struct dpack_decoder *                 decoder,
	const struct dpack_scalar_utest_data * data)
{
	int32_t val;

	assert_int_equal(dpack_decode_int32_range(decoder,
	                                          data->low.int32,
	                                          data->high.int32,
	                                          &val),
	                 data->error);
	if (data->error)
		return;

	assert_true(DPACK_INT32_SIZE_MIN <= data->size);
	assert_true(DPACK_INT32_SIZE_MAX >= data->size);

	assert_int_equal(val, data->value.int32);
}

static void
dpack_scalar_utest_decode_int32_range(void ** state __unused)
{
	static const struct dpack_scalar_utest_data data[] = {
		/* -2147483649 */
		DPACK_UTEST_INT32_RANGE("\xd3"
		                        "\xff\xff\xff\xff"
		                        "\x7f\xff\xff\xff", -ERANGE, 0,             INT32_MIN + 1, INT32_MIN + 2),
		/* -2147483648 */
		DPACK_UTEST_INT32_RANGE("\xd2"
		                        "\x80\x00\x00\x00", -ERANGE, 0,             INT32_MIN + 1, INT32_MIN + 2),
		/* -2147483647 */
		DPACK_UTEST_INT32_RANGE("\xd2"
		                        "\x80\x00\x00\x01", 0,       INT32_MIN + 1, INT32_MIN + 1, INT32_MIN + 2),
		/* -2147483646 */
		DPACK_UTEST_INT32_RANGE("\xd2"
		                        "\x80\x00\x00\x02", 0,       INT32_MIN + 2, INT32_MIN + 1, INT32_MIN + 2),
		/* -2147483645 */
		DPACK_UTEST_INT32_RANGE("\xd2"
		                        "\x80\x00\x00\x03", -ERANGE, 0,             INT32_MIN + 1, INT32_MIN + 2),

		/* -1 */
		DPACK_UTEST_INT32_RANGE("\xff",             -ERANGE, 0,             INT32_C(0),    INT32_C(1)),
		/* 0 */
		DPACK_UTEST_INT32_RANGE("\x00",             0,       INT32_C(0),    INT32_C(0),    INT32_C(1)),
		/* 1 */
		DPACK_UTEST_INT32_RANGE("\x01",             0,       INT32_C(1),    INT32_C(0),    INT32_C(1)),
		/* 2 */
		DPACK_UTEST_INT32_RANGE("\x02",             -ERANGE, 0,             INT32_C(0),    INT32_C(1)),

		/* 2147483644 */
		DPACK_UTEST_INT32_RANGE("\xce"
		                        "\x7f\xff\xff\xfc", -ERANGE, 0,             INT32_MAX - 2, INT32_MAX - 1),
		/* 2147483645 */
		DPACK_UTEST_INT32_RANGE("\xce"
		                        "\x7f\xff\xff\xfd", 0,       INT32_MAX - 2, INT32_MAX - 2, INT32_MAX - 1),
		/* 2147483646 */
		DPACK_UTEST_INT32_RANGE("\xce"
		                        "\x7f\xff\xff\xfe", 0,       INT32_MAX - 1, INT32_MAX - 2, INT32_MAX - 1),
		/* 2147483647 */
		DPACK_UTEST_INT32_RANGE("\xce"
		                        "\x7f\xff\xff\xff", -ERANGE, 0,             INT32_MAX - 2, INT32_MAX - 1),
		/* 2147483648 */
		DPACK_UTEST_INT32_RANGE("\xce"
		                        "\x80\x00\x00\x00", -ERANGE, 0,             INT32_MAX - 2, INT32_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int32_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int32_range(NULL, 1, 2, &val));
	expect_assert_failure(ret = dpack_decode_int32_range(&dec, 1, 2, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int32_range(&dec,
	                                                     INT32_MIN,
	                                                     2,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_int32_range(&dec,
	                                                     1,
	                                                     INT32_MAX,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_int32_range(&dec, 2, 2, &val));
	expect_assert_failure(ret = dpack_decode_int32_range(&dec, 1, 2, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int32_range);
}

#define DPACK_UTEST_UINT64(_packed, _error, _value) \
	{ \
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
		DPACK_UTEST_UINT64("\x00",             0, UINT64_C(0)),
		/* 1 */
		DPACK_UTEST_UINT64("\x01",             0, UINT64_C(1)),
		/* 18446744073709551615 */
		DPACK_UTEST_UINT64("\xcf"
		                   "\xff\xff\xff\xff"
		                   "\xff\xff\xff\xff", 0, UINT64_MAX)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint64_t             val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_encode_uint64(NULL, val));
	expect_assert_failure(ret = dpack_encode_uint64(&enc, val));
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
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint64(NULL, &val));
	expect_assert_failure(ret = dpack_decode_uint64(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint64(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint64);
}

#define DPACK_UTEST_UINT64_MIN(_packed, _error, _value, _low) \
	{ \
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
		DPACK_UTEST_UINT64_MIN("\xff",             -ENOMSG, 0,              UINT64_C(1)),
		/* 0 */
		DPACK_UTEST_UINT64_MIN("\x00",             -ERANGE, 0,              UINT64_C(1)),
		/* 1 */
		DPACK_UTEST_UINT64_MIN("\x01",             0,       UINT64_C(1),    UINT64_C(1)),
		/* 9223372036854775807 */
		DPACK_UTEST_UINT64_MIN("\xcf"
		                       "\x7f\xff\xff\xff"
		                       "\xff\xff\xff\xff", 0,       UINT64_MAX / 2, UINT64_C(1)),
		/* 18446744073709551615 */
		DPACK_UTEST_UINT64_MIN("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xff", 0,       UINT64_MAX,     UINT64_C(1)),

		/* -1 */
		DPACK_UTEST_UINT64_MIN("\xff",             -ENOMSG, 0,              UINT64_MAX / 2),
		/* 0 */
		DPACK_UTEST_UINT64_MIN("\x00",             -ERANGE, 0,              UINT64_MAX / 2),
		/* 1 */
		DPACK_UTEST_UINT64_MIN("\x01",             -ERANGE, 0,              UINT64_MAX / 2),

		/* 9223372036854775806 */
		DPACK_UTEST_UINT64_MIN("\xcf"
		                       "\x7f\xff\xff\xff"
		                       "\xff\xff\xff\xfe", -ERANGE, 0,              UINT64_MAX / 2),
		/* 9223372036854775807 */
		DPACK_UTEST_UINT64_MIN("\xcf"
		                       "\x7f\xff\xff\xff"
		                       "\xff\xff\xff\xff", 0,       UINT64_MAX / 2, UINT64_MAX / 2),
		/* 18446744073709551615 */
		DPACK_UTEST_UINT64_MIN("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xff", 0,       UINT64_MAX,     UINT64_MAX / 2),

		/* 18446744073709551613 */
		DPACK_UTEST_UINT64_MIN("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xfd", -ERANGE, 0,              UINT64_MAX - 1),
		/* 18446744073709551614 */
		DPACK_UTEST_UINT64_MIN("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xfe", 0,       UINT64_MAX - 1, UINT64_MAX - 1),
		/* 18446744073709551615 */
		DPACK_UTEST_UINT64_MIN("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xff", 0,       UINT64_MAX,     UINT64_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint64_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint64_min(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_uint64_min(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint64_min(&dec, 0, &val));
	expect_assert_failure(ret = dpack_decode_uint64_min(&dec,
	                                                    UINT64_MAX,
	                                                    &val));
	expect_assert_failure(ret = dpack_decode_uint64_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint64_min);
}

#define DPACK_UTEST_UINT64_MAX(_packed, _error, _value, _high) \
	{ \
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
		DPACK_UTEST_UINT64_MAX("\xff",             -ENOMSG, 0,                    UINT64_C(1)),
		/* 0 */
		DPACK_UTEST_UINT64_MAX("\x00",             0,       UINT64_C(0),          UINT64_C(1)),
		/* 1 */
		DPACK_UTEST_UINT64_MAX("\x01",             0,       UINT64_C(1),          UINT64_C(1)),
		/* 9223372036854775807 */
		DPACK_UTEST_UINT64_MAX("\xcf"
		                       "\x7f\xff\xff\xff"
		                       "\xff\xff\xff\xff", -ERANGE, 0,                    UINT64_C(1)),
		/* 18446744073709551615 */
		DPACK_UTEST_UINT64_MAX("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xff", -ERANGE, 0,                    UINT64_C(1)),

		/* -1 */
		DPACK_UTEST_UINT64_MAX("\xff",             -ENOMSG, 0,                    UINT64_MAX / 2),
		/* 0 */
		DPACK_UTEST_UINT64_MAX("\x00",             0,       UINT64_C(0),          UINT64_MAX / 2),
		/* 1 */
		DPACK_UTEST_UINT64_MAX("\x01",             0,       UINT64_C(1),          UINT64_MAX / 2),

		/* 9223372036854775806 */
		DPACK_UTEST_UINT64_MAX("\xcf"
		                       "\x7f\xff\xff\xff"
		                       "\xff\xff\xff\xfe", 0,       (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 9223372036854775807 */
		DPACK_UTEST_UINT64_MAX("\xcf"
		                       "\x7f\xff\xff\xff"
		                       "\xff\xff\xff\xff", 0,       UINT64_MAX / 2,       UINT64_MAX / 2),
		/* 18446744073709551615 */
		DPACK_UTEST_UINT64_MAX("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xff", -ERANGE, 0,                    UINT64_MAX / 2),

		/* 18446744073709551613 */
		DPACK_UTEST_UINT64_MAX("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xfd", 0,       UINT64_MAX - 2,       UINT64_MAX - 1),
		/* 18446744073709551614 */
		DPACK_UTEST_UINT64_MAX("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xfe", 0,       UINT64_MAX - 1,       UINT64_MAX - 1),
		/* 18446744073709551615 */
		DPACK_UTEST_UINT64_MAX("\xcf"
		                       "\xff\xff\xff\xff"
		                       "\xff\xff\xff\xff", -ERANGE, 0,                    UINT64_MAX - 1)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint64_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint64_max(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_uint64_max(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint64_max(&dec, 0, &val));
	expect_assert_failure(ret = dpack_decode_uint64_max(&dec,
	                                                    UINT64_MAX,
	                                                    &val));
	expect_assert_failure(ret = dpack_decode_uint64_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint64_max);
}

#define DPACK_UTEST_UINT64_RANGE(_packed, _error, _value, _low, _high) \
	{ \
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
		DPACK_UTEST_UINT64_RANGE("\xff",             -ENOMSG, 0,                    UINT64_C(1), UINT64_C(2)),
		/* 0 */
		DPACK_UTEST_UINT64_RANGE("\x00",             -ERANGE, 0,                    UINT64_C(1), UINT64_C(2)),
		/* 1 */
		DPACK_UTEST_UINT64_RANGE("\x01",             0,       UINT64_C(1),          UINT64_C(1), UINT64_C(2)),
		/* 2 */
		DPACK_UTEST_UINT64_RANGE("\x02",             0,       UINT64_C(2),          UINT64_C(1), UINT64_C(2)),
		/* 3 */
		DPACK_UTEST_UINT64_RANGE("\x03",             -ERANGE, 0,                    UINT64_C(1), UINT64_C(2)),
		/* 9223372036854775807 */
		DPACK_UTEST_UINT64_RANGE("\xcf"
		                         "\x7f\xff\xff\xff"
		                         "\xff\xff\xff\xff", -ERANGE, 0,                    UINT64_C(1), UINT64_C(2)),
		/* 18446744073709551615 */
		DPACK_UTEST_UINT64_RANGE("\xcf"
		                         "\xff\xff\xff\xff"
		                         "\xff\xff\xff\xff", -ERANGE, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX / 2),

		/* -1 */
		DPACK_UTEST_UINT64_RANGE("\xff",             -ENOMSG, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 0 */
		DPACK_UTEST_UINT64_RANGE("\x00",             -ERANGE, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 9223372036854775805 */
		DPACK_UTEST_UINT64_RANGE("\xcf"
		                         "\x7f\xff\xff\xff"
		                         "\xff\xff\xff\xfd", -ERANGE, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 9223372036854775806 */
		DPACK_UTEST_UINT64_RANGE("\xcf"
		                         "\x7f\xff\xff\xff"
		                         "\xff\xff\xff\xfe", 0,       (UINT64_MAX / 2) - 1, (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 9223372036854775807 */
		DPACK_UTEST_UINT64_RANGE("\xcf"
		                         "\x7f\xff\xff\xff"
		                         "\xff\xff\xff\xff", 0,       UINT64_MAX / 2,       (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 9223372036854775808 */
		DPACK_UTEST_UINT64_RANGE("\xcf"
		                         "\x80\x00\x00\x00"
		                         "\x00\x00\x00\x00", -ERANGE, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX / 2),
		/* 18446744073709551615 */
		DPACK_UTEST_UINT64_RANGE("\xcf"
		                         "\xff\xff\xff\xff"
		                         "\xff\xff\xff\xff", -ERANGE, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX - 1),

		/* -1 */
		DPACK_UTEST_UINT64_RANGE("\xff",             -ENOMSG, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX - 1),
		/* 0 */
		DPACK_UTEST_UINT64_RANGE("\x00",             -ERANGE, 0,                    (UINT64_MAX / 2) - 1, UINT64_MAX - 1),
		/* 18446744073709551612 */
		DPACK_UTEST_UINT64_RANGE("\xcf"
		                         "\xff\xff\xff\xff"
		                         "\xff\xff\xff\xfc", -ERANGE, 0,                    UINT64_MAX - 2,       UINT64_MAX - 1),
		/* 18446744073709551613 */
		DPACK_UTEST_UINT64_RANGE("\xcf"
		                         "\xff\xff\xff\xff"
		                         "\xff\xff\xff\xfd", 0,       UINT64_MAX - 2,       UINT64_MAX - 2,       UINT64_MAX - 1),
		/* 18446744073709551614 */
		DPACK_UTEST_UINT64_RANGE("\xcf"
		                         "\xff\xff\xff\xff"
		                         "\xff\xff\xff\xfe", 0,       UINT64_MAX - 1,       UINT64_MAX - 2,       UINT64_MAX - 1),
		/* 18446744073709551615 */
		DPACK_UTEST_UINT64_RANGE("\xcf"
		                         "\xff\xff\xff\xff"
		                         "\xff\xff\xff\xff", -ERANGE, 0,                    UINT64_MAX - 2,       UINT64_MAX - 1),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	uint64_t             val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_uint64_range(NULL,
	                                                      1,
	                                                      2,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_uint64_range(&dec,
	                                                      1,
	                                                      2,
	                                                      &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_uint64_range(&dec,
	                                                      0,
	                                                      2,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_uint64_range(&dec,
	                                                      1,
	                                                      UINT64_MAX,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_uint64_range(&dec,
	                                                      2,
	                                                      2,
	                                                      &val));
	expect_assert_failure(ret = dpack_decode_uint64_range(&dec,
	                                                      1,
	                                                      2,
	                                                      NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_uint64_range);
}

#define DPACK_UTEST_INT64(_packed, _error, _value) \
	{ \
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
		DPACK_UTEST_INT64("\xd3"
		                  "\x80\x00\x00\x00"
		                  "\x00\x00\x00\x00", 0, INT64_MIN),
		/* -9223372036854775807 */
		DPACK_UTEST_INT64("\xd3"
		                  "\x80\x00\x00\x00"
		                  "\x00\x00\x00\x01", 0, INT64_MIN + 1),
		/* -1 */
		DPACK_UTEST_INT64("\xff",             0, INT64_C(-1)),
		/* 0 */
		DPACK_UTEST_INT64("\x00",             0, INT64_C(0)),
		/* 1 */
		DPACK_UTEST_INT64("\x01",             0, INT64_C(1)),
		/* 9223372036854775807 */
		DPACK_UTEST_INT64("\xcf"
		                  "\x7f\xff\xff\xff"
		                  "\xff\xff\xff\xff", 0, INT64_MAX),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int64_t              val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_encode_int64(NULL, val));
	expect_assert_failure(ret = dpack_encode_int64(&enc, val));
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
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int64(NULL, &val));
	expect_assert_failure(ret = dpack_decode_int64(&dec, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int64(&dec, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int64);
}

#define DPACK_UTEST_INT64_MIN(_packed, _error, _value, _low) \
	{ \
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
		DPACK_UTEST_INT64_MIN("\xd3"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x00", -ERANGE, 0,             INT64_MIN + 1),
		/* -9223372036854775807 */
		DPACK_UTEST_INT64_MIN("\xd3"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x01", 0,       INT64_MIN + 1, INT64_MIN + 1),
		/* -9223372036854775806 */
		DPACK_UTEST_INT64_MIN("\xd3"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x02", 0,       INT64_MIN + 2, INT64_MIN + 1),

		/* -1 */
		DPACK_UTEST_INT64_MIN("\xff",             -ERANGE, 0,             INT64_C(0)),
		/* 0 */
		DPACK_UTEST_INT64_MIN("\x00",             0,       INT64_C(0),    INT64_C(0)),
		/* 1 */
		DPACK_UTEST_INT64_MIN("\x01",             0,       INT64_C(1),    INT64_C(0)),

		/* 9223372036854775807 */
		DPACK_UTEST_INT64_MIN("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xff", 0,       INT64_MAX,     INT64_C(0)),
		/* 9223372036854775808 */
		DPACK_UTEST_INT64_MIN("\xcf"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x00", -ERANGE, 0,             INT64_C(0)),

		/* 9223372036854775805 */
		DPACK_UTEST_INT64_MIN("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xfd", -ERANGE, 0,             INT64_MAX - 1),
		/* 9223372036854775806 */
		DPACK_UTEST_INT64_MIN("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xfe", 0,       INT64_MAX - 1, INT64_MAX - 1),
		/* 9223372036854775807 */
		DPACK_UTEST_INT64_MIN("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xff", 0,       INT64_MAX,     INT64_MAX - 1),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int64_t               val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int64_min(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_int64_min(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int64_min(&dec,
	                                                   INT64_MIN,
	                                                   &val));
	expect_assert_failure(ret = dpack_decode_int64_min(&dec,
	                                                   INT64_MAX,
	                                                   &val));
	expect_assert_failure(ret = dpack_decode_int64_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int64_min);
}

#define DPACK_UTEST_INT64_MAX(_packed, _error, _value, _high) \
	{ \
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
		DPACK_UTEST_INT64_MAX("\xd3"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x00", 0,       INT64_MIN,     INT64_MIN + 1),
		/* -9223372036854775807 */
		DPACK_UTEST_INT64_MAX("\xd3"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x01", 0,       INT64_MIN + 1, INT64_MIN + 1),
		/* -9223372036854775806 */
		DPACK_UTEST_INT64_MAX("\xd3"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x02", -ERANGE, 0,             INT64_MIN + 1),

		/* -1 */
		DPACK_UTEST_INT64_MAX("\xff",             0,       INT64_C(-1),   INT64_C(0)),
		/* 0 */
		DPACK_UTEST_INT64_MAX("\x00",             0,       INT64_C(0),    INT64_C(0)),
		/* 1 */
		DPACK_UTEST_INT64_MAX("\x01",             -ERANGE, 0,             INT64_C(0)),

		/* 9223372036854775807 */
		DPACK_UTEST_INT64_MAX("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xff", -ERANGE, 0,             INT64_C(0)),
		/* 9223372036854775808 */
		DPACK_UTEST_INT64_MAX("\xcf"
		                      "\x80\x00\x00\x00"
		                      "\x00\x00\x00\x00", -ERANGE, 0,             INT64_C(0)),

		/* 9223372036854775805 */
		DPACK_UTEST_INT64_MAX("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xfd", 0,       INT64_MAX - 2, INT64_MAX - 1),
		/* 9223372036854775806 */
		DPACK_UTEST_INT64_MAX("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xfe", 0,       INT64_MAX - 1, INT64_MAX - 1),
		/* 9223372036854775807 */
		DPACK_UTEST_INT64_MAX("\xcf"
		                      "\x7f\xff\xff\xff"
		                      "\xff\xff\xff\xff", -ERANGE, 0,             INT64_MAX - 1),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int64_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int64_max(NULL, 1, &val));
	expect_assert_failure(ret = dpack_decode_int64_max(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int64_max(&dec,
	                                                   INT64_MIN,
	                                                   &val));
	expect_assert_failure(ret = dpack_decode_int64_max(&dec,
	                                                   INT64_MAX,
	                                                   &val));
	expect_assert_failure(ret = dpack_decode_int64_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int64_max);
}

#define DPACK_UTEST_INT64_RANGE(_packed, _error, _value, _low, _high) \
	{ \
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
		DPACK_UTEST_INT64_RANGE("\xd3"
		                        "\x80\x00\x00\x00"
		                        "\x00\x00\x00\x00", -ERANGE, 0,             INT64_MIN + 1, INT64_MIN + 2),
		/* -9223372036854775807 */
		DPACK_UTEST_INT64_RANGE("\xd3"
		                        "\x80\x00\x00\x00"
		                        "\x00\x00\x00\x01", 0,       INT64_MIN + 1, INT64_MIN + 1, INT64_MIN + 2),
		/* -9223372036854775806 */
		DPACK_UTEST_INT64_RANGE("\xd3"
		                        "\x80\x00\x00\x00"
		                        "\x00\x00\x00\x02", 0,       INT64_MIN + 2, INT64_MIN + 1, INT64_MIN + 2),
		/* -9223372036854775805 */
		DPACK_UTEST_INT64_RANGE("\xd3"
		                        "\x80\x00\x00\x00"
		                        "\x00\x00\x00\x03", -ERANGE, 0,             INT64_MIN + 1, INT64_MIN + 2),

		/* -1 */
		DPACK_UTEST_INT64_RANGE("\xff",             -ERANGE, 0,             INT64_C(0),    INT64_C(1)),
		/* 0 */
		DPACK_UTEST_INT64_RANGE("\x00",             0,       INT64_C(0),    INT64_C(0),    INT64_C(1)),
		/* 1 */
		DPACK_UTEST_INT64_RANGE("\x01",             0,       INT64_C(1),    INT64_C(0),    INT64_C(1)),
		/* 2 */
		DPACK_UTEST_INT64_RANGE("\x02",             -ERANGE, 0,             INT64_C(0),    INT64_C(1)),

		/* 9223372036854775804 */
		DPACK_UTEST_INT64_RANGE("\xcf"
		                        "\x7f\xff\xff\xff"
		                        "\xff\xff\xff\xfc", -ERANGE, 0,             INT64_MAX - 2, INT64_MAX - 1),
		/* 9223372036854775805 */
		DPACK_UTEST_INT64_RANGE("\xcf"
		                        "\x7f\xff\xff\xff"
		                        "\xff\xff\xff\xfd", 0,       INT64_MAX - 2, INT64_MAX - 2, INT64_MAX - 1),
		/* 9223372036854775806 */
		DPACK_UTEST_INT64_RANGE("\xcf"
		                        "\x7f\xff\xff\xff"
		                        "\xff\xff\xff\xfe", 0,       INT64_MAX - 1, INT64_MAX - 2, INT64_MAX - 1),
		/* 9223372036854775807 */
		DPACK_UTEST_INT64_RANGE("\xcf"
		                        "\x7f\xff\xff\xff"
		                        "\xff\xff\xff\xff", -ERANGE, 0,             INT64_MAX - 2, INT64_MAX - 1),
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	int64_t              val;
	struct dpack_decoder dec = { 0, };
	int                  ret __unused;

	expect_assert_failure(ret = dpack_decode_int64_range(NULL, 1, 2, &val));
	expect_assert_failure(ret = dpack_decode_int64_range(&dec, 1, 2, &val));

	dpack_decoder_init_buffer(&dec, data[0].packed, data[0].size);
	expect_assert_failure(ret = dpack_decode_int64_range(&dec,
	                                                     INT64_MIN,
	                                                     2,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_int64_range(&dec,
	                                                     1,
	                                                     INT64_MAX,
	                                                     &val));
	expect_assert_failure(ret = dpack_decode_int64_range(&dec, 2, 2, &val));
	expect_assert_failure(ret = dpack_decode_int64_range(&dec, 1, 2, NULL));
	dpack_decoder_fini(&dec);
#endif

	dpack_scalar_utest_decode(data,
	                          array_nr(data),
	                          dpack_scalar_utest_unpack_int64_range);
}

static const struct CMUnitTest dpack_stdint_utests[] = {
	cmocka_unit_test(dpack_scalar_utest_encode_bool),
	cmocka_unit_test(dpack_scalar_utest_decode_bool),

	cmocka_unit_test(dpack_scalar_utest_encode_uint8),
	cmocka_unit_test(dpack_scalar_utest_decode_uint8),
	cmocka_unit_test(dpack_scalar_utest_decode_uint8_min),
	cmocka_unit_test(dpack_scalar_utest_decode_uint8_max),
	cmocka_unit_test(dpack_scalar_utest_decode_uint8_range),

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
