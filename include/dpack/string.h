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
#include <stdint.h>
#include <limits.h>
#include <sys/types.h>

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

extern ssize_t
dpack_decode_strdup(struct dpack_decoder  * decoder,
                    char                 ** value) __dpack_export;

extern ssize_t
dpack_decode_strdup_max(struct dpack_decoder  * decoder,
                        size_t                  max_len,
                        char                 ** value) __dpack_export;

extern ssize_t
dpack_decode_strdup_range(struct dpack_decoder  * decoder,
                          size_t                  min_len,
                          size_t                  max_len,
                          char                 ** value) __dpack_export;

extern ssize_t
dpack_decode_strdup_fix(struct dpack_decoder  * decoder,
                        size_t                  len,
                        char                 ** value) __dpack_export;

extern ssize_t
dpack_decode_strcpy(struct dpack_decoder * decoder,
                    size_t                 size,
                    char                 * value) __dpack_export;
extern ssize_t
dpack_decode_strcpy_range(struct dpack_decoder * decoder,
                          size_t                 min_len,
                          size_t                 size,
                          char                 * value) __dpack_export;

extern ssize_t
dpack_decode_strcpy_fix(struct dpack_decoder * decoder,
                        size_t                 size,
                        char                 * value) __dpack_export;

#endif /* _DPACK_STRING_H */
