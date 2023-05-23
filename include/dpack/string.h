/**
 * @file
 * String encoding / decoding interface
 *
 * @author       Grégor Boirie <gregor.boirie@free.fr>
 * @date         10 May 2023
 * @copyright    Copyright (C) 2017-2023 Grégor Boirie.
 * @licensestart GNU Lesser General Public License (LGPL) v3
 *
 * This file is part of libdpack
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, If not, see <http://www.gnu.org/licenses/>.
 * @licenseend
 */
#ifndef _DPACK_STRING_H
#define _DPACK_STRING_H

#include <dpack/cdefs.h>
#include <dpack/mpack.h>

struct dpack_encoder;
struct dpack_decoder;

/**
 * Maximum length of a string
 *
 * Maximum length in bytes of a string excluding the terminating NULL byte.
 */
#define DPACK_STRLEN_MAX     STROLL_CONCAT(CONFIG_DPACK_STRING_MAXLEN, U)

/* Maximum number of characters an msgpack fixstr may encode */
#define DPACK_FIXSTR_LEN_MAX (31U)
/* Maximum number of characters an 8 bits msgpack string may encode */
#define DPACK_STR8_LEN_MAX   UINT8_MAX
/* Maximum number of characters an 16 bits msgpack string may encode */
#define DPACK_STR16_LEN_MAX  UINT16_MAX
/* Maximum number of characters an 32 bits msgpack string may encode */
#define DPACK_STR32_LEN_MAX  UINT32_MAX

/*
 * Check DPACK_STRLEN_MAX definition is sensible.
 * Multiple dpack internal functions (such as dpack_decode_str_tag() for
 * example) return string length using a ssize_t, effectively restricting
 * maximum length to a SSIZE_MAX...
 */
#if DPACK_STRLEN_MAX > SSIZE_MAX
#error dpack cannot encode strings which length is > SSIZE_MAX !
#elif DPACK_STRLEN_MAX < 16U
#error Huh ?!
#endif

/******************************************************************************
 * Msgpack fixstr definitions
 ******************************************************************************/

/* Compute size of an encoded msgpack fixstr */
#define DPACK_FIXSTR_SIZE(_len) \
	(MPACK_TAG_SIZE_FIXSTR + (_len))

#define _DPACK_STR_CONST_SIZE(_len) \
	DPACK_FIXSTR_SIZE(_len)

/******************************************************************************
 * Msgpack 8 bits string definitions
 ******************************************************************************/

#if DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX

/* Compute size of an encoded 8 bits msgpack string */
#define DPACK_STR8_SIZE(_len) \
	(MPACK_TAG_SIZE_STR8 + (_len))

/* Size of an encoded string when length fits into an msgpack 8 bits string. */
#define DPACK_STR8_CONST_SIZE(_len) \
	(((_len) > DPACK_FIXSTR_LEN_MAX) ? DPACK_STR8_SIZE(_len) : \
	                                   DPACK_FIXSTR_SIZE(_len))

#undef _DPACK_STR_CONST_SIZE
#define _DPACK_STR_CONST_SIZE(_len) \
	DPACK_STR8_CONST_SIZE(_len)

#endif /* DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX */

/******************************************************************************
 * Msgpack 16 bits string definitions
 ******************************************************************************/

#if DPACK_STRLEN_MAX > DPACK_STR8_LEN_MAX

/* Compute size of an encoded 16 bits msgpack string */
#define DPACK_STR16_SIZE(_len) \
	(MPACK_TAG_SIZE_STR16 + (_len))

/* Size of an encoded string when length fits into an msgpack 16 bits string. */
#define DPACK_STR16_CONST_SIZE(_len) \
	(((_len) > DPACK_STR8_LEN_MAX) ? DPACK_STR16_SIZE(_len) : \
	                                 DPACK_STR8_CONST_SIZE(_len))

#undef _DPACK_STR_CONST_SIZE
#define _DPACK_STR_CONST_SIZE(_len) \
	DPACK_STR16_CONST_SIZE(_len)

#endif /* DPACK_STRLEN_MAX > DPACK_STR8_LEN_MAX */

