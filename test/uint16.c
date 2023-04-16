
/*
 * Automatically generated thanks to the following command:
 *     dpack-utest-gen.py --outfile /mnt/devel/greg/tidor/dpack/test/uint16.c gen uint16
 */
#include "utest.h"
#include "dpack/scalar.h"

static void
dpack_scalar_utest_unpack_uint16(
	struct dpack_decoder * decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint16_t val;

	assert_int_equal(dpack_decode_uint16(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->uint16);
}

const struct dpack_scalar_utest dpack_scalar_utest_uint16 = {
	.unpack = dpack_scalar_utest_unpack_uint16,
	.data   = {
		/* -1 */
		{ .packed = "\xff", .size = 1, .error = -ENOMSG },
		/* 0 */
		{ .packed = "\x00", .size = 1, .error = 0, .uint16 = 0U },
		/* 1 */
		{ .packed = "\x01", .size = 1, .error = 0, .uint16 = 1U },
		/* 65535 */
		{ .packed = "\xcd\xff\xff", .size = 3, .error = 0, .uint16 = 65535U },
		/* 65536 */
		{ .packed = "\xce\x00\x01\x00\x00", .size = 5, .error = -ERANGE },
		/* End of operations marker */
		{ NULL, }
	}
};

