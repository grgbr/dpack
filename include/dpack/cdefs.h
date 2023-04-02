#ifndef _DPACK_CDEFS_H
#define _DPACK_CDEFS_H

#include <dpack/config.h>
#include <stroll/assert.h>

#define __dpack_export \
	__export_public

#define dpack_assert(_cond) \
	stroll_assert("dpack", _cond)

#endif /* _DPACK_CDEFS_H */
