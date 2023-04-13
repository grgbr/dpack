#ifndef _DPACK_BIN_H
#define _DPACK_BIN_H

#include <dpack/cdefs.h>
#include <dpack/mpack.h>

struct dpack_encoder;
struct dpack_decoder;

#define DPACK_BINSZ_MAX (4U * 1024U * 1024U)

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
 * maximum length to a SSIZE_MAX...
 */
#if DPACK_BINSZ_MAX > SSIZE_MAX
#error dpack cannot encode bin which size is > SSIZE_MAX !
#elif DPACK_BINSZ_MAX < 16U
#error Huh ?!
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
	                                  DPACK_BIN16_SIZE(_sz))

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

/*
 * Given the size of a bin, compute the size of the corresponding encoded
 * msgpack bin.
 *
 * Size value MUST be known at compile time, i.e., constant. Trigger a compile
 * time error otherwise.
 */
#define DPACK_BIN_SIZE(_sz) \
	compile_eval(__builtin_constant_p(_sz), \
	             DPACK_BIN_CONST_SIZE(_sz), \
	             "constant bin size expected")

extern size_t
dpack_bin_size(size_t size) __dpack_export;

extern int
dpack_encode_bin(struct dpack_encoder * encoder,
                 const char *           value,
                 size_t                 size) __dpack_export;

extern ssize_t
dpack_decode_bindup(struct dpack_decoder * decoder,
                    char **                value) __dpack_export;

extern ssize_t
dpack_decode_bindup_equ(struct dpack_decoder * decoder,
                        size_t                 len,
                        char **                value) __dpack_export;

extern ssize_t
dpack_decode_bindup_max(struct dpack_decoder * decoder,
                        size_t                 max_sz,
                        char **                value) __dpack_export;

extern ssize_t
dpack_decode_bindup_range(struct dpack_decoder * decoder,
                          size_t                 min_sz,
                          size_t                 max_sz,
                          char **                value) __dpack_export;

extern ssize_t
dpack_decode_bincpy(struct dpack_decoder * decoder,
                    size_t                 size,
                    char *                 value) __dpack_export;

extern ssize_t
dpack_decode_bincpy_equ(struct dpack_decoder * decoder,
                        size_t                 size,
                        char *                 value) __dpack_export;

extern ssize_t
dpack_decode_bincpy_range(struct dpack_decoder * decoder,
                          size_t                 min_sz,
                          size_t                 max_sz,
                          char *                 value) __dpack_export;

#endif /* _DPACK_BIN_H */
