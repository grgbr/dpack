#ifndef _DPACK_STRING_H
#define _DPACK_STRING_H

#include <dpack/cdefs.h>
#include <stdint.h>
#include <sys/types.h>

struct dpack_encoder;
struct dpack_decoder;

/* Leave room for the terminating NULL byte... */
#define DPACK_STRLEN_MAX (4095U)

#if DPACK_STRLEN_MAX > UINT32_MAX
#error msgpack cannot encode strings which length is > UINT32_MAX !
#endif

extern int
dpack_encode_str(struct dpack_encoder * encoder,
                 const char           * value) __dpack_export;


extern int
dpack_encode_str_fix(struct dpack_encoder * encoder,
                     const char           * value,
                     size_t                 len) __dpack_export;

extern ssize_t
dpack_decode_strdup(struct dpack_decoder  * decoder,
                    char                 ** value) __dpack_export;

extern ssize_t
dpack_decode_strdup_min(struct dpack_decoder  * decoder,
                        size_t                  min_len,
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

#endif /* _DPACK_STRING_H */
