#ifndef _DPACK_ARRAY_H
#define _DPACK_ARRAY_H

#include <dpack/cdefs.h>
#include <dpack/scalar.h>

struct dpack_encoder;
struct dpack_decoder;

#define DPACK_ARRAY_NR_MAX (1024U)

#define DPACK_ARRAY_SIZE_MIN(_elm_size, _elm_nr) \
	(1U + ((_elm_size) * (_elm_nr)))

#if DPACK_ARRAY_NR_MAX <= (15U)

#define DPACK_ARRAY_SIZE_MAX(_elm_size, _elm_nr) \
	(1U + ((_elm_size) * (_elm_nr)))

#elif DPACK_ARRAY_NR_MAX <= ((1U << 16) - 1)

#define DPACK_ARRAY_SIZE_MAX(_elm_size, _elm_nr) \
	(3U + ((_elm_size) * (_elm_nr)))

#else

#define DPACK_ARRAY_SIZE_MAX(_elm_size, _elm_nr) \
	(5U + ((_elm_size) * (_elm_nr)))

#endif

#define DPACK_ARRAY_BOOL_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_BOOL_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_UINT8_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_UINT8_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_UINT8_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE_MAX(DPACK_UINT8_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_UINT16_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_UINT16_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_UINT16_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE_MAX(DPACK_UINT16_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_UINT32_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_UINT32_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_UINT32_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE_MAX(DPACK_UINT32_SIZE_MAX, _elm_nr)

#define DPACK_ARRAY_UINT64_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_UINT64_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_UINT64_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE_MAX(DPACK_UINT64_SIZE_MAX, _elm_nr)

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
