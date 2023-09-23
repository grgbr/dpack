#include "utest.h"
#include "dpack/scalar.h"
#include "dpack/codec.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <errno.h>

#define DPACKUT_INT16(_var, _packed, _error, _value) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int16 = _value \
	}

static void
dpackut_int16_encode(const struct dpackut_scalar_data * data)
{
	struct dpack_encoder enc = { 0, };
	size_t               sz = data->size;
	char                 buff[sz];

	memset(buff, 0xa5, sz);
	dpack_encoder_init_buffer(&enc, buff, sz);

	cute_check_uint(data->size, greater_equal, DPACK_INT16_SIZE_MIN);
	cute_check_uint(data->size, lower_equal, DPACK_INT16_SIZE_MAX);

	cute_check_sint(dpack_encode_int16(&enc, data->value.int16),
	                equal,
	                data->error);
	cute_check_mem(buff, equal, data->packed, sz);

	cute_check_uint(dpack_encoder_space_used(&enc), equal, sz);
	cute_check_uint(dpack_encoder_space_left(&enc), equal, 0);

	dpack_encoder_fini(&enc);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int16_encode_assert)
{
	int16_t              val = false;
	struct dpack_encoder enc = { 0, };
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_encode_int16(NULL, val));
	cute_expect_assertion(ret = dpack_encode_int16(&enc, val));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int16_encode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int16_encode__32768)
{
	/* -32768 */
	DPACKUT_INT16(data, "\xd1\x80\x00", 0, INT16_MIN);
	dpackut_int16_encode(&data);
}

CUTE_TEST(dpackut_int16_encode__32767)
{
	/* -32767 */
	DPACKUT_INT16(data, "\xd1\x80\x01", 0, INT16_MIN + 1);
	dpackut_int16_encode(&data);
}

CUTE_TEST(dpackut_int16_encode__1)
{
	/* -1 */
	DPACKUT_INT16(data, "\xff", 0, INT16_C(-1));
	dpackut_int16_encode(&data);
}

CUTE_TEST(dpackut_int16_encode_0)
{
	/* 0 */
	DPACKUT_INT16(data, "\x00", 0, INT16_C(0));
	dpackut_int16_encode(&data);
}

CUTE_TEST(dpackut_int16_encode_1)
{
	/* 1 */
	DPACKUT_INT16(data, "\x01", 0, INT16_C(1));
	dpackut_int16_encode(&data);
}

CUTE_TEST(dpackut_int16_encode_32767)
{
	/* 32767 */
	DPACKUT_INT16(data, "\xcd\x7f\xff", 0, INT16_MAX);
	dpackut_int16_encode(&data);
}

