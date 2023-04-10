#ifndef _DPACK_COMMON_H
#define _DPACK_COMMON_H

#include "dpack/codec.h"
#include <errno.h>

#if defined(DPACK_ASSERT_INTERN)

#define dpack_assert_intern(_cond) \
	stroll_assert("dpack", _cond)

#else  /* !defined(DPACK_ASSERT_INTERN) */

#define dpack_assert_intern(_cond)

#endif /* defined(DPACK_ASSERT_INTERN) */

#define dpack_assert_encoder(_enc) \
	dpack_assert(_enc); \
	dpack_assert(mpack_writer_error(&(_enc)->mpack) == mpack_ok)

#define dpack_assert_api_encoder(_enc) \
	dpack_assert_api(_enc); \
	dpack_assert_api(mpack_writer_error(&(_enc)->mpack) == mpack_ok)

#define dpack_assert_decoder(_dec) \
	dpack_assert(_dec); \
	dpack_assert(mpack_reader_error(&(_dec)->mpack) == mpack_ok)

#define dpack_assert_api_decoder(_dec) \
	dpack_assert_api(_dec); \
	dpack_assert_api(mpack_reader_error(&(_dec)->mpack) == mpack_ok)

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
	case mpack_error_data:
		return -EBADMSG;
	case mpack_error_eof:
		return -ENODATA;
	case mpack_error_bug:
	default:
		dpack_assert(0);
	}

	unreachable();
}

extern int
dpack_encoder_error_state(struct mpack_writer_t * writer);

extern int
dpack_decoder_error_state(struct mpack_reader_t * reader);

extern int
dpack_decode_tag(struct mpack_reader_t * reader,
                 enum mpack_type_t       type,
                 struct mpack_tag_t    * tag);

static inline void
dpack_decoder_intr(struct dpack_decoder * decoder,
                   enum mpack_type_t      type,
                   unsigned int           nr)
{
	dpack_assert(decoder);
	dpack_assert(decoder->intr);

	decoder->intr(decoder, type, nr);
}

#endif /* _DPACK_COMMON_H */
