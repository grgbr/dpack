#ifndef _DPACK_CDEFS_H
#define _DPACK_CDEFS_H

#include <dpack/config.h>
#include <stroll/assert.h>

#define __dpack_export \
	__export_public

#if defined(DPACK_ASSERT_API)

#define dpack_assert_api(_cond) \
	stroll_assert("dpack", _cond)

#else  /* !defined(DPACK_ASSERT_API) */

#define dpack_assert_api(_cond)

#endif /* defined(DPACK_ASSERT_API) */

#endif /* _DPACK_CDEFS_H */
