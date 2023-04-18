#include "common.h"

#if MPACK_CUSTOM_ASSERT == 1

#include <errno.h>

void __dpack_nonull(1) __noreturn __leaf
mpack_assert_fail(const char * __restrict message)
{
	dpack_assert_intern(message);
	dpack_assert_intern(message[0]);

	/* For skipping first characters of mpack assertion message. */
	size_t start = sizeof("mpack assertion failed at ") - 1;
	char * msg;

	msg = strdup(&message[start]);
	if (msg) {
		char * chr;

		for (chr = strchr(msg, '\n'); chr; chr = strchr(chr + 1, '\n'))
			*chr = ':';
	}
	else
		msg = ":??";

	stroll_assert_fail_msg("dpack:mpack", msg);

	unreachable();
}

#endif /* defined(MPACK_CUSTOM_ASSERT) */
