/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Array encoding / decoding interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      22 Feb 2023
 * @copyright Copyright (C) 2023-2024 Grégor Boirie <gregor.boirie@free.fr>
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _DPACK_ARRAY_H
#define _DPACK_ARRAY_H

#include <dpack/codec.h>

/**
 * Maximum number of elements of a dpack array
 */
#define DPACK_ARRAY_ELMNR_MAX (1024U)

/**
 * Maximum size of a dpack array element
 */
#define DPACK_ARRAY_ELMSZ_MAX (1024U)

#if defined(CONFIG_DPACK_SCALAR)

#include <dpack/scalar.h>

#if DPACK_STDINT_SIZE_MAX > DPACK_ARRAY_ELMSZ_MAX
#error DPack array element cannot hold a single scalar element, \
       increase DPACK_ARRAY_ELMSZ_MAX !
#endif /* DPACK_STDINT_SIZE_MAX > DPACK_ARRAY_ELMSZ_MAX */

#if defined(CONFIG_DPACK_FLOAT)

#if DPACK_FLOAT_SIZE > DPACK_ARRAY_ELMSZ_MAX
#error DPack array element cannot hold a single float element, \
       increase DPACK_ARRAY_ELMSZ_MAX !
#endif /* DPACK_FLOAT_SIZE > DPACK_ARRAY_ELMSZ_MAX */

#endif /* defined(CONFIG_DPACK_FLOAT) */

#if defined(CONFIG_DPACK_DOUBLE)

#if DPACK_DOUBLE_SIZE > DPACK_ARRAY_ELMSZ_MAX
#error DPack array element cannot hold a single double element, \
       increase DPACK_ARRAY_ELMSZ_MAX !
#endif /* DPACK_DOUBLE_SIZE > DPACK_ARRAY_ELMSZ_MAX */

#endif /* defined(CONFIG_DPACK_DOUBLE) */

#endif /* defined(CONFIG_DPACK_SCALAR) */

#if defined(CONFIG_DPACK_STRING)

#include <dpack/string.h>

#if _DPACK_STRSZ_MAX_MIN > DPACK_ARRAY_ELMSZ_MAX
#error DPack array element cannot hold a single string element, \
       increase DPACK_ARRAY_ELMSZ_MAX !
#endif /* _DPACK_STRSZ_MAX_MIN > DPACK_ARRAY_ELMSZ_MAX */

#endif /* defined(CONFIG_DPACK_STRING) */

#if defined(CONFIG_DPACK_BIN)

#include <dpack/bin.h>

#if _DPACK_BINSZ_MAX_MIN > DPACK_ARRAY_ELMSZ_MAX
#error DPack array element cannot hold a single bin element, \
       increase DPACK_ARRAY_ELMSZ_MAX !
#endif /* _DPACK_BINSZ_MAX_MIN > DPACK_ARRAY_ELMSZ_MAX */

#endif /* defined(CONFIG_DPACK_BIN) */

/* Maximum number of elements an msgpack fixarray may encode */
#define _DPACK_FIXARRAY_ELMNR_MAX (15U)
/* Maximum number of elements a 16 bits msgpack array may encode */
#define _DPACK_ARRAY16_ELMNR_MAX  UINT16_MAX
/* Maximum number of elements a 32 bits msgpack array may encode */
#define _DPACK_ARRAY32_ELMNR_MAX  UINT32_MAX

/* Check DPACK_ARRAY_ELMNR_MAX definition is sensible. */
#if DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY32_ELMNR_MAX
#error MsgPack cannot encode arrays containing more that UINT32_MAX elements !
#elif DPACK_ARRAY_ELMNR_MAX < 4U
#error Huh ?!
#endif

/******************************************************************************
 * Msgpack fixarray definitions
 ******************************************************************************/

#define __DPACK_ARRAY_HEAD_SIZE(_elm_nr) \
	MPACK_TAG_SIZE_FIXARRAY

/******************************************************************************
 * Msgpack 16 bits array definitions
 ******************************************************************************/

#if DPACK_ARRAY_ELMNR_MAX > _DPACK_FIXARRAY_ELMNR_MAX

