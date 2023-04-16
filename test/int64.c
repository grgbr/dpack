
/*
 * Automatically generated thanks to the following command:
 *     dpack-utest-gen.py --outfile /mnt/devel/greg/tidor/dpack/test/int64.c gen int64
 */
#include "utest.h"
#include "dpack/scalar.h"

static void
dpack_scalar_utest_unpack_int64(
	struct dpack_decoder * decoder,
	const struct dpack_scalar_utest_data * data)
{
	int64_t val;

	assert_int_equal(dpack_decode_int64(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->int64);
}

const struct dpack_scalar_utest dpack_scalar_utest_int64 = {
	.unpack = dpack_scalar_utest_unpack_int64,
	.data   = {
		/* 9223372036854775808 */
		{ .packed = "\xcf\x80\x00\x00\x00\x00\x00\x00\x00", .size = 9, .error = -ERANGE },
		/* -9223372036854775808 */
		{ .packed = "\xd3\x80\x00\x00\x00\x00\x00\x00\x00", .size = 9, .error = 0, .int64 = -9223372036854775808 },
		/* -9223372036854775807 */
		{ .packed = "\xd3\x80\x00\x00\x00\x00\x00\x00\x01", .size = 9, .error = 0, .int64 = -9223372036854775807 },
		/* -1 */
		{ .packed = "\xff", .size = 1, .error = 0, .int64 = -1 },
		/* 0 */
		{ .packed = "\x00", .size = 1, .error = 0, .int64 = 0 },
		/* 1 */
		{ .packed = "\x01", .size = 1, .error = 0, .int64 = 1 },
		/* 9223372036854775807 */
		{ .packed = "\xcf\x7f\xff\xff\xff\xff\xff\xff\xff", .size = 9, .error = 0, .int64 = 9223372036854775807 },
		/* End of operations marker */
		{ NULL, }
	}
};

