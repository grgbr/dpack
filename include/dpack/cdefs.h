#ifndef _DPACK_CDEFS_H
#define _DPACK_CDEFS_H

#include <assert.h>

#define dpack_assert(cond) \
	assert(cond)

#define dpack_sizeof_member(_type, _member) \
	(sizeof(((_type *)0)->_member))

#endif /* _DPACK_CDEFS_H */