/******************************************************************************
 * Msgpack 32 bits string definitions
 ******************************************************************************/

#if DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX

/* Compute size of an encoded 32 bits msgpack string */
#define DPACK_STR32_SIZE(_len) \
	(MPACK_TAG_SIZE_STR32 + (_len))

/* Size of an encoded string when length fits into an msgpack 32 bits string. */
#define DPACK_STR32_CONST_SIZE(_len) \
	(((_len) > DPACK_STR16_LEN_MAX) ? DPACK_STR32_SIZE(_len) : \
	                                  DPACK_STR16_CONST_SIZE(_len))

#undef _DPACK_STR_CONST_SIZE
#define _DPACK_STR_CONST_SIZE(_len) \
	DPACK_STR32_CONST_SIZE(_len)

#endif /* DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX */

/******************************************************************************
 * Top-level string size definitions
 ******************************************************************************/

/*
 * Given the length of a string, compute the size of the corresponding encoded
 * msgpack string.
 *
 * Length value MUST be known at compile time, i.e., constant. Result is
 * unpredictable otherwise.
 */
#define DPACK_STR_CONST_SIZE(_len) \
	compile_eval(((_len) > 0) && ((_len) <= DPACK_STRLEN_MAX), \
	             _DPACK_STR_CONST_SIZE(_len), \
	             "invalid constant string length")

/**
 * Return size of a serialized string
 *
 * @param[in] _len deserialized string length
 *
 * Given the length of a deserialized string excluding the terminating NULL
 * byte, compute the size of the corresponding string encoded according to the
 * @rstsubst{MessagePack string format}.
 *
 * @note
 * Use this function when @p _len is known at compile time. Use dpack_str_size()
 * otherwise.
 *
 * @warning
 * Length value MUST be known at compile time, i.e., constant. Trigger a compile
 * time error otherwise.
 *
 * @see
 * dpack_str_size()
 */
#define DPACK_STR_SIZE(_len) \
	compile_eval(__builtin_constant_p(_len), \
	             DPACK_STR_CONST_SIZE(_len), \
	             "constant string length expected")

/**
 * Return size of a serialized string
 *
 * @param[in] len deserialized string length
 *
 * Given the length of a deserialized string excluding the terminating NULL
 * byte, compute the size of the corresponding string encoded according to the
 * @rstsubst{MessagePack string format}.
 *
 * @note
 * Use this function when @p len is not known at compile time. Use
 * #DPACK_STR_SIZE otherwise.
 *
 * @see
 * #DPACK_STR_SIZE
 */
extern size_t
dpack_str_size(size_t len) __dpack_const
                           __dpack_nothrow
                           __leaf
                           __warn_result
                           __dpack_export;

