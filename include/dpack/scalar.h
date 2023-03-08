#ifndef _DPACK_STDINT_H
#define _DPACK_STDINT_H

#include <dpack/cdefs.h>
#include <stdbool.h>
#include <stdint.h>

struct dpack_encoder;
struct dpack_decoder;

#define DPACK_STDINT_SIZE_MIN (1U)

/******************************************************************************
 * Boolean
 ******************************************************************************/

extern int
dpack_encode_bool(struct dpack_encoder * encoder, bool value) __dpack_export;

extern int
dpack_decode_bool(struct dpack_decoder * decoder, bool * value) __dpack_export;

/******************************************************************************
 * 8 bits integers
 ******************************************************************************/

#define DPACK_UINT8_SIZE_MIN (DPACK_STDINT_SIZE_MIN)
#define DPACK_UINT8_SIZE_MAX (2U)

extern int
dpack_encode_uint8(struct dpack_encoder * encoder,
                   uint8_t                value) __dpack_export;
extern int
dpack_decode_uint8(struct dpack_decoder * decoder,
                   uint8_t              * value) __dpack_export;
extern int
dpack_decode_uint8_min(struct dpack_decoder * decoder,
                       uint8_t                low,
                       uint8_t              * value) __dpack_export;
extern int
dpack_decode_uint8_max(struct dpack_decoder * decoder,
                       uint8_t                high,
                       uint8_t              * value) __dpack_export;
extern int
dpack_decode_uint8_range(struct dpack_decoder * decoder,
                         uint8_t                low,
                         uint8_t                high,
                         uint8_t              * value) __dpack_export;

#define DPACK_INT8_SIZE_MIN (DPACK_STDINT_SIZE_MIN)
#define DPACK_INT8_SIZE_MAX (2U)

extern int
dpack_encode_int8(struct dpack_encoder * encoder,
                  int8_t                 value) __dpack_export;
extern int
dpack_decode_int8(struct dpack_decoder * decoder,
                  int8_t               * value) __dpack_export;
extern int
dpack_decode_int8_min(struct dpack_decoder * decoder,
                      int8_t                 low,
                      int8_t               * value) __dpack_export;
extern int
dpack_decode_int8_max(struct dpack_decoder * decoder,
                      int8_t                 high,
                      int8_t               * value) __dpack_export;
extern int
dpack_decode_int8_range(struct dpack_decoder * decoder,
                        int8_t                 low,
                        int8_t                 high,
                        int8_t               * value) __dpack_export;

/******************************************************************************
 * 16 bits integers
 ******************************************************************************/

#define DPACK_UINT16_SIZE_MIN (DPACK_STDINT_SIZE_MIN)
#define DPACK_UINT16_SIZE_MAX (3U)

extern int
dpack_encode_uint16(struct dpack_encoder * encoder,
                    uint16_t               value) __dpack_export;
extern int
dpack_decode_uint16(struct dpack_decoder * decoder,
                    uint16_t             * value) __dpack_export;
extern int
dpack_decode_uint16_min(struct dpack_decoder * decoder,
                        uint16_t               low,
                        uint16_t             * value) __dpack_export;
extern int
dpack_decode_uint16_max(struct dpack_decoder * decoder,
                        uint16_t               high,
                        uint16_t             * value) __dpack_export;
extern int
dpack_decode_uint16_range(struct dpack_decoder * decoder,
                          uint16_t               low,
                          uint16_t               high,
                          uint16_t             * value) __dpack_export;

#define DPACK_INT16_SIZE_MIN (DPACK_STDINT_SIZE_MIN)
#define DPACK_INT16_SIZE_MAX (3U)

extern int
dpack_encode_int16(struct dpack_encoder * encoder,
                   int16_t                value) __dpack_export;
extern int
dpack_decode_int16(struct dpack_decoder * decoder,
                   int16_t              * value) __dpack_export;
extern int
dpack_decode_int16_min(struct dpack_decoder * decoder,
                       int16_t                low,
                       int16_t              * value) __dpack_export;
extern int
dpack_decode_int16_max(struct dpack_decoder * decoder,
                       int16_t                high,
                       int16_t              * value) __dpack_export;
extern int
dpack_decode_int16_range(struct dpack_decoder * decoder,
                         int16_t                low,
                         int16_t                high,
                         int16_t              * value) __dpack_export;

/******************************************************************************
 * 32 bits integers
 ******************************************************************************/

