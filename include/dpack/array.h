/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Array encoding / decoding interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      22 Feb 2023
 * @copyright Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _DPACK_ARRAY_H
#define _DPACK_ARRAY_H

#include <dpack/codec.h>
#include <dpack/scalar.h>
#include <dpack/string.h>
#include <dpack/bin.h>

/* Maximum number of elements of a dpack array */
#define DPACK_ARRAY_ELMNR_MAX    (1024U)
/* Maximum size of a dpack array */
#define DPACK_ARRAY_SIZE_MAX     (64U * STROLL_CONST_MAX(DPACK_STRLEN_MAX, \
                                                         DPACK_BINSZ_MAX))

/* Maximum number of elements an msgpack fixarray may encode */
#define DPACK_FIXARRAY_ELMNR_MAX (15U)
/* Maximum number of elements a 16 bits msgpack array may encode */
#define DPACK_ARRAY16_ELMNR_MAX  UINT16_MAX
/* Maximum number of elements a 32 bits msgpack array may encode */
#define DPACK_ARRAY32_ELMNR_MAX  UINT32_MAX

/* Check DPACK_ARRAY_ELMNR_MAX definition is sensible. */
#if DPACK_ARRAY_ELMNR_MAX > DPACK_ARRAY32_ELMNR_MAX
#error msgpack cannot encode arrays containing more that UINT32_MAX elements !
#elif DPACK_ARRAY_ELMNR_MAX < 4U
#error Huh ?!
#endif

/******************************************************************************
 * Msgpack fixarray definitions
 ******************************************************************************/

/* Compute size of an encoded msgpack fixarray */
#define DPACK_FIXARRAY_SIZE(_elm_size, _elm_nr) \
	(MPACK_TAG_SIZE_FIXARRAY + ((_elm_size) * (_elm_nr)))
/* Compute the maximum size of the data portion of an encoded msgpack array. */
#define DPACK_ARRAY_DATA_SIZE_MAX \
	(DPACK_ARRAY_SIZE_MAX - MPACK_TAG_SIZE_FIXARRAY)

#define _DPACK_ARRAY_CONST_SIZE(_elm_size, _elm_nr) \
	DPACK_FIXARRAY_SIZE(_elm_size, _elm_nr)

/******************************************************************************
 * Msgpack 16 bits array definitions
 ******************************************************************************/

#if DPACK_ARRAY_ELMNR_MAX > DPACK_FIXARRAY_ELMNR_MAX

/* Compute size of an encoded 16 bits msgpack array */
#define DPACK_ARRAY16_SIZE(_elm_size, _elm_nr) \
	(MPACK_TAG_SIZE_ARRAY16 + ((_elm_size) * (_elm_nr)))
/* Maximum size of the data portion of an encoded 16 bits msgpack array. */
#undef  DPACK_ARRAY_DATA_SIZE_MAX
#define DPACK_ARRAY_DATA_SIZE_MAX \
	(DPACK_ARRAY_SIZE_MAX - MPACK_TAG_SIZE_ARRAY16)

/*
 * Size of an encoded array when maximum number of elements fits into a 16 bits
 * msgpack array.
 */
#define DPACK_ARRAY16_CONST_SIZE(_elm_size, _elm_nr) \
	(((_elm_nr) > DPACK_FIXARRAY_ELMNR_MAX) ? \
	 DPACK_ARRAY16_SIZE(_elm_size, _elm_nr) : \
	 DPACK_FIXARRAY_SIZE(_elm_size, _elm_nr))

#undef _DPACK_ARRAY_CONST_SIZE
#define _DPACK_ARRAY_CONST_SIZE(_elm_size, _elm_nr) \
	DPACK_ARRAY16_CONST_SIZE(_elm_size, _elm_nr)

#endif /* DPACK_ARRAY_ELMNR_MAX > DPACK_FIXARRAY_ELMNR_MAX */

