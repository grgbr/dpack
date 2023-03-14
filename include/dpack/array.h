#ifndef _DPACK_ARRAY_H
#define _DPACK_ARRAY_H

#include <dpack/cdefs.h>
#include <dpack/scalar.h>
#include <sys/types.h>

struct dpack_encoder;
struct dpack_decoder;

/* Maximum number of elements of a dpack array */
#define DPACK_ARRAY_ELMNR_MAX (1024U)
/* Maximum size of a dpack array */
#define DPACK_ARRAY_SIZE_MAX  ((size_t)4 * 1024U * 1024U)

/******************************************************************************
 * Msgpack fixarray definitions
 ******************************************************************************/

/* Maximum number of elements an msgpack fixarray may encode */
#define DPACK_FIXARRAY_ELMNR_MAX (15U)
/* Compute size of an encoded msgpack fixarray */
#define DPACK_FIXARRAY_SIZE(_elm_size, _elm_nr) \
	(1UL + ((_elm_size) * (_elm_nr)))
/* Compute the maximum size of the data portion of an encoded msgpack array. */
#define DPACK_ARRAY_DATA_SIZE_MAX \
	(DPACK_ARRAY_SIZE_MAX - 1UL)

#define _DPACK_ARRAY_CONST_SIZE(_elm_size, _elm_nr) \
	DPACK_FIXARRAY_SIZE(_elm_size, _elm_nr)

/******************************************************************************
 * Msgpack 16 bits array definitions
 ******************************************************************************/

#if DPACK_ARRAY_ELMNR_MAX > DPACK_FIXARRAY_ELMNR_MAX

/* Maximum number of elements a 16 bits msgpack array may encode */
#define DPACK_ARRAY16_ELMNR_MAX \
	(UINT16_MAX)
/* Compute size of an encoded 16 bits msgpack array */
#define DPACK_ARRAY16_SIZE(_elm_size, _elm_nr) \
	(3UL + ((_elm_size) * (_elm_nr)))
/* Maximum size of the data portion of an encoded 16 bits msgpack array. */
#undef  DPACK_ARRAY_DATA_SIZE_MAX
#define DPACK_ARRAY_DATA_SIZE_MAX \
	(DPACK_ARRAY_SIZE_MAX - 3UL)

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

/******************************************************************************
 * Msgpack 32 bits array definitions
 ******************************************************************************/

#elif DPACK_ARRAY_ELMNR_MAX > DPACK_ARRAY16_ELMNR_MAX

/* Maximum number of elements a 32 bits msgpack array may encode */
#define DPACK_ARRAY32_ELMNR_MAX                 (UINT32_MAX)
/* Compute size of an encoded 32 bits msgpack array */
#define DPACK_ARRAY32_SIZE(_elm_size, _elm_nr)  (5UL + \
                                                 ((_elm_size) * (_elm_nr)))
/* Maximum size of the data portion of an encoded 32 bits msgpack array. */
#undef  DPACK_ARRAY_DATA_SIZE_MAX
#define DPACK_ARRAY_DATA_SIZE_MAX \
	(DPACK_ARRAY_SIZE_MAX - 5UL)

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
	DPACK_ARRAY32_CONST_SIZE(_elm_size, _elm_nr), \

/*
 * Msgpack cannot encode array containing more than UINT32_MAX elements.
 */
#elif DPACK_ARRAY_ELMNR_MAX > UINT32_MAX

#error msgpack cannot encode array which length is > UINT32_MAX !

#endif

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
	             ((_elm_size) <= (DPACK_ARRAY_SIZE_MAX / (_elm_nr))), \
	             _DPACK_ARRAY_CONST_SIZE(_elm_size, _elm_nr), \
	             "invalid constant array size")

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
	             "constant array size expected")

#define DPACK_ARRAY_BOOL_SIZE(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_BOOL_SIZE, _elm_nr)

#define DPACK_ARRAY_UINT8_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT8_SIZE_MIN, _elm_nr)
#define DPACK_ARRAY_UINT8_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT8_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_UINT16_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT16_SIZE_MIN, _elm_nr)
#define DPACK_ARRAY_UINT16_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT16_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_UINT32_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT32_SIZE_MIN, _elm_nr)
#define DPACK_ARRAY_UINT32_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT32_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_UINT64_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT64_SIZE_MIN, _elm_nr)
#define DPACK_ARRAY_UINT64_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE(DPACK_UINT64_SIZE_MAX, _elm_nr)

extern size_t
dpack_array_size(size_t elm_size, unsigned int elm_nr) __dpack_export;

extern void
dpack_array_begin_encode(struct dpack_encoder * encoder,
                         unsigned int           nr) __dpack_export;

extern void
dpack_array_end_encode(struct dpack_encoder * encoder) __dpack_export;

extern int
dpack_array_begin_decode(struct dpack_decoder * decoder,
                         unsigned int         * nr) __dpack_export;
extern int
dpack_array_begin_decode_equ(struct dpack_decoder * decoder,
                             unsigned int           nr) __dpack_export;
extern int
dpack_array_begin_decode_min(struct dpack_decoder * decoder,
                             unsigned int           min_nr,
                             unsigned int         * nr) __dpack_export;
extern int
dpack_array_begin_decode_max(struct dpack_decoder * decoder,
                             unsigned int           max_nr,
                             unsigned int         * nr) __dpack_export;
extern int
dpack_array_begin_decode_range(struct dpack_decoder * decoder,
                               unsigned int           min_nr,
                               unsigned int           max_nr,
                               unsigned int         * nr) __dpack_export;
extern void
dpack_array_end_decode(struct dpack_decoder * decoder) __dpack_export;

#endif /* _DPACK_ARRAY_H */
