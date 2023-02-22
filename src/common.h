#ifndef _DPACK_COMMON_H
#define _DPACK_COMMON_H

#include "dpack/cdefs.h"
#include <mpack.h>
#include <errno.h>

#define unreachable() \
	__builtin_unreachable()

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
		dpack_assert(0);
	case mpack_error_data:
		return -EBADMSG;
	case mpack_error_eof:
		return -ENODATA;
	default:
		dpack_assert(0);
	}

	unreachable();
}

extern int
dpack_decode_tag(struct mpack_reader_t * reader,
                 enum mpack_type_t       type,
                 struct mpack_tag_t    * tag);

#endif /* _DPACK_COMMON_H */
