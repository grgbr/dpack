/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/codec.h"
#include "common.h"
#include <string.h>

/******************************************************************************
 * Encoder / packer
 ******************************************************************************/

#define dpack_encoder_assert_buffer_api(_encoder) \
	dpack_assert_api(_encoder); \
	dpack_encoder_assert_api(&(_encoder)->base); \
	dpack_assert_api((_encoder)->capa); \
	dpack_assert_api((_encoder)->tail <= (_encoder)->capa); \
	dpack_assert_api((_encoder)->buff)

static __dpack_nonull(1) __dpack_pure __warn_result
size_t
dpack_encoder_buffer_left(const struct dpack_encoder * __restrict encoder)
{
	const struct dpack_encoder_buffer * enc =
		(const struct dpack_encoder_buffer *)encoder;

	dpack_encoder_assert_buffer_api(enc);

	return enc->capa - enc->tail;
}

static __dpack_nonull(1) __dpack_pure __warn_result
size_t
dpack_encoder_buffer_used(const struct dpack_encoder * __restrict encoder)
{
	const struct dpack_encoder_buffer * enc =
		(const struct dpack_encoder_buffer *)encoder;

	dpack_encoder_assert_buffer_api(enc);

	return enc->tail;
}

static __dpack_nonull(1, 2)
void
dpack_encoder_buffer_write(struct dpack_encoder * __restrict encoder,
                           const uint8_t * __restrict        data,
                           size_t                            size)
{
	dpack_assert_intern(data);
	dpack_assert_intern(size);

	struct dpack_encoder_buffer * enc = (struct dpack_encoder_buffer *)
	                                    encoder;

	dpack_encoder_assert_buffer_api(enc);
	dpack_assert_intern(size <= (enc->capa - enc->tail));

	memcpy(&enc->buff[enc->tail], data, size);
	enc->tail += size;
}

static __dpack_nonull(1)
void
dpack_encoder_buffer_fini(struct dpack_encoder * __restrict encoder __unused)
{
	dpack_encoder_assert_buffer_api((const struct dpack_encoder_buffer *)
	                                encoder);
}

const struct dpack_encoder_ops dpack_encoder_buffer_ops = {
	.left  = dpack_encoder_buffer_left,
	.used  = dpack_encoder_buffer_used,
	.write = dpack_encoder_buffer_write,
	.fini  = dpack_encoder_buffer_fini
};

void
dpack_encoder_init_buffer(struct dpack_encoder_buffer * __restrict encoder,
                          uint8_t * __restrict                     buffer,
                          size_t                                   size)
{
	dpack_assert_api(encoder);
	dpack_assert_api(buffer);
	dpack_assert_api(size);

	dpack_encoder_init(&encoder->base, &dpack_encoder_buffer_ops);
	encoder->tail = 0;
	encoder->capa = size;
	encoder->buff = buffer;
}

/******************************************************************************
 * Decoder / unpacker
 ******************************************************************************/

#define dpack_decoder_assert_buffer_api(_decoder) \
	dpack_assert_api(_decoder); \
	dpack_decoder_assert_api(&(_decoder)->base); \
	dpack_assert_api((_decoder)->capa); \
	dpack_assert_api((_decoder)->head <= (_decoder)->capa); \
	dpack_assert_api((_decoder)->buff)

static __dpack_nonull(1) __dpack_pure __warn_result
size_t
dpack_decoder_buffer_left(const struct dpack_decoder * __restrict decoder)
{
	const struct dpack_decoder_buffer * dec =
		(const struct dpack_decoder_buffer *)decoder;

	dpack_decoder_assert_buffer_api(dec);

	return dec->capa - dec->head;
}

static __dpack_nonull(1, 2)
void
dpack_decoder_buffer_read(struct dpack_decoder * __restrict decoder,
                          uint8_t * __restrict              data,
                          size_t                            size)
{
	dpack_assert_intern(data);
	dpack_assert_intern(size);

	struct dpack_decoder_buffer * dec = (struct dpack_decoder_buffer *)
	                                    decoder;

	dpack_decoder_assert_buffer_api(dec);
	dpack_assert_intern(size <= (dec->capa - dec->head));

	memcpy(data, &dec->buff[dec->head], size);
	dec->head += size;
}