#define _DPACK_ARRAY16_HEAD_SIZE(_elm_nr) \
	(((_elm_nr) > _DPACK_FIXARRAY_ELMNR_MAX) ? \
	 MPACK_TAG_SIZE_ARRAY16 : \
	 MPACK_TAG_SIZE_FIXARRAY)

#undef __DPACK_ARRAY_HEAD_SIZE
#define __DPACK_ARRAY_HEAD_SIZE(_elm_nr) \
	_DPACK_ARRAY16_HEAD_SIZE(_elm_nr)

#endif /* DPACK_ARRAY_ELMNR_MAX > _DPACK_FIXARRAY_ELMNR_MAX */

/******************************************************************************
 * Msgpack 32 bits array definitions
 ******************************************************************************/

#if DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY16_ELMNR_MAX

#define _DPACK_ARRAY32_HEAD_SIZE(_elm_nr) \
	(((_elm_nr) > _DPACK_ARRAY16_ELMNR_MAX) ? \
	 MPACK_TAG_SIZE_ARRAY32 : \
	 _DPACK_ARRAY16_HEAD_SIZE(_elm_nr))

#undef __DPACK_ARRAY_HEAD_SIZE
#define __DPACK_ARRAY_HEAD_SIZE(_elm_nr) \
	_DPACK_ARRAY32_HEAD_SIZE(_elm_nr)

#endif /* DPACK_ARRAY_ELMNR_MAX > _DPACK_ARRAY16_ELMNR_MAX */

/******************************************************************************
 * Top-level array size definitions
 ******************************************************************************/

/**
 * Maximum size of a dpack array data block
 */
#define DPACK_ARRAY_DATA_SIZE_MAX \
	(DPACK_ARRAY_ELMNR_MAX * DPACK_ARRAY_ELMSZ_MAX)

/**
 * Maximum size of a dpack array
 */
#define DPACK_ARRAY_SIZE_MAX \
	(__DPACK_ARRAY_HEAD_SIZE(DPACK_ARRAY_ELMNR_MAX) + \
	 DPACK_ARRAY_DATA_SIZE_MAX)

#if DPACK_ARRAY_SIZE_MAX > (128U * 1024 * 1024)
#error DPack cannot encode arrays which overall size > 128 MB !
#endif /* DPACK_ARRAY_SIZE_MAX > (128U * 1024 * 1024) */

#define _DPACK_ARRAY_HEAD_SIZE(_elm_nr) \
	compile_eval(((_elm_nr) > 0) && \
	             ((_elm_nr) <= DPACK_ARRAY_ELMNR_MAX), \
	             __DPACK_ARRAY_HEAD_SIZE(_elm_nr), \
	             "invalid constant array length")

/**
 * Size of a serialized array header.
 *
 * @param[in] _elm_nr number of array elements
 *
 * Given the number of elements an msgpack array may contain, compute the size
 * of its header when encoded according to the
 * @rstsubst{MessagePack array format}.
 *
 * @warning
 * Argument value MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - DPACK_ARRAY_FIXED_SIZE
 * - DPACK_ARRAY_MIXED_SIZE
 */
#define DPACK_ARRAY_HEAD_SIZE(_elm_nr) \
	compile_eval(__builtin_constant_p(_elm_nr), \
	             _DPACK_ARRAY_HEAD_SIZE(_elm_nr), \
	             "constant array length expected")

#define _DPACK_ARRAY_MIXED_SIZE(_elm_nr, _data_size) \
	compile_eval(((_elm_nr) > 0) && \
	             ((_elm_nr) <= DPACK_ARRAY_ELMNR_MAX) && \
	             ((_data_size) > 0) && \
	             ((_data_size) <= DPACK_ARRAY_DATA_SIZE_MAX), \
	             (__DPACK_ARRAY_HEAD_SIZE(_elm_nr) + (_data_size)), \
	             "invalid array length or data size")

