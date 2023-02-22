#include "dpack/stdint.h"
#include "dpack/codec.h"
#include "common.h"

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

/******************************************************************************
 * 8 bits integer
 ******************************************************************************/

int
dpack_encode_u8(struct dpack_encoder * encoder, uint8_t value)
{
	dpack_assert(encoder);
	dpack_assert(mpack_writer_error(&encoder->mpack) == mpack_ok);

	mpack_write_u8(&encoder->mpack, value);

	return dpack_encoder_error_state(&encoder->mpack);
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

/******************************************************************************
 * 16 bits integer
 ******************************************************************************/

int
dpack_encode_u16(struct dpack_encoder * encoder, uint16_t value)
{
	dpack_assert(encoder);
	dpack_assert(mpack_writer_error(&encoder->mpack) == mpack_ok);

	mpack_write_u16(&encoder->mpack, value);

	return dpack_encoder_error_state(&encoder->mpack);
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

/******************************************************************************
 * 32 bits integer
 ******************************************************************************/

int
dpack_encode_u32(struct dpack_encoder * encoder, uint32_t value)
{
	dpack_assert(encoder);
	dpack_assert(mpack_writer_error(&encoder->mpack) == mpack_ok);

	mpack_write_u32(&encoder->mpack, value);

	return dpack_encoder_error_state(&encoder->mpack);
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

/******************************************************************************
 * 64 bits integer
 ******************************************************************************/

int
dpack_encode_u64(struct dpack_encoder * encoder, uint64_t value)
{
	dpack_assert(encoder);
	dpack_assert(mpack_writer_error(&encoder->mpack) == mpack_ok);

	mpack_write_u64(&encoder->mpack, value);

	return dpack_encoder_error_state(&encoder->mpack);
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

/******************************************************************************
 * Structure field identifier
 ******************************************************************************/

#define DPACK_FIELDID_MAX ((uint16_t)1024)

int
dpack_encode_fieldid(struct dpack_encoder * encoder, uint16_t id)
{
	dpack_assert(id <= DPACK_FIELDID_MAX);

	return dpack_encode_u16(encoder, id);
}

int
dpack_decode_fieldid(struct dpack_decoder * decoder, uint16_t * id)
{
	dpack_assert(decoder);
	dpack_assert(id);

	uint16_t val;
	int      err;

	err = dpack_decode_u16_max(decoder, DPACK_FIELDID_MAX, &val);
	if (err)
		return err;

	*id = (uint16_t)val;

	return 0;
}
