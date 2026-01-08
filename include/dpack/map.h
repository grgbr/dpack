/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Map encoding / decoding interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      15 Mar 2023
 * @copyright Copyright (C) 2023-2024 Grégor Boirie <gregor.boirie@free.fr>
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _DPACK_MAP_H
#define _DPACK_MAP_H

#include <dpack/codec.h>
#include <dpack/scalar.h>

/**
 * Maximum number of fields of a dpack map
 */
#define DPACK_MAP_FLDNR_MAX (128U)

/**
 * Maximum size of a dpack map field
 */
#define DPACK_MAP_FLDSZ_MAX (1024U)

#if DPACK_STDINT_SIZE_MAX > DPACK_MAP_FLDSZ_MAX
#error DPack map field cannot hold a single scalar element, \
       increase DPACK_MAP_FLDSZ_MAX !
#endif /* DPACK_STDINT_SIZE_MAX > DPACK_MAP_FLDSZ_MAX */

#if defined(CONFIG_DPACK_FLOAT)

#if DPACK_FLOAT_SIZE > DPACK_MAP_FLDSZ_MAX
#error DPack map field cannot hold a single float element, \
       increase DPACK_MAP_FLDSZ_MAX !
#endif /* DPACK_FLOAT_SIZE > DPACK_MAP_FLDSZ_MAX */

#endif /* defined(CONFIG_DPACK_FLOAT) */

#if defined(CONFIG_DPACK_DOUBLE)

#if DPACK_DOUBLE_SIZE > DPACK_MAP_FLDSZ_MAX
#error DPack map field cannot hold a single double element, \
       increase DPACK_MAP_FLDSZ_MAX !
#endif /* DPACK_DOUBLE_SIZE > DPACK_MAP_FLDSZ_MAX */

#endif /* defined(CONFIG_DPACK_DOUBLE) */

#if defined(CONFIG_DPACK_STRING)

#include <dpack/string.h>

#if _DPACK_STRSZ_MAX_MIN > DPACK_MAP_FLDSZ_MAX
#error DPack map field cannot hold a single string element, \
       increase DPACK_MAP_FLDSZ_MAX !
#endif /* _DPACK_STRSZ_MAX_MIN > DPACK_MAP_FLDSZ_MAX */

#endif /* defined(CONFIG_DPACK_STRING) */

#if defined(CONFIG_DPACK_BIN)

#include <dpack/bin.h>

#if _DPACK_BINSZ_MAX_MIN > DPACK_MAP_FLDSZ_MAX
#error DPack map field cannot hold a single bin element, \
       increase DPACK_MAP_FLDSZ_MAX !
#endif /* _DPACK_BINSZ_MAX_MIN > DPACK_MAP_FLDSZ_MAX */

#endif /* defined(CONFIG_DPACK_BIN) */

/* Maximum number of fields an msgpack fixmap may encode */
#define _DPACK_FIXMAP_FLDNR_MAX (15U)
/* Maximum number of fields a 16 bits msgpack map may encode */
#define _DPACK_MAP16_FLDNR_MAX  UINT16_MAX
/* Maximum number of fields a 32 bits msgpack map may encode */
#define _DPACK_MAP32_FLDNR_MAX  UINT32_MAX

/*
 * Check DPACK_MAP_FLDNR_MAX definition is sensible.
 *
 * Only allow half of the highest msgpack capacity since we need to multiply
 * this number by 2 when handling map discard operations.
 * DO NOT TOUCH THIS UPPER LIMIT.
 */
#if DPACK_MAP_FLDNR_MAX > (_DPACK_MAP32_FLDNR_MAX / 2)
#error DPack cannot encode maps containing more than (UINT32_MAX / 2) fields !
#elif DPACK_MAP_FLDNR_MAX < 4U
#error Huh ?!
#endif

/******************************************************************************
 * Msgpack fixmap definitions
 ******************************************************************************/

#define DPACK_FIXMAP_TAG_SIZE 1

#define __DPACK_MAP_HEAD_SIZE(_fld_nr) \
	MPACK_TAG_SIZE_FIXMAP

/******************************************************************************
 * Msgpack 16 bits map definitions
 ******************************************************************************/

#if DPACK_MAP_FLDNR_MAX > _DPACK_FIXMAP_FLDNR_MAX

#define DPACK_MAP16_TAG_SIZE 3

#define _DPACK_MAP16_HEAD_SIZE(_fld_nr) \
	(((_fld_nr) > _DPACK_FIXMAP_FLDNR_MAX) ? \
	 MPACK_TAG_SIZE_MAP16 : \
	 MPACK_TAG_SIZE_FIXMAP)

