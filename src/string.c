/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/string.h"
#include "dpack/codec.h"
#include "common.h"

size_t
dpack_str_size(size_t len)
{
	dpack_assert_api(len);
	dpack_assert_api(len <= DPACK_STRLEN_MAX);

	switch (len) {
	case 1 ... _DPACK_FIXSTR_LEN_MAX:
		return DPACK_FIXSTR_SIZE(len);
#if DPACK_STRLEN_MAX > _DPACK_FIXSTR_LEN_MAX
	case (_DPACK_FIXSTR_LEN_MAX + 1) ... _DPACK_STR8_LEN_MAX:
		return DPACK_STR8_SIZE(len);
#endif
#if DPACK_STRLEN_MAX > _DPACK_STR8_LEN_MAX
	case (_DPACK_STR8_LEN_MAX + 1) ... _DPACK_STR16_LEN_MAX:
		return DPACK_STR16_SIZE(len);
#endif
#if DPACK_STRLEN_MAX > _DPACK_STR16_LEN_MAX
	case (_DPACK_STR16_LEN_MAX + 1) ... DPACK_STRLEN_MAX:
		return DPACK_STR32_SIZE(len);
#endif
	default:
		dpack_assert_api(0);
	}

	unreachable();
}

int
dpack_encode_str_fix(struct dpack_encoder *  __restrict encoder,
                       const char * __restrict          value,
                       size_t                           length)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(value);
	dpack_assert_api(value[0]);
	dpack_assert_api(length);
	dpack_assert_api(length <= DPACK_STRLEN_MAX);
	dpack_assert_api(length == strnlen(value, (DPACK_STRLEN_MAX + 1)));

	int err;

	switch (length) {
	case 1 ... _DPACK_FIXSTR_LEN_MAX:
		err = dpack_write_tag(encoder,
		                      _DPACK_FIXSTR_TAG | (uint8_t)length);
		break;
#if DPACK_STRLEN_MAX > _DPACK_FIXSTR_LEN_MAX
	case (_DPACK_FIXSTR_LEN_MAX + 1) ... _DPACK_STR8_LEN_MAX:
		err = dpack_write_tag(encoder, DPACK_STR8_TAG);
		if (!err) {
			uint8_t val = (uint8_t)length;

			err = dpack_encoder_write(encoder, &val, sizeof(val));
		}
		break;
#endif
#if DPACK_STRLEN_MAX > _DPACK_STR8_LEN_MAX
	case (_DPACK_STR8_LEN_MAX + 1) ... _DPACK_STR16_LEN_MAX:
		err = dpack_write_tag(encoder, DPACK_STR16_TAG);
		if (!err) {
			uint16_t val = htobe16((uint16_t)length);

			err = dpack_encoder_write(encoder,
			                          (const uint8_t *)&val,
			                          sizeof(val));
		}
		break;
#endif
#if DPACK_STRLEN_MAX > _DPACK_STR16_LEN_MAX
	case (_DPACK_STR16_LEN_MAX + 1) ... DPACK_STRLEN_MAX:
		err = dpack_write_tag(encoder, DPACK_STR32_TAG);
		if (!err) {
			uint32_t val = htobe32((uint32_t)length);

			err = dpack_encoder_write(encoder, &val,
			                          (const uint8_t *)&val,
			                          sizeof(val));
		}
		break;
#endif
	default:
		dpack_assert_api(0);
	}

	if (!err)
		return dpack_encoder_write(encoder, value, length);

	return err;
}

int
dpack_encode_str(struct dpack_encoder * __restrict encoder,
                 const char * __restrict           value)
{
	dpack_assert_api_encoder(encoder);
	dpack_assert_api(value);
	dpack_assert_api(value[0]);
	dpack_assert_api(strnlen(value,
	                         (DPACK_STRLEN_MAX + 1)) <= DPACK_STRLEN_MAX);

	return dpack_encode_str_fix(encoder,
	                            value,
	                            strnlen(value, DPACK_STRLEN_MAX));
}

static __dpack_nonull(1) __warn_result
ssize_t
dpack_load_str_tag(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_intern(decoder);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err) {
		switch (tag) {
		case DPACK_FIXSTR_TAG:
			return (ssize_t)(tag & _DPACK_FIXSTR_LEN_MAX);

		case DPACK_STR8_TAG:
			{
				uint8_t len;

				err = dpack_decoder_read(decoder,
				                         &len,
				                         sizeof(len));
				if (!err)
					return (ssize_t)len;
				break;
			}

		case DPACK_STR16_TAG:
			{
				uint16_t len;

				err = dpack_decoder_read(decoder,
				                         (const uint8_t *)&len,
				                         sizeof(len));
				if (!err)
					return (ssize_t)(be16toh(len));
				break;
			}

		case DPACK_STR32_TAG:
			{
				uint32_t len;

				err = dpack_decoder_read(decoder,
				                         (const uint8_t *)&len,
				                         sizeof(len));
				if (!err)
					return (ssize_t)(be32toh(len));
				break;
			}

		default:
			err = -ENOMSG;
		}
	}

	return err;
}

