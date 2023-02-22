#include "dpack/codec.h"
#include "common.h"

/******************************************************************************
 * Encoder / packer
 ******************************************************************************/

size_t
dpack_encoder_space_used(struct dpack_encoder * encoder)
{
	dpack_assert(encoder);

	return mpack_writer_buffer_used(&encoder->mpack);
}

size_t
dpack_encoder_space_left(struct dpack_encoder * encoder)
{
	dpack_assert(encoder);

	return mpack_writer_buffer_left(&encoder->mpack);
}

void
dpack_init_buffer_encoder(struct dpack_encoder * encoder,
                          char *                 buffer,
                          size_t                 size)
{
	dpack_assert(encoder);
	dpack_assert(buffer);
	dpack_assert(size);

	mpack_writer_init(&encoder->mpack, buffer, size);
}

void
dpack_exit_encoder(struct dpack_encoder * encoder)
{
	dpack_assert(encoder);

	/*
	 * As stated into documentation, calling mpack_writer_destroy() with
	 * any unclosed compound types will assert in tracking mode.
	 * Flag an error before destruction to prevent from asserting.
	 */
	mpack_writer_flag_error(&encoder->mpack, mpack_error_invalid);
	mpack_writer_destroy(&encoder->mpack);
}

/******************************************************************************
 * Decoder / unpacker
 ******************************************************************************/

size_t
dpack_decoder_data_left(struct dpack_decoder * decoder)
{
	dpack_assert(decoder);

	return mpack_reader_remaining(&decoder->mpack, NULL);
}

void
dpack_init_buffer_decoder(struct dpack_decoder * decoder,
                          const char *           buffer,
                          size_t                 size)
{
	dpack_assert(decoder);
	dpack_assert(buffer);
	dpack_assert(size);

	mpack_reader_init_data(&decoder->mpack, buffer, size);
}

void
dpack_exit_decoder(struct dpack_decoder * decoder)
{
	dpack_assert(decoder);

	mpack_reader_destroy(&decoder->mpack);
}
