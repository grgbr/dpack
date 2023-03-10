#include "sample-fix.h"
#include "test.h"

static int
pack(struct dpack_encoder *encoder)
{
	int                err;
	struct fix_sample *spl;

	spl = fix_sample_create();
	if (spl == NULL)
		return -1;

	err = fix_sample_check_eight(2);
	if (err)
		goto error_pack;

	err = fix_sample_set_eight(spl, 2);
	if (err)
		goto error_pack;

	err = fix_sample_check_sixteen(3);
	if (err)
		goto error_pack;

	err = fix_sample_set_sixteen(spl, 3);
	if (err)
		goto error_pack;

	/* try invalid range */
	err = fix_sample_check_thirty_two(4000);
	if (!err)
		goto error_pack;

	err = fix_sample_check_thirty_two(4);
	if (err)
		goto error_pack;

	err = fix_sample_set_thirty_two(spl, 4);
	if (err)
		goto error_pack;

	err = fix_sample_pack(encoder, spl);

error_pack:
	fix_sample_destroy(spl);
	return err;
}

static int
unpack(struct dpack_decoder *decoder)
{
	struct fix_sample spl;
	int               err;
	uint8_t         eight;
	uint16_t      sixteen;
	uint32_t   thirty_two;

	err = fix_sample_init(&spl);
	if (err)
		return err;

	err = fix_sample_unpack(decoder, &spl);
	if (err)
		return err;

	err = fix_sample_get_eight(&spl, &eight);
	if (err)
		return err;

	err = fix_sample_get_sixteen(&spl, &sixteen);
	if (err)
		return err;

	err = fix_sample_get_thirty_two(&spl, &thirty_two);
	if (err)
		return err;

	printf("eight: %hhu\nsixteen: %hu\nthirty_two: %u\n",
	       eight,
	       sixteen,
	       thirty_two);

	fix_sample_fini(&spl);
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
