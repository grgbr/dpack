#ifndef _DPACK_SCALAR_H
#define _DPACK_SCALAR_H

#include <dpack/cdefs.h>
#include <dpack/mpack.h>
#include <stdbool.h>
#include <stdint.h>

struct dpack_encoder;
struct dpack_decoder;

/**
 * Minimum size of a serialized integer.
 *
 * Minimum size in bytes of an integer serialized according to the
 * @rstsubst{MessagePack int format}.
 */
#define DPACK_STDINT_SIZE_MIN MPACK_TAG_SIZE_FIXUINT

/******************************************************************************
 * Boolean
 ******************************************************************************/

/**
 * Size of a serialized boolean.
 *
 * Size in bytes of a boolean serialized according to the
 * @rstsubst{MessagePack bool format}.
 */
#define DPACK_BOOL_SIZE 1

/**
 * Encode a boolean according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   boolean value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value boolean into the buffer assigned to
 * @p encoder at initialization time according to the
 * @rstsubst{MessagePack bool format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_bool(struct dpack_encoder * encoder, bool value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

/**
 * Decode a boolean encoded according to the MessagePack format
 *
 * @param[in]  decoder decoder
 * @param[out] value   Location where to store decoded value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type or range
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack bool format} into @p value boolean from buffer assigned
 * to @p encoder at initialization time.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p dencoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
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

/**
 * Minimum size of a serialized 8-bits unsigned integer.
 *
 * Minimum size in bytes of an 8-bits unsigned integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_UINT8_SIZE_MIN (DPACK_STDINT_SIZE_MIN)

/**
 * Maximum size of a serialized 8-bits unsigned integer.
 *
 * Maximum size in bytes of an 8-bits unsigned integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_UINT8_SIZE_MAX MPACK_TAG_SIZE_U8

/**
 * Encode an 8-bits unsigned integer according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   8-bits unsigned integer value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 8-bits unsigned integer into the
 * buffer assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_int8()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_uint8(struct dpack_encoder * encoder,
                   uint8_t                value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

/**
 * Decode an 8-bits unsigned integer encoded according to the MessagePack format
 *
 * @param[in]  decoder decoder
 * @param[out] value   Location where to store decoded value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 * @retval -ERANGE   Invalid MessagePack stream data range
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack int format} into @p value 8-bits unsigned integer from
 * buffer assigned to @p encoder at initialization time.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p dencoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_int8()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
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
                       uint8_t * __restrict   value) __dpack_nonull(1, 3)
                                                     __dpack_nothrow
                                                     __leaf
                                                     __warn_result
                                                     __dpack_export;
extern int
dpack_decode_uint8_range(struct dpack_decoder * decoder,
                         uint8_t                low,
                         uint8_t                high,
                         uint8_t * __restrict   value) __dpack_nonull(1, 4)
                                                       __dpack_nothrow
                                                       __leaf
                                                       __warn_result
                                                       __dpack_export;

/**
 * Minimum size of a serialized 8-bits signed integer.
 *
 * Minimum size in bytes of an 8-bits signed integer serialized according to the
 * @rstsubst{MessagePack int format}.
 */
#define DPACK_INT8_SIZE_MIN (DPACK_STDINT_SIZE_MIN)

/**
 * Maximum size of a serialized 8-bits signed integer.
 *
 * Maximum size in bytes of an 8-bits signed integer serialized according to the
 * @rstsubst{MessagePack int format}.
 */
#define DPACK_INT8_SIZE_MAX MPACK_TAG_SIZE_I8

/**
 * Encode an 8-bits signed integer according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   8-bits signed integer value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 8-bits signed integer into the buffer
 * assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_uint8()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_int8(struct dpack_encoder * encoder,
                  int8_t                 value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

/**
 * Decode an 8-bits signed integer encoded according to the MessagePack format
 *
 * @param[in]  decoder decoder
 * @param[out] value   Location where to store decoded value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 * @retval -ERANGE   Invalid MessagePack stream data range
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack int format} into @p value 8-bits signed integer from
 * buffer assigned to @p encoder at initialization time.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p dencoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_uint8()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
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
                      int8_t * __restrict    value) __dpack_nonull(1, 3)
                                                    __dpack_nothrow
                                                    __leaf
                                                    __warn_result
                                                    __dpack_export;
extern int
dpack_decode_int8_range(struct dpack_decoder * decoder,
                        int8_t                 low,
                        int8_t                 high,
                        int8_t * __restrict    value) __dpack_nonull(1, 4)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;

/******************************************************************************
 * 16 bits integers
 ******************************************************************************/

