#include "test.h"
#include <dpack/codec.h>
#include <getopt.h>
#include <assert.h>

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
	test_show_error("%s: %s (%d).\n", msg, strerror(err), err);

	return -err;
}

static int
pack_to_file(const char * path, const struct test_ops * ops)
{
	char                 * buff;
	struct dpack_encoder   enc;
	int                    err;

	buff = malloc(ops->max_size);
	if (!buff) {
		test_show_error("buffer allocation failed.\n");
		return EXIT_FAILURE;
	}

	dpack_encoder_init_buffer(&enc, buff, ops->max_size);

	err = ops->pack(&enc);
	if (err)
		test_show_error("packing failed: %s (%d).\n",
		                strerror(-err),
		                -err);
	else
		err = save_to_file(path, buff, dpack_encoder_space_used(&enc));

	dpack_encoder_fini(&enc);

	free(buff);

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
	test_show_error("%s: %s (%d).\n", msg, strerror(err), err);

	return -err;
}

static int
unpack_from_file(const char * path, const struct test_ops * ops)
{
	char                 * buff;
	ssize_t                ret;
	struct dpack_decoder   dec;

	buff = malloc(ops->max_size);
	if (!buff) {
		test_show_error("buffer allocation failed.\n");
		return EXIT_FAILURE;
	}

	ret = load_from_file(path, buff, ops->max_size);
	if (ret < (ssize_t)ops->min_size) {
		if (ret >= 0)
			test_show_error("unpacking failed: data missing.\n");

		ret = EXIT_FAILURE;
		goto free;
	}

	dpack_decoder_init_buffer(&dec, buff, (size_t)ret);
	ret = ops->unpack(&dec);
	dpack_decoder_fini(&dec);

	if (ret) {
		test_show_error("unpacking failed: %s (%d).\n",
		                strerror(-ret),
		                (int)-ret);
		ret = EXIT_FAILURE;
		goto free;
	}

	ret = EXIT_SUCCESS;

free:
	free(buff);

	return ret;
}

#define TEST_USAGE \
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
test_show_usage(void)
{
	fprintf(stderr, TEST_USAGE, program_invocation_short_name);
}

int
test_main(int argc, char * const argv[], const struct test_ops * ops)
{
	assert(argv);
	assert(ops);
	assert(ops->min_size);
	assert(ops->min_size <= SSIZE_MAX);
	assert(ops->min_size <= ops->max_size);
	assert(ops->pack);
	assert(ops->unpack);

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
			test_show_usage();
			return EXIT_SUCCESS;

		case '?':
			test_show_error("unrecognized option '%s'.\n\n",
			                argv[optind - 1]);
			test_show_usage();
			return EXIT_FAILURE;

		default:
			test_show_error("unexpected option parsing error.\n");
			return EXIT_FAILURE;
		}
	}

	if (optind >= argc) {
		test_show_error("missing argument(s).\n\n");
		goto usage;
	}

	if (!strcmp(argv[optind], "pack"))
		cmd = PACK_CMD;
	else if (!strcmp(argv[optind], "unpack"))
		cmd = UNPACK_CMD;
	else if (!strcmp(argv[optind], "help")) {
		test_show_usage();
		return EXIT_SUCCESS;
	}
	else {
		test_show_error("unrecognized command '%s'.\n\n", argv[optind]);
		goto usage;
	}

	if ((optind + 1) >= argc) {
		test_show_error("file path missing.\n\n");
		goto usage;
	}

	switch (cmd) {
	case PACK_CMD:
		return pack_to_file(argv[optind + 1], ops);
	case UNPACK_CMD:
		return unpack_from_file(argv[optind + 1], ops);
	default:
		assert(0);
	}

usage:
	test_show_usage();

	return EXIT_FAILURE;
}