#undef __DPACK_MAP_HEAD_SIZE
#define __DPACK_MAP_HEAD_SIZE(_fld_nr) \
	_DPACK_MAP16_HEAD_SIZE(_fld_nr)

#endif /* DPACK_MAP_FLDNR_MAX > _DPACK_FIXMAP_FLDNR_MAX */

/******************************************************************************
 * Msgpack 32 bits map definitions
 ******************************************************************************/

#if DPACK_MAP_FLDNR_MAX > _DPACK_MAP16_FLDNR_MAX

#define DPACK_MAP32_TAG_SIZE 5

#define _DPACK_MAP32_HEAD_SIZE(_fld_nr) \
	(((_fld_nr) > _DPACK_MAP16_FLDNR_MAX) ? \
	 MPACK_TAG_SIZE_MAP32 : \
	 _DPACK_MAP16_HEAD_SIZE(_fld_nr))

#undef __DPACK_MAP_HEAD_SIZE
#define __DPACK_MAP_HEAD_SIZE(_fld_nr) \
	_DPACK_MAP32_HEAD_SIZE(_fld_nr)

#endif /* DPACK_MAP_FLDNR_MAX > _DPACK_MAP16_FLDNR_MAX */

/******************************************************************************
 * Top-level map size definitions
 ******************************************************************************/

/**
 * Minimum size of a dpack map field identifier
 *
 * @see
 * - DPACK_MAP_DATA_SIZE_MAX
 * - DPACK_MAP_HEAD_SIZE()
 * - DPACK_MAP_FLDID_SIZE_MAX
 * - DPACK_MAP_FLDNR_MAX
 * - DPACK_MAP_FLDSZ_MAX
 * - DPACK_MAP_SIZE()
 * - DPACK_MAP_SIZE_MAX
 * - dpack_map_size()
 */
#define DPACK_MAP_FLDID_SIZE_MIN (DPACK_UINT_SIZE_MIN)

/**
 * Maximum size of a dpack map field identifier
 *
 * @see
 * - DPACK_MAP_DATA_SIZE_MAX
 * - DPACK_MAP_HEAD_SIZE()
 * - DPACK_MAP_FLDID_SIZE_MIN
 * - DPACK_MAP_FLDNR_MAX
 * - DPACK_MAP_FLDSZ_MAX
 * - DPACK_MAP_SIZE()
 * - DPACK_MAP_SIZE_MAX
 * - dpack_map_size()
 */
#define DPACK_MAP_FLDID_SIZE_MAX (DPACK_UINT_SIZE_MAX)

/**
 * Maximum size of a dpack map data block
 *
 * @see
 * - DPACK_MAP_HEAD_SIZE()
 * - DPACK_MAP_FLDID_SIZE_MIN
 * - DPACK_MAP_FLDID_SIZE_MAX
 * - DPACK_MAP_FLDNR_MAX
 * - DPACK_MAP_FLDSZ_MAX
 * - DPACK_MAP_SIZE()
 * - DPACK_MAP_SIZE_MAX
 * - dpack_map_size()
 */
#define DPACK_MAP_DATA_SIZE_MAX \
	(DPACK_MAP_FLDNR_MAX * (DPACK_MAP_FLDID_SIZE_MAX + DPACK_MAP_FLDSZ_MAX))

/**
 * Maximum size of a dpack map
 *
 * @see
 * - DPACK_MAP_DATA_SIZE_MAX
 * - DPACK_MAP_HEAD_SIZE()
 * - DPACK_MAP_FLDID_SIZE_MIN
 * - DPACK_MAP_FLDID_SIZE_MAX
 * - DPACK_MAP_FLDNR_MAX
 * - DPACK_MAP_FLDSZ_MAX
 * - DPACK_MAP_SIZE()
 * - dpack_map_size()
 */
#define DPACK_MAP_SIZE_MAX \
	(__DPACK_MAP_HEAD_SIZE(DPACK_MAP_FLDNR_MAX) + DPACK_MAP_DATA_SIZE_MAX)

#if DPACK_MAP_SIZE_MAX > (128U * 1024 * 1024)
#error DPack cannot encode maps which overall size > 128 MB !
#endif /* DPACK_MAP_SIZE_MAX > (128U * 1024 * 1024) */

#define _DPACK_MAP_HEAD_SIZE(_fld_nr) \
	compile_eval(((_fld_nr) > 0) && \
	             ((_fld_nr) <= DPACK_MAP_FLDNR_MAX), \
	             __DPACK_MAP_HEAD_SIZE(_fld_nr), \
	             "invalid constant map length")

