#ifndef _DPACK_MAP_H
#define _DPACK_MAP_H

#include <dpack/codec.h>
#include <dpack/scalar.h>

/* Maximum number of fields of a dpack map */
#define DPACK_MAP_FLDNR_MAX    (32U)
/* Maximum size of a dpack map */
#define DPACK_MAP_SIZE_MAX     (64U * 1024U)

/* Maximum number of elements an msgpack fixmap may encode */
#define DPACK_FIXMAP_FLDNR_MAX (15U)
/* Maximum number of elements a 16 bits msgpack map may encode */
#define DPACK_MAP16_FLDNR_MAX  UINT16_MAX
/* Maximum number of elements a 32 bits msgpack map may encode */
#define DPACK_MAP32_FLDNR_MAX  UINT32_MAX

/*
 * Check DPACK_MAP_FLDNR_MAX definition is sensible.
 *
 * Only allow half of the highest msgpack capacity since we need to multiply
 * this number by 2 when handling map discard operations.
 * DO NOT TOUCH THIS UPPER LIMIT.
 */
#if DPACK_MAP_FLDNR_MAX > (DPACK_MAP32_FLDNR_MAX / 2)
#error dpack cannot encode maps containing more than (UINT32_MAX / 2) fields !
#elif DPACK_MAP_FLDNR_MAX < 4U
#error Huh ?!
#endif

/******************************************************************************
 * Msgpack fixmap definitions
 ******************************************************************************/

/* Size of encoded msgpack fixmap header. */
#define DPACK_FIXMAP_HEAD_SIZE \
	MPACK_TAG_SIZE_FIXMAP

/*
 * Size of an encoded map header when maximum number of elements fits into a
 * fixmap msgpack map (up to 15 elements).
 */
#define _DPACK_MAP_HEAD_CONST_SIZE(_fld_nr) \
	DPACK_FIXMAP_HEAD_SIZE

/******************************************************************************
 * Msgpack 16 bits map definitions
 ******************************************************************************/

#if DPACK_MAP_FLDNR_MAX > DPACK_FIXMAP_FLDNR_MAX

/* Size of encoded msgpack 16 bits map header. */
#define DPACK_MAP16_HEAD_SIZE \
	MPACK_TAG_SIZE_MAP16

/*
 * Size of an encoded map header when maximum number of elements fits into a 16
 * bits msgpack map.
 */
#define DPACK_MAP16_HEAD_CONST_SIZE(_fld_nr) \
	(((_fld_nr) > DPACK_FIXMAP_FLDNR_MAX) ? \
	 DPACK_MAP16_HEAD_SIZE : \
	 DPACK_FIXMAP_HEAD_SIZE)

#undef _DPACK_MAP_HEAD_CONST_SIZE
#define _DPACK_MAP_HEAD_CONST_SIZE(_fld_nr) \
	DPACK_MAP16_HEAD_CONST_SIZE(_fld_nr)

#endif /* DPACK_MAP_FLDNR_MAX > DPACK_FIXMAP_FLDNR_MAX */

/******************************************************************************
 * Msgpack 32 bits map definitions
 ******************************************************************************/

#if DPACK_MAP_FLDNR_MAX > DPACK_MAP16_FLDNR_MAX

/* Size of encoded msgpack 32 bits map header. */
#define DPACK_MAP32_HEAD_SIZE \
	MPACK_TAG_SIZE_MAP32

/*
 * Size of an encoded map header when maximum number of elements fits into a 32
 * bits msgpack map.
 */
#define DPACK_MAP32_HEAD_CONST_SIZE(_fld_nr) \
	(((_fld_nr) > DPACK_MAP16_FLDNR_MAX) ? \
	 DPACK_MAP32_HEAD_SIZE : \
	 DPACK_MAP16_HEAD_CONST_SIZE(_fld_nr))

#undef _DPACK_MAP_HEAD_CONST_SIZE
#define _DPACK_MAP_HEAD_CONST_SIZE(_fld_nr) \
	DPACK_MAP32_HEAD_CONST_SIZE(_fld_nr)

#endif /* DPACK_MAP_FLDNR_MAX > DPACK_MAP16_FLDNR_MAX */

/******************************************************************************
 * Top-level map size definitions
 ******************************************************************************/

