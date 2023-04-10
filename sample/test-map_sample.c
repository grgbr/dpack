#include "map_sample.h"
#include "test.h"

static int
pack(struct dpack_encoder * encoder)
{
	struct map_sample spl = MAP_SAMPLE_INIT;
	int               ret;

	map_sample_lend_astring(&spl, "a test sample string");
	map_sample_set_ashort(&spl, INT16_MIN + 2);
	map_sample_set_abool(&spl, true);

	ret = map_sample_pack(encoder, &spl);

	map_sample_fini(&spl);

	return ret;
}

static void
dump(const struct map_sample * sample)
{
	int16_t      ashort = 0;
	const char * astring = NULL;
	bool         abool = false;
	uint32_t     anuint;

	/*
	 * No need to check errors returned since we have just unpacked
	 * successfully.
	 */
	map_sample_get_ashort(sample, &ashort);
	printf("ashort : %hd\n", ashort);

	/* astring field is optional with a default value. */
	map_sample_get_astring(sample, &astring);
	printf("astring: %s\n", astring);

	/*
	 * No need to check errors returned since we have just unpacked
	 * successfully.
	 */
	map_sample_get_abool(sample, &abool);
	printf("abool  : %s\n", abool ? "true" : "false");

	/* anuint field is also optional. */
	if (map_sample_get_anuint(sample, &anuint))
		printf("anuint : %s\n", "NA");
	else
		printf("anuint : %u\n", anuint);
}

static int
unpack(struct dpack_decoder * decoder)
{
	struct map_sample spl = MAP_SAMPLE_INIT;
	int               ret;

	ret = map_sample_unpack(decoder, &spl);
	if (ret)
		goto fini;

	dump(&spl);

fini:
	map_sample_fini(&spl);

	return ret;
}

static const struct test_ops ops = {
	.min_size = MAP_SAMPLE_PACKED_SIZE_MIN,
	.max_size = MAP_SAMPLE_PACKED_SIZE_MAX,
	.pack     = pack,
	.unpack   = unpack
};

int main(int argc, char * const argv[])
{
	return test_main(argc, argv, &ops);
}
