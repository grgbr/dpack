#include "dpack/string.h"
#include "dpack/codec.h"
#include "utest.h"

struct dpack_str_utest_data {
	const size_t len;
	const int    error;
	size_t       size;
	char *       packed;
	char *       value;
};

#define DPACK_STR_UTEST_DATA_INIT(_len, _error) \
	{ \
		.error = _error, \
		.len   = _len \
	}

typedef int (dpack_str_utest_pack_fn)(struct dpack_encoder *,
                                      const struct dpack_str_utest_data *);

static size_t
dpack_utest_str_size(size_t len)
{
	switch (len) {
	case 1 ... 31:
		return len + 1;
	case 32 ... 255:
		return len + 2;
	case 256 ... 65535:
		return len + 3;
	case 65536 ... 4294967295:
		return len + 5;
	default:
		fail();
	}

	unreachable();
}

static int
dpack_str_utest_gen_data(struct dpack_str_utest_data * data)
{
	unsigned int b;
	char *       p;

	data->size = dpack_utest_str_size(data->len);
	data->packed = malloc(data->size);
	if (!data->packed)
		return -ENOMEM;

	p = data->packed;
	switch (data->len) {
	case 1 ... DPACK_FIXSTR_LEN_MAX:
		*p++ = (char)0xa0 | (char)(data->len & 0x1f);
		break;
#if DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX
	case (DPACK_FIXSTR_LEN_MAX + 1) ... DPACK_STR8_LEN_MAX:
		*p++ = (char)0xd9;
		*p++ = (char)(data->len & 0xff);
		break;
#endif
#if DPACK_STRLEN_MAX > DPACK_STR8_LEN_MAX
	case (DPACK_STR8_LEN_MAX + 1) ... DPACK_STR16_LEN_MAX:
		*p++ = (char)0xda;
		*p++ = (char)((data->len >> 8) & 0xff);
		*p++ = (char)(data->len & 0xff);
		break;
#endif
#if DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX
	case (DPACK_STR16_LEN_MAX + 1) ... DPACK_STRLEN_MAX:
		*p++ = 0xdb;
		*p++ = (char)((data->len >> 24) & 0xff);
		*p++ = (char)((data->len >> 16) & 0xff);
		*p++ = (char)((data->len >> 8) & 0xff);
		*p++ = (char)(data->len & 0xff);
		break;
#endif
	default:
		return -ENOSYS;
	}
	for (b = 0; b < data->len; b++)
		*p++ = '0' + (char)(b % 10);

	data->value = malloc(data->len + 1);
	if (!data->value) {
		free(data->packed);
		return -ENOMEM;
	}

	p = data->value;
	for (b = 0; b < data->len; b++)
		*p++ = '0' + (char)(b % 10);
	data->value[data->len] = '\0';

	return 0;
}

static void
dpack_str_utest_fini_data(struct dpack_str_utest_data * data)
{
	free(data->packed);
	free(data->value);
}

static void
dpack_str_utest_encode(struct dpack_str_utest_data * data,
                       unsigned int                  nr,
                       dpack_str_utest_pack_fn *     pack)
{
	struct dpack_encoder enc = { 0, };
	unsigned int         d;

	for (d = 0; d < nr; d++) {
		size_t len = data[d].len;
		int    err;
		size_t sz;
		char * buff;

		err = dpack_str_utest_gen_data(&data[d]);
		if (err)
			fail();

		assert_int_equal(len, strlen(data[d].value));

		sz = data[d].size;
		buff = malloc(sz);
		if (!buff) {
			dpack_str_utest_fini_data(&data[d]);
			fail();
		}

		memset(buff, 0xa5, sz);
		dpack_encoder_init_buffer(&enc, buff, sz);

		err = pack(&enc, &data[d]);
		assert_int_equal(err, data[d].error);
		assert_memory_equal(buff, data[d].packed, sz);

		assert_int_equal(dpack_str_size(len), sz);

		assert_int_equal(dpack_encoder_space_used(&enc), sz);
		assert_int_equal(dpack_encoder_space_left(&enc), 0);

		dpack_encoder_fini(&enc);

		free(buff);
		dpack_str_utest_fini_data(&data[d]);
	}
}

#if defined(ZERO_STR_SIZE_UTEST)

static void
dpack_zero_str_size_utest(void ** state __unused)
{
	/* Should fail to compile since 0 length string are not allowed. */
	int sz = DPACK_STR_SIZE(0);
}

#endif /* defined(ZERO_STR_SIZE_UTEST) */

#if defined(NONCONST_STR_SIZE_UTEST)

static void
dpack_nonconst_str_size_utest(void ** state __unused)
{
	unsigned int len;

	/* Should fail to compile since len is not constant. */
	assert_int_equal(DPACK_STR_SIZE(len), 0);
}

#endif /* defined(NONCONST_STR_SIZE_UTEST) */

#if defined(MAXLEN_STR_SIZE_UTEST)

