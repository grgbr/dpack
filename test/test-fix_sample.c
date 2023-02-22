#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "fix_sample.h"
#include <dpack/codec.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>

#define show_error(_format, ...) \
	fprintf(stderr, \
	        "%s: " _format, \
	        program_invocation_short_name, \
	        ## __VA_ARGS__)

static int
save_to_file(const char * path, const char * buffer, size_t size)
{
	assert(path);
	assert(buffer);
	assert(size);

	FILE       * out;
	const char * msg;
	int          err = 0;

	out = fopen(path, "w");
	if (!out) {
		msg = "opening output file failed";
		err = errno;
		goto err;
	}

	if (fwrite(buffer, size, 1, out) != 1) {
		msg = "saving data failed";
		err = errno;
	}

	if (fclose(out)) {
		if (!err) {
			msg = "closing output file failed";
			err = errno;
		}
	}

	if (!err)
		return 0;

err:
	show_error("%s: %s (%d).\n", msg, strerror(err), err);

	return -err;
}

static int
pack_to_file(const char * path)
{
	char                    buff[FIX_SAMPLE_PACKED_SIZE_MAX];
	struct dpack_encoder    enc;
	int                     err;
	const struct fix_sample spl = {
		.eight      = 2,
		.sixteen    = 3,
		.thirty_two = 4
	};

	dpack_init_buffer_encoder(&enc, buff, sizeof(buff));

	err = fix_sample_pack(&enc, &spl);
	if (err)
		show_error("packing data failed: %s (%d).\n",
		           strerror(-err),
		           -err);
	else
		err = save_to_file(path, buff, dpack_encoder_space_used(&enc));

	dpack_exit_encoder(&enc);

	return err ? EXIT_FAILURE : EXIT_SUCCESS;
}

static ssize_t
load_from_file(const char * path, char * buffer, size_t size)
{
	assert(path);
	assert(buffer);
	assert(size);
	assert(size <= SSIZE_MAX);

	FILE       * in;
	size_t       sz;
	const char * msg;
	int          err = 0;

	in = fopen(path, "r");
	if (!in) {
		msg = "opening input file failed";
		err = errno;
		goto err;
	}

	sz = fread(buffer, 1, size, in);
	if (sz < 1) {
		msg = "loading data failed";
		err = feof(in) ? ENODATA : errno;
	}

	if (fclose(in)) {
		if (!err) {
			msg = "closing input file failed";
			err = errno;
		}
	}

	if (!err)
		return (ssize_t)sz;

err:
	show_error("%s: %s (%d).\n", msg, strerror(err), err);

	return -err;
}

static int
unpack_from_file(const char * path)
{
	char                 buff[FIX_SAMPLE_PACKED_SIZE_MAX];
	ssize_t              ret;
	struct dpack_decoder dec;
	struct fix_sample    spl;

	ret = load_from_file(path, buff, sizeof(buff));
	if (ret < FIX_SAMPLE_PACKED_SIZE_MIN) {
		if (ret >= 0)
			show_error("unpacking data failed: data missing.\n");

		return EXIT_FAILURE;
	}

	dpack_init_buffer_decoder(&dec, buff, (size_t)ret);
	ret = fix_sample_unpack(&dec, &spl);
	dpack_exit_decoder(&dec);

	if (ret) {
		show_error("unpacking data failed: %s (%d).\n",
		           strerror(-ret),
		           (int)-ret);
		return EXIT_FAILURE;
	}

	printf("eight: %hhu\nsixteen: %hu\nthirty_two: %u\n",
	       spl.eight,
	       spl.sixteen,
	       spl.thirty_two);

	return EXIT_SUCCESS;
}

#define USAGE \
"Usage: %1$s [OPTIONS] <CMD>\n" \
"Sample application showing how to pack / unpack data using dpack.\n" \
"\n" \
"With OPTIONS:\n" \
"    -h|--help -- this help message\n" \
"Where CMD:\n" \
"    pack <PATH>   -- serialize data into file identified by <PATH>\n" \
"    unpack <PATH> -- deserialize data from file identified by <PATH>\n" \
"    help          -- this help message\n"

static void
show_usage(void)
{
	fprintf(stderr, USAGE, program_invocation_short_name);
}

int
main(int argc, char * const argv[])
{
	int                           ret;
	enum { PACK_CMD, UNPACK_CMD } cmd;

	opterr = 0;
	while (true) {
		static const struct option opts[] = {
			{ "help", no_argument, NULL, 'h' },
			{ NULL,   0,           NULL,  0 }
		};

		ret = getopt_long(argc, argv, "h", opts, NULL);
		if (ret < 0)
			/* End of command line option parsing. */
			break;

		switch (ret) {
		case 'h':
			show_usage();
			return EXIT_SUCCESS;

		case '?':
			show_error("unrecognized option '%s'.\n\n",
			           argv[optind - 1]);
			show_usage();
			return EXIT_FAILURE;

		default:
			show_error("unexpected option parsing error.\n");
			return EXIT_FAILURE;
		}
	}

	if (optind >= argc) {
		show_error("missing argument(s).\n\n");
		goto usage;
	}

	if (!strcmp(argv[optind], "pack"))
		cmd = PACK_CMD;
	else if (!strcmp(argv[optind], "unpack"))
		cmd = UNPACK_CMD;
	else if (!strcmp(argv[optind], "help")) {
		show_usage();
		return EXIT_SUCCESS;
	}
	else {
		show_error("unrecognized command '%s'.\n\n", argv[optind]);
		goto usage;
	}

	if ((optind + 1) >= argc) {
		show_error("file path missing.\n\n");
		goto usage;
	}

	switch (cmd) {
	case PACK_CMD:
		return pack_to_file(argv[optind + 1]);
	case UNPACK_CMD:
		return unpack_from_file(argv[optind + 1]);
	default:
		assert(0);
	}

usage:
	show_usage();

	return EXIT_FAILURE;
}
