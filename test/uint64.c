
/*
 * Automatically generated thanks to the following command:
 *     dpack-utest-gen.py --outfile /mnt/devel/greg/tidor/dpack/test/uint64.c gen uint64
 */
#include "utest.h"
#include "dpack/scalar.h"

static void
dpack_scalar_utest_unpack_uint64(
	struct dpack_decoder * decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint64_t val;

	assert_int_equal(dpack_decode_uint64(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->uint64);
}

const struct dpack_scalar_utest dpack_scalar_utest_uint64 = {
	.unpack = dpack_scalar_utest_unpack_uint64,
	.data   = {
		/* -1 */
		{ .packed = "\xff", .size = 1, .error = -ENOMSG },
		/* 0 */
		{ .packed = "\x00", .size = 1, .error = 0, .uint64 = 0U },
		/* 1 */
		{ .packed = "\x01", .size = 1, .error = 0, .uint64 = 1U },
		/* 18446744073709551615 */
		{ .packed = "\xcf\xff\xff\xff\xff\xff\xff\xff\xff", .size = 9, .error = 0, .uint64 = 18446744073709551615U },
		/* End of operations marker */
		{ NULL, }
	}
};

