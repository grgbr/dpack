#include "utest.h"
#include "dpack/scalar.h"
#include "dpack/codec.h"
#include <errno.h>

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
