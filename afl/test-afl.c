// SPDX-License-Identifier: MIT
#include <afl-fix.h>
#include <dpack/codec.h>
#include <stdlib.h>
#include <stdio.h>

__AFL_FUZZ_INIT();

#define STRING_VALUE "test string"

static int
save_to_file(const char *path, const char *buffer, size_t size)
{
	assert(path);
	assert(buffer);
	assert(size);

	FILE       *out;
	int         err = 0;

	out = fopen(path, "w");
	if (!out) {
		err = errno;
		goto err;
	}

	if (fwrite(buffer, size, 1, out) != 1)
		err = errno;

	if (fclose(out)) {
		if (!err)
			err = errno;
	}

	if (!err)
		return 0;

err:
	return -err;
}

static int
pack_to_file(const char *path)
{
	char                  *buff;
	struct dpack_encoder   enc;
	int                    err = 0;
	struct afl_sample      spl;
	char                  *string;



	buff = malloc(AFL_SAMPLE_PACKED_SIZE_MAX);
	if (!buff)
		return EXIT_FAILURE;

	string = malloc(sizeof(STRING_VALUE));
	if (!string)
		return EXIT_FAILURE;

	strncpy(string, STRING_VALUE, sizeof(STRING_VALUE));

	dpack_encoder_init_buffer(&enc, buff, AFL_SAMPLE_PACKED_SIZE_MAX);
	err |= afl_sample_init(&spl);
	afl_sample_set_string(&spl, string);
	afl_sample_set_s64(&spl, -1);
	err |= afl_sample_pack(&enc, &spl);
	err |= save_to_file(path, buff, dpack_encoder_space_used(&enc));
	dpack_encoder_fini(&enc);
	afl_sample_fini(&spl);

	free(buff);

	return err ? EXIT_FAILURE : EXIT_SUCCESS;
}

int main(int argc, char * const argv[])
{
	struct dpack_decoder   dec;
	struct dpack_encoder   enc;
	struct afl_sample      spl;
	char                  *out;
	int                    ret;


	if (argc == 2)
		return pack_to_file(argv[1]);

	out = malloc(AFL_SAMPLE_PACKED_SIZE_MAX);
	if (!out)
		return EXIT_FAILURE;


#ifdef __AFL_HAVE_MANUAL_CONTROL
	__AFL_INIT();
#endif
	unsigned char *buf = __AFL_FUZZ_TESTCASE_BUF;

	while (__AFL_LOOP(10000)) {
		size_t len = __AFL_FUZZ_TESTCASE_LEN;

		if (len < AFL_SAMPLE_PACKED_SIZE_MIN)
			continue;

		if (len > AFL_SAMPLE_PACKED_SIZE_MAX)
			continue;

		ret = afl_sample_init(&spl);
		dpack_decoder_init_buffer(&dec, (const char *)buf, len);
		dpack_encoder_init_buffer(&enc, out, AFL_SAMPLE_PACKED_SIZE_MAX);
		ret |= afl_sample_unpack(&dec, &spl);
		if (!ret)
			ret = afl_sample_pack(&enc, &spl);
		dpack_encoder_fini(&enc);
		dpack_decoder_fini(&dec);
		afl_sample_fini(&spl);
	}
	free(out);
	return 0;
}