/******************************************************************************
 * Msgpack 32 bits array definitions
 ******************************************************************************/

#if DPACK_ARRAY_ELMNR_MAX > DPACK_ARRAY16_ELMNR_MAX

/* Compute size of an encoded 32 bits msgpack array */
#define DPACK_ARRAY32_SIZE(_elm_size, _elm_nr) \
	(MPACK_TAG_SIZE_ARRAY32 + ((_elm_size) * (_elm_nr)))
/* Maximum size of the data portion of an encoded 32 bits msgpack array. */
#undef  DPACK_ARRAY_DATA_SIZE_MAX
#define DPACK_ARRAY_DATA_SIZE_MAX \
	(DPACK_ARRAY_SIZE_MAX - MPACK_TAG_SIZE_ARRAY32)

/*
 * Size of an encoded array when maximum number of elements fits into a 32 bits
 * msgpack array.
 */
#define DPACK_ARRAY32_CONST_SIZE(_elm_size, _elm_nr) \
	(((_elm_nr) > DPACK_ARRAY16_ELMNR_MAX) ? \
	 DPACK_ARRAY32_SIZE(_elm_size, _elm_nr) : \
	 DPACK_ARRAY16_CONST_SIZE(_elm_size, _elm_nr))

#undef _DPACK_ARRAY_CONST_SIZE
#define _DPACK_ARRAY_CONST_SIZE(_elm_size, _elm_nr) \
	DPACK_ARRAY32_CONST_SIZE(_elm_size, _elm_nr) \

#endif /* DPACK_ARRAY_ELMNR_MAX > DPACK_ARRAY16_ELMNR_MAX */

/*
 * Check DPACK_ARRAY_ELMNR_MAX and DPACK_ARRAY_SIZE_MAX are consistent.
 *
 * Ensure that given the DPACK_ARRAY_SIZE_MAX set above, an array may hold
 * DPACK_ARRAY_ELMNR_MAX number of elements of at least 1 byte size each.
 */
#if (1U * DPACK_ARRAY_ELMNR_MAX) > DPACK_ARRAY_DATA_SIZE_MAX
#error Array cannot hold elements which size > 0
#endif

/******************************************************************************
 * Top-level array size definitions
 ******************************************************************************/

/*
 * Maximum size of an array element.
 * Set this to an arbitrary limit of one half of the maximum size of the data
 * portion of an encoded msgpack array.
 */
#define DPACK_ARRAY_ELMSIZE_MAX \
	(DPACK_ARRAY_DATA_SIZE_MAX / 2U)

/*
 * Given the size of an element and the number of elements that an array may
 * contain, compute the size of the corresponding encoded msgpack array.
 *
 * Argument values MUST be known at compile time, i.e., constant. Result is
 * unpredictable otherwise.
 */
#define DPACK_ARRAY_CONST_SIZE(_elm_size, _elm_nr) \
	compile_eval(((_elm_size) > 0) && \
	             ((_elm_size) < DPACK_ARRAY_ELMSIZE_MAX) && \
	             ((_elm_nr) > 0) && \
	             ((_elm_nr) <= DPACK_ARRAY_ELMNR_MAX) && \
	             (((_elm_size) * (_elm_nr)) <= DPACK_ARRAY_SIZE_MAX), \
	             _DPACK_ARRAY_CONST_SIZE(_elm_size, _elm_nr), \
	             "invalid constant size of array element or length")

/*
 * Given the size of an element and the number of elements that an array may
 * contain, compute the size of the corresponding encoded msgpack array.
 *
 * Argument values MUST be known at compile time, i.e., constant. Trigger a
 * compile time error otherwise.
 */
#define DPACK_ARRAY_SIZE(_elm_size, _elm_nr) \
	compile_eval(__builtin_constant_p((_elm_size) * (_elm_nr)), \
	             DPACK_ARRAY_CONST_SIZE(_elm_size, _elm_nr), \
	             "constant size of array element and length expected")

