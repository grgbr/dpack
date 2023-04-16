
/*
 * Automatically generated thanks to the following command:
 *     dpack-utest-gen.py --outfile /mnt/devel/greg/tidor/dpack/test/uint8.c gen uint8
 */
#include "utest.h"
#include "dpack/scalar.h"

static void
dpack_scalar_utest_unpack_uint8(
	struct dpack_decoder * decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint8_t val;

	assert_int_equal(dpack_decode_uint8(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->uint8);
}

const struct dpack_scalar_utest dpack_scalar_utest_uint8 = {
	.unpack = dpack_scalar_utest_unpack_uint8,
	.data   = {
		/* -1 */
		{ .packed = "\xff", .size = 1, .error = -ENOMSG },
		/* 0 */
		{ .packed = "\x00", .size = 1, .error = 0, .uint8 = 0U },
		/* 1 */
		{ .packed = "\x01", .size = 1, .error = 0, .uint8 = 1U },
		/* 255 */
		{ .packed = "\xcc\xff", .size = 2, .error = 0, .uint8 = 255U },
		/* 256 */
		{ .packed = "\xcd\x01\x00", .size = 3, .error = -ERANGE },
		/* End of operations marker */
		{ NULL, }
	}
};