/**
 * Size of a serialized array.
 *
 * @param[in] _elm_nr    number of array elements
 * @param[in] _data_size size of encoded array data
 *
 * Compute the size of an array encoded according to the
 * @rstsubst{MessagePack array format} that holds @p _elm_nr elements and where
 * the cumulated size of all encoded elements is given by @p _data_size.
 *
 * Use #DPACK_ARRAY_MIXED_SIZE when encoding / decoding an array which holds
 * elements of multiple sizes. Use #DPACK_ARRAY_FIXED_SIZE otherwise.
 *
 * @warning
 * Arguments values MUST be known at compile time, i.e., constant. This triggers
 * a compile time error otherwise.
 *
 * @see
 * - DPACK_ARRAY_FIXED_SIZE
 * - DPACK_ARRAY_HEAD_SIZE
 * - dpack_array_mixed_size()
 */
#define DPACK_ARRAY_MIXED_SIZE(_elm_nr, _data_size) \
	compile_eval(__builtin_constant_p(_elm_nr) && \
	             __builtin_constant_p(_data_size), \
	             _DPACK_ARRAY_MIXED_SIZE(_elm_nr, _data_size), \
	             "constant array length and data size expected")

/**
 * Size of a serialized array.
 *
 * @param[in] _elm_nr   number of array elements
 * @param[in] _elm_size size of a single encoded array element
 *
 * Compute the size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr elements of @p
 * _elm_size bytes each.
 *
 * Use #DPACK_ARRAY_FIXED_SIZE when encoding / decoding an array which holds
 * elements of the same size. Use #DPACK_ARRAY_MIXED_SIZE otherwise.
 *
 * @warning
 * Arguments values MUST be known at compile time, i.e., constant. This triggers
 * a compile time error otherwise.
 *
 * @see
 * - DPACK_ARRAY_MIXED_SIZE
 * - DPACK_ARRAY_HEAD_SIZE
 * - dpack_array_fixed_size()
 */
#define DPACK_ARRAY_FIXED_SIZE(_elm_nr, _elm_size) \
	DPACK_ARRAY_MIXED_SIZE(_elm_nr, (_elm_nr) * (_elm_size))

#if defined(CONFIG_DPACK_SCALAR)

/**
 * Size of a serialized array of boolean elements
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr boolean elements.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_BOOL_SIZE
 */
#define DPACK_ARRAY_BOOL_SIZE(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_BOOL_SIZE)

/**
 * Minimum size of a serialized array of 8-bits signed integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the minimum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 8-bits signed
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_INT8_SIZE_MAX
 * - DPACK_INT8_SIZE_MIN
 */
#define DPACK_ARRAY_INT8_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_INT8_SIZE_MIN)

/**
 * Maximum size of a serialized array of 8-bits signed integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the maximum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 8-bits signed
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_INT8_SIZE_MIN
 * - DPACK_INT8_SIZE_MAX
 */
#define DPACK_ARRAY_INT8_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_INT8_SIZE_MAX)

/**
 * Minimum size of a serialized array of 8-bits unsigned integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the minimum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 8-bits unsigned
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_UINT8_SIZE_MAX
 * - DPACK_UINT8_SIZE_MIN
 */
#define DPACK_ARRAY_UINT8_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_UINT8_SIZE_MIN)

/**
 * Maximum size of a serialized array of 8-bits unsigned integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the maximum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 8-bits unsigned
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_UINT8_SIZE_MIN
 * - DPACK_UINT8_SIZE_MAX
 */
#define DPACK_ARRAY_UINT8_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_UINT8_SIZE_MAX)

/**
 * Minimum size of a serialized array of 16-bits signed integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the minimum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 16-bits signed
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_INT16_SIZE_MAX
 * - DPACK_INT16_SIZE_MIN
 */
#define DPACK_ARRAY_INT16_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_INT16_SIZE_MIN)

/**
 * Maximum size of a serialized array of 16-bits signed integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the maximum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 16-bits signed
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_INT16_SIZE_MIN
 * - DPACK_INT16_SIZE_MAX
 */
#define DPACK_ARRAY_INT16_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_INT16_SIZE_MAX)

/**
 * Minimum size of a serialized array of 16-bits unsigned integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the minimum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 16-bits unsigned
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_UINT16_SIZE_MAX
 * - DPACK_UINT16_SIZE_MIN
 */
