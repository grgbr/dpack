#ifndef _DPACK_COMMON_H
#define _DPACK_COMMON_H

#include <mpack.h>
#include <errno.h>
#include <assert.h>

#define unreachable() \
	__builtin_unreachable()

#define dpack_assert(cond) \
	assert(cond)

static inline int
dpack_errno_from_mpack(enum mpack_error_t err)
{
	switch (err) {
	case mpack_ok:
		return 0;
	case mpack_error_io:
		return -EIO;
	case mpack_error_invalid:
		return -EPROTO;
	case mpack_error_unsupported:
		return -ENOTSUP;
	case mpack_error_type:
		return -ENOMSG;
	case mpack_error_too_big:
		return -EMSGSIZE;
	case mpack_error_memory:
		return -ENOMEM;
	case mpack_error_bug:
		assert(0);
	case mpack_error_data:
		return -EBADMSG;
	case mpack_error_eof:
		return -ENODATA;
	default:
		dpack_assert(0);
	}

	unreachable();
}

#endif /* _DPACK_COMMON_H */
