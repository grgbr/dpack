#ifndef _DPACK_ARRAY_H
#define _DPACK_ARRAY_H

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

#define DPACK_ARRAY_U8_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_U8_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_U8_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE_MAX(DPACK_U8_SIZE_MAX, _elm_nr)

extern int
dpack_array_encode_u8(struct dpack_encoder * encoder,
                      const uint8_t        * array,
	              unsigned int           nr);
extern int
dpack_array_decode_u8(struct dpack_decoder * decoder,
                      uint8_t              * array,
	              unsigned int           nr);

extern int
dpack_array_decode_u8_min(struct dpack_decoder * decoder,
                          uint8_t                low,
                          uint8_t              * array,
	                  unsigned int           nr);

/******************************************************************************
 * 16 bits integer arrays
 ******************************************************************************/

#define DPACK_ARRAY_U16_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_U16_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_U16_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE_MAX(DPACK_U16_SIZE_MAX, _elm_nr)

extern int
dpack_array_encode_u16(struct dpack_encoder * encoder,
                       const uint16_t       * array,
	               unsigned int           nr);
extern int
dpack_array_decode_u16(struct dpack_decoder * decoder,
                       uint16_t             * array,
	               unsigned int           nr);

extern int
dpack_array_decode_u16_min(struct dpack_decoder * decoder,
                           uint16_t               low,
                           uint16_t             * array,
	                   unsigned int           nr);

/******************************************************************************
 * 32 bits integer arrays
 ******************************************************************************/

#define DPACK_ARRAY_U32_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_U32_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_U32_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE_MAX(DPACK_U32_SIZE_MAX, _elm_nr)

extern int
dpack_array_encode_u32(struct dpack_encoder * encoder,
                       const uint32_t       * array,
	               unsigned int           nr);
extern int
dpack_array_decode_u32(struct dpack_decoder * decoder,
                       uint32_t             * array,
	               unsigned int           nr);

extern int
dpack_array_decode_u32_min(struct dpack_decoder * decoder,
                           uint32_t               low,
                           uint32_t             * array,
	                   unsigned int           nr);

/******************************************************************************
 * 64 bits integer arrays
 ******************************************************************************/

#define DPACK_ARRAY_U64_SIZE_MIN(_elm_nr) \
	DPACK_ARRAY_SIZE_MIN(DPACK_U64_SIZE_MIN, _elm_nr)

#define DPACK_ARRAY_U64_SIZE_MAX(_elm_nr) \
	DPACK_ARRAY_SIZE_MAX(DPACK_U64_SIZE_MAX, _elm_nr)

extern int
dpack_array_encode_u64(struct dpack_encoder * encoder,
                       const uint64_t       * array,
	               unsigned int           nr);
extern int
dpack_array_decode_u64(struct dpack_decoder * decoder,
                       uint64_t             * array,
	               unsigned int           nr);

extern int
dpack_array_decode_u64_min(struct dpack_decoder * decoder,
                           uint64_t               low,
                           uint64_t             * array,
	                   unsigned int           nr);

#endif /* _DPACK_ARRAY_H */
