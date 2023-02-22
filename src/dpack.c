#include "dpack.h"
#include <mpack.h>
#include <errno.h>

#include <assert.h>
#define dpack_assert(cond) assert(cond)

#define unreachable() __builtin_unreachable()

static inline int
dpack_errno_from_mpack(enum mpack_error_t err)
{
	switch (err) {
	case mpack_ok:
		return 0;
	case mpack_error_io:
		return -EIO;
	case mpack_error_invalid:
		return -EPROTO;
	case mpack_error_unsupported:
		return -ENOTSUP;
	case mpack_error_type:
		return -ENOMSG;
	case mpack_error_too_big:
		return -EMSGSIZE;
	case mpack_error_memory:
		return -ENOMEM;
	case mpack_error_bug:
		assert(0);
	case mpack_error_data:
		return -EBADMSG;
	case mpack_error_eof:
		return -ENODATA;
	default:
		dpack_assert(0);
	}

	unreachable();
}

/******************************************************************************
 * Encoder / packer
 ******************************************************************************/

static int
dpack_encoder_error_state(struct mpack_writer_t * writer)
{
	dpack_assert(writer);

	enum mpack_error_t err;

	err = mpack_writer_error(writer);
	if (err != mpack_ok)
		return dpack_errno_from_mpack(err);

	return 0;
}

int
dpack_encode_u8(struct dpack_encoder * encoder, uint8_t value)
{
	dpack_assert(encoder);
	dpack_assert(mpack_writer_error(&encoder->mpack) == mpack_ok);

	mpack_write_u8(&encoder->mpack, value);

	return dpack_encoder_error_state(&encoder->mpack);
}

int
dpack_encode_u16(struct dpack_encoder * encoder, uint16_t value)
{
	dpack_assert(encoder);
	dpack_assert(mpack_writer_error(&encoder->mpack) == mpack_ok);

	mpack_write_u16(&encoder->mpack, value);

	return dpack_encoder_error_state(&encoder->mpack);
}

int
dpack_encode_u32(struct dpack_encoder * encoder, uint32_t value)
{
	dpack_assert(encoder);
	dpack_assert(mpack_writer_error(&encoder->mpack) == mpack_ok);

	mpack_write_u32(&encoder->mpack, value);

	return dpack_encoder_error_state(&encoder->mpack);
}

int
dpack_encode_u64(struct dpack_encoder * encoder, uint64_t value)
{
	dpack_assert(encoder);
	dpack_assert(mpack_writer_error(&encoder->mpack) == mpack_ok);

	mpack_write_u64(&encoder->mpack, value);

	return dpack_encoder_error_state(&encoder->mpack);
}

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

static int
dpack_decode_tag(struct mpack_reader_t * reader,
                 enum mpack_type_t       type,
                 struct mpack_tag_t    * tag)
{
	dpack_assert(reader);
	dpack_assert(tag);
	dpack_assert(mpack_reader_error(reader) == mpack_ok);

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

static int
dpack_xtract_u64_min(struct mpack_reader_t * reader,
                     uint64_t                low,
                     uint64_t              * value)
{
	dpack_assert(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_uint, &tag);
	if (err)
		return err;

	*value = mpack_tag_uint_value(&tag);
	if (*value < low) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -ERANGE;
	}

	return 0;
}

static int
dpack_xtract_u64_max(struct mpack_reader_t * reader,
                     uint64_t                high,
                     uint64_t              * value)
{
	dpack_assert(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_uint, &tag);
	if (err)
		return err;

	*value = mpack_tag_uint_value(&tag);
	if (*value > high) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -ERANGE;
	}

	return 0;
}

static int
dpack_xtract_u64_range(struct mpack_reader_t * reader,
                       uint64_t                low,
                       uint64_t                high,
                       uint64_t              * value)
{
	dpack_assert(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_uint, &tag);
	if (err)
		return err;

	*value = mpack_tag_uint_value(&tag);
	if ((*value < low) || (*value > high)) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -ERANGE;
	}

	return 0;
}

int
dpack_decode_u8(struct dpack_decoder * decoder, uint8_t * value)
{
	dpack_assert(decoder);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_max(&decoder->mpack, (uint64_t)UINT8_MAX, &val);
	if (err)
		return err;

	*value = (uint8_t)val;

	return 0;
}

int
dpack_decode_u8_min(struct dpack_decoder * decoder,
                    uint8_t                low,
                    uint8_t              * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_range(&decoder->mpack,
	                             (uint64_t)low,
	                             (uint64_t)UINT8_MAX,
	                             &val);
	if (err)
		return err;

	*value = (uint8_t)val;

	return 0;
}

