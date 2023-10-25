/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Bin encoding / decoding interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      20 May 2023
 * @copyright Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */
#ifndef _DPACK_BIN_H
#define _DPACK_BIN_H

#include <dpack/cdefs.h>
#include <dpack/mpack.h>

struct dpack_encoder;
struct dpack_decoder;

/**
 * Maximum size of a bin
 *
 * Maximum size in bytes of a bin.
 */
#define DPACK_BINSZ_MAX (64U * 1024U)

/* Maximum number of bytes an 8 bits msgpack bin may encode */
#define DPACK_BIN8_SIZE_MAX  UINT8_MAX
/* Maximum number of bytes a 16 bits msgpack bin may encode */
#define DPACK_BIN16_SIZE_MAX UINT16_MAX
/* Maximum number of bytes a 32 bits msgpack bin may encode */
#define DPACK_BIN32_SIZE_MAX UINT32_MAX

/*
 * Check DPACK_BINSZ_MAX definition is sensible.
 * Multiple dpack internal functions (such as dpack_decode_bin_tag() for
 * example) return bin size using a ssize_t, effectively restricting
 * maximum size to a SSIZE_MAX...
 * In addition, restrict maximum size to 128 MB !
 */
#if DPACK_BINSZ_MAX > (128U * 1024 * 1024)
#error dpack cannot encode bin which size is > 128 MB !
#elif DPACK_BINSZ_MAX < 16U
#error dpack cannot encode bin which size is < 16 !
#endif

/******************************************************************************
 * Msgpack 8 bits bin definitions
 ******************************************************************************/

/* Compute size of an 8 bits encoded msgpack bin */
#define DPACK_BIN8_SIZE(_sz) \
	(MPACK_TAG_SIZE_BIN8 + (_sz))

#define _DPACK_BIN_CONST_SIZE(_sz) \
	DPACK_BIN8_SIZE(_sz)

/******************************************************************************
 * Msgpack 16 bits bin definitions
 ******************************************************************************/

#if DPACK_BINSZ_MAX > DPACK_BIN8_SIZE_MAX

/* Compute size of an encoded 16 bits msgpack bin */
#define DPACK_BIN16_SIZE(_sz) \
	(MPACK_TAG_SIZE_BIN16 + (_sz))

/* Size of an encoded bin when size fits into an msgpack 16 bits bin. */
#define DPACK_BIN16_CONST_SIZE(_sz) \
	(((_sz) > DPACK_BIN8_SIZE_MAX) ? DPACK_BIN16_SIZE(_sz) : \
	                                 DPACK_BIN8_SIZE(_sz))

#undef _DPACK_BIN_CONST_SIZE
#define _DPACK_BIN_CONST_SIZE(_sz) \
	DPACK_BIN16_CONST_SIZE(_sz)

#endif /* DPACK_BINSZ_MAX > DPACK_BIN8_SIZE_MAX */

/******************************************************************************
 * Msgpack 32 bits bin definitions
 ******************************************************************************/

#if DPACK_BINSZ_MAX > DPACK_BIN16_SIZE_MAX

/* Compute size of an encoded 32 bits msgpack bin */
#define DPACK_BIN32_SIZE(_sz) \
	(MPACK_TAG_SIZE_BIN32 + (_sz))

/* Size of an encoded bin when size fits into an msgpack 32 bits bin. */
#define DPACK_BIN32_CONST_SIZE(_sz) \
	(((_sz) > DPACK_BIN16_SIZE_MAX) ? DPACK_BIN32_SIZE(_sz) : \
	                                  DPACK_BIN16_CONST_SIZE(_sz))

#undef _DPACK_BIN_CONST_SIZE
#define _DPACK_BIN_CONST_SIZE(_sz) \
	DPACK_BIN32_CONST_SIZE(_sz)

#endif /* DPACK_BINSZ_MAX > DPACK_BIN16_SIZE_MAX */

/******************************************************************************
 * Top-level bin size definitions
 ******************************************************************************/

/*
 * Given the size of a bin, compute the size of the corresponding encoded
 * msgpack bin.
 *
 * Size value MUST be known at compile time, i.e., constant. Result is
 * unpredictable otherwise.
 */
#define DPACK_BIN_CONST_SIZE(_sz) \
	compile_eval(((_sz) > 0) && ((_sz) <= DPACK_BINSZ_MAX), \
	             _DPACK_BIN_CONST_SIZE(_sz), \
	             "invalid constant bin size")

