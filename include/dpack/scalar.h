#ifndef _DPACK_SCALAR_H
#define _DPACK_SCALAR_H

#include <dpack/cdefs.h>
#include <dpack/mpack.h>
#include <stdbool.h>
#include <stdint.h>

struct dpack_encoder;
struct dpack_decoder;

#define DPACK_STDINT_SIZE_MIN MPACK_TAG_SIZE_FIXUINT

/******************************************************************************
 * Boolean
 ******************************************************************************/

#define DPACK_BOOL_SIZE 1

extern int
dpack_encode_bool(struct dpack_encoder * encoder, bool value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

extern int
dpack_decode_bool(struct dpack_decoder * decoder,
                  bool * __restrict      value) __dpack_nonull(1, 2)
                                                __dpack_nothrow
                                                __leaf
                                                __warn_result
                                                __dpack_export;

/******************************************************************************
 * 8 bits integers
 ******************************************************************************/

#define DPACK_UINT8_SIZE_MIN (DPACK_STDINT_SIZE_MIN)
#define DPACK_UINT8_SIZE_MAX MPACK_TAG_SIZE_U8

extern int
dpack_encode_uint8(struct dpack_encoder * encoder,
                   uint8_t                value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

extern int
dpack_decode_uint8(struct dpack_decoder * decoder,
                   uint8_t * __restrict   value) __dpack_nonull(1, 2)
                                                 __dpack_nothrow
                                                 __leaf
                                                 __warn_result
                                                 __dpack_export;
extern int
dpack_decode_uint8_min(struct dpack_decoder * decoder,
                       uint8_t                low,
                       uint8_t * __restrict   value) __dpack_nonull(1, 3)
                                                     __dpack_nothrow
                                                     __leaf
                                                     __warn_result
                                                     __dpack_export;
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
#define DPACK_INT8_SIZE_MAX MPACK_TAG_SIZE_I8

extern int
dpack_encode_int8(struct dpack_encoder * encoder,
                  int8_t                 value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

extern int
dpack_decode_int8(struct dpack_decoder * decoder,
                  int8_t * __restrict    value) __dpack_nonull(1, 2)
                                                __dpack_nothrow
                                                __leaf
                                                __warn_result
                                                __dpack_export;
extern int
dpack_decode_int8_min(struct dpack_decoder * decoder,
                      int8_t                 low,
                      int8_t               * value) __dpack_nonull(1, 3)
                                                    __dpack_nothrow
                                                    __leaf
                                                    __warn_result
                                                    __dpack_export;
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
#define DPACK_UINT16_SIZE_MAX MPACK_TAG_SIZE_U16

extern int
dpack_encode_uint16(struct dpack_encoder * encoder,
                    uint16_t               value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;
extern int
dpack_decode_uint16(struct dpack_decoder * decoder,
                    uint16_t * __restrict  value) __dpack_nonull(1, 2)
                                                  __dpack_nothrow
                                                  __leaf
                                                  __warn_result
                                                  __dpack_export;
extern int
dpack_decode_uint16_min(struct dpack_decoder * decoder,
                        uint16_t               low,
                        uint16_t * __restrict  value) __dpack_nonull(1, 3)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;
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
#define DPACK_INT16_SIZE_MAX MPACK_TAG_SIZE_I16

extern int
dpack_encode_int16(struct dpack_encoder * encoder,
                   int16_t                value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;
extern int
dpack_decode_int16(struct dpack_decoder * decoder,
                   int16_t * __restrict   value) __dpack_nonull(1, 2)
                                                 __dpack_nothrow
                                                 __leaf
                                                 __warn_result
                                                 __dpack_export;
extern int
dpack_decode_int16_min(struct dpack_decoder * decoder,
                       int16_t                low,
                       int16_t * __restrict   value) __dpack_nonull(1, 3)
                                                     __dpack_nothrow
                                                     __leaf
                                                     __warn_result
                                                     __dpack_export;
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
#define DPACK_UINT32_SIZE_MAX MPACK_TAG_SIZE_U32

extern int
dpack_encode_uint32(struct dpack_encoder * encoder,
                    uint32_t               value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;
extern int
dpack_decode_uint32(struct dpack_decoder * decoder,
                    uint32_t * __restrict  value) __dpack_nonull(1, 2)
                                                  __dpack_nothrow
                                                  __leaf
                                                  __warn_result
                                                  __dpack_export;
extern int
dpack_decode_uint32_min(struct dpack_decoder * decoder,
                        uint32_t               low,
                        uint32_t * __restrict  value) __dpack_nonull(1, 3)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;
extern int
dpack_decode_uint32_max(struct dpack_decoder * decoder,
                        uint32_t               high,
                        uint32_t             * value) __dpack_export;
extern int
dpack_decode_uint32_range(struct dpack_decoder * decoder,
                          uint32_t               low,
                          uint32_t               high,
                          uint32_t             * value) __dpack_export;

#define DPACK_UINT_SIZE_MIN DPACK_UINT32_SIZE_MIN
#define DPACK_UINT_SIZE_MAX DPACK_UINT32_SIZE_MAX

static inline int __dpack_nonull(1) __dpack_nothrow __warn_result
dpack_encode_uint(struct dpack_encoder * encoder, unsigned int value)
{
	return dpack_encode_uint32(encoder, value);
}

static inline int __dpack_nonull(1, 2) __dpack_nothrow __warn_result
dpack_decode_uint(struct dpack_decoder * decoder, unsigned int * value)
{
	return dpack_decode_uint32(decoder, value);
}

static inline int __dpack_nonull(1, 3) __dpack_nothrow __warn_result
dpack_decode_uint_min(struct dpack_decoder * decoder,
                      unsigned int           low,
                      unsigned int         * value)
{
	return dpack_decode_uint32_min(decoder, low, value);
}

static inline int
dpack_decode_uint_max(struct dpack_decoder * decoder,
                      unsigned int           high,
                      unsigned int         * value)
{
	return dpack_decode_uint32_max(decoder, high, value);
}

static inline int
dpack_decode_uint_range(struct dpack_decoder * decoder,
                        unsigned int           low,
                        unsigned int           high,
                        unsigned int         * value)
{
	return dpack_decode_uint32_range(decoder, low, high, value);
}

#define DPACK_INT32_SIZE_MIN (DPACK_STDINT_SIZE_MIN)
#define DPACK_INT32_SIZE_MAX MPACK_TAG_SIZE_I32

extern int
dpack_encode_int32(struct dpack_encoder * encoder,
                   int32_t                value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;
extern int
dpack_decode_int32(struct dpack_decoder * decoder,
                   int32_t * __restrict   value) __dpack_nonull(1, 2)
                                                 __dpack_nothrow
                                                 __leaf
                                                 __warn_result
                                                 __dpack_export;
extern int
dpack_decode_int32_min(struct dpack_decoder * decoder,
                       int32_t                low,
                       int32_t * __restrict   value) __dpack_nonull(1, 3)
                                                     __dpack_nothrow
                                                     __leaf
                                                     __warn_result
                                                     __dpack_export;
extern int
dpack_decode_int32_max(struct dpack_decoder * decoder,
                       int32_t                high,
                       int32_t              * value) __dpack_export;
extern int
dpack_decode_int32_range(struct dpack_decoder * decoder,
                         int32_t                low,
                         int32_t                high,
                         int32_t              * value) __dpack_export;

#define DPACK_INT_SIZE_MIN DPACK_INT32_SIZE_MIN
#define DPACK_INT_SIZE_MAX DPACK_INT32_SIZE_MAX

static inline int __dpack_nonull(1) __dpack_nothrow __warn_result
dpack_encode_int(struct dpack_encoder * encoder, int value)
{
	return dpack_encode_int32(encoder, value);
}

static inline int __dpack_nonull(1, 2) __dpack_nothrow __warn_result
dpack_decode_int(struct dpack_decoder * decoder, int * value)
{
	return dpack_decode_int32(decoder, value);
}

static inline int __dpack_nonull(1, 3) __dpack_nothrow __warn_result
dpack_decode_int_min(struct dpack_decoder * decoder,
                     int                    low,
                     int                  * value)
{
	return dpack_decode_int32_min(decoder, low, value);
}

static inline int
dpack_decode_int_max(struct dpack_decoder * decoder,
                     int                    high,
                     int                  * value)
{
	return dpack_decode_int32_max(decoder, high, value);
}

static inline int
dpack_decode_int_range(struct dpack_decoder * decoder,
                       int                    low,
                       int                    high,
                       int                  * value)
{
	return dpack_decode_int32_range(decoder, low, high, value);
}

/******************************************************************************
 * 64 bits integers
 ******************************************************************************/

#define DPACK_UINT64_SIZE_MIN (DPACK_STDINT_SIZE_MIN)
#define DPACK_UINT64_SIZE_MAX MPACK_TAG_SIZE_U64

extern int
dpack_encode_uint64(struct dpack_encoder * encoder,
                    uint64_t               value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;
extern int
dpack_decode_uint64(struct dpack_decoder * decoder,
                    uint64_t * __restrict  value) __dpack_nonull(1, 2)
                                                  __dpack_nothrow
                                                  __leaf
                                                  __warn_result
                                                  __dpack_export;
extern int
dpack_decode_uint64_min(struct dpack_decoder * decoder,
                        uint64_t               low,
                        uint64_t * __restrict  value) __dpack_nonull(1, 3)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;
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
#define DPACK_INT64_SIZE_MAX MPACK_TAG_SIZE_I64

extern int
dpack_encode_int64(struct dpack_encoder * encoder,
                   int64_t                value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;
extern int
dpack_decode_int64(struct dpack_decoder * decoder,
                   int64_t * __restrict   value) __dpack_nonull(1, 2)
                                                 __dpack_nothrow
                                                 __leaf
                                                 __warn_result
                                                 __dpack_export;
extern int
dpack_decode_int64_min(struct dpack_decoder * decoder,
                       int64_t                low,
                       int64_t * __restrict   value) __dpack_nonull(1, 3)
                                                     __dpack_nothrow
                                                     __leaf
                                                     __warn_result
                                                     __dpack_export;
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
 * Single precision floating point
 ******************************************************************************/

#if defined(CONFIG_DPACK_FLOAT)

#define DPACK_FLOAT_SIZE MPACK_TAG_SIZE_FLOAT

extern int
dpack_encode_float(struct dpack_encoder * encoder, float value) __dpack_export;

extern int
dpack_decode_float(struct dpack_decoder * decoder, float * value)
	__dpack_export;

extern int
dpack_decode_float_min(struct dpack_decoder * decoder, float low, float * value)
	__dpack_export;

extern int
dpack_decode_float_max(struct dpack_decoder * decoder,
                       float                  high,
                       float *                value) __dpack_export;

extern int
dpack_decode_float_range(struct dpack_decoder * decoder,
                         float                  low,
                         float                  high,
                         float *                value) __dpack_export;

#endif /* defined(CONFIG_DPACK_FLOAT) */

/******************************************************************************
 * Double precision floating point
 ******************************************************************************/

#if defined(CONFIG_DPACK_DOUBLE)

#define DPACK_DOUBLE_SIZE MPACK_TAG_SIZE_DOUBLE

extern int
dpack_encode_double(struct dpack_encoder * encoder, double value)
	__dpack_export;

extern int
dpack_decode_double(struct dpack_decoder * decoder, double * value)
	__dpack_export;

extern int
dpack_decode_double_min(struct dpack_decoder * decoder,
                        double                 low,
                        double *               value) __dpack_export;

extern int
dpack_decode_double_max(struct dpack_decoder * decoder,
                       double                  high,
                       double *                value) __dpack_export;

extern int
dpack_decode_double_range(struct dpack_decoder * decoder,
                         double                  low,
                         double                  high,
                         double *                value) __dpack_export;

#endif /* defined(CONFIG_DPACK_DOUBLE) */

#endif /* _DPACK_SCALAR_H */