/*
 * Given the number of fields a map may contain, compute the size of the
 * corresponding encoded msgpack map metada.
 *
 * Argument value MUST be known at compile time, i.e., constant. Result is
 * unpredictable otherwise.
 */
#define DPACK_MAP_HEAD_CONST_SIZE(_fld_nr) \
	compile_eval(((_fld_nr) > 0) && \
	             ((_fld_nr) <= DPACK_MAP_FLDNR_MAX), \
	             _DPACK_MAP_HEAD_CONST_SIZE(_fld_nr), \
	             "invalid constant map length")

/*
 * Given the number of fields a map may contain, compute the size of the
 * corresponding encoded msgpack map metada.
 *
 * Argument value MUST be known at compile time, i.e., constant. Trigger a
 * compile time error otherwise.
 */
#define DPACK_MAP_HEAD_SIZE(_fld_nr) \
	compile_eval(__builtin_constant_p(_fld_nr), \
	             DPACK_MAP_HEAD_CONST_SIZE(_fld_nr), \
	             "constant map length expected")

/******************************************************************************
 * Map encoding
 ******************************************************************************/

extern void
dpack_map_begin_encode(struct dpack_encoder * encoder,
                       unsigned int           nr) __dpack_export;

extern void
dpack_map_end_encode(struct dpack_encoder * encoder) __dpack_export;

/******************************************************************************
 * Map field identifiers encoding
 ******************************************************************************/

#define DPACK_MAP_FLDID_MAX      (1024U)
#define DPACK_MAP_FLDID_SIZE_MIN (DPACK_UINT_SIZE_MIN)
#define DPACK_MAP_FLDID_SIZE_MAX (DPACK_UINT_SIZE_MAX)

static inline int
dpack_map_encode_fldid(struct dpack_encoder * encoder, unsigned int id)
{
	dpack_assert_api(id <= DPACK_MAP_FLDID_MAX);

	return dpack_encode_uint(encoder, id);
}

static inline int
dpack_map_decode_fldid(struct dpack_decoder * decoder, unsigned int * id)
{
	return dpack_decode_uint_max(decoder, DPACK_MAP_FLDID_MAX, id);
}

/******************************************************************************
 * Map boolean encoding
 ******************************************************************************/

#define DPACK_MAP_BOOL_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_BOOL_SIZE)
#define DPACK_MAP_BOOL_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_BOOL_SIZE)

extern int
dpack_map_encode_bool(struct dpack_encoder * encoder,
                      uint16_t               id,
                      bool                   value) __dpack_export;

/******************************************************************************
 * Map 8 bits integers encoding
 ******************************************************************************/

#define DPACK_MAP_UINT8_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_UINT8_SIZE_MIN)
#define DPACK_MAP_UINT8_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_UINT8_SIZE_MAX)

extern int
dpack_map_encode_uint8(struct dpack_encoder * encoder,
                       uint16_t               id,
                       uint8_t                value) __dpack_export;

#define DPACK_MAP_INT8_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_INT8_SIZE_MIN)
#define DPACK_MAP_INT8_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_INT8_SIZE_MAX)

extern int
dpack_map_encode_int8(struct dpack_encoder * encoder,
                      uint16_t               id,
                      int8_t                 value) __dpack_export;

/******************************************************************************
 * Map 16 bits integers encoding
 ******************************************************************************/

#define DPACK_MAP_UINT16_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_UINT16_SIZE_MIN)
#define DPACK_MAP_UINT16_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_UINT16_SIZE_MAX)

extern int
dpack_map_encode_uint16(struct dpack_encoder * encoder,
                        uint16_t               id,
                        uint16_t               value) __dpack_export;

#define DPACK_MAP_INT16_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_INT16_SIZE_MIN)
#define DPACK_MAP_INT16_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_INT16_SIZE_MAX)

extern int
dpack_map_encode_int16(struct dpack_encoder * encoder,
                       uint16_t               id,
                       int16_t                value) __dpack_export;

/******************************************************************************
 * Map 32 bits integers encoding
 ******************************************************************************/

#define DPACK_MAP_UINT32_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_UINT32_SIZE_MIN)
#define DPACK_MAP_UINT32_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_UINT32_SIZE_MAX)

extern int
dpack_map_encode_uint32(struct dpack_encoder * encoder,
                        uint16_t               id,
                        uint32_t               value) __dpack_export;

