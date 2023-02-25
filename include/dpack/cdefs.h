#ifndef _DPACK_CDEFS_H
#define _DPACK_CDEFS_H

#include <assert.h>

#define __dpack_export \
	__attribute__((visibility("default")))

#define dpack_assert(cond) \
	assert(cond)

#define dpack_sizeof_member(_type, _member) \
	(sizeof(((_type *)0)->_member))

#endif /* _DPACK_CDEFS_H */