int
dpack_decode_u8_max(struct dpack_decoder * decoder,
                    uint8_t                high,
                    uint8_t              * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_max(&decoder->mpack, (uint64_t)high, &val);
	if (err)
		return err;

	*value = (uint8_t)val;

	return 0;
}

int
dpack_decode_u8_range(struct dpack_decoder * decoder,
                      uint8_t                low,
                      uint8_t                high,
                      uint8_t              * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_range(&decoder->mpack,
	                             (uint64_t)low,
	                             (uint64_t)high,
	                             &val);
	if (err)
		return err;

	*value = (uint8_t)val;

	return 0;
}

int
dpack_decode_u16(struct dpack_decoder * decoder, uint16_t * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_max(&decoder->mpack, (uint64_t)UINT16_MAX, &val);
	if (err)
		return err;

	*value = (uint16_t)val;

	return 0;
}

int
dpack_decode_u16_min(struct dpack_decoder * decoder,
                     uint16_t               low,
                     uint16_t             * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_range(&decoder->mpack,
	                             (uint64_t)low,
	                             (uint64_t)UINT16_MAX,
	                             &val);
	if (err)
		return err;

	*value = (uint16_t)val;

	return 0;
}

int
dpack_decode_u16_max(struct dpack_decoder * decoder,
                     uint16_t               high,
                     uint16_t             * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_max(&decoder->mpack, (uint64_t)high, &val);
	if (err)
		return err;

	*value = (uint16_t)val;

	return 0;
}

int
dpack_decode_u16_range(struct dpack_decoder * decoder,
                       uint16_t               low,
                       uint16_t               high,
                       uint16_t             * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_range(&decoder->mpack,
	                             (uint64_t)low,
	                             (uint64_t)high,
	                             &val);
	if (err)
		return err;

	*value = (uint16_t)val;

	return 0;
}

int
dpack_decode_u32(struct dpack_decoder * decoder, uint32_t * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_max(&decoder->mpack, (uint64_t)UINT32_MAX, &val);
	if (err)
		return err;

	*value = (uint32_t)val;

	return 0;
}

int
dpack_decode_u32_min(struct dpack_decoder * decoder,
                     uint32_t               low,
                     uint32_t             * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_range(&decoder->mpack,
	                             (uint64_t)low,
	                             (uint64_t)UINT32_MAX,
	                             &val);
	if (err)
		return err;

	*value = (uint32_t)val;

	return 0;
}

int
dpack_decode_u32_max(struct dpack_decoder * decoder,
                     uint32_t               high,
                     uint32_t             * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_max(&decoder->mpack, (uint64_t)high, &val);
	if (err)
		return err;

	*value = (uint32_t)val;

	return 0;
}

int
dpack_decode_u32_range(struct dpack_decoder * decoder,
                       uint32_t               low,
                       uint32_t               high,
                       uint32_t             * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_range(&decoder->mpack,
	                             (uint64_t)low,
	                             (uint64_t)high,
	                             &val);
	if (err)
		return err;

	*value = (uint32_t)val;

	return 0;
}

int
dpack_decode_u64(struct dpack_decoder * decoder, uint64_t * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(&decoder->mpack, mpack_type_uint, &tag);
	if (err)
		return err;

	*value = mpack_tag_uint_value(&tag);

	return 0;
}

int
dpack_decode_u64_min(struct dpack_decoder * decoder,
                     uint64_t               low,
                     uint64_t             * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_min(&decoder->mpack, low, &val);
	if (err)
		return err;

	*value = val;

	return 0;
}

int
dpack_decode_u64_max(struct dpack_decoder * decoder,
                     uint64_t               high,
                     uint64_t             * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_max(&decoder->mpack, high, &val);
	if (err)
		return err;

	*value = val;

	return 0;
}

int
dpack_decode_u64_range(struct dpack_decoder * decoder,
                       uint64_t               low,
                       uint64_t               high,
                       uint64_t             * value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	uint64_t val;
	int      err;

	err = dpack_xtract_u64_range(&decoder->mpack, low, high, &val);
	if (err)
		return err;

	*value = val;

	return 0;
}

int
dpack_decode_field_id(struct dpack_decoder * decoder, uint16_t * id)
{
	dpack_assert(decoder);
	dpack_assert(id);

	uint16_t val;
	int      err;

#define DPACK_FIELDID_MAX ((uint16_t)1024)
	err = dpack_decode_u16_max(decoder, DPACK_FIELDID_MAX, &val);
	if (err)
		return err;

	*id = (uint16_t)val;

	return 0;
}

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