/**
 * Size of a serialized dpack map header.
 *
 * @param[in] _fld_nr number of map fields
 *
 * Given the number of fields an msgpack @rstlnk{map} may contain,
 * compute the size of its header when encoded according to the
 * @rstsubst{MessagePack map format}.
 *
 * @warning
 * Argument value MUST be known at compile time, i.e., constant. This triggers a
 * compile time error otherwise.
 *
 * @see
 * - DPACK_MAP_SIZE()
 * - DPACK_MAP_DATA_SIZE_MAX
 *
 * @see
 * - DPACK_MAP_DATA_SIZE_MAX
 * - DPACK_MAP_FLDID_SIZE_MIN
 * - DPACK_MAP_FLDID_SIZE_MAX
 * - DPACK_MAP_FLDNR_MAX
 * - DPACK_MAP_FLDSZ_MAX
 * - DPACK_MAP_SIZE()
 * - DPACK_MAP_SIZE_MAX
 * - dpack_map_size()
 */
#define DPACK_MAP_HEAD_SIZE(_fld_nr) \
	compile_eval(__builtin_constant_p(_fld_nr), \
	             _DPACK_MAP_HEAD_SIZE(_fld_nr), \
	             "constant map length expected")

#define _DPACK_MAP_SIZE(_fld_nr, _data_size) \
	compile_eval(((_fld_nr) > 0) && \
	             ((_fld_nr) <= DPACK_MAP_FLDNR_MAX) && \
	             ((_data_size) > 0) && \
	             ((_data_size) <= DPACK_MAP_DATA_SIZE_MAX), \
	             (_DPACK_MAP_HEAD_SIZE(_fld_nr) + (_data_size)), \
	             "invalid map length or data size")

/**
 * Size of a serialized dpack map
 *
 * @param[in] _fld_nr    number of map fields
 * @param[in] _data_size size of encoded map data
 *
 * Compute the size of a @rstlnk{map} encoded according to the
 * @rstsubst{MessagePack map format} that holds @p _fld_nr fields and where
 * the cumulated size of all encoded fields is given by @p _data_size.
 *
 * @warning
 * Arguments values MUST be known at compile time, i.e., constant. This triggers
 * a compile time error otherwise.
 *
 * @see
 * - DPACK_MAP_DATA_SIZE_MAX
 * - DPACK_MAP_HEAD_SIZE()
 * - DPACK_MAP_FLDID_SIZE_MIN
 * - DPACK_MAP_FLDID_SIZE_MAX
 * - DPACK_MAP_FLDNR_MAX
 * - DPACK_MAP_FLDSZ_MAX
 * - DPACK_MAP_SIZE_MAX
 * - dpack_map_size()
 */
#define DPACK_MAP_SIZE(_fld_nr, _data_size) \
	compile_eval(__builtin_constant_p(_fld_nr) && \
	             __builtin_constant_p(_data_size), \
	             _DPACK_MAP_SIZE(_fld_nr, _data_size), \
	             "constant map length and data size expected")

/**
 * Size of a serialized dpack map.
 *
 * @param[in] fld_nr    number of map fields
 * @param[in] data_size size of encoded map data
 *
 * @return size of encoded map
 *
 * Compute the size of a @rstlnk{map} encoded according to the
 * @rstsubst{MessagePack map format} that holds @p fld_nr fields and where
 * the cumulated size of all encoded fields is given by @p data_size.
 *
 * @note
 * Use this function when @p fld_nr and / or @p data_size are not known at
 * compile time. Use #DPACK_MAP_SIZE() otherwise.
 *
 * @see
 * - DPACK_MAP_DATA_SIZE_MAX
 * - DPACK_MAP_HEAD_SIZE()
 * - DPACK_MAP_FLDID_SIZE_MIN
 * - DPACK_MAP_FLDID_SIZE_MAX
 * - DPACK_MAP_FLDNR_MAX
 * - DPACK_MAP_FLDSZ_MAX
 * - DPACK_MAP_SIZE()
 * - DPACK_MAP_SIZE_MAX
 */
extern size_t
dpack_map_size(unsigned int fld_nr, size_t data_size)
	__dpack_const __dpack_nothrow __leaf __warn_result __dpack_export;

/******************************************************************************
 * Map encoding
 ******************************************************************************/

