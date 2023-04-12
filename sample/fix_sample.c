#include "fix_sample.h"
#include <dpack/codec.h>
#include <stroll/assert.h>

#if defined(CONFIG_DPACK_ASSERT_API)

#define fix_sample_assert(condition) \
	stroll_assert("fix_sample", condition)

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

#define fix_sample_assert(condition)

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

int
fix_sample_pack(struct dpack_encoder * encoder, const struct fix_sample * data)
{
	fix_sample_assert(encoder);
	fix_sample_assert(data);
	fix_sample_assert(dpack_encoder_space_left(encoder) >=
	                  FIX_SAMPLE_PACKED_SIZE_MIN);

	int err;

	err = dpack_encode_uint8(encoder, data->eight);
	if (err)
		return err;

	err = dpack_encode_uint16(encoder, data->sixteen);
	if (err)
		return err;

	err = dpack_encode_uint32(encoder, data->thirty_two);
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

	err = dpack_decode_uint8(decoder, &data->eight);
	if (err)
		return err;

	err = dpack_decode_uint16(decoder, &data->sixteen);
	if (err)
		return err;

	err = dpack_decode_uint32(decoder, &data->thirty_two);
	if (err)
		return err;

	return 0;
}