static __dpack_nonull(1)
void
dpack_decoder_buffer_discard(struct dpack_decoder * __restrict decoder,
                             size_t                            size)
{
	dpack_assert_intern(size);

	struct dpack_decoder_buffer * dec = (struct dpack_decoder_buffer *)
	                                    decoder;

	dpack_decoder_assert_buffer_api(dec);
	dpack_assert_intern(size <= (dec->capa - dec->head));

	dec->head += size;
}

static __dpack_nonull(1)
void
dpack_decoder_buffer_fini(struct dpack_decoder * __restrict decoder __unused)
{
	dpack_decoder_assert_buffer_api((const struct dpack_decoder_buffer *)
	                                decoder);
}

const struct dpack_decoder_ops dpack_decoder_buffer_ops = {
	.left = dpack_decoder_buffer_left,
	.read = dpack_decoder_buffer_read,
	.disc = dpack_decoder_buffer_discard,
	.fini = dpack_decoder_buffer_fini
};

void
dpack_decoder_init_buffer(struct dpack_decoder_buffer * __restrict decoder,
                          const uint8_t * __restrict               buffer,
                          size_t                                   size)
{
	dpack_assert_api(decoder);
	dpack_assert_api(buffer);
	dpack_assert_api(size);

	dpack_decoder_init(&decoder->base, &dpack_decoder_buffer_ops);
	decoder->head = 0;
	decoder->capa = size;
	decoder->buff = buffer;
}

#if 0

/******************************************************************************
 * Encoder / packer
 ******************************************************************************/

int
dpack_encoder_error_state(struct mpack_writer_t * writer)
{
	dpack_assert_intern(writer);

	enum mpack_error_t err;

	err = mpack_writer_error(writer);
	if (err != mpack_ok)
		return dpack_errno_from_mpack(err);

	return 0;
}

size_t
dpack_encoder_space_used(struct dpack_encoder * encoder)
{
	dpack_assert_api(encoder);

	return mpack_writer_buffer_used(&encoder->mpack);
}

size_t
dpack_encoder_space_left(struct dpack_encoder * encoder)
{
	dpack_assert_api(encoder);

	return mpack_writer_buffer_left(&encoder->mpack);
}

/*
 * Watch out !!
 *
 * This function is marked as __leaf for now. However, it calls
 * mpack_writer_flag_error() which in turn may call a function from the current
 * compilation unit thanks to the writer error_fn() function pointer of mpack.
 *
 * If modifying dpack_encoder_init_buffer() and / or registering an error
 * function (thanks to mpack_writer_set_error_handler()) is required for
 * internal DPack purposes, MAKE SURE you return to current compilation unit
 * only by return or by exception handling.
 *
 * See Stroll's __leaf documentation for more infos.
 */
void
dpack_encoder_init_buffer(struct dpack_encoder * encoder,
                          char *                 buffer,
                          size_t                 size)
{
	dpack_assert_api(encoder);
	dpack_assert_api(buffer);
	dpack_assert_api(size);

	mpack_writer_init(&encoder->mpack, buffer, size);
}

/*
 * Watch out !!
 *
 * This function is marked as __leaf for now. However, it may call multiple
 * function pointers registered by the caller. See writer's flush(), teardown,
 * error_fn() function pointers.
 *
 * If modifying dpack_encoder_fini() and / or registering function pointers is
 * required for internal DPack purposes, MAKE SURE you return to current
 * compilation unit only by return or by exception handling.
 *
 * See Stroll's __leaf documentation for more infos.
 */
void
dpack_encoder_fini(struct dpack_encoder * encoder, bool abort __unused)
{
	dpack_assert_api(encoder);

#if MPACK_WRITE_TRACKING == 1
	/*
	 * As stated into documentation, calling mpack_writer_destroy() with
	 * any unclosed compound types will assert in tracking mode.
	 * Flag an error before destruction to prevent from asserting.
	 */
	if (abort)
		mpack_writer_flag_error(&encoder->mpack, mpack_error_data);
#endif /* defined(CONFIG_DPACK_DEBUG) */

	mpack_writer_destroy(&encoder->mpack);
}

/******************************************************************************
 * Decoder / unpacker
 ******************************************************************************/

int
dpack_decoder_error_state(struct mpack_reader_t * reader)
{
	dpack_assert_intern(reader);

	enum mpack_error_t err;

	err = mpack_reader_error(reader);
	if (err != mpack_ok)
		return dpack_errno_from_mpack(err);

	return 0;
}