static void
dpackut_int16_decode(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int16_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int16(&dec, &val), equal, data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int16);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT16_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT16_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int16_decode_assert)
{
	int16_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT16_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int16(NULL, &val));
	cute_expect_assertion(ret = dpack_decode_int16(&dec, &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int16(&dec, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int16_decode_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int16_decode__32769)
{
	/* -32769 */
	DPACKUT_INT16(data, "\xd2\xff\xff\x7f\xff", -ERANGE, INT16_C(0));
	dpackut_int16_decode(&data);
}

CUTE_TEST(dpackut_int16_decode__32768)
{
	/* -32768 */
	DPACKUT_INT16(data, "\xd1\x80\x00", 0, INT16_MIN);
	dpackut_int16_decode(&data);
}

CUTE_TEST(dpackut_int16_decode__32767)
{
	/* -32767 */
	DPACKUT_INT16(data, "\xd1\x80\x01", 0, INT16_MIN + 1);
	dpackut_int16_decode(&data);
}

CUTE_TEST(dpackut_int16_decode__1)
{
	/* -1 */
	DPACKUT_INT16(data, "\xff", 0, INT16_C(-1));
	dpackut_int16_decode(&data);
}

CUTE_TEST(dpackut_int16_decode_0)
{
	/* 0 */
	DPACKUT_INT16(data, "\x00", 0, INT16_C(0));
	dpackut_int16_decode(&data);
}

CUTE_TEST(dpackut_int16_decode_1)
{
	/* 1 */
	DPACKUT_INT16(data, "\x01", 0, INT16_C(1));
	dpackut_int16_decode(&data);
}

CUTE_TEST(dpackut_int16_decode_32767)
{
	/* 32767 */
	DPACKUT_INT16(data, "\xcd\x7f\xff", 0, INT16_MAX);
	dpackut_int16_decode(&data);
}

CUTE_TEST(dpackut_int16_decode_32768)
{
	/* 32768 */
	DPACKUT_INT16(data, "\xcd\x80\x00", -ERANGE, INT16_C(0));
	dpackut_int16_decode(&data);
}

#define DPACKUT_INT16_MIN(_var, _packed, _error, _value, _low) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int16 = _value, \
		.low.int16   = _low \
	}

static void
dpackut_int16_decode_min(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int16_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int16_min(&dec, data->low.int16, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int16);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT16_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT16_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if  defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int16_decode_min_assert)
{
	int16_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT16_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int16_min(NULL, 1, &val));
	cute_expect_assertion(ret = dpack_decode_int16_min(&dec, 1, &val));
	cute_expect_assertion(ret = dpack_decode_int16_min(&dec,
	                                                   INT16_MIN,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_int16_min(&dec,
	                                                   INT16_MAX,
	                                                   &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int16_min(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int16_decode_min_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int16_decode_min__32769__32767)
{
	/* -32769 */
	DPACKUT_INT16_MIN(data,
	                  "\xd2\xff\xff\x7f\xff",
	                  -ERANGE,
	                  0,
	                  INT16_MIN + 1);
	dpackut_int16_decode_min(&data);
}

CUTE_TEST(dpackut_int16_decode_min__32768__32767)
{
	/* -32768 */
	DPACKUT_INT16_MIN(data, "\xd1\x80\x00", -ERANGE, 0, INT16_MIN + 1);
	dpackut_int16_decode_min(&data);
}

CUTE_TEST(dpackut_int16_decode_min__32767__32767)
{
	/* -32767 */
	DPACKUT_INT16_MIN(data,
	                  "\xd1\x80\x01",
	                  0,
	                  INT16_MIN + 1,
	                  INT16_MIN + 1);
	dpackut_int16_decode_min(&data);
}

CUTE_TEST(dpackut_int16_decode_min__32766__32767)
{
	/* -32766 */
	DPACKUT_INT16_MIN(data,
	                  "\xd1\x80\x02",
	                  0,
	                  INT16_MIN + 2,
	                  INT16_MIN + 1);
	dpackut_int16_decode_min(&data);
}

CUTE_TEST(dpackut_int16_decode_min__1_0)
{
	/* -1 */
	DPACKUT_INT16_MIN(data, "\xff", -ERANGE, 0, INT16_C(0));
	dpackut_int16_decode_min(&data);
}

CUTE_TEST(dpackut_int16_decode_min_0_0)
{
	/* 0 */
	DPACKUT_INT16_MIN(data, "\x00", 0, INT16_C(0), INT16_C(0));
	dpackut_int16_decode_min(&data);
}

CUTE_TEST(dpackut_int16_decode_min_1_0)
{
	/* 1 */
	DPACKUT_INT16_MIN(data, "\x01", 0, INT16_C(1), INT16_C(0));
	dpackut_int16_decode_min(&data);
}

CUTE_TEST(dpackut_int16_decode_min_32767_0)
{
	/* 32767 */
	DPACKUT_INT16_MIN(data, "\xcd\x7f\xff", 0, INT16_MAX, INT16_C(0));
	dpackut_int16_decode_min(&data);
}

CUTE_TEST(dpackut_int16_decode_min_32768_0)
{
	/* 32768 */
	DPACKUT_INT16_MIN(data, "\xcd\x80\x00", -ERANGE, 0, INT16_C(0));
	dpackut_int16_decode_min(&data);
}

CUTE_TEST(dpackut_int16_decode_min_32765_32766)
{
	/* 32765 */
	DPACKUT_INT16_MIN(data, "\xcd\x7f\xfd", -ERANGE, 0, INT16_MAX - 1);
	dpackut_int16_decode_min(&data);
}

CUTE_TEST(dpackut_int16_decode_min_32766_32766)
{
	/* 32766 */
	DPACKUT_INT16_MIN(data,
	                  "\xcd\x7f\xfe",
	                  0,
	                  INT16_MAX - 1,
	                  INT16_MAX - 1);
	dpackut_int16_decode_min(&data);
}

CUTE_TEST(dpackut_int16_decode_min_32767_32766)
{
	/* 32767 */
	DPACKUT_INT16_MIN(data, "\xcd\x7f\xff", 0, INT16_MAX, INT16_MAX - 1);
	dpackut_int16_decode_min(&data);
}

CUTE_TEST(dpackut_int16_decode_min_32768_32766)
{
	/* 32768 */
	DPACKUT_INT16_MIN(data, "\xcd\x80\x00", -ERANGE, 0, INT16_MAX - 1);
	dpackut_int16_decode_min(&data);
}

#define DPACKUT_INT16_MAX(_var, _packed, _error, _value, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int16 = _value, \
		.high.int16  = _high \
	}

static void
dpackut_int16_decode_max(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int16_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int16_max(&dec, data->high.int16, &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int16);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT16_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT16_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if  defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int16_decode_max_assert)
{
	int16_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT16_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int16_max(NULL, 1, &val));
	cute_expect_assertion(ret = dpack_decode_int16_max(&dec, 1, &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int16_max(&dec,
	                                                   INT16_MIN,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_int16_max(&dec,
	                                                   INT16_MAX,
	                                                   &val));
	cute_expect_assertion(ret = dpack_decode_int16_max(&dec, 1, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int16_decode_max_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int16_decode_max__32769__32767)
{
	/* -32769 */
	DPACKUT_INT16_MAX(data,
	                  "\xd2\xff\xff\x7f\xff",
	                  -ERANGE,
	                  0,
	                  INT16_MIN + 1);
	dpackut_int16_decode_max(&data);
}

CUTE_TEST(dpackut_int16_decode_max__32768__32767)
{
	/* -32768 */
	DPACKUT_INT16_MAX(data, "\xd1\x80\x00", 0, INT16_MIN, INT16_MIN + 1);
	dpackut_int16_decode_max(&data);
}

CUTE_TEST(dpackut_int16_decode_max__32767__32767)
{
	/* -32767 */
	DPACKUT_INT16_MAX(data,
	                  "\xd1\x80\x01",
	                  0,
	                  INT16_MIN + 1,
	                  INT16_MIN + 1);
	dpackut_int16_decode_max(&data);
}

CUTE_TEST(dpackut_int16_decode_max__32766__32767)
{
	/* -32766 */
	DPACKUT_INT16_MAX(data, "\xd1\x80\x02", -ERANGE, 0, INT16_MIN + 1);
	dpackut_int16_decode_max(&data);
}

CUTE_TEST(dpackut_int16_decode_max__1_0)
{
	/* -1 */
	DPACKUT_INT16_MAX(data, "\xff", 0, INT16_C(-1), INT16_C(0));
	dpackut_int16_decode_max(&data);
}

CUTE_TEST(dpackut_int16_decode_max_0_0)
{
	/* 0 */
	DPACKUT_INT16_MAX(data, "\x00", 0, INT16_C(0), INT16_C(0));
	dpackut_int16_decode_max(&data);
}

CUTE_TEST(dpackut_int16_decode_max_1_0)
{
	/* 1 */
	DPACKUT_INT16_MAX(data, "\x01", -ERANGE, 0, INT16_C(0));
	dpackut_int16_decode_max(&data);
}

CUTE_TEST(dpackut_int16_decode_max_32767_0)
{
	/* 32767 */
	DPACKUT_INT16_MAX(data, "\xcd\x7f\xff", -ERANGE, 0, INT16_C(0));
	dpackut_int16_decode_max(&data);
}

CUTE_TEST(dpackut_int16_decode_max_32768_0)
{
	/* 32768 */
	DPACKUT_INT16_MAX(data, "\xcd\x80\x00", -ERANGE, 0, INT16_C(0));
	dpackut_int16_decode_max(&data);
}

CUTE_TEST(dpackut_int16_decode_max_32765_32766)
{
	/* 32765 */
	DPACKUT_INT16_MAX(data,
	                  "\xcd\x7f\xfd",
	                  0,
	                  INT16_MAX - 2,
	                  INT16_MAX - 1);
	dpackut_int16_decode_max(&data);
}

CUTE_TEST(dpackut_int16_decode_max_32766_32766)
{
	/* 32766 */
	DPACKUT_INT16_MAX(data,
	                  "\xcd\x7f\xfe",
	                  0,
	                  INT16_MAX - 1,
	                  INT16_MAX - 1);
	dpackut_int16_decode_max(&data);
}

CUTE_TEST(dpackut_int16_decode_max_32767_32766)
{
	/* 32767 */
	DPACKUT_INT16_MAX(data, "\xcd\x7f\xff", -ERANGE, 0, INT16_MAX - 1);
	dpackut_int16_decode_max(&data);
}

CUTE_TEST(dpackut_int16_decode_max_32768_32766)
{
	/* 32768 */
	DPACKUT_INT16_MAX(data, "\xcd\x80\x00", -ERANGE, 0, INT16_MAX - 1);
	dpackut_int16_decode_max(&data);
}

#define DPACKUT_INT16_RANGE(_var, _packed, _error, _value, _low, _high) \
	const struct dpackut_scalar_data _var = { \
		.packed      = _packed, \
		.size        = sizeof(_packed) - 1, \
		.error       = _error, \
		.value.int16 = _value, \
		.low.int16   = _low, \
		.high.int16  = _high \
	}

static void
dpackut_int16_decode_range(const struct dpackut_scalar_data * data)
{
	struct dpack_decoder dec = { 0, };
	int16_t              val;

	dpack_decoder_init_buffer(&dec, data->packed, data->size);

	cute_check_sint(dpack_decode_int16_range(&dec,
	                                         data->low.int16,
	                                         data->high.int16,
	                                         &val),
	                equal,
	                data->error);
	if (!data->error) {
		cute_check_sint(val, equal, data->value.int16);
		cute_check_uint(data->size,
		                greater_equal,
		                DPACK_INT16_SIZE_MIN);
		cute_check_uint(data->size, lower_equal, DPACK_INT16_SIZE_MAX);
		cute_check_uint(dpack_decoder_data_left(&dec), equal, 0);
	}

	dpack_decoder_fini(&dec);
}

#if  defined(CONFIG_DPACK_ASSERT_API)

CUTE_TEST(dpackut_int16_decode_range_assert)
{
	int16_t              val;
	struct dpack_decoder dec = { 0, };
	char                 buff[DPACK_INT16_SIZE_MAX];
	int                  ret __unused;

	cute_expect_assertion(ret = dpack_decode_int16_range(NULL, 1, 2, &val));
	cute_expect_assertion(ret = dpack_decode_int16_range(&dec, 1, 2, &val));

	dpack_decoder_init_buffer(&dec, buff, sizeof(buff));
	cute_expect_assertion(ret = dpack_decode_int16_range(&dec,
	                                                     INT16_MIN,
	                                                     2,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_int16_range(&dec,
	                                                     1,
	                                                     INT16_MAX,
	                                                     &val));
	cute_expect_assertion(ret = dpack_decode_int16_range(&dec, 2, 2, &val));
	cute_expect_assertion(ret = dpack_decode_int16_range(&dec, 1, 2, NULL));
	dpack_decoder_fini(&dec);
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int16_decode_range_assert)
{
	cute_skip("assertion unsupported");
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

CUTE_TEST(dpackut_int16_decode_range__32769__32767__32766)
{
	/* -32769 */
	DPACKUT_INT16_RANGE(data,
	                    "\xd2\xff\xff\x7f\xff",
	                    -ERANGE,
	                    0,
	                    INT16_MIN + 1,
	                    INT16_MIN + 2);
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range__32768__32767__32766)
{
	/* -32768 */
	DPACKUT_INT16_RANGE(data,
	                    "\xd1\x80\x00",
	                    -ERANGE,
	                    0,
	                    INT16_MIN + 1,
	                    INT16_MIN + 2);
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range__32767__32767__32766)
{
	/* -32767 */
	DPACKUT_INT16_RANGE(data,
	                    "\xd1\x80\x01",
	                    0,
	                    INT16_MIN + 1,
	                    INT16_MIN + 1,
	                    INT16_MIN + 2);
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range__32766__32767__32766)
{
	/* -32766 */
	DPACKUT_INT16_RANGE(data,
	                    "\xd1\x80\x02",
	                    0,
	                    INT16_MIN + 2,
	                    INT16_MIN + 1,
	                    INT16_MIN + 2);
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range__32765__32767__32766)
{
	/* -32765 */
	DPACKUT_INT16_RANGE(data,
	                    "\xd1\x80\x03",
	                    -ERANGE,
	                    0,
	                    INT16_MIN + 1,
	                    INT16_MIN + 2);
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range__1_0_1)
{
	/* -1 */
	DPACKUT_INT16_RANGE(data, "\xff", -ERANGE, 0, INT16_C(0), INT16_C(1));
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range_0_0_1)
{
	/* 0 */
	DPACKUT_INT16_RANGE(data,
	                    "\x00",
	                    0,
	                    INT16_C(0),
	                    INT16_C(0),
	                    INT16_C(1));
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range_1_0_1)
{
	/* 1 */
	DPACKUT_INT16_RANGE(data,
	                    "\x01",
	                    0,
	                    INT16_C(1),
	                    INT16_C(0),
	                    INT16_C(1));
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range_2_0_1)
{
	/* 2 */
	DPACKUT_INT16_RANGE(data,
	                    "\x02",
	                    -ERANGE,
	                    0,
	                    INT16_C(0),
	                    INT16_C(1));
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range_32764_32765_32766)
{
	/* 32764 */
	DPACKUT_INT16_RANGE(data,
	                    "\xcd\x7f\xfc",
	                    -ERANGE,
	                    0,
	                    INT16_MAX - 2,
	                    INT16_MAX - 1);
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range_32765_32765_32766)
{
	/* 32765 */
	DPACKUT_INT16_RANGE(data,
	                    "\xcd\x7f\xfd",
	                    0,
	                    INT16_MAX - 2,
	                    INT16_MAX - 2,
	                    INT16_MAX - 1);
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range_32766_32765_32766)
{
	/* 32766 */
	DPACKUT_INT16_RANGE(data,
	                    "\xcd\x7f\xfe",
	                    0,
	                    INT16_MAX - 1,
	                    INT16_MAX - 2,
	                    INT16_MAX - 1);
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range_32767_32765_32766)
{
	/* 32767 */
	DPACKUT_INT16_RANGE(data,
	                    "\xcd\x7f\xff",
	                    -ERANGE,
	                    0,
	                    INT16_MAX - 2,
	                    INT16_MAX - 1);
	dpackut_int16_decode_range(&data);
}

CUTE_TEST(dpackut_int16_decode_range_32768_32765_32766)
{
	/* 32768 */
	DPACKUT_INT16_RANGE(data,
	                    "\xcd\x80\x00",
	                    -ERANGE,
	                    0,
	                    INT16_MAX - 2,
	                    INT16_MAX - 1);
	dpackut_int16_decode_range(&data);
}

CUTE_GROUP(dpackut_int16_group) = {
	CUTE_REF(dpackut_int16_encode_assert),
	CUTE_REF(dpackut_int16_encode__32768),
	CUTE_REF(dpackut_int16_encode__32767),
	CUTE_REF(dpackut_int16_encode__1),
	CUTE_REF(dpackut_int16_encode_0),
	CUTE_REF(dpackut_int16_encode_1),
	CUTE_REF(dpackut_int16_encode_32767),

	CUTE_REF(dpackut_int16_decode_assert),
	CUTE_REF(dpackut_int16_decode__32769),
	CUTE_REF(dpackut_int16_decode__32768),
	CUTE_REF(dpackut_int16_decode__32767),
	CUTE_REF(dpackut_int16_decode__1),
	CUTE_REF(dpackut_int16_decode_0),
	CUTE_REF(dpackut_int16_decode_1),
	CUTE_REF(dpackut_int16_decode_32767),
	CUTE_REF(dpackut_int16_decode_32768),

	CUTE_REF(dpackut_int16_decode_min_assert),
	CUTE_REF(dpackut_int16_decode_min__32769__32767),
	CUTE_REF(dpackut_int16_decode_min__32768__32767),
	CUTE_REF(dpackut_int16_decode_min__32767__32767),
	CUTE_REF(dpackut_int16_decode_min__32766__32767),
	CUTE_REF(dpackut_int16_decode_min__1_0),
	CUTE_REF(dpackut_int16_decode_min_0_0),
	CUTE_REF(dpackut_int16_decode_min_1_0),
	CUTE_REF(dpackut_int16_decode_min_32767_0),
	CUTE_REF(dpackut_int16_decode_min_32768_0),
	CUTE_REF(dpackut_int16_decode_min_32765_32766),
	CUTE_REF(dpackut_int16_decode_min_32766_32766),
	CUTE_REF(dpackut_int16_decode_min_32767_32766),
	CUTE_REF(dpackut_int16_decode_min_32768_32766),

	CUTE_REF(dpackut_int16_decode_max_assert),
	CUTE_REF(dpackut_int16_decode_max__32769__32767),
	CUTE_REF(dpackut_int16_decode_max_assert),
	CUTE_REF(dpackut_int16_decode_max__32769__32767),
	CUTE_REF(dpackut_int16_decode_max__32768__32767),
	CUTE_REF(dpackut_int16_decode_max__32767__32767),
	CUTE_REF(dpackut_int16_decode_max__32766__32767),
	CUTE_REF(dpackut_int16_decode_max__1_0),
	CUTE_REF(dpackut_int16_decode_max_0_0),
	CUTE_REF(dpackut_int16_decode_max_1_0),
	CUTE_REF(dpackut_int16_decode_max_32767_0),
	CUTE_REF(dpackut_int16_decode_max_32768_0),
	CUTE_REF(dpackut_int16_decode_max_32765_32766),
	CUTE_REF(dpackut_int16_decode_max_32766_32766),
	CUTE_REF(dpackut_int16_decode_max_32767_32766),
	CUTE_REF(dpackut_int16_decode_max_32768_32766),

	CUTE_REF(dpackut_int16_decode_range_assert),
	CUTE_REF(dpackut_int16_decode_range__32769__32767__32766),
	CUTE_REF(dpackut_int16_decode_range_assert),
	CUTE_REF(dpackut_int16_decode_range__32769__32767__32766),
	CUTE_REF(dpackut_int16_decode_range__32768__32767__32766),
	CUTE_REF(dpackut_int16_decode_range__32767__32767__32766),
	CUTE_REF(dpackut_int16_decode_range__32766__32767__32766),
	CUTE_REF(dpackut_int16_decode_range__32765__32767__32766),
	CUTE_REF(dpackut_int16_decode_range__1_0_1),
	CUTE_REF(dpackut_int16_decode_range_0_0_1),
	CUTE_REF(dpackut_int16_decode_range_1_0_1),
	CUTE_REF(dpackut_int16_decode_range_2_0_1),
	CUTE_REF(dpackut_int16_decode_range_32764_32765_32766),
	CUTE_REF(dpackut_int16_decode_range_32765_32765_32766),
	CUTE_REF(dpackut_int16_decode_range_32766_32765_32766),
	CUTE_REF(dpackut_int16_decode_range_32767_32765_32766),
	CUTE_REF(dpackut_int16_decode_range_32768_32765_32766)
};

CUTE_SUITE_EXTERN(dpackut_int16_suite,
                  dpackut_int16_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
