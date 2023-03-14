#include "dpack/array.h"
#include "dpack/codec.h"
#include "common.h"

size_t
dpack_array_size(size_t elm_size, unsigned int elm_nr)
{
	dpack_assert(elm_size);
	dpack_assert(elm_size <= DPACK_ARRAY_ELMSIZE_MAX);
	dpack_assert(elm_nr);
	dpack_assert(elm_nr <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert(elm_size <= (DPACK_ARRAY_SIZE_MAX / elm_nr));

	switch (elm_nr) {
	case 1 ... DPACK_FIXARRAY_ELMNR_MAX:
		return DPACK_FIXARRAY_SIZE(elm_size, elm_nr);
#if DPACK_ARRAY_ELMNR_MAX > DPACK_FIXARRAY_ELMNR_MAX
	case (DPACK_FIXARRAY_ELMNR_MAX + 1) ... DPACK_ARRAY16_ELMNR_MAX:
		return DPACK_ARRAY16_SIZE(elm_size, elm_nr);
#endif
#if DPACK_ARRAY_ELMNR_MAX > DPACK_ARRAY16_ELMNR_MAX
	case (DPACK_ARRAY16_ELMNR_MAX + 1) ... DPACK_ARRAY32_ELMNR_MAX:
		return DPACK_ARRAY32_SIZE(elm_size, elm_nr);
#endif
	default:
		dpack_assert(0);
	}

	unreachable();
}

void
dpack_array_begin_encode(struct dpack_encoder * encoder, unsigned int nr)
{
	dpack_assert_encoder(encoder);
	dpack_assert(nr);
	dpack_assert(nr <= DPACK_ARRAY_ELMNR_MAX);

	mpack_start_array(&encoder->mpack, nr);
}

void
dpack_array_end_encode(struct dpack_encoder * encoder)
{
	dpack_assert_encoder(encoder);

	mpack_finish_array(&encoder->mpack);
}

static int
dpack_array_xtract_nr(struct mpack_reader_t * reader, unsigned int * nr)
{
	dpack_assert(reader);
	dpack_assert(mpack_reader_error(reader) == mpack_ok);
	dpack_assert(nr);

	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_array, &tag);
	if (err)
		return err;

	*nr = (unsigned int)mpack_tag_array_count(&tag);

	return 0;
}

int
dpack_array_begin_decode(struct dpack_decoder * decoder, unsigned int * nr)
{
	dpack_assert_decoder(decoder);
	dpack_assert(nr);

	return dpack_array_begin_decode_range(decoder,
	                                      1,
	                                      DPACK_ARRAY_ELMNR_MAX,
	                                      nr);
}

int
dpack_array_begin_decode_equ(struct dpack_decoder * decoder, unsigned int nr)
{
	dpack_assert_decoder(decoder);
	dpack_assert(nr);
	dpack_assert(nr <= DPACK_ARRAY_ELMNR_MAX);

	unsigned int cnt;
	int          err;

	err = dpack_array_xtract_nr(&decoder->mpack, &cnt);
	if (err)
		return err;

	if (cnt != nr) {
		mpack_reader_flag_error(&decoder->mpack, mpack_error_type);
		return -ENOMSG;
	}

	return 0;
}

int
dpack_array_begin_decode_min(struct dpack_decoder * decoder,
                             unsigned int           min_nr,
                             unsigned int         * nr)
{
	dpack_assert_decoder(decoder);
	dpack_assert(min_nr);
	dpack_assert(nr);

	return dpack_array_begin_decode_range(decoder,
	                                      min_nr,
	                                      DPACK_ARRAY_ELMNR_MAX,
	                                      nr);
}

int
dpack_array_begin_decode_max(struct dpack_decoder * decoder,
                             unsigned int           max_nr,
                             unsigned int         * nr)
{
	dpack_assert_decoder(decoder);
	dpack_assert(max_nr);
	dpack_assert(max_nr <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert(nr);

	return dpack_array_begin_decode_range(decoder,
	                                      1,
	                                      max_nr,
	                                      nr);
}

int
dpack_array_begin_decode_range(struct dpack_decoder * decoder,
                               unsigned int           min_nr,
                               unsigned int           max_nr,
                               unsigned int         * nr)
{
	dpack_assert_decoder(decoder);
	dpack_assert(min_nr);
	dpack_assert(min_nr < max_nr);
	dpack_assert(max_nr <= DPACK_ARRAY_ELMNR_MAX);
	dpack_assert(nr);

	int err;

	err = dpack_array_xtract_nr(&decoder->mpack, nr);
	if (err)
		return err;

	if ((*nr < min_nr) || (*nr > max_nr)) {
		mpack_reader_flag_error(&decoder->mpack, mpack_error_type);
		return -ENOMSG;
	}

	return 0;
}

void
dpack_array_end_decode(struct dpack_decoder * decoder)
{
	dpack_assert(decoder);

	mpack_done_array(&decoder->mpack);
}
