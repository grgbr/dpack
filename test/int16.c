
/*
 * Automatically generated thanks to the following command:
 *     dpack-utest-gen.py --outfile /mnt/devel/greg/tidor/dpack/test/int16.c gen int16
 */
#include "utest.h"
#include "dpack/scalar.h"

static void
dpack_scalar_utest_unpack_int16(
	struct dpack_decoder * decoder,
	const struct dpack_scalar_utest_data * data)
{
	int16_t val;

	assert_int_equal(dpack_decode_int16(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->int16);
}

const struct dpack_scalar_utest dpack_scalar_utest_int16 = {
	.unpack = dpack_scalar_utest_unpack_int16,
	.data   = {
		/* -32769 */
		{ .packed = "\xd2\xff\xff\x7f\xff", .size = 5, .error = -ERANGE },
		/* -32768 */
		{ .packed = "\xd1\x80\x00", .size = 3, .error = 0, .int16 = -32768 },
		/* -32767 */
		{ .packed = "\xd1\x80\x01", .size = 3, .error = 0, .int16 = -32767 },
		/* -1 */
		{ .packed = "\xff", .size = 1, .error = 0, .int16 = -1 },
		/* 0 */
		{ .packed = "\x00", .size = 1, .error = 0, .int16 = 0 },
		/* 1 */
		{ .packed = "\x01", .size = 1, .error = 0, .int16 = 1 },
		/* 32767 */
		{ .packed = "\xcd\x7f\xff", .size = 3, .error = 0, .int16 = 32767 },
		/* 32768 */
		{ .packed = "\xcd\x80\x00", .size = 3, .error = -ERANGE },
		/* End of operations marker */
		{ NULL, }
	}
};