#define DPACK_MAP_INT32_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_INT32_SIZE_MIN)
#define DPACK_MAP_INT32_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_INT32_SIZE_MAX)

extern int
dpack_map_encode_int32(struct dpack_encoder * encoder,
                       uint16_t               id,
                       int32_t                value) __dpack_export;

/******************************************************************************
 * Map 64 bits integers encoding
 ******************************************************************************/

#define DPACK_MAP_UINT64_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_UINT64_SIZE_MIN)
#define DPACK_MAP_UINT64_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_UINT64_SIZE_MAX)

extern int
dpack_map_encode_uint64(struct dpack_encoder * encoder,
                        uint16_t               id,
                        uint64_t               value) __dpack_export;

#define DPACK_MAP_INT64_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_INT64_SIZE_MIN)
#define DPACK_MAP_INT64_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_INT64_SIZE_MAX)

extern int
dpack_map_encode_int64(struct dpack_encoder * encoder,
                       uint16_t               id,
                       int64_t                value) __dpack_export;

/******************************************************************************
 * Map strings encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_STRING)

#include <dpack/string.h>

#define DPACK_MAP_STR_SIZE_MIN(_len) \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_STR_SIZE(_len))
#define DPACK_MAP_STR_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_STR_SIZE(_len))

extern int
dpack_map_encode_str(struct dpack_encoder * encoder,
                     uint16_t               id,
                     const char           * value) __dpack_export;

extern int
dpack_map_encode_str_fix(struct dpack_encoder * encoder,
                         uint16_t               id,
                         const char           * value,
                         size_t                 len) __dpack_export;

#endif /* defined(CONFIG_DPACK_STRING) */

/******************************************************************************
 * Map lvstr encoding
 ******************************************************************************/

#if defined(CONFIG_DPACK_LVSTR)

#include <dpack/lvstr.h>

#define DPACK_MAP_LVSTR_SIZE_MIN(_len) \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_LVSTR_SIZE(_len))
#define DPACK_MAP_LVSTR_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_LVSTR_SIZE(_len))

extern int
dpack_map_encode_lvstr(struct dpack_encoder *      encoder,
                       uint16_t                    id,
                       const struct stroll_lvstr * value) __dpack_export;

#endif /* defined(CONFIG_DPACK_LVSTR) */

/******************************************************************************
 * Map decoding
 ******************************************************************************/

extern int
dpack_map_decode_equ(struct dpack_decoder * decoder,
                     unsigned int           nr,
                     dpack_decode_item_fn * decode,
                     void                 * data) __dpack_export;

extern int
dpack_map_decode_range(struct dpack_decoder * decoder,
                       unsigned int           min_nr,
                       unsigned int           max_nr,
                       dpack_decode_item_fn * decode,
                       void                 * data) __dpack_export;

static inline int
dpack_map_decode(struct dpack_decoder * decoder,
                 dpack_decode_item_fn * decode,
                 void                 * data)
{
	dpack_assert_api(decoder);
	dpack_assert_api(decode);

	return dpack_map_decode_range(decoder,
	                              1,
	                              DPACK_MAP_FLDNR_MAX,
	                              decode,
	                              data);
}

static inline int
dpack_map_decode_min(struct dpack_decoder * decoder,
                     unsigned int           min_nr,
                     dpack_decode_item_fn * decode,
                     void                 * data)
{
	dpack_assert_api(decoder);
	dpack_assert_api(min_nr);
	dpack_assert_api(min_nr < DPACK_MAP_FLDNR_MAX);
	dpack_assert_api(decode);

	return dpack_map_decode_range(decoder,
	                              min_nr,
	                              DPACK_MAP_FLDNR_MAX,
	                              decode,
	                              data);
}

static inline int
dpack_map_decode_max(struct dpack_decoder * decoder,
                     unsigned int           max_nr,
                     dpack_decode_item_fn * decode,
                     void                 * data)
{
	dpack_assert_api(decoder);
	dpack_assert_api(max_nr);
	dpack_assert_api(max_nr <= DPACK_MAP_FLDNR_MAX);
	dpack_assert_api(decode);

	return dpack_map_decode_range(decoder, 1, max_nr, decode, data);
}

#endif /* _DPACK_MAP_H */