/**
 * Minimum size of a serialized 16-bits unsigned integer.
 *
 * Minimum size in bytes of a 16-bits unsigned integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_UINT16_SIZE_MIN (DPACK_STDINT_SIZE_MIN)

/**
 * Maximum size of a serialized 16-bits unsigned integer.
 *
 * Maximum size in bytes of a 16-bits unsigned integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_UINT16_SIZE_MAX MPACK_TAG_SIZE_U16

/**
 * Encode a 16-bits unsigned integer according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   16-bits unsigned integer value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 16-bits unsigned integer into the
 * buffer assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_int16()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_uint16(struct dpack_encoder * encoder,
                    uint16_t               value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

/**
 * Decode a 16-bits unsigned integer encoded according to the MessagePack
 * format
 *
 * @param[in]  decoder decoder
 * @param[out] value   Location where to store decoded value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 * @retval -ERANGE   Invalid MessagePack stream data range
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack int format} into @p value 16-bits unsigned integer from
 * buffer assigned to @p encoder at initialization time.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p dencoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_int16()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
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
                        uint16_t * __restrict  value) __dpack_nonull(1, 3)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;
extern int
dpack_decode_uint16_range(struct dpack_decoder * decoder,
                          uint16_t               low,
                          uint16_t               high,
                          uint16_t * __restrict  value) __dpack_nonull(1, 4)
                                                        __dpack_nothrow
                                                        __leaf
                                                        __warn_result
                                                        __dpack_export;

/**
 * Minimum size of a serialized 16-bits signed integer.
 *
 * Minimum size in bytes of a 16-bits signed integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_INT16_SIZE_MIN (DPACK_STDINT_SIZE_MIN)

/**
 * Maximum size of a serialized 16-bits signed integer.
 *
 * Maximum size in bytes of a 16-bits signed integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_INT16_SIZE_MAX MPACK_TAG_SIZE_I16

/**
 * Encode a 16-bits signed integer according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   16-bits signed integer value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 16-bits signed integer into the buffer
 * assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_uint16()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_int16(struct dpack_encoder * encoder,
                   int16_t                value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

/**
 * Decode an 16-bits signed integer encoded according to the MessagePack format
 *
 * @param[in]  decoder decoder
 * @param[out] value   Location where to store decoded value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 * @retval -ERANGE   Invalid MessagePack stream data range
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack int format} into @p value 16-bits signed integer from
 * buffer assigned to @p encoder at initialization time.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p dencoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_uint16()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
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
                       int16_t * __restrict   value) __dpack_nonull(1, 3)
                                                     __dpack_nothrow
                                                     __leaf
                                                     __warn_result
                                                     __dpack_export;
extern int
dpack_decode_int16_range(struct dpack_decoder * decoder,
                         int16_t                low,
                         int16_t                high,
                         int16_t * __restrict   value) __dpack_nonull(1, 4)
                                                       __dpack_nothrow
                                                       __leaf
                                                       __warn_result
                                                       __dpack_export;

/******************************************************************************
 * 32 bits integers
 ******************************************************************************/

/**
 * Minimum size of a serialized 32-bits unsigned integer.
 *
 * Minimum size in bytes of a 32-bits unsigned integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_UINT32_SIZE_MIN (DPACK_STDINT_SIZE_MIN)

/**
 * Maximum size of a serialized 32-bits unsigned integer.
 *
 * Maximum size in bytes of a 32-bits unsigned integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_UINT32_SIZE_MAX MPACK_TAG_SIZE_U32

/**
 * Encode a 32-bits unsigned integer according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   32-bits unsigned integer value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 32-bits unsigned integer into the
 * buffer assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_int32()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_uint32(struct dpack_encoder * encoder,
                    uint32_t               value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

/**
 * Decode a 32-bits unsigned integer encoded according to the MessagePack
 * format
 *
 * @param[in]  decoder decoder
 * @param[out] value   Location where to store decoded value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 * @retval -ERANGE   Invalid MessagePack stream data range
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack int format} into @p value 32-bits unsigned integer from
 * buffer assigned to @p encoder at initialization time.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p dencoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_int32()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
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
                        uint32_t * __restrict  value) __dpack_nonull(1, 3)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;
extern int
dpack_decode_uint32_range(struct dpack_decoder * decoder,
                          uint32_t               low,
                          uint32_t               high,
                          uint32_t * __restrict  value) __dpack_nonull(1, 4)
                                                        __dpack_nothrow
                                                        __leaf
                                                        __warn_result
                                                        __dpack_export;

/**
 * Minimum size of a serialized unsigned integer.
 *
 * Minimum size in bytes of an unsigned integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_UINT_SIZE_MIN DPACK_UINT32_SIZE_MIN

/**
 * Maximum size of a serialized unsigned integer.
 *
 * Maximum size in bytes of an unsigned integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_UINT_SIZE_MAX DPACK_UINT32_SIZE_MAX

/**
 * Encode an unsigned integer according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   unsigned integer value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value unsigned integer into the buffer
 * assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_int()
 * - dpack_encoder_init_buffer()
 */
