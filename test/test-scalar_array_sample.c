#include "scalar_array_sample.h"
#include "test.h"

static int
pack(struct dpack_encoder * encoder)
{
	const struct scalar_array_sample spl = {
		.thirty_two = 200,
		.array      = { 0, 1, 2, 3, 4, 5, 6 },
		.eight      = 128
	};

	return scalar_array_sample_pack(encoder, &spl);
}

static int
unpack(struct dpack_decoder * decoder)
{
	struct scalar_array_sample spl;
	int                        err;
	unsigned int               e;

	err = scalar_array_sample_unpack(decoder, &spl);
	if (err)
		return err;

	printf("thirty_two: %u\n", spl.thirty_two);
	for (e = 0; e < dpack_array_nr(spl.array); e++)
		printf("array[%u]: %hu\n", e, spl.array[e]);
	printf("eight: %hhu\n", spl.eight);

	return 0;
}

static const struct test_ops ops = {
	.min_size = SCALAR_ARRAY_SAMPLE_PACKED_SIZE_MIN,
	.max_size = SCALAR_ARRAY_SAMPLE_PACKED_SIZE_MAX,
	.pack     = pack,
	.unpack   = unpack
};

int main(int argc, char * const argv[])
{
	return test_main(argc, argv, &ops);
}
