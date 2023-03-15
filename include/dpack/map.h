#ifndef _DPACK_MAP_H
#define _DPACK_MAP_H

#include <dpack/cdefs.h>
#include <dpack/scalar.h>
#include <dpack/string.h>

struct dpack_encoder;
struct dpack_decoder;

/* Maximum number of fields of a dpack map */
#define DPACK_MAP_FLDNR_MAX (128U)

/******************************************************************************
 * Map field identifier
 ******************************************************************************/

#define DPACK_MAP_FLDID_MAX      ((uint16_t)1024)
#define DPACK_MAP_FLDID_SIZE_MIN (DPACK_UINT16_SIZE_MIN)
#define DPACK_MAP_FLDID_SIZE_MAX (DPACK_UINT16_SIZE_MAX)

static inline int
dpack_map_encode_fldid(struct dpack_encoder * encoder, uint16_t id)
{
	dpack_assert(id <= DPACK_MAP_FLDID_MAX);

	return dpack_encode_uint16(encoder, id);
}

static inline int
dpack_map_decode_fldid(struct dpack_decoder * decoder, uint16_t * id)
{
	return dpack_decode_uint16_max(decoder, DPACK_MAP_FLDID_MAX, id);
}

/******************************************************************************
 * Boolean
 ******************************************************************************/

#define DPACK_MAP_BOOL_SIZE_MIN \
	(DPACK_MAP_FLDID_SIZE_MIN + DPACK_BOOL_SIZE_MIN)
#define DPACK_MAP_BOOL_SIZE_MAX \
	(DPACK_MAP_FLDID_SIZE_MAX + DPACK_BOOL_SIZE_MAX)

extern int
dpack_map_encode_bool(struct dpack_encoder * encoder,
                      uint16_t               id,
                      bool                   value) __dpack_export;

/******************************************************************************
 * 8 bits integers
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
 * 16 bits integers
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
 * 32 bits integers
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
 * 64 bits integers
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
 * Strings
 ******************************************************************************/

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

#endif /* _DPACK_MAP_H */