static inline int __dpack_nonull(1) __dpack_nothrow __warn_result
dpack_encode_uint(struct dpack_encoder * encoder, unsigned int value)
{
	return dpack_encode_uint32(encoder, value);
}

/**
 * Decode an unsigned integer encoded according to the MessagePack format
 *
 * @param[in]  decoder decoder
 * @param[out] value   Location where to store decoded value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 * @retval -ERANGE   Invalid MessagePack stream data range
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack int format} into @p value unsigned integer from buffer
 * assigned to @p encoder at initialization time.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p dencoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_int()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
static inline int __dpack_nonull(1, 2) __dpack_nothrow __warn_result
dpack_decode_uint(struct dpack_decoder *    decoder,
                  unsigned int * __restrict value)
{
	return dpack_decode_uint32(decoder, value);
}

static inline int __dpack_nonull(1, 3) __dpack_nothrow __warn_result
dpack_decode_uint_min(struct dpack_decoder *    decoder,
                      unsigned int              low,
                      unsigned int * __restrict value)
{
	return dpack_decode_uint32_min(decoder, low, value);
}

static inline int __dpack_nonull(1, 3) __dpack_nothrow __warn_result
dpack_decode_uint_max(struct dpack_decoder *    decoder,
                      unsigned int              high,
                      unsigned int * __restrict value)
{
	return dpack_decode_uint32_max(decoder, high, value);
}

static inline int __dpack_nonull(1, 4) __dpack_nothrow __warn_result
dpack_decode_uint_range(struct dpack_decoder *    decoder,
                        unsigned int              low,
                        unsigned int              high,
                        unsigned int * __restrict value)
{
	return dpack_decode_uint32_range(decoder, low, high, value);
}

/**
 * Minimum size of a serialized 32-bits signed integer.
 *
 * Minimum size in bytes of a 32-bits signed integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_INT32_SIZE_MIN (DPACK_STDINT_SIZE_MIN)

/**
 * Maximum size of a serialized 32-bits signed integer.
 *
 * Maximum size in bytes of a 32-bits signed integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_INT32_SIZE_MAX MPACK_TAG_SIZE_I32

/**
 * Encode a 32-bits signed integer according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   32-bits signed integer value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 32-bits signed integer into the buffer
 * assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_uint32()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_int32(struct dpack_encoder * encoder,
                   int32_t                value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

/**
 * Decode an 32-bits signed integer encoded according to the MessagePack format
 *
 * @param[in]  decoder decoder
 * @param[out] value   Location where to store decoded value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 * @retval -ERANGE   Invalid MessagePack stream data range
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack int format} into @p value 32-bits signed integer from
 * buffer assigned to @p encoder at initialization time.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p dencoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_uint32()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
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
                       int32_t * __restrict   value) __dpack_nonull(1, 3)
                                                     __dpack_nothrow
                                                     __leaf
                                                     __warn_result
                                                     __dpack_export;
extern int
dpack_decode_int32_range(struct dpack_decoder * decoder,
                         int32_t                low,
                         int32_t                high,
                         int32_t * __restrict   value) __dpack_nonull(1, 4)
                                                       __dpack_nothrow
                                                       __leaf
                                                       __warn_result
                                                       __dpack_export;

/**
 * Minimum size of a serialized signed integer.
 *
 * Minimum size in bytes of a signed integer serialized according to the
 * @rstsubst{MessagePack int format}.
 */
#define DPACK_INT_SIZE_MIN DPACK_INT32_SIZE_MIN

/**
 * Maximum size of a serialized signed integer.
 *
 * Maximum size in bytes of a signed integer serialized according to the
 * @rstsubst{MessagePack int format}.
 */
#define DPACK_INT_SIZE_MAX DPACK_INT32_SIZE_MAX

/**
 * Encode a signed integer according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   signed integer value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value signed integer into the buffer
 * assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_uint()
 * - dpack_encoder_init_buffer()
 */
static inline int __dpack_nonull(1) __dpack_nothrow __warn_result
dpack_encode_int(struct dpack_encoder * encoder, int value)
{
	return dpack_encode_int32(encoder, value);
}