#define DPACK_ARRAY_UINT16_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_UINT16_SIZE_MIN)

/**
 * Maximum size of a serialized array of 16-bits unsigned integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the maximum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 16-bits unsigned
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_UINT16_SIZE_MIN
 * - DPACK_UINT16_SIZE_MAX
 */
#define DPACK_ARRAY_UINT16_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_UINT16_SIZE_MAX)

/**
 * Minimum size of a serialized array of 32-bits signed integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the minimum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 32-bits signed
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_INT32_SIZE_MAX
 * - DPACK_INT32_SIZE_MIN
 */
#define DPACK_ARRAY_INT32_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_INT32_SIZE_MIN)

/**
 * Maximum size of a serialized array of 32-bits signed integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the maximum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 32-bits signed
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_INT32_SIZE_MIN
 * - DPACK_INT32_SIZE_MAX
 */
#define DPACK_ARRAY_INT32_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_INT32_SIZE_MAX)

/**
 * Minimum size of a serialized array of 32-bits unsigned integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the minimum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 32-bits unsigned
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_UINT32_SIZE_MAX
 * - DPACK_UINT32_SIZE_MIN
 */
#define DPACK_ARRAY_UINT32_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_UINT32_SIZE_MIN)

/**
 * Maximum size of a serialized array of 32-bits unsigned integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the maximum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 32-bits unsigned
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_UINT32_SIZE_MIN
 * - DPACK_UINT32_SIZE_MAX
 */
#define DPACK_ARRAY_UINT32_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_UINT32_SIZE_MAX)

/**
 * Minimum size of a serialized array of 64-bits signed integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the minimum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 64-bits signed
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_INT64_SIZE_MAX
 * - DPACK_INT64_SIZE_MIN
 */
#define DPACK_ARRAY_INT64_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_INT64_SIZE_MIN)

/**
 * Maximum size of a serialized array of 64-bits signed integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the maximum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 64-bits signed
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_INT64_SIZE_MIN
 * - DPACK_INT64_SIZE_MAX
 */
#define DPACK_ARRAY_INT64_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_INT64_SIZE_MAX)

/**
 * Minimum size of a serialized array of 64-bits unsigned integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the minimum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 64-bits unsigned
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_UINT64_SIZE_MAX
 * - DPACK_UINT64_SIZE_MIN
 */
#define DPACK_ARRAY_UINT64_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_UINT64_SIZE_MIN)

/**
 * Maximum size of a serialized array of 64-bits unsigned integers
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the maximum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr 64-bits unsigned
 * integers.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_UINT64_SIZE_MIN
 * - DPACK_UINT64_SIZE_MAX
 */
#define DPACK_ARRAY_UINT64_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_UINT64_SIZE_MAX)

#if defined(CONFIG_DPACK_FLOAT)

/**
 * Size of a serialized array of single precision floating point number elements
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr single precision
 * floating point number elements.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_FLOAT_SIZE
 */
#define DPACK_ARRAY_FLOAT_SIZE(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_FLOAT_SIZE)

#endif /* defined(CONFIG_DPACK_FLOAT) */

#if defined(CONFIG_DPACK_DOUBLE)

/**
 * Size of a serialized array of double precision floating point number elements
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr double precision
 * floating point number elements.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_DOUBLE_SIZE
 */
#define DPACK_ARRAY_DOUBLE_SIZE(_elm_nr) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_DOUBLE_SIZE)

#endif /* defined(CONFIG_DPACK_DOUBLE) */

#endif /* defined(CONFIG_DPACK_SCALAR) */

#if defined(CONFIG_DPACK_STRING)

/**
 * Size of a serialized array of string elements
 *
 * @param[in] _elm_nr number of array elements
 * @param[in] _len    length of a single string element
 *
 * Compute the size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr string elements of
 * @p _len length each (excluding the terminating ``NULL`` byte).
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_STR_SIZE_MIN
 * - DPACK_ARRAY_STR_SIZE_MAX
 * - DPACK_STR_SIZE
 */
