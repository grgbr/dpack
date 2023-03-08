#ifndef _DPACK_ARRAY_H
#define _DPACK_ARRAY_H

#include <dpack/cdefs.h>
#include <stdint.h>

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

#define dpack_array_nr(_array) \
	(sizeof(_array) / sizeof(_array[0]))

/******************************************************************************
 * 8 bits integer arrays
 ******************************************************************************/

#define DPACK_ARRAY_UINT8_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_UINT8_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_UINT8_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE_MAX(DPACK_UINT8_SIZE_MAX, _elm_nr)

extern int
dpack_array_encode_uint8(struct dpack_encoder * encoder,
                         const uint8_t        * array,
	                 unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint8(struct dpack_decoder * decoder,
                         uint8_t              * array,
                         unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint8_min(struct dpack_decoder * decoder,
                             uint8_t                low,
                             uint8_t              * array,
                             unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint8_max(struct dpack_decoder * decoder,
                             uint8_t                high,
                             uint8_t              * array,
                             unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint8_range(struct dpack_decoder * decoder,
                               uint8_t                low,
                               uint8_t                high,
                               uint8_t              * array,
	                       unsigned int           nr) __dpack_export;

/******************************************************************************
 * 16 bits integer arrays
 ******************************************************************************/

#define DPACK_ARRAY_UINT16_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_UINT16_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_UINT16_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE_MAX(DPACK_UINT16_SIZE_MAX, _elm_nr)

extern int
dpack_array_encode_uint16(struct dpack_encoder * encoder,
                          const uint16_t       * array,
                          unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint16(struct dpack_decoder * decoder,
                          uint16_t             * array,
                          unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint16_min(struct dpack_decoder * decoder,
                              uint16_t               low,
                              uint16_t             * array,
                              unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint16_max(struct dpack_decoder * decoder,
                              uint16_t               high,
                              uint16_t             * array,
                              unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint16_range(struct dpack_decoder * decoder,
                                uint16_t               low,
                                uint16_t               high,
                                uint16_t             * array,
                                unsigned int           nr) __dpack_export;

/******************************************************************************
 * 32 bits integer arrays
 ******************************************************************************/

#define DPACK_ARRAY_UINT32_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_UINT32_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_UINT32_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE_MAX(DPACK_UINT32_SIZE_MAX, _elm_nr)

extern int
dpack_array_encode_uint32(struct dpack_encoder * encoder,
                          const uint32_t       * array,
                          unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint32(struct dpack_decoder * decoder,
                          uint32_t             * array,
                          unsigned int           nr) __dpack_export;

extern int
dpack_array_decode_uint32_min(struct dpack_decoder * decoder,
                              uint32_t               low,
                              uint32_t             * array,
                              unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint32_max(struct dpack_decoder * decoder,
                              uint32_t               high,
                              uint32_t             * array,
                              unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint32_range(struct dpack_decoder * decoder,
                                uint32_t               low,
                                uint32_t               high,
                                uint32_t             * array,
                                unsigned int           nr) __dpack_export;

/******************************************************************************
 * 64 bits integer arrays
 ******************************************************************************/

#define DPACK_ARRAY_UINT64_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_UINT64_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_UINT64_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE_MAX(DPACK_UINT64_SIZE_MAX, _elm_nr)

extern int
dpack_array_encode_uint64(struct dpack_encoder * encoder,
                          const uint64_t       * array,
                          unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint64(struct dpack_decoder * decoder,
                          uint64_t             * array,
                          unsigned int           nr) __dpack_export;

extern int
dpack_array_decode_uint64_min(struct dpack_decoder * decoder,
                              uint64_t               low,
                              uint64_t             * array,
                              unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint64_max(struct dpack_decoder * decoder,
                              uint64_t               high,
                              uint64_t             * array,
                              unsigned int           nr) __dpack_export;
extern int
dpack_array_decode_uint64_range(struct dpack_decoder * decoder,
                                uint64_t               low,
                                uint64_t               high,
                                uint64_t             * array,
                                unsigned int           nr) __dpack_export;

#endif /* _DPACK_ARRAY_H */
