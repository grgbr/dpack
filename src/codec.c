#include "dpack/codec.h"
#include "common.h"

/******************************************************************************
 * Encoder / packer
 ******************************************************************************/

int
dpack_encoder_error_state(struct mpack_writer_t * writer)
{
	dpack_assert_intern(writer);

	enum mpack_error_t err;

	err = mpack_writer_error(writer);
	if (err != mpack_ok)
		return dpack_errno_from_mpack(err);

	return 0;
}

size_t
dpack_encoder_space_used(struct dpack_encoder * encoder)
{
	dpack_assert_api(encoder);

	return mpack_writer_buffer_used(&encoder->mpack);
}

size_t
dpack_encoder_space_left(struct dpack_encoder * encoder)
{
	dpack_assert_api(encoder);

	return mpack_writer_buffer_left(&encoder->mpack);
}

void
dpack_encoder_init_buffer(struct dpack_encoder * encoder,
                          char *                 buffer,
                          size_t                 size)
{
	dpack_assert_api(encoder);
	dpack_assert_api(buffer);
	dpack_assert_api(size);

	mpack_writer_init(&encoder->mpack, buffer, size);
}

void
dpack_encoder_fini(struct dpack_encoder * encoder)
{
	dpack_assert_api(encoder);

	/*
	 * As stated into documentation, calling mpack_writer_destroy() with
	 * any unclosed compound types will assert in tracking mode.
	 * Flag an error before destruction to prevent from asserting.
	 */
	mpack_writer_destroy(&encoder->mpack);
}

/******************************************************************************
 * Decoder / unpacker
 ******************************************************************************/

int
dpack_decoder_error_state(struct mpack_reader_t * reader)
{
	dpack_assert_intern(reader);

	enum mpack_error_t err;

	err = mpack_reader_error(reader);
	if (err != mpack_ok)
		return dpack_errno_from_mpack(err);

	return 0;
}

int
dpack_decode_tag(struct mpack_reader_t *         reader,
                 enum mpack_type_t               type,
                 struct mpack_tag_t * __restrict tag)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(tag);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);

	enum mpack_error_t err;

	*tag = mpack_read_tag(reader);
	err = mpack_reader_error(reader);
	if (err != mpack_ok)
		return dpack_errno_from_mpack(err);

	if (mpack_tag_type(tag) != type) {
		mpack_reader_flag_error(reader, mpack_error_type);
		return -ENOMSG;
	}

	return 0;
}

size_t
dpack_decoder_data_left(struct dpack_decoder * decoder)
{
	dpack_assert_api(decoder);

	return mpack_reader_remaining(&decoder->mpack, NULL);
}

static void
dpack_decoder_discard(struct dpack_decoder * decoder,
                      enum mpack_type_t      type,
                      unsigned int           nr)
{
	dpack_assert_intern(decoder);

	if (dpack_decoder_error_state(&decoder->mpack))
		return;

	while (nr--)
		mpack_discard(&decoder->mpack);

	mpack_done_type(&decoder->mpack, type);
}

int
dpack_decoder_skip(struct dpack_decoder * decoder)
{
	/*
	 * Decoder MUST NOT be in error state since this interface is primarily
	 * meant for skipping deprecated / unwanted content.
	 */
	dpack_assert_api_decoder(decoder);

	mpack_discard(&decoder->mpack);

	return dpack_decoder_error_state(&decoder->mpack);
}

void
dpack_decoder_init_skip_buffer(struct dpack_decoder * decoder,
                               const char *           buffer,
                               size_t                 size)
{
	dpack_assert_api(decoder);
	dpack_assert_api(buffer);
	dpack_assert_api(size);

	mpack_reader_init_data(&decoder->mpack, buffer, size);
	decoder->intr = dpack_decoder_discard;
}

static void
dpack_decoder_abort(struct dpack_decoder * decoder,
                    enum mpack_type_t      type __unused,
                    unsigned int           nr __unused)
{
	dpack_assert_intern(decoder);

	mpack_reader_flag_error(&decoder->mpack, mpack_error_data);
}

void
dpack_decoder_init_buffer(struct dpack_decoder * decoder,
                          const char *           buffer,
                          size_t                 size)
{
	dpack_assert_api(decoder);
	dpack_assert_api(buffer);
	dpack_assert_api(size);

	mpack_reader_init_data(&decoder->mpack, buffer, size);
	decoder->intr = dpack_decoder_abort;
}

void
dpack_decoder_fini(struct dpack_decoder * decoder)
{
	dpack_assert_api(decoder);

	mpack_reader_destroy(&decoder->mpack);
}
