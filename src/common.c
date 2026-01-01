/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "common.h"

#if 0
#if MPACK_CUSTOM_ASSERT == 1

#include <errno.h>

static __thread char dpack_mpack_assert_msg[LINE_MAX];

void __dpack_nonull(1) __noreturn __leaf
mpack_assert_fail(const char * __restrict message)
{
	dpack_assert_intern(message);
	dpack_assert_intern(message[0]);

	/* For skipping first characters of mpack assertion message. */
	size_t start = sizeof("mpack assertion failed at ") - 1;
	size_t len;
	char * chr;

	len = strnlen(&message[start], LINE_MAX - 1);
	memcpy(dpack_mpack_assert_msg, &message[start], len);
	dpack_mpack_assert_msg[len] = '\0';

	for (chr = strchr(dpack_mpack_assert_msg, '\n');
	     chr;
	     chr = strchr(chr + 1, '\n'))
		*chr = ':';

	stroll_assert_fail_msg("dpack:mpack", dpack_mpack_assert_msg);

	unreachable();
}

#endif /* defined(MPACK_CUSTOM_ASSERT) */
#endif
