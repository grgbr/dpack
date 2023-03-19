#include "common.h"

#if MPACK_CUSTOM_ASSERT == 1

#include <errno.h>

void
mpack_assert_fail(const char * message)
{
	fprintf(stderr,
	        "%s: mpack: %s\n",
	        program_invocation_short_name,
	        message);
}

#endif /* defined(MPACK_CUSTOM_ASSERT) */
