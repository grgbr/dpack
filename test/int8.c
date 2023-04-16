
/*
 * Automatically generated thanks to the following command:
 *     dpack-utest-gen.py --outfile /mnt/devel/greg/tidor/dpack/test/int8.c gen int8
 */
#include "utest.h"
#include "dpack/scalar.h"

static void
dpack_scalar_utest_unpack_int8(
	struct dpack_decoder * decoder,
	const struct dpack_scalar_utest_data * data)
{
	int8_t val;

	assert_int_equal(dpack_decode_int8(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->int8);
}

const struct dpack_scalar_utest dpack_scalar_utest_int8 = {
	.unpack = dpack_scalar_utest_unpack_int8,
	.data   = {
		/* -129 */
		{ .packed = "\xd1\xff\x7f", .size = 3, .error = -ERANGE },
		/* -128 */
		{ .packed = "\xd0\x80", .size = 2, .error = 0, .int8 = -128 },
		/* -127 */
		{ .packed = "\xd0\x81", .size = 2, .error = 0, .int8 = -127 },
		/* -1 */
		{ .packed = "\xff", .size = 1, .error = 0, .int8 = -1 },
		/* 0 */
		{ .packed = "\x00", .size = 1, .error = 0, .int8 = 0 },
		/* 1 */
		{ .packed = "\x01", .size = 1, .error = 0, .int8 = 1 },
		/* 127 */
		{ .packed = "\x7f", .size = 1, .error = 0, .int8 = 127 },
		/* 128 */
		{ .packed = "\xcc\x80", .size = 2, .error = -ERANGE },
		/* End of operations marker */
		{ NULL, }
	}
};

