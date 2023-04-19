#ifndef _DPACK_COMMON_H
#define _DPACK_COMMON_H

#include "dpack/codec.h"
#include <stroll/assert.h>
#include <errno.h>

#if defined(CONFIG_DPACK_ASSERT_API)

#define dpack_assert_api(_cond) \
	stroll_assert("dpack", _cond)

#define dpack_assert_api_encoder(_enc) \
	dpack_assert_api(_enc); \
	dpack_assert_api(mpack_writer_error(&(_enc)->mpack) == mpack_ok)

#define dpack_assert_api_decoder(_dec) \
	dpack_assert_api(_dec); \
	dpack_assert_api(mpack_reader_error(&(_dec)->mpack) == mpack_ok)

#else  /* !defined(CONFIG_DPACK_ASSERT_API) */

#define dpack_assert_api(_cond)
#define dpack_assert_api_encoder(_enc)
#define dpack_assert_api_decoder(_dec)

#endif /* defined(CONFIG_DPACK_ASSERT_API) */

#if defined(CONFIG_DPACK_ASSERT_INTERN)

#define dpack_assert_intern(_cond) \
	stroll_assert("dpack", _cond)

#else  /* !defined(CONFIG_DPACK_ASSERT_INTERN) */

#define dpack_assert_intern(_cond)

#endif /* defined(CONFIG_DPACK_ASSERT_INTERN) */

static inline int __dpack_const __dpack_nothrow __warn_result
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
		dpack_assert_intern(0);
	}

	unreachable();
}

extern int
dpack_encoder_error_state(struct mpack_writer_t * writer);

/*
 * Watch out !!
 *
 * This function is marked as __leaf for now. However, it calls
 * mpack_reader_flag_error() which in turn may call a function from the current
 * compilation unit thanks to the reader error_fn() function pointer of mpack.
 * 
 * If modifying dpack_decoder_error_state() and / or registering an error
 * function (thanks to 	mpack_reader_set_error_handler()) is required for
 * internal DPack purposes, MAKE SURE you return to current compilation unit
 * only by return or by exception handling.
 *
 * See Stroll's __leaf documentation for more infos.
 */
extern int
dpack_decoder_error_state(struct mpack_reader_t * reader)
	__dpack_nonull(1) __dpack_pure __dpack_nothrow __leaf __warn_result;

extern int
dpack_decode_tag(struct mpack_reader_t *         reader,
                 enum mpack_type_t               type,
                 struct mpack_tag_t * __restrict tag)
	__dpack_nonull(1, 3) __dpack_nothrow __leaf __warn_result;

static inline void __dpack_nonull(1) __dpack_nothrow
dpack_decoder_intr(struct dpack_decoder * decoder,
                   enum mpack_type_t      type,
                   unsigned int           nr)
{
	dpack_assert_intern(decoder);
	dpack_assert_intern(decoder->intr);

	decoder->intr(decoder, type, nr);
}

#endif /* _DPACK_COMMON_H */