static void
dpack_maxlen_str_size_utest(void ** state __unused)
{
	/*
	 * Should fail to compile since requested length > maximum string length
	 * allowed.
	 */
	assert_int_equal(DPACK_STR_SIZE(DPACK_STRLEN_MAX + 1), 0);
}

#endif /* defined(MAXLEN_STR_SIZE_UTEST) */

static void
dpack_fixstr_sizes_utest(void ** state __unused)
{
	/* Msgpack fixstr sizes. */
	assert_int_equal(DPACK_STR_SIZE(1),  2);
	assert_int_equal(dpack_str_size(1),  2);
	assert_int_equal(DPACK_STR_SIZE(2),  3);
	assert_int_equal(dpack_str_size(2),  3);
#if DPACK_STRLEN_MAX >= 30
	assert_int_equal(DPACK_STR_SIZE(30), 31);
	assert_int_equal(dpack_str_size(30), 31);
#endif
#if DPACK_STRLEN_MAX >= 31
	assert_int_equal(DPACK_STR_SIZE(31), 32);
	assert_int_equal(dpack_str_size(31), 32);
#endif
}

#if DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX

static void
dpack_str8_sizes_utest(void ** state __unused)
{
	/* Msgpack 8 bits string sizes. */
	assert_int_equal(DPACK_STR_SIZE(32),            34);
	assert_int_equal(dpack_str_size(32),            34);
#if DPACK_STRLEN_MAX >= 33
	assert_int_equal(DPACK_STR_SIZE(33),            35);
	assert_int_equal(dpack_str_size(33),            35);
#endif
#if DPACK_STRLEN_MAX >= (UINT8_MAX - 2)
	assert_int_equal(DPACK_STR_SIZE(UINT8_MAX - 2), UINT8_MAX);
	assert_int_equal(dpack_str_size(UINT8_MAX - 2), UINT8_MAX);
#endif
#if DPACK_STRLEN_MAX >= (UINT8_MAX - 1)
	assert_int_equal(DPACK_STR_SIZE(UINT8_MAX - 1), UINT8_MAX + 1);
	assert_int_equal(dpack_str_size(UINT8_MAX - 1), UINT8_MAX + 1);
#endif
#if DPACK_STRLEN_MAX >= UINT8_MAX
	assert_int_equal(DPACK_STR_SIZE(UINT8_MAX),     UINT8_MAX + 2);
	assert_int_equal(dpack_str_size(UINT8_MAX),     UINT8_MAX + 2);
#endif
}

#else  /* !(DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX) */

static void
dpack_str8_sizes_utest(void ** state __unused)
{
	skip();
}

#endif /* DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX */

#if DPACK_STRLEN_MAX > DPACK_STR8_LEN_MAX

static void
dpack_str16_sizes_utest(void ** state __unused)
{
	/* Msgpack 16 bits string sizes. */
	assert_int_equal(DPACK_STR_SIZE(UINT8_MAX + 1),  UINT8_MAX + 1 + 3);
	assert_int_equal(dpack_str_size(UINT8_MAX + 1),  UINT8_MAX + 1 + 3);
#if DPACK_STRLEN_MAX >= (UINT8_MAX + 2)
	assert_int_equal(DPACK_STR_SIZE(UINT8_MAX + 2),  UINT8_MAX + 2 + 3);
	assert_int_equal(dpack_str_size(UINT8_MAX + 2),  UINT8_MAX + 2 + 3);
#endif
#if DPACK_STRLEN_MAX >= (UINT16_MAX - 2)
	assert_int_equal(DPACK_STR_SIZE(UINT16_MAX - 2), UINT16_MAX - 2 + 3);
	assert_int_equal(dpack_str_size(UINT16_MAX - 2), UINT16_MAX - 2 + 3);
#endif
#if DPACK_STRLEN_MAX >= (UINT16_MAX - 1)
	assert_int_equal(DPACK_STR_SIZE(UINT16_MAX - 1), UINT16_MAX - 1 + 3);
	assert_int_equal(dpack_str_size(UINT16_MAX - 1), UINT16_MAX - 1 + 3);
#endif
#if DPACK_STRLEN_MAX >= UINT16_MAX
	assert_int_equal(DPACK_STR_SIZE(UINT16_MAX),     UINT16_MAX + 3);
	assert_int_equal(dpack_str_size(UINT16_MAX),     UINT16_MAX + 3);
#endif
}

#else  /* !(DPACK_STRLEN_MAX > DPACK_STR8_LEN_MAX) */

static void
dpack_str16_sizes_utest(void ** state __unused)
{
	skip();
}

#endif /* DPACK_STRLEN_MAX > DPACK_STR8_LEN_MAX */

#if DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX

