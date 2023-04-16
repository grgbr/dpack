
/*
 * Automatically generated thanks to the following command:
 *     dpack-utest-gen.py --outfile /mnt/devel/greg/tidor/dpack/test/uint32.c gen uint32
 */
#include "utest.h"
#include "dpack/scalar.h"

static void
dpack_scalar_utest_unpack_uint32(
	struct dpack_decoder * decoder,
	const struct dpack_scalar_utest_data * data)
{
	uint32_t val;

	assert_int_equal(dpack_decode_uint32(decoder, &val), data->error);
	if (!data->error)
		assert_int_equal(val, data->uint32);
}

const struct dpack_scalar_utest dpack_scalar_utest_uint32 = {
	.unpack = dpack_scalar_utest_unpack_uint32,
	.data   = {
		/* -1 */
		{ .packed = "\xff", .size = 1, .error = -ENOMSG },
		/* 0 */
		{ .packed = "\x00", .size = 1, .error = 0, .uint32 = 0U },
		/* 1 */
		{ .packed = "\x01", .size = 1, .error = 0, .uint32 = 1U },
		/* 4294967295 */
		{ .packed = "\xce\xff\xff\xff\xff", .size = 5, .error = 0, .uint32 = 4294967295U },
		/* 4294967296 */
		{ .packed = "\xcf\x00\x00\x00\x01\x00\x00\x00\x00", .size = 9, .error = -ERANGE },
		/* End of operations marker */
		{ NULL, }
	}
};

