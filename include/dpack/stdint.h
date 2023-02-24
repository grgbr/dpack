#ifndef _DPACK_STDINT_H
#define _DPACK_STDINT_H

#include <dpack/cdefs.h>
#include <mpack.h>
#include <stdint.h>

struct dpack_encoder;
struct dpack_decoder;

#define DPACK_UINT_SIZE_MIN (1U)

/******************************************************************************
 * 8 bits integers
 ******************************************************************************/

#define DPACK_U8_SIZE_MIN (DPACK_UINT_SIZE_MIN)
#define DPACK_U8_SIZE_MAX (2U)

extern int
dpack_encode_u8(struct dpack_encoder * encoder, uint8_t value);
extern int
dpack_decode_u8(struct dpack_decoder * decoder, uint8_t * value);
extern int
dpack_decode_u8_min(struct dpack_decoder * decoder,
                    uint8_t                low,
                    uint8_t              * value);
extern int
dpack_decode_u8_max(struct dpack_decoder * decoder,
                    uint8_t                high,
                    uint8_t              * value);
extern int
dpack_decode_u8_range(struct dpack_decoder * decoder,
                      uint8_t                low,
                      uint8_t                high,
                      uint8_t              * value);

/******************************************************************************
 * 16 bits integers
 ******************************************************************************/

#define DPACK_U16_SIZE_MIN (DPACK_UINT_SIZE_MIN)
#define DPACK_U16_SIZE_MAX (3U)

extern int
dpack_encode_u16(struct dpack_encoder * encoder, uint16_t value);
extern int
dpack_decode_u16(struct dpack_decoder * decoder, uint16_t * value);
extern int
dpack_decode_u16_min(struct dpack_decoder * decoder,
                     uint16_t               low,
                     uint16_t             * value);
extern int
dpack_decode_u16_max(struct dpack_decoder * decoder,
                     uint16_t               high,
                     uint16_t             * value);
extern int
dpack_decode_u16_range(struct dpack_decoder * decoder,
                       uint16_t               low,
                       uint16_t               high,
                       uint16_t             * value);

/******************************************************************************
 * 32 bits integers
 ******************************************************************************/

#define DPACK_U32_SIZE_MIN (DPACK_UINT_SIZE_MIN)
#define DPACK_U32_SIZE_MAX (5U)

extern int
dpack_encode_u32(struct dpack_encoder * encoder, uint32_t value);
extern int
dpack_decode_u32(struct dpack_decoder * decoder, uint32_t * value);
extern int
dpack_decode_u32_min(struct dpack_decoder * decoder,
                     uint32_t               low,
                     uint32_t             * value);
extern int
dpack_decode_u32_max(struct dpack_decoder * decoder,
                     uint32_t               high,
                     uint32_t             * value);
extern int
dpack_decode_u32_range(struct dpack_decoder * decoder,
                       uint32_t               low,
                       uint32_t               high,
                       uint32_t             * value);

/******************************************************************************
 * 64 bits integers
 ******************************************************************************/

#define DPACK_U64_SIZE_MIN (DPACK_UINT_SIZE_MIN)
#define DPACK_U64_SIZE_MAX (9U)

extern int
dpack_encode_u64(struct dpack_encoder * encoder, uint64_t value);
extern int
dpack_decode_u64(struct dpack_decoder * decoder, uint64_t * value);
extern int
dpack_decode_u64_min(struct dpack_decoder * decoder,
                     uint64_t               low,
                     uint64_t             * value);
extern int
dpack_decode_u64_max(struct dpack_decoder * decoder,
                     uint64_t               high,
                     uint64_t             * value);
extern int
dpack_decode_u64_range(struct dpack_decoder * decoder,
                       uint64_t               low,
                       uint64_t               high,
                       uint64_t             * value);

/******************************************************************************
 * Structure field identifier
 ******************************************************************************/

#define DPACK_FIELDID_MAX      ((uint16_t)1024)
#define DPACK_FIELDID_SIZE_MIN (DPACK_U16_SIZE_MIN)
#define DPACK_FIELDID_SIZE_MAX (DPACK_U16_SIZE_MAX)

static inline int
dpack_encode_fieldid(struct dpack_encoder * encoder, uint16_t id)
{
	dpack_assert(id <= DPACK_FIELDID_MAX);

	return dpack_encode_u16(encoder, id);
}

static inline int
dpack_decode_fieldid(struct dpack_decoder * decoder, uint16_t * id)
{
	return dpack_decode_u16_max(decoder, DPACK_FIELDID_MAX, id);
}

#endif /* _DPACK_STDINT_H */