static void
dpack_str32_sizes_utest(void ** state __unused)
{
	/* Msgpack 32 bits string sizes. */
	assert_int_equal(DPACK_STR_SIZE(UINT16_MAX + 1), UINT16_MAX + 1 + 5);
	assert_int_equal(dpack_str_size(UINT16_MAX + 1), UINT16_MAX + 1 + 5);
#if DPACK_STRLEN_MAX >= (UINT16_MAX + 2)
	assert_int_equal(DPACK_STR_SIZE(UINT16_MAX + 2), UINT16_MAX + 2 + 5);
	assert_int_equal(dpack_str_size(UINT16_MAX + 2), UINT16_MAX + 2 + 5);
#endif
#if DPACK_STRLEN_MAX >= (UINT32_MAX - 2)
	assert_int_equal(DPACK_STR_SIZE(UINT32_MAX - 2), UINT32_MAX - 2 + 5);
	assert_int_equal(dpack_str_size(UINT32_MAX - 2), UINT32_MAX - 2 + 5);
#endif
#if DPACK_STRLEN_MAX >= (UINT32_MAX - 1)
	assert_int_equal(DPACK_STR_SIZE(UINT32_MAX - 1), UINT32_MAX - 1 + 5);
	assert_int_equal(dpack_str_size(UINT32_MAX - 1), UINT32_MAX - 1 + 5);
#endif
#if DPACK_STRLEN_MAX >= UINT32_MAX
	assert_int_equal(DPACK_STR_SIZE(UINT32_MAX),     UINT32_MAX + 5);
	assert_int_equal(dpack_str_size(UINT32_MAX),     UINT32_MAX + 5);
#endif
}

#else  /* !(DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX) */

static void
dpack_str32_sizes_utest(void ** state __unused)
{
	skip();
}

#endif /* DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX */

#if defined(CONFIG_DPACK_ASSERT_API)

static void
dpack_str_utest_assert_sizes(void ** state __unused)
{
	size_t sz __unused;

	expect_assert_failure(sz = dpack_str_size(0));
	expect_assert_failure(sz = dpack_str_size(DPACK_STRLEN_MAX + 1));
}

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

static void
dpack_str_utest_assert_sizes(void ** state __unused)
{
	skip();
}

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#define DPACK_UTEST_STR(_packed, _error, _value) \
	{ \
		.packed = _packed, \
		.size   = sizeof(_packed) - 1, \
		.error  = _error, \
		.value  = _value \
	}

static int
dpack_utest_pack_str(struct dpack_encoder *              encoder,
                     const struct dpack_str_utest_data * data)
{
	return dpack_encode_str(encoder, data->value);
}

static void
dpack_utest_encode_str(void ** state __unused)
{
	static struct dpack_str_utest_data data[] = {
		DPACK_STR_UTEST_DATA_INIT(1,  0),
#if DPACK_STRLEN_MAX >= 31
		DPACK_STR_UTEST_DATA_INIT(31, 0),
#endif
#if DPACK_STRLEN_MAX >= 32
		DPACK_STR_UTEST_DATA_INIT(32, 0),
#endif
#if DPACK_STRLEN_MAX >= 255
		DPACK_STR_UTEST_DATA_INIT(255, 0),
#endif
#if DPACK_STRLEN_MAX >= 256
		DPACK_STR_UTEST_DATA_INIT(256, 0),
#endif
#if DPACK_STRLEN_MAX >= 65535
		DPACK_STR_UTEST_DATA_INIT(65535, 0),
#endif
#if DPACK_STRLEN_MAX >= 65536
		DPACK_STR_UTEST_DATA_INIT(65536, 0),
#endif
		DPACK_STR_UTEST_DATA_INIT(DPACK_STRLEN_MAX, 0)
	};

#if defined(CONFIG_DPACK_ASSERT_API)
	struct dpack_encoder enc = { 0, };
	char *               buff;
	int                  ret __unused;
	const char *         str = *state;

	expect_assert_failure(ret = dpack_encode_str(NULL, "test"));
	expect_assert_failure(ret = dpack_encode_str(&enc, "test"));

	buff = malloc(DPACK_STRLEN_MAX + 2);
	assert_non_null(buff);
	memset(buff, 'f', DPACK_STRLEN_MAX + 1);
	buff[DPACK_STRLEN_MAX + 1] = '\0';

	dpack_encoder_init_buffer(&enc, buff, DPACK_STRLEN_MAX + 2);
	expect_assert_failure(ret = dpack_encode_str(&enc, buff));
	expect_assert_failure(ret = dpack_encode_str(&enc, NULL));
	expect_assert_failure(ret = dpack_encode_str(&enc, ""));
	expect_assert_failure(ret = dpack_encode_str(&enc, str));
	dpack_encoder_fini(&enc);

	free(buff);
#endif

	dpack_str_utest_encode(data,
	                       array_nr(data),
	                       dpack_utest_pack_str);
}

static const struct CMUnitTest dpack_str_utests[] = {
	cmocka_unit_test(dpack_fixstr_sizes_utest),
	cmocka_unit_test(dpack_str8_sizes_utest),
	cmocka_unit_test(dpack_str16_sizes_utest),
	cmocka_unit_test(dpack_str32_sizes_utest),
	cmocka_unit_test(dpack_str_utest_assert_sizes),
	cmocka_unit_test(dpack_utest_encode_str)
};

int
main(void)
{
	return cmocka_run_group_tests_name("string size",
	                                   dpack_str_utests,
	                                   NULL,
	                                   NULL);
}
