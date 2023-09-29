/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _SCALAR_ARRAY_SAMPLE_H
#define _SCALAR_ARRAY_SAMPLE_H

#include <dpack/scalar.h>
#include <dpack/array.h>
#include <stdlib.h>

#define SCALAR_ARRAY_SAMPLE_NR (7U)

struct scalar_array_sample {
	uint32_t thirty_two;
	uint16_t array[SCALAR_ARRAY_SAMPLE_NR];
	uint8_t  eight;
};

#define SCALAR_ARRAY_SAMPLE_PACKED_SIZE_MIN \
	(DPACK_UINT32_SIZE_MIN + \
	 DPACK_ARRAY_UINT16_SIZE_MIN(SCALAR_ARRAY_SAMPLE_NR) + \
	 DPACK_UINT32_SIZE_MIN)

#define SCALAR_ARRAY_SAMPLE_PACKED_SIZE_MAX \
	(DPACK_UINT32_SIZE_MAX + \
	 DPACK_ARRAY_UINT16_SIZE_MAX(SCALAR_ARRAY_SAMPLE_NR) + \
	 DPACK_UINT32_SIZE_MAX)

extern int
scalar_array_sample_pack(struct dpack_encoder             * encoder,
                         const struct scalar_array_sample * data);

extern int
scalar_array_sample_unpack(struct dpack_decoder       * decoder,
                           struct scalar_array_sample * data);

static inline struct scalar_array_sample *
scalar_array_sample_alloc(void)
{
	return malloc(sizeof(struct scalar_array_sample));
}

static inline void
scalar_array_sample_free(struct scalar_array_sample * data)
{
	free(data);
}

#endif /* _SCALAR_ARRAY_SAMPLE_H */
