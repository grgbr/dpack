#ifndef _FIX_SAMPLE_H
#define _FIX_SAMPLE_H

#include <dpack/stdint.h>
#include <stdlib.h>

struct fix_sample {
	uint8_t  eight;
	uint16_t sixteen;
	uint32_t thirty_two;
};

#define FIX_SAMPLE_PACKED_SIZE_MIN (DPACK_U8_SIZE_MIN + \
                                    DPACK_U16_SIZE_MIN + \
                                    DPACK_U32_SIZE_MIN)

#define FIX_SAMPLE_PACKED_SIZE_MAX (DPACK_U8_SIZE_MAX + \
                                    DPACK_U16_SIZE_MAX + \
                                    DPACK_U32_SIZE_MAX)

extern int
fix_sample_pack(struct dpack_encoder * encoder, const struct fix_sample * data);

extern int
fix_sample_unpack(struct dpack_decoder * decoder, struct fix_sample * data);

static inline struct fix_sample *
fix_sample_alloc(void)
{
	return malloc(sizeof(struct fix_sample));
}

static inline void
fix_sample_free(struct fix_sample * data)
{
	free(data);
}

#endif /* _FIX_SAMPLE_H */