/**
 * Start encoding a dpack map.
 *
 * @param[inout] encoder encoder
 * @param[in]    nr      number of map fields
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Start encoding a @rstlnk{map} containing @p nr fields. Call to this function
 * must be followed by @p nr fields. Map encoding must be completed by a call to
 * dpack_map_end_encode().
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p nr is zero or greater than #DPACK_MAP_FLDNR_MAX, result is undefined.
 *   An assertion is triggered otherwise.
 *
 * @see
 * - dpack_map_end_encode()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_begin_encode(struct dpack_encoder * __restrict encoder,
                       unsigned int                      nr)
	__dpack_nonull(1) __warn_result __dpack_export;

/**
 * Complete encoding of a dpack map.
 *
 * @param[inout] encoder encoder
 *
 * Complete encoding of an @rstlnk{map} started using dpack_map_begin_encode().
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   a call to dpack_map_end_encode() has not been preceded by the expected
 *   number of fields, the result of data encoding is undefined. An assertion
 *   is triggered otherwise.
 *
 * @see
 * - dpack_map_begin_encode()
 * - dpack_encoder_init_buffer()
 */
static inline __dpack_nonull(1)
void
dpack_map_end_encode(struct dpack_encoder * __restrict encoder __unused)
{
	dpack_encoder_assert_api(encoder);
}

/******************************************************************************
 * Map field identifiers encoding
 ******************************************************************************/

/**
 * Encode a dpack map field identifier.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p id @rstlnk{map} field identifier into the
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
 * - dpack_map_decode_fldid()
 * - dpack_encoder_init_buffer()
 */
static inline __dpack_nonull(1) __warn_result
int
dpack_map_encode_fldid(struct dpack_encoder * __restrict encoder,
                       unsigned int                      id)
{
	dpack_encoder_assert_api(encoder);

	return dpack_encode_uint(encoder, id);
}

/**
 * Decode a dpack map field identifier.
 *
 * @param[inout] decoder decoder
 * @param[out]   id      location where to store decoded field identifier
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
 * Decode / unpack / deserialize a @rstlnk{map} field identifier encoded
 * according to the @rstsubst{MessagePack int format} into @p id unsigned
 * integer from buffer assigned to @p decoder at initialization time.
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
 * - dpack_map_encode_fldid()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
static inline __dpack_nonull(1, 2) __warn_result
int
dpack_map_decode_fldid(struct dpack_decoder * __restrict decoder,
                       unsigned int * __restrict         id)
{
	dpack_decoder_assert_api(decoder);

	return dpack_decode_uint(decoder, id);
}

/******************************************************************************
 * Map boolean encoding
 ******************************************************************************/

/**
 * Minimum size of an encoded boolean dpack map field.
 */
#define DPACK_MAP_BOOL_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_BOOL_SIZE)

/**
 * Maximum size of an encoded boolean dpack map field.
 */
#define DPACK_MAP_BOOL_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_BOOL_SIZE)

/**
 * Encode a boolean dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value boolean @rstlnk{map} field into the
 * buffer assigned to @p encoder at initialization time according to the
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
 * - DPACK_MAP_BOOL_SIZE_MIN
 * - DPACK_MAP_BOOL_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_bool(struct dpack_encoder * __restrict encoder,
                      unsigned int                      id,
                      bool                              value)
	__dpack_nonull(1) __warn_result __dpack_export;

/******************************************************************************
 * Map 8 bits integers encoding
 ******************************************************************************/

/**
 * Minimum size of an encoded 8-bits unsigned integer dpack map field.
 */
#define DPACK_MAP_UINT8_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_UINT8_SIZE_MIN)

/**
 * Maximum size of an encoded 8-bits unsigned integer dpack map field.
 */
#define DPACK_MAP_UINT8_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_UINT8_SIZE_MAX)

/**
 * Encode an 8-bits unsigned integer dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 8-bits unsigned integer @rstlnk{map}
 * field into the buffer assigned to @p encoder at initialization time according
 * to the @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_UINT8_SIZE_MIN
 * - DPACK_MAP_UINT8_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_uint8(struct dpack_encoder * __restrict encoder,
                       unsigned int                      id,
                       uint8_t                           value)
	__dpack_nonull(1) __warn_result __dpack_export;

/**
 * Minimum size of an encoded 8-bits signed integer dpack map field.
 */
#define DPACK_MAP_INT8_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_INT8_SIZE_MIN)

/**
 * Maximum size of an encoded 8-bits signed integer dpack map field.
 */
#define DPACK_MAP_INT8_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_INT8_SIZE_MAX)

/**
 * Encode an 8-bits signed integer dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 8-bits signed integer @rstlnk{map}
 * field into the buffer assigned to @p encoder at initialization time according
 * to the @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_INT8_SIZE_MIN
 * - DPACK_MAP_INT8_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_int8(struct dpack_encoder * __restrict encoder,
                      unsigned int                      id,
                      int8_t                            value)
	__dpack_nonull(1) __warn_result __dpack_export;

/******************************************************************************
 * Map 16 bits integers encoding
 ******************************************************************************/

