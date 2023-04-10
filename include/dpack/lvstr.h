#ifndef _DPACK_LVSTR_H
#define _DPACK_LVSTR_H

#include <dpack/string.h>
#include <stroll/lvstr.h>

#define DPACK_LVSTRLEN_MAX \
	STROLL_CONST_MIN(DPACK_STRLEN_MAX, STROLL_LVSTR_LEN_MAX)

/*
 * Check DPACK_LVSTRLEN_MAX definition is sensible.
 * Multiple dpack internal functions (such as dpack_decode_str_tag() for
 * example) return string length using a ssize_t, effectively restricting
 * maximum length to a SSIZE_MAX...
 */
#if DPACK_LVSTRLEN_MAX > SSIZE_MAX
#error dpack cannot encode lvstr which length is > SSIZE_MAX !
#elif DPACK_LVSTRLEN_MAX < 16U
#error Huh ?!
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

/*
 * Given the length of a lvstr, compute the size of the corresponding encoded
 * msgpack string.
 *
 * Length value MUST be known at compile time, i.e., constant. Trigger a compile
 * time error otherwise.
 */
#define DPACK_LVSTR_SIZE(_len) \
	compile_eval(__builtin_constant_p(_len), \
	             DPACK_LVSTR_CONST_SIZE(_len), \
	             "constant lvstr length expected")

extern size_t
dpack_lvstr_size(size_t len) __dpack_export;

extern int
dpack_encode_lvstr(struct dpack_encoder *      encoder,
                   const struct stroll_lvstr * value) __dpack_export;

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
