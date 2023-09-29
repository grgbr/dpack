/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "scalar_array_sample.h"
#include <dpack/codec.h>
#include <stroll/assert.h>

#if defined(CONFIG_DPACK_ASSERT_API)

#define scalar_array_sample_assert(_cond) \
	stroll_assert("scalar array sample", _cond)

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

#define scalar_array_sample_assert(_cond)

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

int
scalar_array_sample_pack(struct dpack_encoder             * encoder,
                         const struct scalar_array_sample * data)
{
	scalar_array_sample_assert(encoder);
	scalar_array_sample_assert(data);
	scalar_array_sample_assert(dpack_encoder_space_left(encoder) >=
	                           SCALAR_ARRAY_SAMPLE_PACKED_SIZE_MIN);

	int          err;
	unsigned int elm;

	err = dpack_encode_uint32(encoder, data->thirty_two);
	if (err)
		return err;

	dpack_array_begin_encode(encoder, array_nr(data->array));
	for (elm = 0; elm < array_nr(data->array); elm++) {
		err = dpack_encode_uint16(encoder, data->array[elm]);
		if (err)
			return err;
	}
	dpack_array_end_encode(encoder);

	err = dpack_encode_uint8(encoder, data->eight);
	if (err)
		return err;

	return 0;
}

static int
scalar_array_sample_unpack_elem(struct dpack_decoder * decoder,
                                unsigned int           id,
                                void                 * data)
{
	scalar_array_sample_assert(decoder);
	scalar_array_sample_assert(id < SCALAR_ARRAY_SAMPLE_NR);
	scalar_array_sample_assert(data);

	uint16_t * array = (uint16_t *)data;

	return dpack_decode_uint16(decoder, &array[id]);
}

int
scalar_array_sample_unpack(struct dpack_decoder       * decoder,
                           struct scalar_array_sample * data)
{
	scalar_array_sample_assert(decoder);
	scalar_array_sample_assert(data);
	scalar_array_sample_assert(dpack_decoder_data_left(decoder) >=
	                           SCALAR_ARRAY_SAMPLE_PACKED_SIZE_MIN);

	int err;

	err = dpack_decode_uint32(decoder, &data->thirty_two);
	if (err)
		return err;

	err = dpack_array_decode_equ(decoder,
	                             array_nr(data->array),
	                             scalar_array_sample_unpack_elem,
	                             data->array);
	if (err)
		return err;

	err = dpack_decode_uint8(decoder, &data->eight);
	if (err)
		return err;

	return 0;
}
