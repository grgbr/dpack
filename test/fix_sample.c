#include "fix_sample.h"
#include <assert.h>

#define fix_sample_assert(condition) assert(condition)

int
fix_sample_pack(struct dpack_encoder * encoder, const struct fix_sample * data)
{
	fix_sample_assert(encoder);
	fix_sample_assert(data);
	fix_sample_assert(dpack_encoder_space_left(encoder) >=
	                  FIX_SAMPLE_PACKED_SIZE_MIN);

	int err;

	err = dpack_encode_u8(encoder, data->eight);
	if (err)
		return err;

	err = dpack_encode_u16(encoder, data->sixteen);
	if (err)
		return err;

	err = dpack_encode_u32(encoder, data->thirty_two);
	if (err)
		return err;

	return 0;
}

int
fix_sample_unpack(struct dpack_decoder * decoder, struct fix_sample * data)
{
	fix_sample_assert(decoder);
	fix_sample_assert(data);
	fix_sample_assert(dpack_decoder_data_left(decoder) >=
	                  FIX_SAMPLE_PACKED_SIZE_MIN);

	int err;

	err = dpack_decode_u8(decoder, &data->eight);
	if (err)
		return err;

	err = dpack_decode_u16(decoder, &data->sixteen);
	if (err)
		return err;

	err = dpack_decode_u32(decoder, &data->thirty_two);
	if (err)
		return err;

	return 0;
}