/**
 * Return size of a serialized bin
 *
 * @param[in] _sz deserialized bin size
 *
 * Given the size of a deserialized bin, compute the size of the corresponding
 * bin encoded according to the @rstsubst{MessagePack bin format}.
 *
 * @note
 * Use this function when @p _sz is known at compile time. Use dpack_bin_size()
 * otherwise.
 *
 * @warning
 * Size value MUST be known at compile time, i.e., constant. Trigger a compile
 * time error otherwise.
 *
 * @see
 * dpack_bin_size()
 */
#define DPACK_BIN_SIZE(_sz) \
	compile_eval(__builtin_constant_p(_sz), \
	             DPACK_BIN_CONST_SIZE(_sz), \
	             "constant bin size expected")

/**
 * Return size of a serialized bin
 *
 * @param[in] size deserialized bin size
 *
 * Given the size of a deserialized bin, compute the size of the corresponding
 * bin encoded according to the @rstsubst{MessagePack bin format}.
 *
 * @note
 * Use this function when @p size is not known at compile time. Use
 * #DPACK_BIN_SIZE otherwise.
 *
 * @see
 * #DPACK_BIN_SIZE
 */
extern size_t
dpack_bin_size(size_t size) __dpack_const
                            __dpack_nothrow
                            __leaf
                            __warn_result
                            __dpack_export;