/**
 * Minimum size of an encoded 16-bits unsigned integer dpack map field.
 */
#define DPACK_MAP_UINT16_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_UINT16_SIZE_MIN)

/**
 * Maximum size of an encoded 16-bits unsigned integer dpack map field.
 */
#define DPACK_MAP_UINT16_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_UINT16_SIZE_MAX)

/**
 * Encode an 16-bits unsigned integer dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 16-bits unsigned integer @rstlnk{map}
 * field into the buffer assigned to @p encoder at initialization time according
 * to the @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_UINT16_SIZE_MIN
 * - DPACK_MAP_UINT16_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_uint16(struct dpack_encoder * __restrict encoder,
                        unsigned int                      id,
                        uint16_t                          value)
	__dpack_nonull(1) __warn_result __dpack_export;

/**
 * Minimum size of an encoded 16-bits signed integer dpack map field.
 */
#define DPACK_MAP_INT16_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_INT16_SIZE_MIN)

/**
 * Maximum size of an encoded 16-bits signed integer dpack map field.
 */
#define DPACK_MAP_INT16_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_INT16_SIZE_MAX)

/**
 * Encode an 16-bits signed integer dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 16-bits signed integer @rstlnk{map}
 * field into the buffer assigned to @p encoder at initialization time according
 * to the @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_INT16_SIZE_MIN
 * - DPACK_MAP_INT16_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_int16(struct dpack_encoder * __restrict encoder,
                       unsigned int                      id,
                       int16_t                           value)
	__dpack_nonull(1) __warn_result __dpack_export;

/******************************************************************************
 * Map 32 bits integers encoding
 ******************************************************************************/

/**
 * Minimum size of an encoded 32-bits unsigned integer dpack map field.
 */
#define DPACK_MAP_UINT32_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_UINT32_SIZE_MIN)

/**
 * Maximum size of an encoded 32-bits unsigned integer dpack map field.
 */
#define DPACK_MAP_UINT32_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_UINT32_SIZE_MAX)

/**
 * Encode an 32-bits unsigned integer dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 32-bits unsigned integer @rstlnk{map}
 * field into the buffer assigned to @p encoder at initialization time according
 * to the @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_UINT32_SIZE_MIN
 * - DPACK_MAP_UINT32_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_uint32(struct dpack_encoder * __restrict encoder,
                        unsigned int                      id,
                        uint32_t                          value)
	__dpack_nonull(1) __warn_result __dpack_export;

/**
 * Minimum size of an encoded 32-bits signed integer dpack map field.
 */
#define DPACK_MAP_INT32_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_INT32_SIZE_MIN)

/**
 * Maximum size of an encoded 32-bits signed integer dpack map field.
 */
#define DPACK_MAP_INT32_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_INT32_SIZE_MAX)

/**
 * Encode an 32-bits signed integer dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 32-bits signed integer @rstlnk{map}
 * field into the buffer assigned to @p encoder at initialization time according
 * to the @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_INT32_SIZE_MIN
 * - DPACK_MAP_INT32_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_int32(struct dpack_encoder * __restrict encoder,
                       unsigned int                      id,
                       int32_t                           value)
	__dpack_nonull(1) __warn_result __dpack_export;

/******************************************************************************
 * Map 64 bits integers encoding
 ******************************************************************************/

/**
 * Minimum size of an encoded 64-bits unsigned integer dpack map field.
 */
#define DPACK_MAP_UINT64_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_UINT64_SIZE_MIN)

/**
 * Maximum size of an encoded 64-bits unsigned integer dpack map field.
 */
#define DPACK_MAP_UINT64_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_UINT64_SIZE_MAX)

/**
 * Encode an 64-bits unsigned integer dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 64-bits unsigned integer @rstlnk{map}
 * field into the buffer assigned to @p encoder at initialization time according
 * to the @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_UINT64_SIZE_MIN
 * - DPACK_MAP_UINT64_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_uint64(struct dpack_encoder * __restrict encoder,
                        unsigned int                      id,
                        uint64_t                          value)
	__dpack_nonull(1) __warn_result __dpack_export;

/**
 * Minimum size of an encoded 64-bits signed integer dpack map field.
 */
#define DPACK_MAP_INT64_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_INT64_SIZE_MIN)

/**
 * Maximum size of an encoded 64-bits signed integer dpack map field.
 */
#define DPACK_MAP_INT64_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_INT64_SIZE_MAX)

