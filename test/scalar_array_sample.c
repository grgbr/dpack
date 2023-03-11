#include "scalar_array_sample.h"
#include <dpack/codec.h>
#include <assert.h>

#define scalar_array_sample_assert(condition) \
	assert(condition)

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

	dpack_array_begin_encode(encoder, dpack_array_nr(data->array));
	for (elm = 0; elm < dpack_array_nr(data->array); elm++) {
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

int
scalar_array_sample_unpack(struct dpack_decoder       * decoder,
                           struct scalar_array_sample * data)
{
	scalar_array_sample_assert(decoder);
	scalar_array_sample_assert(data);
	scalar_array_sample_assert(dpack_decoder_data_left(decoder) >=
	                           SCALAR_ARRAY_SAMPLE_PACKED_SIZE_MIN);

	int          err;
	unsigned int elm;

	err = dpack_decode_uint32(decoder, &data->thirty_two);
	if (err)
		return err;

	err = dpack_array_begin_decode_equ(decoder,
	                                   dpack_array_nr(data->array));
	if (err)
		return err;

	for (elm = 0; elm < dpack_array_nr(data->array); elm++) {
		err = dpack_decode_uint16(decoder, &data->array[elm]);
		if (err)
			return err;
	}

	dpack_array_end_decode(decoder);

	err = dpack_decode_uint8(decoder, &data->eight);
	if (err)
		return err;

	return 0;
}
