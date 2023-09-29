/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _DPACK_TEST_H
#define _DPACK_TEST_H

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#define test_show_error(_format, ...) \
	fprintf(stderr, \
	        "%s: " _format, \
	        program_invocation_short_name, \
	        ## __VA_ARGS__)

struct dpack_encoder;
struct dpack_decoder;

struct test_ops {
	size_t min_size;
	size_t max_size;
	int (*pack)(struct dpack_encoder *);
	int (*unpack)(struct dpack_decoder *);
};

extern int
test_main(int argc, char * const argv[], const struct test_ops * ops);

#endif  /* _DPACK_TEST_H */
