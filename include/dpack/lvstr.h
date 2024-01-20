/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Length-Value String encoding / decoding interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      10 Apr 2023
 * @copyright Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _DPACK_LVSTR_H
#define _DPACK_LVSTR_H

#include <dpack/string.h>
#include <stroll/lvstr.h>

/**
 * Maximum length of a lvstr
 *
 * Maximum length in bytes of a @rstlnk{lvstr}.
 */
#define DPACK_LVSTRLEN_MAX \
	STROLL_CONST_MIN(DPACK_STRLEN_MAX, STROLL_LVSTR_LEN_MAX)

/* Check DPACK_LVSTRLEN_MAX definition is sensible. */
#if DPACK_LVSTRLEN_MAX < 16U
#error dpack cannot encode lvstr which length is < 16 !
#endif

/******************************************************************************
 * Msgpack fixstr definitions
 ******************************************************************************/

#define _DPACK_LVSTR_CONST_SIZE(_len) \
	DPACK_FIXSTR_SIZE(_len)

/******************************************************************************
 * Msgpack 8 bits lvstr definitions
 ******************************************************************************/

#if DPACK_LVSTRLEN_MAX > DPACK_FIXSTR_LEN_MAX

#undef _DPACK_LVSTR_CONST_SIZE
#define _DPACK_LVSTR_CONST_SIZE(_len) \
	DPACK_STR8_CONST_SIZE(_len)

#endif /* DPACK_LVSTRLEN_MAX > DPACK_FIXSTR_LEN_MAX */

/******************************************************************************
 * Msgpack 16 bits lvstr definitions
 ******************************************************************************/

#if DPACK_LVSTRLEN_MAX > DPACK_STR8_LEN_MAX

#undef _DPACK_LVSTR_CONST_SIZE
#define _DPACK_LVSTR_CONST_SIZE(_len) \
	DPACK_STR16_CONST_SIZE(_len)

#endif /* DPACK_LVSTRLEN_MAX > DPACK_STR8_LEN_MAX */

/******************************************************************************
 * Msgpack 32 bits lvstr definitions
 ******************************************************************************/

#if DPACK_LVSTRLEN_MAX > DPACK_STR16_LEN_MAX

#undef _DPACK_LVSTR_CONST_SIZE
#define _DPACK_LVSTR_CONST_SIZE(_len) \
	DPACK_STR32_CONST_SIZE(_len)

#endif /* DPACK_LVSTRLEN_MAX > DPACK_STR16_LEN_MAX */

/******************************************************************************
 * Top-level lvstr size definitions
 ******************************************************************************/

/*
 * Given the length of a lvstr, compute the size of the corresponding encoded
 * msgpack string.
 *
 * Length value MUST be known at compile time, i.e., constant. Result is
 * unpredictable otherwise.
 */
#define DPACK_LVSTR_CONST_SIZE(_len) \
	compile_eval(((_len) > 0) && \
	             ((_len) <= DPACK_LVSTRLEN_MAX), \
	             _DPACK_LVSTR_CONST_SIZE(_len), \
	             "invalid constant lvstr length")

/**
 * Return size of a serialized lvstr
 *
 * @param[in] _len deserialized lvstr length
 *
 * Given the length of a deserialized @rstlnk{lvstr}, compute the size of the
 * corresponding string encoded according to the
 * @rstsubst{MessagePack string format}.
 *
 * @note
 * Use this function when @p _len is known at compile time. Use
 * dpack_lvstr_size() otherwise.
 *
 * @warning
 * Length value MUST be known at compile time, i.e., constant. Trigger a compile
 * time error otherwise.
 *
 * @see
 * dpack_lvstr_size()
 */
#define DPACK_LVSTR_SIZE(_len) \
	compile_eval(__builtin_constant_p(_len), \
	             DPACK_LVSTR_CONST_SIZE(_len), \
	             "constant lvstr length expected")

/**
 * Return size of a serialized lvstr
 *
 * @param[in] len deserialized lvstr length
 *
 * Given the length of a deserialized @rstlnk{lvstr}, compute the size of the
 * corresponding string encoded according to the
 * @rstsubst{MessagePack string format}.
 *
 * @note
 * Use this function when @p len is not known at compile time. Use
 * #DPACK_LVSTR_SIZE otherwise.
 *
 * @see
 * #DPACK_LVSTR_SIZE
 */
extern size_t
dpack_lvstr_size(size_t len) __dpack_const
                             __dpack_nothrow
                             __leaf
                             __warn_result
                             __dpack_export;

/**
 * Encode a lvstr according to the MessagePack string format
 *
 * @param[inout] encoder encoder
 * @param[in]    value   lvstr to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize @rstlnk{lvstr} @p value into the
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
 *   @p value lvstr is not initialized or initialized with a ``NULL`` C string,
 *   result is undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p value lvstr length is zero or greater than #DPACK_LVSTRLEN_MAX, result
 *   is undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_encode_lvstr_range()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_encode_lvstr(struct dpack_encoder *      encoder,
                   const struct stroll_lvstr * value) __dpack_nonull(1, 2)
                                                      __dpack_nothrow
                                                      __leaf
                                                      __warn_result
                                                      __dpack_export;

extern int
dpack_decode_lvstr_range(struct dpack_decoder * decoder,
                         size_t                 min_len,
                         size_t                 max_len,
                         struct stroll_lvstr *  value) __dpack_export;

static inline int
dpack_decode_lvstr(struct dpack_decoder * decoder,
                   struct stroll_lvstr *  value)
{
	return dpack_decode_lvstr_range(decoder, 1, DPACK_LVSTRLEN_MAX, value);
}

static inline int
dpack_decode_lvstr_max(struct dpack_decoder * decoder,
                       size_t                 max_len,
                       struct stroll_lvstr *  value)
{
	return dpack_decode_lvstr_range(decoder, 1, max_len, value);
}

extern int
dpack_decode_lvstr_fix(struct dpack_decoder * decoder,
                       size_t                 len,
                       struct stroll_lvstr *  value) __dpack_export;

#endif /* _DPACK_LVSTR_H */