/**
 * Encode an 64-bits signed integer dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value 64-bits signed integer @rstlnk{map}
 * field into the buffer assigned to @p encoder at initialization time according
 * to the @rstsubst{MessagePack int format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_INT64_SIZE_MIN
 * - DPACK_MAP_INT64_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_int64(struct dpack_encoder * __restrict encoder,
                       unsigned int                      id,
                       int64_t                           value)
	__dpack_nonull(1) __warn_result __dpack_export;

/******************************************************************************
 * Map single precision floats encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_FLOAT)

/**
 * Minimum size of an encoded single precision floating point number dpack map
 * field.
 */
#define DPACK_MAP_FLOAT_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_FLOAT_SIZE)

/**
 * Maximum size of an encoded single precision floating point number dpack map
 * field.
 */
#define DPACK_MAP_FLOAT_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_FLOAT_SIZE)

/**
 * Encode a single precision floating point number dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value single precision floating point number
 * @rstlnk{map} field into the buffer assigned to @p encoder at initialization
 * time according to the @rstsubst{MessagePack float format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_FLOAT_SIZE_MIN
 * - DPACK_MAP_FLOAT_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_float(struct dpack_encoder * __restrict encoder,
                       unsigned int                      id,
                       float                             value)
	__dpack_nonull(1) __warn_result __dpack_export;

#endif /* defined(CONFIG_DPACK_FLOAT) */

/******************************************************************************
 * Map double precision floats encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_DOUBLE)

/**
 * Minimum size of an encoded double precision floating point number dpack map
 * field.
 */
#define DPACK_MAP_DOUBLE_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_DOUBLE_SIZE)

/**
 * Maximum size of an encoded double precision floating point number dpack map
 * field.
 */
#define DPACK_MAP_DOUBLE_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_DOUBLE_SIZE)

/**
 * Encode a double precision floating point number dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value double precision floating point number
 * @rstlnk{map} field into the buffer assigned to @p encoder at initialization
 * time according to the @rstsubst{MessagePack float format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_DOUBLE_SIZE_MIN
 * - DPACK_MAP_DOUBLE_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_double(struct dpack_encoder * __restrict encoder,
                        unsigned int                      id,
                        double                            value)
	__dpack_nonull(1) __warn_result __dpack_export;

#endif /* defined(CONFIG_DPACK_DOUBLE) */

/******************************************************************************
 * Map strings encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_STRING)

/**
 * Size of an encoded string dpack map field.
 *
 * @param[in] _len length of decoded string
 *
 * Compute the size of a @rstlnk{string} @rstlnk{map} field encoded according to
 * the @rstsubst{MessagePack string format} (excluding the terminating ``NULL``
 * byte).
 */
#define DPACK_MAP_STR_SIZE(_len) \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_STR_SIZE(_len))

/**
 * Minimum size of an encoded string dpack map field.
 */
#define DPACK_MAP_STR_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_STR_SIZE(1))

/**
 * Maximum size of an encoded string dpack map field.
 */
#define DPACK_MAP_STR_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_STR_SIZE(DPACK_STRLEN_MAX))

/**
 * Encode a string dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value @rstlnk{string} @rstlnk{map} field
 * into the buffer assigned to @p encoder at initialization time according to
 * the @rstsubst{MessagePack string format}.
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
 * - DPACK_MAP_STR_SIZE()
 * - DPACK_MAP_STR_SIZE_MIN
 * - DPACK_MAP_STR_SIZE_MAX
 * - dpack_map_encode_str_fix()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_str(struct dpack_encoder * __restrict encoder,
                     unsigned int                      id,
                     const char * __restrict           value)
	__dpack_nonull(1, 3) __warn_result __dpack_export;

/**
 * Encode a string dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 * @param[in]    len     length of @p value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value @rstlnk{string} @rstlnk{map} field
 * into the buffer assigned to @p encoder at initialization time according to
 * the @rstsubst{MessagePack string format}.
 *
 * This function is similar to dpack_map_encode_str() except that @p len bytes
 * of @p value are encoded.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p value string length is different from @p len argument, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p value string length is zero or greater than #DPACK_STRLEN_MAX, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_STR_SIZE()
 * - DPACK_MAP_STR_SIZE_MIN
 * - DPACK_MAP_STR_SIZE_MAX
 * - dpack_map_encode_str()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_str_fix(struct dpack_encoder  * __restrict encoder,
                         unsigned int                       id,
                         const char *__restrict             value,
                         size_t                             length)
	__dpack_nonull(1, 3) __warn_result __dpack_export;

#endif /* defined(CONFIG_DPACK_STRING) */

