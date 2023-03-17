#include "dpack/string.h"
#include "dpack/codec.h"
#include "common.h"

size_t
dpack_str_size(size_t len)
{
	dpack_assert(len);
	dpack_assert(len <= DPACK_STRLEN_MAX);

	switch (len) {
	case 1 ... DPACK_FIXSTR_LEN_MAX:
		return DPACK_FIXSTR_SIZE(len);
#if DPACK_STRLEN_MAX > DPACK_FIXSTR_LEN_MAX
	case (DPACK_FIXSTR_LEN_MAX + 1) ... DPACK_STR8_LEN_MAX:
		return DPACK_STR8_SIZE(len);
#endif
#if DPACK_STRLEN_MAX > DPACK_STR16_LEN_MAX
	case (DPACK_STR8_LEN_MAX + 1) ... DPACK_STR16_LEN_MAX:
		return DPACK_STR16_SIZE(len);
#endif
#if DPACK_STRLEN_MAX > DPACK_STR32_LEN_MAX
	case (DPACK_STR16_LEN_MAX + 1) ... DPACK_STR32_LEN_MAX:
		return DPACK_STR32_SIZE(len);
#endif
	default:
		dpack_assert(0);
	}

	unreachable();
}

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
	dpack_assert(mpack_reader_error(reader) == mpack_ok);
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
dpack_xtract_strdup(struct mpack_reader_t * reader, char ** value, uint32_t len)
{
	dpack_assert(reader);
	dpack_assert(mpack_reader_error(reader) == mpack_ok);
	dpack_assert(value);
	dpack_assert(len);
	dpack_assert(len <= DPACK_STRLEN_MAX);

	char * str;

	str = mpack_read_bytes_alloc(reader, len);
	if (!str)
		return dpack_decoder_error_state(reader);

	/*
	 * Ensure the read string contains no NULL byte since msgpack do not
	 * serialize terminating NULL byte.
	 */
	if (memchr(str, 0, len)) {
		mpack_reader_flag_error(reader, mpack_error_type);
		free(str);
		return -EBADMSG;
	}

	*value = str;

	/*
	 * Give mpack a chance to track bytes read. This is not required in case
	 * of reader error since no more operations may be performed with it.
	 */
	mpack_done_str(reader);

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

	return dpack_xtract_strdup(&decoder->mpack, value, len);
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

	return dpack_xtract_strdup(&decoder->mpack, value, len);
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

	return dpack_xtract_strdup(&decoder->mpack, value, len);
}

static int
dpack_decode_str_tag_fix(struct mpack_reader_t * reader, size_t len)
{
	dpack_assert(mpack_reader_error(reader) == mpack_ok);
	dpack_assert(len);
	dpack_assert(len <= DPACK_STRLEN_MAX);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_str, &tag);
	if (err)
		return err;

	if (mpack_tag_str_length(&tag) != len) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -EBADMSG;
	}

	return 0;
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

	int err;

	err = dpack_decode_str_tag_fix(&decoder->mpack, len);
	if (err)
		return err;

	return dpack_xtract_strdup(&decoder->mpack, value, len);
}

static ssize_t
dpack_xtract_strcpy(struct mpack_reader_t * reader,
                    char                  * value,
                    uint32_t                len)
{
	dpack_assert(reader);
	dpack_assert(mpack_reader_error(reader) == mpack_ok);
	dpack_assert(value);
	dpack_assert(len);
	dpack_assert(len <= DPACK_STRLEN_MAX);

	int err;

	mpack_read_bytes(reader, value, len);
	err = dpack_decoder_error_state(reader);
	if (err)
		goto err;

	/*
	 * Ensure the read string contains no NULL byte since msgpack do not
	 * serialize terminating NULL byte.
	 */
	if (memchr(value, 0, len)) {
		mpack_reader_flag_error(reader, mpack_error_type);
		err = -EBADMSG;
		goto err;
	}

	value[len] = '\0';

	/*
	 * Give mpack a chance to track bytes read. This is not required in case
	 * of reader error since no more operations may be performed with it.
	 */
	mpack_done_str(reader);

	return (ssize_t)len;

err:
	/*
	 * Just to make sure the caller is safe to use value even when ignoring
	 * returned error code.
	 */
	value[0] = '\0';

	return err;
}

ssize_t
dpack_decode_strcpy(struct dpack_decoder * decoder,
                    size_t                 size,
                    char                 * value)
{
	dpack_assert(decoder);
	dpack_assert(size > 1);
	dpack_assert(size <= (DPACK_STRLEN_MAX + 1));
	dpack_assert(value);

	ssize_t len;

	len = dpack_decode_str_tag(&decoder->mpack, 1, size - 1);
	if (len < 0)
		return len;

	return dpack_xtract_strcpy(&decoder->mpack, value, len);
}

ssize_t
dpack_decode_strcpy_range(struct dpack_decoder * decoder,
                          size_t                 min_len,
                          size_t                 size,
                          char                 * value)
{
	dpack_assert(decoder);
	dpack_assert(min_len);
	dpack_assert((min_len + 1) < size);
	dpack_assert(size <= (DPACK_STRLEN_MAX + 1));
	dpack_assert(value);

	ssize_t len;

	len = dpack_decode_str_tag(&decoder->mpack, min_len, size - 1);
	if (len < 0)
		return len;

	return dpack_xtract_strcpy(&decoder->mpack, value, len);
}

ssize_t
dpack_decode_strcpy_fix(struct dpack_decoder * decoder,
                        size_t                 size,
                        char                 * value)
{
	dpack_assert(decoder);
	dpack_assert(size > 1);
	dpack_assert(value);

	int err;

	err = dpack_decode_str_tag_fix(&decoder->mpack, size - 1);
	if (err)
		return err;

	return dpack_xtract_strcpy(&decoder->mpack, value, size - 1);
}
