#include "dpack/bin.h"
#include "dpack/codec.h"
#include "common.h"

size_t
dpack_bin_size(size_t size)
{
	dpack_assert_api(size);
	dpack_assert_api(size <= DPACK_BINSZ_MAX);

	switch (size) {
	case 1 ... DPACK_BIN8_SIZE_MAX:
		return DPACK_BIN8_SIZE(size);
#if DPACK_BINSZ_MAX > DPACK_BIN8_SIZE_MAX
	case (DPACK_BIN8_SIZE_MAX + 1) ... DPACK_BIN16_SIZE_MAX:
		return DPACK_BIN16_SIZE(size);
#endif
#if DPACK_BINSZ_MAX > DPACK_BIN16_SIZE_MAX
	case (DPACK_BIN16_SIZE_MAX + 1) ... DPACK_BINSZ_MAX:
		return DPACK_BIN32_SIZE(size);
#endif
	default:
		dpack_assert_api(0);
	}

	unreachable();
}

int
dpack_encode_bin(struct dpack_encoder * encoder,
                 const char *           value,
                 size_t                 size)
{
	dpack_assert_api_encoder(encoder);
	dpack_assert_api(value);
	dpack_assert_api(size);
	dpack_assert_api(size <= DPACK_BINSZ_MAX);

	mpack_write_bin(&encoder->mpack, value, (uint32_t)size);

	return dpack_encoder_error_state(&encoder->mpack);
}

static ssize_t
dpack_decode_bin_tag(struct mpack_reader_t * reader,
                     size_t                  min_sz,
                     size_t                  max_sz)
{
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);
	dpack_assert_intern(min_sz);
	dpack_assert_intern(min_sz < max_sz);
	dpack_assert_intern(max_sz <= DPACK_BINSZ_MAX);

	struct mpack_tag_t tag;
	int                err;
	uint32_t           sz;

	err = dpack_decode_tag(reader, mpack_type_bin, &tag);
	if (err)
		return err;

	sz = mpack_tag_bin_length(&tag);
	if (sz < min_sz) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -EBADMSG;
	}
	else if (sz > max_sz) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -EMSGSIZE;
	}

	return (ssize_t)sz;
}

static int
dpack_decode_bin_tag_equ(struct mpack_reader_t * reader, size_t size)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);
	dpack_assert_intern(size);
	dpack_assert_intern(size <= DPACK_BINSZ_MAX);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_bin, &tag);
	if (err)
		return err;

	if (mpack_tag_bin_length(&tag) != size) {
		mpack_reader_flag_error(reader, mpack_error_data);
		return -EBADMSG;
	}

	return 0;
}

static ssize_t
dpack_xtract_bindup(struct mpack_reader_t * reader,
                    char **                 value,
                    uint32_t                size)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);
	dpack_assert_intern(value);
	dpack_assert_intern(size);
	dpack_assert_intern(size <= DPACK_BINSZ_MAX);

	char * bin;

	bin = mpack_read_bytes_alloc(reader, size);
	if (!bin)
		return dpack_decoder_error_state(reader);

	*value = bin;

	/*
	 * Give mpack a chance to track bytes read. This is not required in case
	 * of reader error since no more operations may be performed with it.
	 */
	mpack_done_bin(reader);

	return (ssize_t)size;
}

static ssize_t
dpack_xtract_bincpy(struct mpack_reader_t * reader,
                    char *                  value,
                    uint32_t                size)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);
	dpack_assert_intern(value);
	dpack_assert_intern(size);
	dpack_assert_intern(size <= DPACK_BINSZ_MAX);

	int err;

	mpack_read_bytes(reader, value, size);
	err = dpack_decoder_error_state(reader);
	if (err)
		return err;

	/*
	 * Give mpack a chance to track bytes read. This is not required in case
	 * of reader error since no more operations may be performed with it.
	 */
	mpack_done_bin(reader);

	return (ssize_t)size;
}

ssize_t
dpack_decode_bindup(struct dpack_decoder * decoder, char ** value)
{
	dpack_assert_api(decoder);
	dpack_assert_api(value);

	ssize_t sz;

	sz = dpack_decode_bin_tag(&decoder->mpack, 1, DPACK_BINSZ_MAX);
	if (sz < 0)
		return sz;

	return dpack_xtract_bindup(&decoder->mpack, value, (uint32_t)sz);
}

ssize_t
dpack_decode_bindup_equ(struct dpack_decoder * decoder,
                        size_t                 size,
                        char **                value)
{
	dpack_assert_api(decoder);
	dpack_assert_api(size);
	dpack_assert_api(size <= DPACK_BINSZ_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_bin_tag_equ(&decoder->mpack, size);
	if (err)
		return err;

	return dpack_xtract_bindup(&decoder->mpack, value, (uint32_t)size);
}

ssize_t
dpack_decode_bindup_max(struct dpack_decoder * decoder,
                        size_t                 max_sz,
                        char **                value)
{
	dpack_assert_api(decoder);
	dpack_assert_api(max_sz);
	dpack_assert_api(max_sz <= DPACK_BINSZ_MAX);
	dpack_assert_api(value);

	ssize_t sz;

	sz = dpack_decode_bin_tag(&decoder->mpack, 1, max_sz);
	if (sz < 0)
		return sz;

	return dpack_xtract_bindup(&decoder->mpack, value, (uint32_t)sz);
}

ssize_t
dpack_decode_bindup_range(struct dpack_decoder * decoder,
                          size_t                 min_sz,
                          size_t                 max_sz,
                          char **                value)
{
	dpack_assert_api(decoder);
	dpack_assert_api(min_sz);
	dpack_assert_api(min_sz < max_sz);
	dpack_assert_api(max_sz <= DPACK_BINSZ_MAX);
	dpack_assert_api(value);

	ssize_t sz;

	sz = dpack_decode_bin_tag(&decoder->mpack, min_sz, max_sz);
	if (sz < 0)
		return sz;

	return dpack_xtract_bindup(&decoder->mpack, value, (uint32_t)sz);
}

ssize_t
dpack_decode_bincpy(struct dpack_decoder * decoder,
                    size_t                 size,
                    char *                 value)
{
	dpack_assert_api(decoder);
	dpack_assert_api(size);
	dpack_assert_api(size <= DPACK_BINSZ_MAX);
	dpack_assert_api(value);

	ssize_t sz;

	sz = dpack_decode_bin_tag(&decoder->mpack, 1, size);
	if (sz < 0)
		return sz;

	return dpack_xtract_bincpy(&decoder->mpack, value, (uint32_t)sz);
}

ssize_t
dpack_decode_bincpy_equ(struct dpack_decoder * decoder,
                        size_t                 size,
                        char *                 value)
{
	dpack_assert_api(decoder);
	dpack_assert_api(size);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_bin_tag_equ(&decoder->mpack, size);
	if (err)
		return err;

	return dpack_xtract_bincpy(&decoder->mpack, value, (uint32_t)size);
}

ssize_t
dpack_decode_bincpy_range(struct dpack_decoder * decoder,
                          size_t                 min_sz,
                          size_t                 max_sz,
                          char                 * value)
{
	dpack_assert_api(decoder);
	dpack_assert_api(min_sz);
	dpack_assert_api(min_sz < max_sz);
	dpack_assert_api(max_sz <= DPACK_BINSZ_MAX);
	dpack_assert_api(value);

	ssize_t sz;

	sz = dpack_decode_bin_tag(&decoder->mpack, min_sz, max_sz);
	if (sz < 0)
		return sz;

	return dpack_xtract_bincpy(&decoder->mpack, value, (uint32_t)sz);
}