/******************************************************************************
 * Map lvstr encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_LVSTR)

#include <dpack/lvstr.h>

/**
 * Size of an encoded lvstr dpack map field.
 *
 * @param[in] _len length of decoded lvstr
 *
 * Compute the size of a @rstlnk{lvstr} @rstlnk{map} field encoded according to
 * the @rstsubst{MessagePack string format} (excluding the terminating ``NULL``
 * byte).
 */
#define DPACK_MAP_LVSTR_SIZE(_len) \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_LVSTR_SIZE(_len))

/**
 * Minimum size of an encoded lvstr dpack map field.
 */
#define DPACK_MAP_LVSTR_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_LVSTR_SIZE(1))

/**
 * Maximum size of an encoded lvstr dpack map field.
 */
#define DPACK_MAP_LVSTR_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_LVSTR_SIZE(DPACK_LVSTRLEN_MAX))

/**
 * Encode a lvstr dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value @rstlnk{lvstr} @rstlnk{map} field into
 * the buffer assigned to @p encoder at initialization time according to the
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
 * - DPACK_MAP_LVSTR_SIZE()
 * - DPACK_MAP_LVSTR_SIZE_MIN
 * - DPACK_MAP_LVSTR_SIZE_MAX
 * - dpack_map_encode_str()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_lvstr(struct dpack_encoder * __restrict      encoder,
                       unsigned int                           id,
                       const struct stroll_lvstr * __restrict value)
	__dpack_nonull(1, 3) __warn_result __dpack_export;

#endif /* defined(CONFIG_DPACK_LVSTR) */

/******************************************************************************
 * Map bins encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_BIN)

/**
 * Size of an encoded bin dpack map field.
 *
 * @param[in] _size decoded bin size
 *
 * Compute the size of a @rstlnk{bin} @rstlnk{map} field encoded according to
 * the @rstsubst{MessagePack bin format}.
 */
#define DPACK_MAP_BIN_SIZE(_size) \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_BIN_SIZE(_size))

/**
 * Minimum size of an encoded bin dpack map field.
 */
#define DPACK_MAP_BIN_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_BIN_SIZE(1))

/**
 * Maximum size of an encoded bin dpack map field.
 */
#define DPACK_MAP_BIN_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_BIN_SIZE(DPACK_BINSZ_MAX))

/**
 * Encode a bin dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    value   field value to encode
 * @param[in]    size    size of @p value
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value @rstlnk{bin} @rstlnk{map} field into
 * the buffer assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack bin format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p size bin size is zero or greater than #DPACK_BINSZ_MAX, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_BIN_SIZE()
 * - DPACK_MAP_BIN_SIZE_MIN
 * - DPACK_MAP_BIN_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_bin(struct dpack_encoder * __restrict encoder,
                     unsigned int                      id,
                     const uint8_t * __restrict        value,
                     size_t                            size)
	__dpack_nonull(1, 3) __warn_result __dpack_export;

#endif /* defined(CONFIG_DPACK_BIN) */

/******************************************************************************
 * Map Nil / NULL encoding
 ******************************************************************************/

/**
 * Minimum size of an encoded nil dpack map field.
 */
#define DPACK_MAP_NIL_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_NIL_SIZE)

/**
 * Maximum size of an encoded nil dpack map field.
 */
#define DPACK_MAP_NIL_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_NIL_SIZE)

/**
 * Encode a nil dpack map field.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Encode / pack / serialize the @p value @rstlnk{nil} @rstlnk{map} field into
 * the buffer assigned to @p encoder at initialization time according to the
 * @rstsubst{MessagePack bin format}.
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 *
 * @see
 * - DPACK_MAP_NIL_SIZE_MIN
 * - DPACK_MAP_NIL_SIZE_MAX
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_encode_nil(struct dpack_encoder * __restrict encoder,
                     unsigned int                      id)
	__dpack_nonull(1) __warn_result __dpack_export;

/******************************************************************************
 * Nested collections encoding
 ******************************************************************************/

/**
 * Minimum size of an encoded nested collection dpack map
 *
 * @param[in] _data_size size of encoded map data
 *
 * Compute the minimum size of a nested collection @rstlnk{map} encoded
 * according to the @rstsubst{MessagePack map format} where the cumulated size
 * of all nested encoded fields is given by @p _data_size.
 *
 * @warning
 * Arguments values MUST be known at compile time, i.e., constant. This triggers
 * a compile time error otherwise.
 *
 * @see
 * - dpack_map_begin_encode_nest_map()
 * - dpack_map_begin_encode_nest_array()
 * - DPACK_MAP_SIZE()
 */
#define DPACK_MAP_NEST_SIZE_MIN(_data_size) \
	(DPACK_MAP_FLDID_SIZE_MIN + _data_size)