#define DPACK_ARRAY_STR_SIZE(_elm_nr, _len) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_STR_SIZE(_len))

/**
 * Minimum size of a serialized array of strings
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the minimum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr string elements.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_STR_SIZE_MAX
 * - DPACK_ARRAY_STR_SIZE
 */
#define DPACK_ARRAY_STR_SIZE_MIN(_elm_nr) \
        DPACK_ARRAY_STR_SIZE(_elm_nr, 1)

/**
 * Maximum size of a serialized array of strings
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the maximum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr string elements.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_STR_SIZE_MIN
 * - DPACK_ARRAY_STR_SIZE
 * - DPACK_STRLEN_MAX
 */
#define DPACK_ARRAY_STR_SIZE_MAX(_elm_nr) \
        DPACK_ARRAY_STR_SIZE(_elm_nr, DPACK_STRLEN_MAX)

#endif /* defined(CONFIG_DPACK_STRING) */

#if defined(CONFIG_DPACK_BIN)

/**
 * Size of a serialized array of bin elements
 *
 * @param[in] _elm_nr number of array elements
 * @param[in] _sz     size of a single bin element
 *
 * Compute the size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr bin elements of
 * @p _sz bytes each.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_BIN_SIZE_MIN
 * - DPACK_ARRAY_BIN_SIZE_MAX
 * - DPACK_BIN_SIZE
 */
#define DPACK_ARRAY_BIN_SIZE(_elm_nr, _sz) \
	DPACK_ARRAY_FIXED_SIZE(_elm_nr, DPACK_BIN_SIZE(_sz))

/**
 * Minimum size of a serialized array of bins
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the minimum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr bin elements.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_BIN_SIZE_MAX
 * - DPACK_ARRAY_BIN_SIZE
 */
#define DPACK_ARRAY_BIN_SIZE_MIN(_elm_nr) \
        DPACK_ARRAY_BIN_SIZE(_elm_nr, 1)

/**
 * Maximum size of a serialized array of bins
 *
 * @param[in] _elm_nr number of array elements
 *
 * Compute the maximum size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p _elm_nr bin elements.
 *
 * @warning
 * @p _elm_nr MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - dpack_array_fixed_size()
 * - DPACK_ARRAY_BIN_SIZE_MIN
 * - DPACK_ARRAY_BIN_SIZE
 * - DPACK_BINSZ_MAX
 */
#define DPACK_ARRAY_BIN_SIZE_MAX(_elm_nr) \
        DPACK_ARRAY_BIN_SIZE(_elm_nr, DPACK_BINSZ_MAX)

#endif /* defined(CONFIG_DPACK_BIN) */

/**
 * Size of a serialized array.
 *
 * @param[in] elm_nr    number of array elements
 * @param[in] data_size size of encoded array data
 *
 * @return size of array
 *
 * Compute the size of an array encoded according to the
 * @rstsubst{MessagePack array format} that holds @p elm_nr elements and where
 * the cumulated size of all encoded elements is given by @p data_size.
 *
 * @note
 * Use this function when @p elm_nr and / or @p data_size are not known at
 * compile time. Use #DPACK_ARRAY_MIXED_SIZE otherwise.
 *
 * @see
 * - DPACK_ARRAY_MIXED_SIZE
 * - dpack_array_fixed_size()
 */
extern size_t
dpack_array_mixed_size(unsigned int elm_nr, size_t data_size)
	__dpack_const __dpack_nothrow __leaf __warn_result __dpack_export;

/**
 * Size of a serialized array.
 *
 * @param[in] elm_nr   number of array elements
 * @param[in] elm_size size of a single encoded array element
 *
 * @return size of array
 *
 * Compute the size of an array encoded according to the
 * @rstsubst{MessagePack array format} and holding @p elm_nr elements of @p
 * elm_size bytes each.
 *
 * @note
 * Use this function when @p elm_nr and / or @p elm_size are not known at
 * compile time. Use #DPACK_ARRAY_FIXED_SIZE otherwise.
 *
 * @see
 * - DPACK_ARRAY_FIXED_SIZE
 * - dpack_array_mixed_size()
 */