static __dpack_nonull(1) __warn_result
ssize_t
dpack_decode_str_tag(struct dpack_decoder * __restrict decoder,
                     size_t                            min_len,
                     size_t                            max_len)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(min_len);
	dpack_assert_intern(min_len <= max_len);
	dpack_assert_intern(max_len <= DPACK_STRLEN_MAX);

	ssize_t len;

	len = dpack_load_str_tag(decoder);
	if ((len < 0) ||
	    (((size_t)len >= min_len) && ((size_t)len <= max_len)))
		return len;
	else if (!len)
		return -EBADMSG;

	return -EMSGSIZE;
}

static __dpack_nonull(1, 2) __warn_result
int
dpack_xtract_strdup(struct dpack_decoder * __restrict decoder,
                    char ** __restrict                value,
                    size_t                            len)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(value);
	dpack_assert_intern(len);
	dpack_assert_intern(len <= DPACK_STRLEN_MAX);

	char * str;

	str = malloc(len + 1);
	if (!str)
		return -errno;

	dpack_decoder_read(decoder, str, len);

	/*
	 * Ensure the read string contains no NULL byte since msgpack do not
	 * serialize terminating NULL byte.
	 */
	if (!memchr(str, 0, len)) {
		*value = str;
		return 0;
	}

	free(str);

	return -EBADMSG;
}

ssize_t
dpack_decode_strdup(struct dpack_decoder * __restrict decoder,
                    char ** __restrict                value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	ssize_t len;

	len = dpack_decode_str_tag(decoder, 1, DPACK_STRLEN_MAX);
	dpack_assert_intern(len);
	if (len < 0)
		return len;

	return dpack_xtract_strdup(decoder, value, (size_t)len);
}

#error finish me!!
static __dpack_nonull(1) __dpack_nothrow __warn_result
int
dpack_decode_str_tag_equ(struct dpack_decoder * __restrict decoder,
                         size_t                            len)
{
	dpack_decoder_assert_intern(decoder);
	dpack_assert_intern(len);
	dpack_assert_intern(len <= DPACK_STRLEN_MAX);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_str, &tag);
	if (err)
		return err;

	if (mpack_tag_str_length(&tag) != len) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -EMSGSIZE;
	}

	return 0;
}

ssize_t
dpack_decode_strdup_equ(struct dpack_decoder * decoder,
                        size_t                 len,
                        char ** __restrict     value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(len);
	dpack_assert_api(len <= DPACK_STRLEN_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_str_tag_equ(&decoder->mpack, len);
	if (err)
		return err;

	return dpack_xtract_strdup(&decoder->mpack, value, (uint32_t)len);
}

ssize_t
dpack_decode_strdup_max(struct dpack_decoder  * decoder,
                        size_t                  max_len,
                        char ** __restrict      value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(max_len > 1);
	dpack_assert_api(max_len <= DPACK_STRLEN_MAX);
	dpack_assert_api(value);

	ssize_t len;

	len = dpack_decode_str_tag(&decoder->mpack, 1, max_len);
	if (len < 0)
		return len;

	return dpack_xtract_strdup(&decoder->mpack, value, (uint32_t)len);
}

ssize_t
dpack_decode_strdup_range(struct dpack_decoder * decoder,
                          size_t                 min_len,
                          size_t                 max_len,
                          char ** __restrict     value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(min_len);
	dpack_assert_api(min_len < max_len);
	dpack_assert_api(max_len <= DPACK_STRLEN_MAX);
	dpack_assert_api(value);

	ssize_t len;

	len = dpack_decode_str_tag(&decoder->mpack, min_len, max_len);
	if (len < 0)
		return len;

	return dpack_xtract_strdup(&decoder->mpack, value, (uint32_t)len);
}

static ssize_t __dpack_nonull(1, 2) __dpack_nothrow __warn_result
dpack_xtract_strcpy(struct mpack_reader_t * reader,
                    char                  * value,
                    uint32_t                len)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);
	dpack_assert_intern(value);
	dpack_assert_intern(len);
	dpack_assert_intern(len <= DPACK_STRLEN_MAX);

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
                    char *                 value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(size > 1);
	dpack_assert_api(size <= (DPACK_STRLEN_MAX + 1));
	dpack_assert_api(value);

	ssize_t len;

	len = dpack_decode_str_tag(&decoder->mpack, 1, size - 1);
	if (len < 0)
		return len;

	return dpack_xtract_strcpy(&decoder->mpack, value, (uint32_t)len);
}

ssize_t
dpack_decode_strcpy_equ(struct dpack_decoder * decoder,
                        size_t                 size,
                        char                 * value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(size > 1);
	dpack_assert_api(size <= (DPACK_STRLEN_MAX + 1));
	dpack_assert_api(value);

	int err;

	err = dpack_decode_str_tag_equ(&decoder->mpack, size - 1);
	if (err)
		return err;

	return dpack_xtract_strcpy(&decoder->mpack, value, (uint32_t)size - 1);
}

ssize_t
dpack_decode_strcpy_range(struct dpack_decoder * decoder,
                          size_t                 min_len,
                          size_t                 max_size,
                          char                 * value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(min_len);
	dpack_assert_api((min_len + 1) < max_size);
	dpack_assert_api(max_size <= (DPACK_STRLEN_MAX + 1));
	dpack_assert_api(value);

	ssize_t len;

	len = dpack_decode_str_tag(&decoder->mpack, min_len, max_size - 1);
	if (len < 0)
		return len;

	return dpack_xtract_strcpy(&decoder->mpack, value, (uint32_t)len);
}
