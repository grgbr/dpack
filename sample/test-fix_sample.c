/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "fix_sample.h"
#include "test.h"

static int
pack(struct dpack_encoder * encoder)
{
	const struct fix_sample spl = {
		.eight      = 2,
		.sixteen    = 3,
		.thirty_two = 4,
		.single     = 7.37f
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

	printf("eight: %hhu\nsixteen: %hu\nthirty_two: %u\nsingle: %f\n",
	       spl.eight,
	       spl.sixteen,
	       spl.thirty_two,
	       spl.single);

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
