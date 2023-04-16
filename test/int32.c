
/*
 * Automatically generated thanks to the following command:
 *     dpack-utest-gen.py --outfile /mnt/devel/greg/tidor/dpack/test/int32.c gen int32
 */
#include "utest.h"
#include "dpack/scalar.h"

static void
dpack_scalar_utest_unpack_int32(
	struct dpack_decoder * decoder,
	const struct dpack_scalar_utest_data * data)
{
	int32_t val;

	assert_int_equal(dpack_decode_int32(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->int32);
}

const struct dpack_scalar_utest dpack_scalar_utest_int32 = {
	.unpack = dpack_scalar_utest_unpack_int32,
	.data   = {
		/* -2147483649 */
		{ .packed = "\xd3\xff\xff\xff\xff\x7f\xff\xff\xff", .size = 9, .error = -ERANGE },
		/* -2147483648 */
		{ .packed = "\xd2\x80\x00\x00\x00", .size = 5, .error = 0, .int32 = -2147483648 },
		/* -2147483647 */
		{ .packed = "\xd2\x80\x00\x00\x01", .size = 5, .error = 0, .int32 = -2147483647 },
		/* -1 */
		{ .packed = "\xff", .size = 1, .error = 0, .int32 = -1 },
		/* 0 */
		{ .packed = "\x00", .size = 1, .error = 0, .int32 = 0 },
		/* 1 */
		{ .packed = "\x01", .size = 1, .error = 0, .int32 = 1 },
		/* 2147483647 */
		{ .packed = "\xce\x7f\xff\xff\xff", .size = 5, .error = 0, .int32 = 2147483647 },
		/* 2147483648 */
		{ .packed = "\xce\x80\x00\x00\x00", .size = 5, .error = -ERANGE },
		/* End of operations marker */
		{ NULL, }
	}
};