static inline size_t __dpack_const __dpack_nothrow __warn_result
dpack_array_fixed_size(unsigned int elm_nr, size_t elm_size)
{
	return dpack_array_mixed_size(elm_nr, elm_nr * elm_size);
}

/******************************************************************************
 * Array encoding
 ******************************************************************************/

/**
 * Start encoding an array.
 *
 * @param[inout] encoder encoder
 * @param[in]    nr      number of array elements
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Start encoding an array containing @p nr elements. Call to this function must
 * be followed by @p nr elements. Array encoding must be completed by a call to
 * dpack_array_end_encode().
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p nr is zero or greater than #DPACK_ARRAY_ELMNR_MAX, result is undefined.
 *   An assertion is triggered otherwise.
 *
 * @see
 * - dpack_array_end_encode()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_array_begin_encode(struct dpack_encoder * encoder,
                         unsigned int           nr)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

/**
 * Complete encoding of an array.
 *
 * @param[inout] encoder encoder
 *
 * Complete encoding of an array started using dpack_array_begin_encode().
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   a call to dpack_array_end_encode() has not been preceded by the expected
 *   number of elements, the result of data encoding is undefined. An assertion
 *   is triggered otherwise.
 *
 * @see
 * - dpack_array_begin_encode()
 * - dpack_encoder_init_buffer()
 */
extern void
dpack_array_end_encode(struct dpack_encoder * encoder)
	__dpack_nonull(1) __dpack_nothrow __leaf __dpack_export;

/******************************************************************************
 * Array decoding
 ******************************************************************************/

/**
 * Decode an array encoded according to the MessagePack format
 *
 * @param[inout] decoder decoder
 * @param[in]    decode  array element decoding callback
 * @param[inout] data    arbitrary location to client data
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type or range
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 * @retval < 0       @p decode() return value
 *
 * Decode / unpack / deserialize an array encoded according to the
 * @rstsubst{MessagePack array format} from buffer assigned to @p decoder at
 * initialization time.
 *
 * For each array item, dpack_array_decode() calls the @p decode() callback
 * function as described in #dpack_decode_item_fn. More specifically, the
 * @p decode() callback shall be given :
 * - @p decoder as the @p decode() @p decoder argument,
 * - the index, starting from zero, of the next item within the array being
 *   decoded as the @p decode() @p id argument,
 * - @p data as the @p decode() @p data argument.
 *
 * The optional @p data argument is meant for applicative purpose only. It
 * should point to an arbitrary location owned by the caller of
 * dpack_array_decode() and shall be passed as-is to @p decode().
 *
 * Finally, @p decode() should **return** ``0`` when successfully decoding an
 * item. A *negative error* code instructs dpack_array_decode() to
 * interrupt the current array decoding process and to use it as return value.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p decoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern int
dpack_array_decode(struct dpack_decoder * decoder,
                   dpack_decode_item_fn * decode,
                   void                 * data) __dpack_nonull(1, 2)
                                                __warn_result
                                                __dpack_export;

extern int
dpack_array_decode_equ(struct dpack_decoder * decoder,
                       unsigned int           nr,
                       dpack_decode_item_fn * decode,
                       void                 * data) __dpack_nonull(1, 3)
                                                    __warn_result
                                                    __dpack_export;

extern int
dpack_array_decode_min(struct dpack_decoder * decoder,
                       unsigned int           min_nr,
                       dpack_decode_item_fn * decode,
                       void                 * data) __dpack_nonull(1, 3)
                                                    __warn_result
                                                    __dpack_export;

extern int
dpack_array_decode_max(struct dpack_decoder * decoder,
                       unsigned int           max_nr,
                       dpack_decode_item_fn * decode,
                       void                 * data) __dpack_nonull(1, 3)
                                                    __warn_result
                                                    __dpack_export;

extern int
dpack_array_decode_range(struct dpack_decoder * decoder,
                         unsigned int           min_nr,
                         unsigned int           max_nr,
                         dpack_decode_item_fn * decode,
                         void                 * data) __dpack_nonull(1, 4)
                                                      __warn_result
                                                      __dpack_export;

#endif /* _DPACK_ARRAY_H */
