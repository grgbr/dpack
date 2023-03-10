#ifndef _DPACK_ARRAY_H
#define _DPACK_ARRAY_H

#include <dpack/cdefs.h>
#include <dpack/scalar.h>
#include <sys/types.h>

struct dpack_encoder;
struct dpack_decoder;

#define DPACK_ARRAY_ELMNR_MAX (1024U)
#define DPACK_ARRAY_SIZE_MAX  ((size_t)4 * 1024U * 1024U)

/* Maximum number of elements an msgpack fixarray may encode */
#define DPACK_FIXARRAY_ELMNR_MAX (15U)
/* Compute size of an encoded msgpack fixarray */
#define DPACK_FIXARRAY_SIZE(_elm_size, _elm_nr) \
	(1UL + ((_elm_size) * (_elm_nr)))
/* Compute the maximum size of the data portion of an encoded msgpack array */
#define DPACK_ARRAY_DATA_SIZE_MAX \
	(DPACK_ARRAY_SIZE_MAX - 1UL)

#define DPACK_FIXARRAY_CONST_SIZE(_elm_size, _elm_nr) \
	DPACK_FIXARRAY_SIZE(_elm_size, _elm_nr)

#define DPACK_ARRAY_EVAL_CONST_SIZE(_elm_size, _elm_nr) \
	DPACK_FIXARRAY_CONST_SIZE(_elm_size, _elm_nr)

#if DPACK_ARRAY_ELMNR_MAX > DPACK_FIXARRAY_ELMNR_MAX

#define DPACK_ARRAY16_ELMNR_MAX                 (UINT16_MAX)
#define DPACK_ARRAY16_SIZE(_elm_size, _elm_nr)  (3UL + \
                                                 ((_elm_size) * (_elm_nr)))
#undef  DPACK_ARRAY_DATA_SIZE_MAX
#define DPACK_ARRAY_DATA_SIZE_MAX               (DPACK_ARRAY_SIZE_MAX - 3UL)

#define DPACK_ARRAY16_CONST_SIZE(_elm_size, _elm_nr) \
	(((_elm_nr) > DPACK_FIXARRAY_ELMNR_MAX) ? \
	 DPACK_ARRAY16_SIZE(_elm_size, _elm_nr) : \
	 DPACK_FIXARRAY_CONST_SIZE(_elm_size, _elm_nr))

#undef DPACK_ARRAY_EVAL_CONST_SIZE
#define DPACK_ARRAY_EVAL_CONST_SIZE(_elm_size, _elm_nr) \
	DPACK_ARRAY16_CONST_SIZE(_elm_size, _elm_nr)

#elif DPACK_ARRAY_ELMNR_MAX > DPACK_ARRAY16_ELMNR_MAX

#define DPACK_ARRAY32_ELMNR_MAX                 (UINT32_MAX)
#define DPACK_ARRAY32_SIZE(_elm_size, _elm_nr)  (5UL + \
                                                 ((_elm_size) * (_elm_nr)))
#undef  DPACK_ARRAY_DATA_SIZE_MAX
#define DPACK_ARRAY_DATA_SIZE_MAX               (DPACK_ARRAY_SIZE_MAX - 5UL)

#define DPACK_ARRAY32_CONST_SIZE(_elm_size, _elm_nr) \
	(((_elm_nr) > DPACK_ARRAY16_ELMNR_MAX) ? \
	 DPACK_ARRAY32_SIZE(_elm_size, _elm_nr) : \
	 DPACK_ARRAY16_CONST_SIZE(_elm_size, _elm_nr))

#undef DPACK_ARRAY_EVAL_CONST_SIZE
#define DPACK_ARRAY_EVAL_CONST_SIZE(_elm_size, _elm_nr) \
	DPACK_ARRAY32_CONST_SIZE(_elm_size, _elm_nr), \

#elif DPACK_ARRAY_ELMNR_MAX > UINT32_MAX

#error msgpack cannot encode array which length is > UINT32_MAX !

#endif

#define DPACK_ARRAY_ELMSIZE_MAX                 (DPACK_ARRAY_DATA_SIZE_MAX / 2U)

#define DPACK_ARRAY_CONST_SIZE(_elm_size, _elm_nr) \
	compile_eval(((_elm_size) > 0) && \
	             ((_elm_size) < DPACK_ARRAY_ELMSIZE_MAX) && \
	             ((_elm_nr) > 0) && \
	             ((_elm_nr) <= DPACK_ARRAY_ELMNR_MAX) && \
	             ((_elm_size) <= (DPACK_ARRAY_SIZE_MAX / (_elm_nr))), \
	             DPACK_ARRAY_EVAL_CONST_SIZE(_elm_size, _elm_nr), \
	             "Invalid constant array size")

#define DPACK_ARRAY_SIZE(_elm_size, _elm_nr) \
	compile_eval(__builtin_constant_p((_elm_size) * (_elm_nr)), \
	             DPACK_ARRAY_CONST_SIZE(_elm_size, _elm_nr), \
	             "Constant array size expected")

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