#define DPACK_ARRAY_BOOL_SIZE(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_BOOL_SIZE, _elm_nr)

#define DPACK_ARRAY_INT8_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_INT8_SIZE_MIN, _elm_nr)
#define DPACK_ARRAY_INT8_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_INT8_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_UINT8_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT8_SIZE_MIN, _elm_nr)
#define DPACK_ARRAY_UINT8_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT8_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_INT16_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_INT16_SIZE_MIN, _elm_nr)
#define DPACK_ARRAY_INT16_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_INT16_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_UINT16_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT16_SIZE_MIN, _elm_nr)
#define DPACK_ARRAY_UINT16_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT16_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_INT32_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_INT32_SIZE_MIN, _elm_nr)
#define DPACK_ARRAY_INT32_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_INT32_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_UINT32_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT32_SIZE_MIN, _elm_nr)
#define DPACK_ARRAY_UINT32_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT32_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_INT64_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_INT64_SIZE_MIN, _elm_nr)
#define DPACK_ARRAY_INT64_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_INT64_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_UINT64_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT64_SIZE_MIN, _elm_nr)
#define DPACK_ARRAY_UINT64_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT64_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_FLOAT_SIZE(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_FLOAT_SIZE, _elm_nr)

#define DPACK_ARRAY_DOUBLE_SIZE(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_DOUBLE_SIZE, _elm_nr)

#define DPACK_ARRAY_STR_SIZE(_len, _elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_STR_SIZE(_len), _elm_nr)
#define DPACK_ARRAY_STR_SIZE_MIN(_elm_nr) \
        DPACK_ARRAY_STR_SIZE(1, _elm_nr)
#define DPACK_ARRAY_STR_SIZE_MAX(_elm_nr) \
        DPACK_ARRAY_STR_SIZE(DPACK_STRLEN_MAX, _elm_nr)

#define DPACK_ARRAY_BIN_SIZE(_sz, _elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_BIN_SIZE(_sz), _elm_nr)
#define DPACK_ARRAY_BIN_SIZE_MIN(_elm_nr) \
        DPACK_ARRAY_BIN_SIZE(1, _elm_nr)
#define DPACK_ARRAY_BIN_SIZE_MAX(_elm_nr) \
        DPACK_ARRAY_BIN_SIZE(DPACK_BINSZ_MAX, _elm_nr)

extern size_t
dpack_array_size(size_t elm_size, unsigned int elm_nr) __dpack_export;

/******************************************************************************
 * Array encoding
 ******************************************************************************/

extern int
dpack_array_begin_encode(struct dpack_encoder * encoder,
                         unsigned int           nr)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

extern void
dpack_array_end_encode(struct dpack_encoder * encoder)
	__dpack_nonull(1) __dpack_nothrow __leaf __dpack_export;

/******************************************************************************
 * Array decoding
 ******************************************************************************/

extern int
dpack_array_decode(struct dpack_decoder * decoder,
                   dpack_decode_item_fn * decode,
                   void                 * data) __dpack_export;

extern int
dpack_array_decode_equ(struct dpack_decoder * decoder,
                       unsigned int           nr,
                       dpack_decode_item_fn * decode,
                       void                 * data) __dpack_export;

extern int
dpack_array_decode_min(struct dpack_decoder * decoder,
                     unsigned int           min_nr,
                     dpack_decode_item_fn * decode,
                     void                 * data) __dpack_export;

extern int
dpack_array_decode_max(struct dpack_decoder * decoder,
                       unsigned int           max_nr,
                       dpack_decode_item_fn * decode,
                       void                 * data) __dpack_export;

extern int
dpack_array_decode_range(struct dpack_decoder * decoder,
                         unsigned int           min_nr,
                         unsigned int           max_nr,
                         dpack_decode_item_fn * decode,
                         void                 * data) __dpack_export;

#endif /* _DPACK_ARRAY_H */