/**
 * Decode an signed integer encoded according to the MessagePack format
 *
 * @param[in]  decoder decoder
 * @param[out] value   Location where to store decoded value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 * @retval -ERANGE   Invalid MessagePack stream data range
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack int format} into @p value signed integer from buffer
 * assigned to @p encoder at initialization time.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p dencoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_uint()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
static inline int __dpack_nonull(1, 2) __dpack_nothrow __warn_result
dpack_decode_int(struct dpack_decoder * decoder, int * __restrict value)
{
	return dpack_decode_int32(decoder, value);
}

static inline int __dpack_nonull(1, 3) __dpack_nothrow __warn_result
dpack_decode_int_min(struct dpack_decoder * decoder,
                     int                    low,
                     int * __restrict       value)
{
	return dpack_decode_int32_min(decoder, low, value);
}

static inline int __dpack_nonull(1, 3) __dpack_nothrow __warn_result
dpack_decode_int_max(struct dpack_decoder * decoder,
                     int                    high,
                     int * __restrict       value)
{
	return dpack_decode_int32_max(decoder, high, value);
}

static inline int __dpack_nonull(1, 4) __dpack_nothrow __warn_result
dpack_decode_int_range(struct dpack_decoder * decoder,
                       int                    low,
                       int                    high,
                       int * __restrict       value)
{
	return dpack_decode_int32_range(decoder, low, high, value);
}

/******************************************************************************
 * 64 bits integers
 ******************************************************************************/

/**
 * Minimum size of a serialized 64-bits unsigned integer.
 *
 * Minimum size in bytes of a 64-bits unsigned integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_UINT64_SIZE_MIN (DPACK_STDINT_SIZE_MIN)

/**
 * Maximum size of a serialized 64-bits unsigned integer.
 *
 * Maximum size in bytes of a 64-bits unsigned integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_UINT64_SIZE_MAX MPACK_TAG_SIZE_U64

/**
 * Encode a 64-bits unsigned integer according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   64-bits unsigned integer value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 64-bits unsigned integer into the
 * buffer assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_int64()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_uint64(struct dpack_encoder * encoder,
                    uint64_t               value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

/**
 * Decode a 64-bits unsigned integer encoded according to the MessagePack format
 *
 * @param[in]  decoder decoder
 * @param[out] value   Location where to store decoded value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 * @retval -ERANGE   Invalid MessagePack stream data range
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack int format} into @p value 64-bits unsigned integer from
 * buffer assigned to @p encoder at initialization time.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p dencoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_int64()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
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
                        uint64_t * __restrict  value) __dpack_nonull(1, 3)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;
extern int
dpack_decode_uint64_range(struct dpack_decoder * decoder,
                          uint64_t               low,
                          uint64_t               high,
                          uint64_t * __restrict  value) __dpack_nonull(1, 4)
                                                        __dpack_nothrow
                                                        __leaf
                                                        __warn_result
                                                        __dpack_export;

/**
 * Minimum size of a serialized 64-bits signed integer.
 *
 * Minimum size in bytes of a 64-bits signed integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_INT64_SIZE_MIN (DPACK_STDINT_SIZE_MIN)

/**
 * Maximum size of a serialized 64-bits signed integer.
 *
 * Maximum size in bytes of a 64-bits signed integer serialized according to
 * the @rstsubst{MessagePack int format}.
 */
#define DPACK_INT64_SIZE_MAX MPACK_TAG_SIZE_I64

/**
 * Encode a 64-bits signed integer according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   64-bits signed integer value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 64-bits signed integer into the buffer
 * assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_uint64()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_int64(struct dpack_encoder * encoder,
                   int64_t                value)
	__dpack_nonull(1) __dpack_nothrow __leaf __warn_result __dpack_export;

/**
 * Decode an 64-bits signed integer encoded according to the MessagePack format
 *
 * @param[in]  decoder decoder
 * @param[out] value   Location where to store decoded value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 * @retval -ERANGE   Invalid MessagePack stream data range
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack int format} into @p value 64-bits signed integer from
 * buffer assigned to @p encoder at initialization time.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p dencoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_uint64()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
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
                       int64_t * __restrict   value) __dpack_nonull(1, 3)
                                                     __dpack_nothrow
                                                     __leaf
                                                     __warn_result
                                                     __dpack_export;
extern int
dpack_decode_int64_range(struct dpack_decoder * decoder,
                         int64_t                low,
                         int64_t                high,
                         int64_t * __restrict   value) __dpack_nonull(1, 4)
                                                       __dpack_nothrow
                                                       __leaf
                                                       __warn_result
                                                       __dpack_export;

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
                        double                 high,
                        double *               value) __dpack_export;

extern int
dpack_decode_double_range(struct dpack_decoder * decoder,
                          double                 low,
                          double                 high,
                          double *               value) __dpack_export;

#endif /* defined(CONFIG_DPACK_DOUBLE) */

#endif /* _DPACK_SCALAR_H */