/*
 * Watch out !!
 *
 * This function is marked as __leaf for now. However, it calls
 * mpack_reader_flag_error() which in turn may call a function from the current
 * compilation unit thanks to the reader error_fn() function pointer of mpack.
 *
 * If modifying dpack_decode_tag() and / or registering an error
 * function (thanks to mpack_reader_set_error_handler()) is required for
 * internal DPack purposes, MAKE SURE you return to current compilation unit
 * only by return or by exception handling.
 *
 * See Stroll's __leaf documentation for more infos.
 */
int
dpack_decode_tag(struct mpack_reader_t *         reader,
                 enum mpack_type_t               type,
                 struct mpack_tag_t * __restrict tag)
{
	dpack_assert_intern(reader);
	dpack_assert_intern(tag);
	dpack_assert_intern(mpack_reader_error(reader) == mpack_ok);

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

static void __dpack_nonull(1) __dpack_nothrow
dpack_decoder_discard(struct dpack_decoder * decoder,
                      enum mpack_type_t      type,
                      unsigned int           nr)
{
	dpack_assert_intern(decoder);

	if (dpack_decoder_error_state(&decoder->mpack))
		return;

	while (nr--)
		mpack_discard(&decoder->mpack);

	mpack_done_type(&decoder->mpack, type);
}

int
dpack_decoder_skip(struct dpack_decoder * decoder)
{
	/*
	 * Decoder MUST NOT be in error state since this interface is primarily
	 * meant for skipping deprecated / unwanted content.
	 */
	dpack_assert_api_decoder(decoder);

	mpack_discard(&decoder->mpack);

	return dpack_decoder_error_state(&decoder->mpack);
}

/*
 * Watch out !!
 *
 * This function is marked as __leaf for now. However, when mpack read tracking
 * is enabled, it calls mpack_reader_flag_error() which in turn may call a
 * function from the current compilation unit thanks to the reader error_fn()
 * function pointer of mpack.
 *
 * If modifying dpack_decoder_init_skip_buffer() and / or registering an error
 * function (thanks to mpack_reader_set_error_handler()) is required for
 * internal DPack purposes, MAKE SURE you return to current compilation unit
 * only by return or by exception handling.
 *
 * See Stroll's __leaf documentation for more infos.
 */
void
dpack_decoder_init_skip_buffer(struct dpack_decoder * decoder,
                               const char *           buffer,
                               size_t                 size)
{
	dpack_assert_api(decoder);
	dpack_assert_api(buffer);
	dpack_assert_api(size);

	mpack_reader_init_data(&decoder->mpack, buffer, size);
	decoder->intr = dpack_decoder_discard;
}

static void __dpack_nonull(1) __dpack_nothrow
dpack_decoder_abort(struct dpack_decoder * decoder,
                    enum mpack_type_t      type __unused,
                    unsigned int           nr __unused)
{
	dpack_assert_intern(decoder);

	mpack_reader_flag_error(&decoder->mpack, mpack_error_data);
}

/*
 * Watch out !!
 *
 * This function is marked as __leaf for now. However, when mpack read tracking
 * is enabled, it calls mpack_reader_flag_error() which in turn may call a
 * function from the current compilation unit thanks to the reader error_fn()
 * function pointer of mpack.
 *
 * If modifying dpack_decoder_init_buffer() and / or registering an error
 * function (thanks to mpack_reader_set_error_handler()) is required for
 * internal DPack purposes, MAKE SURE you return to current compilation unit
 * only by return or by exception handling.
 *
 * See Stroll's __leaf documentation for more infos.
 */
void
dpack_decoder_init_buffer(struct dpack_decoder * decoder,
                          const char *           buffer,
                          size_t                 size)
{
	dpack_assert_api(decoder);
	dpack_assert_api(buffer);
	dpack_assert_api(size);

	mpack_reader_init_data(&decoder->mpack, buffer, size);
	decoder->intr = dpack_decoder_abort;
}

/*
 * Watch out !!
 *
 * This function is marked as __leaf for now. However, it may call multiple
 * function pointers registered by the caller. See reader's teardown and
 * error_fn() function pointers.
 *
 * If modifying dpack_decoder_fini() and / or registering function pointers is
 * required for internal DPack purposes, MAKE SURE you return to current
 * compilation unit only by return or by exception handling.
 *
 * See Stroll's __leaf documentation for more infos.
 */
void
dpack_decoder_fini(struct dpack_decoder * decoder)
{
	dpack_assert_api(decoder);

	mpack_reader_destroy(&decoder->mpack);
}

#endif
