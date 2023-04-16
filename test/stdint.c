#include "utest.h"

static void
dpack_scalar_utest_run(void ** state)
{
	const struct dpack_scalar_utest      * test = *state;
	const struct dpack_scalar_utest_data * data;

	for (data = test->data; data->packed; data++) {
		struct dpack_decoder dec;

		dpack_decoder_init_buffer(&dec, data->packed, data->size);

		test->unpack(&dec, data);

		dpack_decoder_fini(&dec);
	}
}

extern const struct dpack_scalar_utest dpack_scalar_utest_uint8;
extern const struct dpack_scalar_utest dpack_scalar_utest_int8;
extern const struct dpack_scalar_utest dpack_scalar_utest_uint16;
extern const struct dpack_scalar_utest dpack_scalar_utest_int16;
extern const struct dpack_scalar_utest dpack_scalar_utest_uint32;
extern const struct dpack_scalar_utest dpack_scalar_utest_int32;
extern const struct dpack_scalar_utest dpack_scalar_utest_uint64;
extern const struct dpack_scalar_utest dpack_scalar_utest_int64;

#define DPACK_SCALAR_UNIT_TEST(_test) \
	{ .name          = STROLL_STRING(_test), \
	  .test_func     = dpack_scalar_utest_run, \
	  .setup_func    = NULL, \
	  .teardown_func = NULL, \
	  .initial_state = (void *)&(_test) \
	}

static const struct CMUnitTest dpack_stdint_utests[] = {
	DPACK_SCALAR_UNIT_TEST(dpack_scalar_utest_uint8),
	DPACK_SCALAR_UNIT_TEST(dpack_scalar_utest_int8),
	DPACK_SCALAR_UNIT_TEST(dpack_scalar_utest_uint16),
	DPACK_SCALAR_UNIT_TEST(dpack_scalar_utest_int16),
	DPACK_SCALAR_UNIT_TEST(dpack_scalar_utest_uint32),
	DPACK_SCALAR_UNIT_TEST(dpack_scalar_utest_int32),
	DPACK_SCALAR_UNIT_TEST(dpack_scalar_utest_uint64),
	DPACK_SCALAR_UNIT_TEST(dpack_scalar_utest_int64)
};

int
main(void)
{
	return cmocka_run_group_tests_name("integers",
	                                   dpack_stdint_utests,
	                                   NULL,
	                                   NULL);
}
