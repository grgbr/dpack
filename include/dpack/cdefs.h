#ifndef _DPACK_CDEFS_H
#define _DPACK_CDEFS_H

#include <dpack/config.h>
#include <stroll/cdefs.h>

#define __dpack_export \
	__export_public

#if defined(CONFIG_DPACK_ASSERT_API) || defined(CONFIG_DPACK_ASSERT_INTERN)

#define __dpack_nonull(_arg_index, ...)

#else   /* !(defined(CONFIG_DPACK_ASSERT_API) || \
             defined(CONFIG_DPACK_ASSERT_INTERN)) */

#define __dpack_nonull(_arg_index, ...) \
	__nonull(_arg_index, ## __VA_ARGS__)

#endif /* defined(CONFIG_DPACK_ASSERT_API) || \
          defined(CONFIG_DPACK_ASSERT_INTERN) */

#endif /* _DPACK_CDEFS_H */
