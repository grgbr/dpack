#include "dpack/array.h"
#include "dpack/scalar.h"
#include "dpack/codec.h"
#include "common.h"

#define dpack_array_assert(_codec, _array, _nr) \
	dpack_assert(_codec); \
	dpack_assert(_array); \
	dpack_assert(_nr); \
	dpack_assert((_nr) < DPACK_ARRAY_NR_MAX)

#define DPACK_ARRAY_DEFINE_ENCODE(_name, _type, _func) \
	int \
	_name(struct dpack_encoder * encoder, \
	      const _type          * array, \
	      unsigned int           nr) \
	{ \
		dpack_array_assert(encoder, array, nr); \
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

#define dpack_array_assert_decoder(_decoder, _array, _nr) \
	dpack_array_assert(_decoder, _array, _nr); \
	dpack_assert(mpack_reader_error(&(_decoder)->mpack) == mpack_ok)

#define DPACK_ARRAY_DEFINE_DECODE(_name, _type, _func) \
	int \
	_name(struct dpack_decoder * decoder, _type * array, unsigned int nr) \
	{ \
		dpack_array_assert_decoder(decoder, array, nr); \
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

#define DPACK_ARRAY_DEFINE_DECODE_MIN(_name, _type, _func) \
	int \
	_name(struct dpack_decoder * decoder, \
	      _type                  low, \
	      _type                * array, \
	      unsigned int nr) \
	{ \
		dpack_array_assert_decoder(decoder, array, nr); \
		\
		int          err; \
		unsigned int elm; \
		\
		err = dpack_array_begin(&decoder->mpack, nr); \
		if (err) \
			return err; \
		\
		for (elm = 0; elm < nr; elm++) { \
			err = _func(decoder, low, &array[elm]); \
			if (err) \
				return err; \
		} \
		\
		dpack_array_end(&decoder->mpack); \
		\
		return 0; \
	}

#define DPACK_ARRAY_DEFINE_DECODE_MAX(_name, _type, _func) \
	int \
	_name(struct dpack_decoder * decoder, \
	      _type                  high, \
	      _type                * array, \
	      unsigned int nr) \
	{ \
		dpack_array_assert_decoder(decoder, array, nr); \
		\
		int          err; \
		unsigned int elm; \
		\
		err = dpack_array_begin(&decoder->mpack, nr); \
		if (err) \
			return err; \
		\
		for (elm = 0; elm < nr; elm++) { \
			err = _func(decoder, high, &array[elm]); \
			if (err) \
				return err; \
		} \
		\
		dpack_array_end(&decoder->mpack); \
		\
		return 0; \
	}

#define DPACK_ARRAY_DEFINE_DECODE_RANGE(_name, _type, _func) \
	int \
	_name(struct dpack_decoder * decoder, \
	      _type                  low, \
	      _type                  high, \
	      _type                * array, \
	      unsigned int nr) \
	{ \
		dpack_array_assert_decoder(decoder, array, nr); \
		\
		int          err; \
		unsigned int elm; \
		\
		err = dpack_array_begin(&decoder->mpack, nr); \
		if (err) \
			return err; \
		\
		for (elm = 0; elm < nr; elm++) { \
			err = _func(decoder, low, high, &array[elm]); \
			if (err) \
				return err; \
		} \
		\
		dpack_array_end(&decoder->mpack); \
		\
		return 0; \
	}

/******************************************************************************
 * 8 bits integer arrays
 ******************************************************************************/

DPACK_ARRAY_DEFINE_ENCODE(dpack_array_encode_uint8,
                          uint8_t,
                          dpack_encode_uint8)
DPACK_ARRAY_DEFINE_DECODE(dpack_array_decode_uint8,
                          uint8_t,
                          dpack_decode_uint8)
DPACK_ARRAY_DEFINE_DECODE_MIN(dpack_array_decode_uint8_min,
                              uint8_t,
                              dpack_decode_uint8_min)
DPACK_ARRAY_DEFINE_DECODE_MAX(dpack_array_decode_uint8_max,
                              uint8_t,
                              dpack_decode_uint8_max)
DPACK_ARRAY_DEFINE_DECODE_RANGE(dpack_array_decode_uint8_range,
                                uint8_t,
                                dpack_decode_uint8_range)

/******************************************************************************
 * 16 bits integer arrays
 ******************************************************************************/

DPACK_ARRAY_DEFINE_ENCODE(dpack_array_encode_uint16,
                          uint16_t,
                          dpack_encode_uint16)
DPACK_ARRAY_DEFINE_DECODE(dpack_array_decode_uint16,
                          uint16_t,
                          dpack_decode_uint16)
DPACK_ARRAY_DEFINE_DECODE_MIN(dpack_array_decode_uint16_min,
                              uint16_t,
                              dpack_decode_uint16_min)
DPACK_ARRAY_DEFINE_DECODE_MAX(dpack_array_decode_uint16_max,
                              uint16_t,
                              dpack_decode_uint16_max)
DPACK_ARRAY_DEFINE_DECODE_RANGE(dpack_array_decode_uint16_range,
                                uint16_t,
                                dpack_decode_uint16_range)

/******************************************************************************
 * 32 bits integer arrays
 ******************************************************************************/

DPACK_ARRAY_DEFINE_ENCODE(dpack_array_encode_uint32,
                          uint32_t,
                          dpack_encode_uint32)
DPACK_ARRAY_DEFINE_DECODE(dpack_array_decode_uint32,
                          uint32_t,
                          dpack_decode_uint32)
DPACK_ARRAY_DEFINE_DECODE_MIN(dpack_array_decode_uint32_min,
                              uint32_t,
                              dpack_decode_uint32_min)
DPACK_ARRAY_DEFINE_DECODE_MAX(dpack_array_decode_uint32_max,
                              uint32_t,
                              dpack_decode_uint32_max)
DPACK_ARRAY_DEFINE_DECODE_RANGE(dpack_array_decode_uint32_range,
                                uint32_t,
                                dpack_decode_uint32_range)

/******************************************************************************
 * 64 bits integer arrays
 ******************************************************************************/

DPACK_ARRAY_DEFINE_ENCODE(dpack_array_encode_uint64,
                          uint64_t,
                          dpack_encode_uint64)
DPACK_ARRAY_DEFINE_DECODE(dpack_array_decode_uint64,
                          uint64_t,
                          dpack_decode_uint64)
DPACK_ARRAY_DEFINE_DECODE_MIN(dpack_array_decode_uint64_min,
                              uint64_t,
                              dpack_decode_uint64_min)
DPACK_ARRAY_DEFINE_DECODE_MAX(dpack_array_decode_uint64_max,
                              uint64_t,
                              dpack_decode_uint64_max)
DPACK_ARRAY_DEFINE_DECODE_RANGE(dpack_array_decode_uint64_range,
                                uint64_t,
                                dpack_decode_uint64_range)
