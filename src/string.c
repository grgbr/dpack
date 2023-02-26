#include "dpack/string.h"
#include "dpack/codec.h"
#include "common.h"

int
dpack_encode_str(struct dpack_encoder * encoder, const char * value)
{
	dpack_assert(encoder);
	dpack_assert(mpack_writer_error(&encoder->mpack) == mpack_ok);
	dpack_assert(value[0]);
	dpack_assert(strnlen(value,
	                     (DPACK_STRLEN_MAX + 1)) <= DPACK_STRLEN_MAX);

	mpack_write_cstr(&encoder->mpack, value);

	return dpack_encoder_error_state(&encoder->mpack);
}

int
dpack_encode_str_fix(struct dpack_encoder * encoder,
                     const char           * value,
                     size_t                 len)
{
	dpack_assert(encoder);
	dpack_assert(mpack_writer_error(&encoder->mpack) == mpack_ok);
	dpack_assert(len);
	dpack_assert(value[0]);
	dpack_assert(len <= DPACK_STRLEN_MAX);

	mpack_write_str(&encoder->mpack, value, len);

	return dpack_encoder_error_state(&encoder->mpack);
}

static ssize_t
dpack_decode_str_tag(struct mpack_reader_t * reader,
                     size_t                  min_len,
                     size_t                  max_len)
{
	dpack_assert(min_len);
	dpack_assert(min_len < max_len);
	dpack_assert(max_len <= DPACK_STRLEN_MAX);

	struct mpack_tag_t tag;
	int                err;
	uint32_t           len;

	err = dpack_decode_tag(reader, mpack_type_str, &tag);
	if (err)
		return err;

	len = mpack_tag_str_length(&tag);
	if (len < min_len) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -EBADMSG;
	}
	else if (len > max_len) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -EMSGSIZE;
	}

	return (ssize_t)len;
}

static ssize_t
dpack_xtract_str(struct mpack_reader_t * reader, char ** value, uint32_t len)
{
	dpack_assert(reader);
	dpack_assert(mpack_reader_error(reader) == mpack_ok);
	dpack_assert(value);
	dpack_assert(len <= DPACK_STRLEN_MAX);

	char * str;

	str = mpack_read_bytes_alloc(reader, len);
	if (!str)
		return dpack_decoder_error_state(reader);

	*value = str;

	return (ssize_t)len;
}

ssize_t
dpack_decode_strdup(struct dpack_decoder * decoder, char ** value)
{
	dpack_assert(decoder);
	dpack_assert(value);

	ssize_t len;

	len = dpack_decode_str_tag(&decoder->mpack, 1, DPACK_STRLEN_MAX);
	if (len < 0)
		return len;

	return dpack_xtract_str(&decoder->mpack, value, len);
}

ssize_t
dpack_decode_strdup_min(struct dpack_decoder  * decoder,
                        size_t                  min_len,
                        char                 ** value)
{
	dpack_assert(decoder);
	dpack_assert(min_len);
	dpack_assert(value);

	ssize_t len;

	len = dpack_decode_str_tag(&decoder->mpack, min_len, DPACK_STRLEN_MAX);
	if (len < 0)
		return len;

	return dpack_xtract_str(&decoder->mpack, value, len);
}

ssize_t
dpack_decode_strdup_max(struct dpack_decoder  * decoder,
                        size_t                  max_len,
                        char                 ** value)
{
	dpack_assert(decoder);
	dpack_assert(max_len);
	dpack_assert(max_len <= DPACK_STRLEN_MAX);
	dpack_assert(value);

	ssize_t len;

	len = dpack_decode_str_tag(&decoder->mpack, 1, max_len);
	if (len < 0)
		return len;

	return dpack_xtract_str(&decoder->mpack, value, len);
}

ssize_t
dpack_decode_strdup_range(struct dpack_decoder  * decoder,
                          size_t                  min_len,
                          size_t                  max_len,
                          char                 ** value)
{
	dpack_assert(decoder);
	dpack_assert(min_len);
	dpack_assert(min_len < max_len);
	dpack_assert(max_len <= DPACK_STRLEN_MAX);
	dpack_assert(value);

	ssize_t len;

	len = dpack_decode_str_tag(&decoder->mpack, min_len, max_len);
	if (len < 0)
		return len;

	return dpack_xtract_str(&decoder->mpack, value, len);
}

ssize_t
dpack_decode_strdup_fix(struct dpack_decoder  * decoder,
                        size_t                  len,
                        char                 ** value)
{
	dpack_assert(decoder);
	dpack_assert(len);
	dpack_assert(len <= DPACK_STRLEN_MAX);
	dpack_assert(value);

	struct mpack_reader_t * reader = &decoder->mpack;
	struct mpack_tag_t      tag;
	int                     err;

	err = dpack_decode_tag(reader, mpack_type_str, &tag);
	if (err)
		return err;

	if (mpack_tag_str_length(&tag) != len) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -EBADMSG;
	}

	return dpack_xtract_str(reader, value, len);
}
