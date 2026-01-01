/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _DPACK_UTEST_H
#define _DPACK_UTEST_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

union dpackut_scalar_value {
	bool     boolean;
	uint8_t  uint8;
	int8_t   int8;
	uint16_t uint16;
	int16_t  int16;
	uint32_t uint32;
	int32_t  int32;
	uint64_t uint64;
	int64_t  int64;
	float    f32;
	double   f64;
};

struct dpackut_scalar_data {
	const uint8_t *            packed;
	size_t                     size;
	int                        error;
	union dpackut_scalar_value value;
	union dpackut_scalar_value low;
	union dpackut_scalar_value high;
};

struct dpack_decoder;

typedef void (dpackut_unpack_fn)(struct dpack_decoder *,
                                 const struct dpackut_scalar_data *);

struct dpack_encoder;

typedef int (dpackut_pack_fn)(struct dpack_encoder *,
                              const struct dpackut_scalar_data *);

extern void free(void * ptr);
extern void dpackut_expect_free(const void * data, size_t size);

extern void * malloc(size_t size);
extern int    dpackut_expect_malloc(void);

#endif /* _DPACK_UTEST_H */