/**
 * Encode a string according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   string to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the NULL byte terminated string @p value into the
 * buffer assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack string format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p value string length is zero or greater than #DPACK_STRLEN_MAX, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_str_fix()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_str(struct dpack_encoder  * encoder,
                 const char * __restrict value) __dpack_nonull(1, 2)
                                                __dpack_nothrow
                                                __leaf
                                                __warn_result
                                                __dpack_export;

/**
 * Encode a string according to the MessagePack format
 *
 * @param[in] encoder encoder
 * @param[in] value   string to encode
 * @param[in] len     length of @p value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value string into the buffer assigned to @p
 * encoder at initialization time according to the
 * @rstsubst{MessagePack string format}.
 *
 * This function is similar to dpack_encode_str() except that at most @p len
 * bytes of @p value are encoded.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p len is zero or greater than #DPACK_STRLEN_MAX, result is undefined. An
 *   assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p value string length is different from @p len argument, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_str()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_str_fix(struct dpack_encoder  * encoder,
                     const char * __restrict value,
                     size_t                  len) __dpack_nonull(1, 2)
                                                  __dpack_nothrow
                                                  __leaf
                                                  __warn_result
                                                  __dpack_export;

/**
 * Decode and allocate a string encoded according to the MessagePack format
 *
 * @param[in]  decoder decoder
 * @param[out] value   location where to store pointer to allocated string
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -EBADMSG  Invalid MessagePack string data
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack string format} into @p value string from buffer
 * assigned to @p decoder at initialization time.
 *
 * The decoded string will be allocated using @man{malloc(3)}. A pointer to the
 * string is returned via the @p value argument. The allocated string should be
 * released using @man{free(3)} once no longer needed.
 *
 * The allocated string is guaranteed to be ``NULL`` terminated.
 * 
 * Decoding a string longer than #DPACK_STRLEN_MAX will cause a ``-EMSGSIZE``
 * error code to be returned.
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
 * - dpack_decode_strcpy()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_strdup(struct dpack_decoder * decoder,
                    char ** __restrict     value) __dpack_nonull(1, 2)
                                                  __dpack_nothrow
                                                  __leaf
                                                  __warn_result
                                                  __dpack_export;

/**
 * Decode and allocate a string encoded according to the MessagePack format
 * with requested length
 *
 * @param[in]  decoder decoder
 * @param[in]  len     expected length of decoded string
 * @param[out] value   location where to store pointer to allocated string
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -EBADMSG  Invalid MessagePack string data
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack string format} into @p value string from buffer
 * assigned to @p decoder at initialization time.
 *
 * Decoding fails with a ``-EMSGSIZE`` error code when length of the decoded
 * string is different from the specified @p len value.
 *
 * The decoded string will be allocated using @man{malloc(3)}. A pointer to the
 * string is returned via the @p value argument. The allocated string should be
 * released using @man{free(3)} once no longer needed.
 *
 * The allocated string is guaranteed to be ``NULL`` terminated.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p decoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p len value is zero or greater than #DPACK_STRLEN_MAX, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_strcpy_equ()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_strdup_equ(struct dpack_decoder * decoder,
                        size_t                 len,
                        char ** __restrict     value) __dpack_nonull(1, 3)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;

/**
 * Decode and allocate a string encoded according to the MessagePack format
 * with requested maximum length
 *
 * @param[in]  decoder decoder
 * @param[in]  max_len maximum length of decoded string
 * @param[out] value   location where to store pointer to allocated string
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -EBADMSG  Invalid MessagePack string data
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack string format} into @p value string from buffer
 * assigned to @p decoder at initialization time.
 *
 * Decoding fails with a ``-EMSGSIZE`` error code when length of the decoded
 * string is larger than the specified @p max_len value.
 *
 * The decoded string will be allocated using @man{malloc(3)}. A pointer to the
 * string is returned via the @p value argument. The allocated string should be
 * released using @man{free(3)} once no longer needed.
 *
 * The allocated string is guaranteed to be ``NULL`` terminated.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p decoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p max_len value is ``<= 1`` or greater than #DPACK_STRLEN_MAX, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_strcpy()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_strdup_max(struct dpack_decoder * decoder,
                        size_t                 max_len,
                        char ** __restrict     value) __dpack_nonull(1, 3)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;

/**
 * Decode and allocate a string encoded according to the MessagePack format
 * with requested minimum and maximum length
 *
 * @param[in]  decoder decoder
 * @param[in]  min_len minimum length of decoded string
 * @param[in]  max_len maximum length of decoded string
 * @param[out] value   location where to store pointer to allocated string
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -EBADMSG  Invalid MessagePack string data
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack string format} into @p value string from buffer
 * assigned to @p decoder at initialization time.
 *
 * Decoding fails with a ``-EMSGSIZE`` error code when length of the decoded
 * string:
 * - is smaller than the specified @p min_len value,
 * - or larger than the specified @p max_len value.
 *
 * The decoded string will be allocated using @man{malloc(3)}. A pointer to the
 * string is returned via the @p value argument. The allocated string should be
 * released using @man{free(3)} once no longer needed.
 *
 * The allocated string is guaranteed to be ``NULL`` terminated.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p decoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p min_len value is zero or greater than @p max_len, result is undefined.
 *   An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p max_len value is ``<= 1`` or greater than #DPACK_STRLEN_MAX, result is
 *   undefined.  An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_strcpy_range()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_strdup_range(struct dpack_decoder * decoder,
                          size_t                 min_len,
                          size_t                 max_len,
                          char ** __restrict     value) __dpack_nonull(1, 4)
                                                        __dpack_nothrow
                                                        __leaf
                                                        __warn_result
                                                        __dpack_export;

/**
 * Decode a string encoded according to the MessagePack format
 *
 * @param[in]  decoder decoder
 * @param[in]  size    size of decoded string storage area
 * @param[out] value   location where to store decoded string
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -EBADMSG  Invalid MessagePack string data
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack string format} into @p value string from buffer
 * assigned to @p decoder at initialization time.
 *
 * The decoded string will be stored into the memory area specified by @p value.
 * @p value should point to a memory area of size @p size bytes and should be
 * large enough to hold a string which size is at most ``#DPACK_STRLEN_MAX + 1``
 * bytes long including the usual terminating ``NULL`` byte.
 * 
 * Decoding a string which length is larger than #DPACK_STRLEN_MAX will cause a
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
 *   @p size is ``<= 1`` or greater than ``DPACK_STRLEN_MAX + 1``, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_strdup()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_strcpy(struct dpack_decoder * decoder,
                    size_t                 size,
                    char *                 value) __dpack_nonull(1, 3)
                                                  __dpack_nothrow
                                                  __leaf
                                                  __warn_result
                                                  __dpack_export;

/**
 * Decode a string encoded according to the MessagePack format with requested
 * size.
 *
 * @param[in]  decoder decoder
 * @param[in]  size    expected size of decoded string
 * @param[out] value   location where to store decoded string
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -EBADMSG  Invalid MessagePack string data
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack string format} into @p value string from buffer
 * assigned to @p decoder at initialization time.
 *
 * The decoded string will be stored into the memory area specified by @p value
 * including the usual terminating ``NULL`` byte. @p value should point to a
 * memory area of size @p size bytes long.
 * 
 * Decoding a string which length is different from ``size - 1`` will cause a
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
 *   @p size is ``<= 1`` or greater than ``DPACK_STRLEN_MAX + 1``, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_strdup_equ()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_strcpy_equ(struct dpack_decoder * decoder,
                        size_t                 size,
                        char *                 value) __dpack_nonull(1, 3)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;

/**
 * Decode a string encoded according to the MessagePack format with requested
 * minimum length and maximum size.
 *
 * @param[in]  decoder  decoder
 * @param[in]  min_len  minimum length of decoded string
 * @param[in]  max_size maximum size of decoded string
 * @param[out] value    location where to store decoded string
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EPROTO   Not a valid MessagePack stream
 * @retval -ENOTSUP  Unsupported MessagePack stream data
 * @retval -EBADMSG  Invalid MessagePack string data
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Decode / unpack / deserialize data item encoded according to the
 * @rstsubst{MessagePack string format} into @p value string from buffer
 * assigned to @p decoder at initialization time.
 *
 * The decoded string will be stored into the memory area specified by @p value
 * including the usual terminating ``NULL`` byte.  @p value should point to a
 * memory area of size @p max_size.
 * 
 * Decoding a string which length is smaller than @p min_len or greater than
 * ``max_size - 1`` will cause a ``-EMSGSIZE`` error code to be returned.
 *
 * @warning
 * - @p decoder *MUST* have been initialized using dpack_decoder_init_buffer()
 *   or dpack_decoder_init_skip_buffer() before calling this function. Result is
 *   undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p decoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
  * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p min_len value is zero or greater than ``max_size - 1``, result is
 *   undefined.  An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p max_size value is ``<= 2`` or greater than ``DPACK_STRLEN_MAX + 1``,
 *   result is undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decode_strdup_range()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
extern ssize_t
dpack_decode_strcpy_range(struct dpack_decoder * decoder,
                          size_t                 min_len,
                          size_t                 max_size,
                          char *                 value) __dpack_nonull(1, 4)
                                                        __dpack_nothrow
                                                        __leaf
                                                        __warn_result
                                                        __dpack_export;

#endif /* _DPACK_STRING_H */
