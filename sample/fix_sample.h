/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _FIX_SAMPLE_H
#define _FIX_SAMPLE_H

#include <dpack/scalar.h>
#include <stdlib.h>

struct fix_sample {
	uint8_t  eight;
	uint16_t sixteen;
	uint32_t thirty_two;
	float    single;
};

#define FIX_SAMPLE_PACKED_SIZE_MIN (DPACK_UINT8_SIZE_MIN + \
                                    DPACK_UINT16_SIZE_MIN + \
                                    DPACK_UINT32_SIZE_MIN + \
                                    DPACK_FLOAT_SIZE)

#define FIX_SAMPLE_PACKED_SIZE_MAX (DPACK_UINT8_SIZE_MAX + \
                                    DPACK_UINT16_SIZE_MAX + \
                                    DPACK_UINT32_SIZE_MAX + \
                                    DPACK_FLOAT_SIZE)

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