/**
 * Encode a bin according to the MessagePack format
 *
 * @param[inout] encoder encoder
 * @param[in]    value   bin to encode
 * @param[in]    size    size of bin to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value bin into the buffer assigned to @p
 * encoder at initialization time according to the
 * @rstsubst{MessagePack bin format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   bin @p size is zero or greater than #DPACK_BINSZ_MAX, result is undefined.
 *   An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_bin(struct dpack_encoder * encoder,
                 const char *           value,
                 size_t                 size) __dpack_nonull(1, 2)
                                              __dpack_nothrow
                                              __leaf
                                              __warn_result
                                              __dpack_export;

/**
 * Decode and allocate a bin encoded according to the MessagePack format
 *
 * @param[inout] decoder decoder
 * @param[out]   value   location where to store pointer to allocated bin
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack bin format} into @p value bin from buffer assigned to
 * @p decoder at initialization time.
 *
 * The decoded bin will be allocated using @man{malloc(3)}. A pointer to the bin
 * is returned via the @p value argument. The allocated bin should be released
 * using @man{free(3)} once no longer needed.
 *
 * Decoding a bin larger than #DPACK_BINSZ_MAX will cause a ``-EMSGSIZE`` error
 * code to be returned.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p decoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_bincpy()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_bindup(struct dpack_decoder * decoder,
                    char ** __restrict     value) __dpack_nonull(1, 2)
                                                  __dpack_nothrow
                                                  __leaf
                                                  __warn_result
                                                  __dpack_export;

/**
 * Decode and allocate a bin encoded according to the MessagePack format
 * with requested size
 *
 * @param[inout] decoder decoder
 * @param[in]    size    expected size of decoded bin
 * @param[out]   value   location where to store pointer to allocated bin
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack bin format} into @p value bin from buffer assigned to
 * @p decoder at initialization time.
 *
 * Decoding fails with a ``-EMSGSIZE`` error code when size of the decoded bin
 * is different from the specified @p size value.
 *
 * The decoded bin will be allocated using @man{malloc(3)}. A pointer to the
 * bin is returned via the @p value argument. The allocated bin should be
 * released using @man{free(3)} once no longer needed.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p decoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p size value is zero or greater than #DPACK_BINSZ_MAX, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_bincpy_equ()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_bindup_equ(struct dpack_decoder * decoder,
                        size_t                 size,
                        char ** __restrict     value) __dpack_nonull(1, 3)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;

/**
 * Decode and allocate a bin encoded according to the MessagePack format with
 * requested maximum size
 *
 * @param[inout] decoder decoder
 * @param[in]    max_sz  maximum size of decoded bin
 * @param[out]   value   location where to store pointer to allocated bin
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack bin format} into @p value bin from buffer assigned to
 * @p decoder at initialization time.
 *
 * Decoding fails with a ``-EMSGSIZE`` error code when size of the decoded bin
 * is larger than the specified @p max_sz value.
 *
 * The decoded bin will be allocated using @man{malloc(3)}. A pointer to the
 * bin is returned via the @p value argument. The allocated bin should be
 * released using @man{free(3)} once no longer needed.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p decoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p max_siz value is zero or greater than #DPACK_BINSZ_MAX, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_bincpy()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_bindup_max(struct dpack_decoder * decoder,
                        size_t                 max_sz,
                        char ** __restrict     value) __dpack_nonull(1, 3)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;

/**
 * Decode and allocate a bin encoded according to the MessagePack format
 * with requested minimum and maximum size
 *
 * @param[inout] decoder decoder
 * @param[in]    min_sz  minimum size of decoded bin
 * @param[in]    max_sz  maximum size of decoded bin
 * @param[out]   value   location where to store pointer to allocated bin
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack bin format} into @p value bin from buffer
 * assigned to @p decoder at initialization time.
 *
 * Decoding fails with a ``-EMSGSIZE`` error code when size of the decoded
 * bin:
 * - is smaller than the specified @p min_sz value,
 * - or larger than the specified @p max_sz value.
 *
 * The decoded bin will be allocated using @man{malloc(3)}. A pointer to the
 * bin is returned via the @p value argument. The allocated bin should be
 * released using @man{free(3)} once no longer needed.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p decoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p min_sz value is zero or greater than @p max_sz, result is undefined.
 *   An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p max_sz value is ``<= 1`` or greater than #DPACK_BINSZ_MAX, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_bincpy_range()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_bindup_range(struct dpack_decoder * decoder,
                          size_t                 min_sz,
                          size_t                 max_sz,
                          char ** __restrict     value) __dpack_nonull(1, 4)
                                                        __dpack_nothrow
                                                        __leaf
                                                        __warn_result
                                                        __dpack_export;

/**
 * Decode a bin encoded according to the MessagePack format
 *
 * @param[inout] decoder decoder
 * @param[in]    size    size of decoded bin storage area
 * @param[out]   value   location where to store decoded bin
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack bin format} into @p value from buffer assigned to
 * @p decoder at initialization time.
 *
 * The decoded bin will be stored into the memory area specified by @p value.
 * @p value should point to a memory area of size @p size bytes and should be
 * large enough to hold at most #DPACK_BINSZ_MAX bytes of data.
 *
 * Decoding a bin larger than @p size will cause a ``-EMSGSIZE`` error code to
 * be returned.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p decoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p size is zero or greater than ``DPACK_BINSZ_MAX``, result is undefined.
 *   An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_bindup()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_bincpy(struct dpack_decoder * decoder,
                    size_t                 size,
                    char *                 value) __dpack_nonull(1, 3)
                                                  __dpack_nothrow
                                                  __leaf
                                                  __warn_result
                                                  __dpack_export;

/**
 * Decode a bin encoded according to the MessagePack format with requested size.
 *
 * @param[inout] decoder decoder
 * @param[in]    size    expected size of decoded bin
 * @param[out]   value   location where to store decoded bin
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack bin format} into @p value from buffer assigned to
 * @p decoder at initialization time.
 *
 * The decoded bin will be stored into the memory area specified by @p value.
 * @p value should point to a memory area of size @p size bytes or more.
 *
 * Decoding a bin which size is different from @p size will cause a
 * ``-EMSGSIZE`` error code to be returned.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p decoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p size is zero or greater than ``DPACK_BINSZ_MAX``, result is undefined.
 *   An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_bindup_equ()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_bincpy_equ(struct dpack_decoder * decoder,
                        size_t                 size,
                        char *                 value) __dpack_nonull(1, 3)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;

/**
 * Decode a bin encoded according to the MessagePack format with requested
 * minimum and maximum size.
 *
 * @param[inout] decoder decoder
 * @param[in]    min_sz  minimum size of decoded bin
 * @param[in]    max_sz  maximum size of decoded bin
 * @param[out]   value   location where to store decoded bin
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -ENOMSG   Invalid MessagePack stream data type
 * @retval -EMSGSIZE Not enough space to complete operation
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack bin format} into @p value from buffer assigned to
 * @p decoder at initialization time.
 *
 * The decoded bin will be stored into the memory area specified by @p value.
 * @p value should point to a memory area of size @p max_sz bytes or more.
 *
 * Decoding a bin which size is smaller than @p min_sz or greater than
 * @p max_sz will cause a ``-EMSGSIZE`` error code to be returned.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p decoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p min_sz is zero or greater than or equal to @p max_sz, result is
 *   undefined.  An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p max_size value is greater than #DPACK_BINSZ_MAX, result is undefined. An
 *   assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_bindup_range()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_bincpy_range(struct dpack_decoder * decoder,
                          size_t                 min_sz,
                          size_t                 max_sz,
                          char *                 value) __dpack_nonull(1, 4)
                                                        __dpack_nothrow
                                                        __leaf
                                                        __warn_result
                                                        __dpack_export;

#endif /* _DPACK_BIN_H */
