#include "dpack/array.h"
#include "dpack/stdint.h"
#include "dpack/codec.h"
#include "common.h"

#define DPACK_ARRAY_DEFINE_ENCODE(_name, _type, _func) \
	int \
	_name(struct dpack_encoder * encoder, \
	      const _type          * array, \
	      unsigned int           nr) \
	{ \
		dpack_assert(encoder); \
		dpack_assert(array); \
		dpack_assert(nr); \
		dpack_assert(nr < DPACK_ARRAY_NR_MAX); \
		dpack_assert(mpack_writer_error(&encoder->mpack) == mpack_ok); \
		\
		unsigned int elm; \
		\
		mpack_start_array(&encoder->mpack, nr); \
		for (elm = 0; elm < nr; elm++) { \
			int err; \
			\
			err = _func(encoder, array[elm]); \
			if (err) \
				return err; \
		} \
		mpack_finish_array(&encoder->mpack); \
		\
		return 0; \
	}

DPACK_ARRAY_DEFINE_ENCODE(dpack_array_encode_u8,
                          uint8_t,
                          dpack_encode_u8)
DPACK_ARRAY_DEFINE_ENCODE(dpack_array_encode_u16,
                          uint16_t,
                          dpack_encode_u16)
DPACK_ARRAY_DEFINE_ENCODE(dpack_array_encode_u32,
                          uint32_t,
                          dpack_encode_u32)
DPACK_ARRAY_DEFINE_ENCODE(dpack_array_encode_u64,
                          uint64_t,
                          dpack_encode_u64)

static int
dpack_array_begin(struct mpack_reader_t * reader, unsigned int nr)
{
	struct mpack_tag_t tag;
	int                err;

	err = dpack_decode_tag(reader, mpack_type_array, &tag);
	if (err)
		return err;

	if (mpack_tag_array_count(&tag) != nr) {
		mpack_reader_flag_error(reader, mpack_error_type);
		return -ENOMSG;
	}

	return 0;
}

static inline void
dpack_array_end(struct mpack_reader_t * reader)
{
	mpack_done_array(reader);
}

#define DPACK_ARRAY_DEFINE_DECODE(_name, _type, _func) \
	int \
	_name(struct dpack_decoder * decoder, _type * array, unsigned int nr) \
	{ \
		dpack_assert(decoder); \
		dpack_assert(array); \
		dpack_assert(nr); \
		dpack_assert(nr < DPACK_ARRAY_NR_MAX); \
		dpack_assert(mpack_reader_error(&decoder->mpack) == mpack_ok); \
		\
		int          err; \
		unsigned int elm; \
		\
		err = dpack_array_begin(&decoder->mpack, nr); \
		if (err) \
			return err; \
		\
		for (elm = 0; elm < nr; elm++) { \
			err = _func(decoder, &array[elm]); \
			if (err) \
				return err; \
		} \
		\
		dpack_array_end(&decoder->mpack); \
		\
		return 0; \
	}

DPACK_ARRAY_DEFINE_DECODE(dpack_array_decode_u8,
                          uint8_t,
                          dpack_decode_u8)
DPACK_ARRAY_DEFINE_DECODE(dpack_array_decode_u16,
                          uint16_t,
                          dpack_decode_u16)
DPACK_ARRAY_DEFINE_DECODE(dpack_array_decode_u32,
                          uint32_t,
                          dpack_decode_u32)
DPACK_ARRAY_DEFINE_DECODE(dpack_array_decode_u64,
                          uint64_t,
                          dpack_decode_u64)
