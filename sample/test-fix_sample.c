#include "fix_sample.h"
#include "test.h"

static int
pack(struct dpack_encoder * encoder)
{
	const struct fix_sample spl = {
		.eight      = 2,
		.sixteen    = 3,
		.thirty_two = 4
	};

	return fix_sample_pack(encoder, &spl);
}

static int
unpack(struct dpack_decoder * decoder)
{
	struct fix_sample spl;
	int               err;

	err = fix_sample_unpack(decoder, &spl);
	if (err)
		return err;

	printf("eight: %hhu\nsixteen: %hu\nthirty_two: %u\n",
	       spl.eight,
	       spl.sixteen,
	       spl.thirty_two);

	return 0;
}

static const struct test_ops ops = {
	.min_size = FIX_SAMPLE_PACKED_SIZE_MIN,
	.max_size = FIX_SAMPLE_PACKED_SIZE_MAX,
	.pack     = pack,
	.unpack   = unpack
};

int main(int argc, char * const argv[])
{
	return test_main(argc, argv, &ops);
}