#define DPACK_UINT32_SIZE_MIN (DPACK_STDINT_SIZE_MIN)
#define DPACK_UINT32_SIZE_MAX (5U)

extern int
dpack_encode_uint32(struct dpack_encoder * encoder,
                    uint32_t               value) __dpack_export;
extern int
dpack_decode_uint32(struct dpack_decoder * decoder,
                    uint32_t             * value) __dpack_export;
extern int
dpack_decode_uint32_min(struct dpack_decoder * decoder,
                        uint32_t               low,
                        uint32_t             * value) __dpack_export;
extern int
dpack_decode_uint32_max(struct dpack_decoder * decoder,
                        uint32_t               high,
                        uint32_t             * value) __dpack_export;
extern int
dpack_decode_uint32_range(struct dpack_decoder * decoder,
                          uint32_t               low,
                          uint32_t               high,
                          uint32_t             * value) __dpack_export;

#define DPACK_INT32_SIZE_MIN (DPACK_STDINT_SIZE_MIN)
#define DPACK_INT32_SIZE_MAX (5U)

extern int
dpack_encode_int32(struct dpack_encoder * encoder,
                   int32_t                value) __dpack_export;
extern int
dpack_decode_int32(struct dpack_decoder * decoder,
                   int32_t              * value) __dpack_export;
extern int
dpack_decode_int32_min(struct dpack_decoder * decoder,
                       int32_t                low,
                       int32_t              * value) __dpack_export;
extern int
dpack_decode_int32_max(struct dpack_decoder * decoder,
                       int32_t                high,
                       int32_t              * value) __dpack_export;
extern int
dpack_decode_int32_range(struct dpack_decoder * decoder,
                         int32_t                low,
                         int32_t                high,
                         int32_t              * value) __dpack_export;

/******************************************************************************
 * 64 bits integers
 ******************************************************************************/

#define DPACK_UINT64_SIZE_MIN (DPACK_STDINT_SIZE_MIN)
#define DPACK_UINT64_SIZE_MAX (9U)

extern int
dpack_encode_uint64(struct dpack_encoder * encoder,
                    uint64_t               value) __dpack_export;
extern int
dpack_decode_uint64(struct dpack_decoder * decoder,
                    uint64_t             * value) __dpack_export;
extern int
dpack_decode_uint64_min(struct dpack_decoder * decoder,
                        uint64_t               low,
                        uint64_t             * value) __dpack_export;
extern int
dpack_decode_uint64_max(struct dpack_decoder * decoder,
                        uint64_t               high,
                        uint64_t             * value) __dpack_export;
extern int
dpack_decode_uint64_range(struct dpack_decoder * decoder,
                          uint64_t               low,
                          uint64_t               high,
                          uint64_t             * value) __dpack_export;

#define DPACK_INT64_SIZE_MIN (DPACK_STDINT_SIZE_MIN)
#define DPACK_INT64_SIZE_MAX (9U)

extern int
dpack_encode_int64(struct dpack_encoder * encoder,
                   int64_t                value) __dpack_export;
extern int
dpack_decode_int64(struct dpack_decoder * decoder,
                   int64_t              * value) __dpack_export;
extern int
dpack_decode_int64_min(struct dpack_decoder * decoder,
                       int64_t                low,
                       int64_t              * value) __dpack_export;
extern int
dpack_decode_int64_max(struct dpack_decoder * decoder,
                       int64_t                high,
                       int64_t              * value) __dpack_export;
extern int
dpack_decode_int64_range(struct dpack_decoder * decoder,
                         int64_t                low,
                         int64_t                high,
                         int64_t              * value) __dpack_export;

/******************************************************************************
 * Structure field identifier
 ******************************************************************************/

#define DPACK_FIELDID_MAX      ((uint16_t)1024)
#define DPACK_FIELDID_SIZE_MIN (DPACK_UINT16_SIZE_MIN)
#define DPACK_FIELDID_SIZE_MAX (DPACK_UINT16_SIZE_MAX)

static inline int
dpack_encode_fieldid(struct dpack_encoder * encoder, uint16_t id)
{
	dpack_assert(id <= DPACK_FIELDID_MAX);

	return dpack_encode_uint16(encoder, id);
}

static inline int
dpack_decode_fieldid(struct dpack_decoder * decoder, uint16_t * id)
{
	return dpack_decode_uint16_max(decoder, DPACK_FIELDID_MAX, id);
}

#endif /* _DPACK_STDINT_H */