/**
 * Maximum size of an encoded nested collection dpack map
 *
 * @param[in] _data_size size of encoded map data
 *
 * Compute the minimum size of a nested collection @rstlnk{map} encoded
 * according to the @rstsubst{MessagePack map format} where the cumulated size
 * of all nested encoded fields is given by @p _data_size.
 *
 * @warning
 * Arguments values MUST be known at compile time, i.e., constant. This triggers
 * a compile time error otherwise.
 *
 * @see
 * - dpack_map_begin_encode_nest_map()
 * - dpack_map_begin_encode_nest_array()
 * - DPACK_MAP_SIZE()
 */
#define DPACK_MAP_NEST_SIZE_MAX(_data_size) \
	(DPACK_MAP_FLDID_SIZE_MAX + _data_size)

/**
 * Start encoding a nested dpack map.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    nr      number of nested map fields
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Start encoding a @rstlnk{map} containing @p nr fields that is nested into
 * another @rstlnk{map}. Call to this function must be followed by @p nr fields.
 * Nested map encoding must be completed by a call to dpack_map_end_encode().
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p nr is zero or greater than #DPACK_MAP_FLDNR_MAX, result is undefined.
 *   An assertion is triggered otherwise.
 *
 * @see
 * - dpack_map_end_encode()
 * - dpack_map_begin_encode()
 * - dpack_map_begin_encode_nest_array()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_begin_encode_nest_map(struct dpack_encoder * __restrict encoder,
                                unsigned int                      id,
                                unsigned int                      nr)
	__dpack_nonull(1) __warn_result __dpack_export;

#if defined(CONFIG_DPACK_ARRAY)

/**
 * Start encoding an array nested into a dpack map.
 *
 * @param[inout] encoder encoder
 * @param[in]    id      field identifier to encode
 * @param[in]    nr      number of array elements
 *
 * @return an errno like error code
 * @retval 0         Success
 * @retval -EMSGSIZE Not enough space to complete operation
 * @retval -ENOMEM   Memory allocation failure
 *
 * Start encoding a @rstlnk{array} containing @p nr elements and that is nested
 * into a @rstlnk{map}. Call to this function must be followed by @p nr fields.
 * Nested array encoding must be completed by a call to
 * dpack_array_end_encode().
 *
 * @warning
 * - @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 *   before calling this function. Result is undefined otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p encoder is in error state before calling this function, result is
 *   undefined. An assertion is triggered otherwise.
 * - When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 *   @p nr is zero or greater than #DPACK_ARRAY_ELMNR_MAX, result is undefined.
 *   An assertion is triggered otherwise.
 *
 * @see
 * - dpack_array_end_encode()
 * - dpack_map_begin_encode()
 * - dpack_map_begin_encode_nest_map()
 * - dpack_encoder_init_buffer()
 */
extern int
dpack_map_begin_encode_nest_array(struct dpack_encoder * __restrict encoder,
                                  unsigned int                      id,
                                  unsigned int                      nr)
	__dpack_nonull(1) __warn_result __dpack_export;

#endif /* defined(CONFIG_DPACK_ARRAY) */

/******************************************************************************
 * Map decoding
 ******************************************************************************/

extern int
dpack_map_decode(struct dpack_decoder * __restrict decoder,
                 dpack_decode_item_fn *            decode,
                 void * __restrict                 data)
	__dpack_nonull(1, 2) __warn_result __dpack_export;

extern int
dpack_map_decode_equ(struct dpack_decoder * __restrict decoder,
                     unsigned int                      nr,
                     dpack_decode_item_fn *            decode,
                     void * __restrict                 data)
	__dpack_nonull(1, 3) __warn_result __dpack_export;

extern int
dpack_map_decode_min(struct dpack_decoder * __restrict decoder,
                     unsigned int                      min_nr,
                     dpack_decode_item_fn *            decode,
                     void * __restrict                 data)
	__dpack_nonull(1, 3) __warn_result __dpack_export;

extern int
dpack_map_decode_max(struct dpack_decoder * __restrict decoder,
                     unsigned int                      max_nr,
                     dpack_decode_item_fn *            decode,
                     void * __restrict                 data)
	__dpack_nonull(1, 3) __warn_result __dpack_export;

extern int
dpack_map_decode_range(struct dpack_decoder * __restrict decoder,
                       unsigned int           min_nr,
                       unsigned int           max_nr,
                       dpack_decode_item_fn * decode,
                       void * __restrict      data)
	__dpack_nonull(1, 4) __warn_result __dpack_export;

#endif /* _DPACK_MAP_H */
