/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Common definitions
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      22 Feb 2023
 * @copyright Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _DPACK_CDEFS_H
#define _DPACK_CDEFS_H

#include <dpack/config.h>
#include <stroll/cdefs.h>

#define __dpack_export  __export_public

#if defined(CONFIG_DPACK_ASSERT_API) || defined(CONFIG_DPACK_ASSERT_INTERN)

#define __dpack_nonull(_arg_index, ...)
#define __dpack_const
#define __dpack_pure
#define __dpack_nothrow

#else   /* !(defined(CONFIG_DPACK_ASSERT_API) || \
             defined(CONFIG_DPACK_ASSERT_INTERN)) */

#define __dpack_nonull(_arg_index, ...) \
	__nonull(_arg_index, ## __VA_ARGS__)

#define __dpack_const   __const
#define __dpack_pure    __pure
#define __dpack_nothrow __nothrow

#endif /* defined(CONFIG_DPACK_ASSERT_API) || \
          defined(CONFIG_DPACK_ASSERT_INTERN) */

#endif /* _DPACK_CDEFS_H */
