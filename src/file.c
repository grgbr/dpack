/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/codec.h"
#include "common.h"
#include <utils/fd.h>
#include <stroll/page.h>
#include <string.h>

/******************************************************************************
 * File descriptor based encoder / packer
 ******************************************************************************/

#define dpack_encoder_assert_fd_api(_encoder) \
	dpack_assert_api(_encoder); \
	dpack_encoder_assert_api(&(_encoder)->base); \
	dpack_assert_api((_encoder)->tail < stroll_page_size()); \
	dpack_assert_api((_encoder)->buff)

#define dpack_encoder_assert_fd_intern(_encoder) \
	dpack_assert_intern(_encoder); \
	dpack_encoder_assert_intern(&(_encoder)->base); \
	dpack_assert_intern((_encoder)->tail < stroll_page_size()); \
	dpack_assert_intern((_encoder)->buff)

static __dpack_nonull(1) __dpack_pure __warn_result
size_t
dpack_encoder_fd_left(const struct dpack_encoder * __restrict encoder)
{
	dpack_encoder_assert_fd_api((const struct dpack_encoder_fd *)encoder);

	return SIZE_MAX;
}

static __dpack_nonull(1) __dpack_pure __warn_result
size_t
dpack_encoder_fd_used(const struct dpack_encoder * __restrict encoder)
{
	dpack_encoder_assert_fd_api((const struct dpack_encoder_fd *)encoder);

	const struct dpack_encoder_fd * enc =
		(const struct dpack_encoder_fd *)encoder;

	return enc->off + enc->tail;
}

static __dpack_nonull(1, 2) __warn_result
int
dpack_encoder_fd_flush(struct dpack_encoder_fd * __restrict encoder,
                       const uint8_t * __restrict           buffer,
                       size_t                               size)
{
	dpack_encoder_assert_fd_intern(encoder);
	dpack_assert_intern(encoder->fd >= 0);
	dpack_assert_intern(buffer);
	dpack_assert_intern(size);

	while (true) {
		ssize_t bytes;

		bytes = ufd_nointr_write(encoder->fd,
		                         (const char *)buffer,
		                         size);
		if ((size_t)bytes == size)
			return 0;
		else if (bytes < 0)
			return (int)bytes;

		buffer += bytes;
		size -= (size_t)bytes;
	}

	unreachable();
}

static __dpack_nonull(1, 2) __warn_result
int
dpack_encoder_fd_write(struct dpack_encoder * __restrict encoder,
                       const uint8_t * __restrict        data,
                       size_t                            size)
{
	dpack_encoder_assert_fd_api((struct dpack_encoder_fd *)encoder);
	dpack_assert_api(((const struct dpack_encoder_fd *)encoder)->fd >= 0);
	dpack_assert_api(data);
	dpack_assert_api(size);

	struct dpack_encoder_fd * enc = (struct dpack_encoder_fd *)encoder;
	size_t                    pgsz = stroll_page_size();
	size_t                    avail = pgsz - enc->tail;
	int                       err;

	if (size < avail)
		/* Entire data fit into buffer... */
		goto buff;

	/* Store as much data as we can into buffer and flush it onto disk. */
	memcpy(&enc->buff[enc->tail], data, avail);
	err = dpack_encoder_fd_flush(enc, enc->buff, pgsz);
	if (err)
		goto err;
	enc->tail = 0;
	enc->off += pgsz;

	size -= avail;
	if (size) {
		/* There are still somer more data to write. */
		data += avail;
		if (size < pgsz)
			/* Remaining data fit into buffer. */
			goto buff;

		/* Flush remaining data onto disk. */
		err = dpack_encoder_fd_flush(enc, data, size);
		if (err)
			goto err;
		enc->off += size;
	}

	return 0;

buff:
	/* Store remaining data into buffer. */
	memcpy(&enc->buff[enc->tail], data, size);
	enc->tail += size;

	return 0;

err:
	enc->fd = -1;

	return err;
}

static __dpack_nonull(1) __warn_result
int
dpack_encoder_fd_fini(struct dpack_encoder * __restrict encoder)
{
	dpack_encoder_assert_fd_api((const struct dpack_encoder_fd *)encoder);

	struct dpack_encoder_fd * enc = (struct dpack_encoder_fd *)encoder;
	int                       ret;

	if (enc->tail && (enc->fd >= 0))
		ret = dpack_encoder_fd_flush(enc, enc->buff, enc->tail);
	else
		ret = 0;

	free(enc->buff);

	return ret;
}

static const struct dpack_encoder_ops dpack_encoder_fd_ops = {
	.left  = dpack_encoder_fd_left,
	.used  = dpack_encoder_fd_used,
	.write = dpack_encoder_fd_write,
	.fini  = dpack_encoder_fd_fini
};

int
dpack_encoder_init_fd(struct dpack_encoder_fd * __restrict encoder, int fd)
{
	dpack_assert_api(encoder);
	dpack_assert_api(fd >= 0);

	uint8_t * buff;

	buff = malloc(stroll_page_size());
	if (!buff)
		return -errno;

	dpack_encoder_init(&encoder->base, &dpack_encoder_fd_ops);
	encoder->tail = 0;
	encoder->buff = buff;
	encoder->off = 0;
	encoder->fd = fd;

	return 0;
}

#if 0

/******************************************************************************
 * File descriptor based decoder / unpacker
 ******************************************************************************/

#define dpack_decoder_assert_fd_api(_decoder) \
	dpack_assert_api(_decoder); \
	dpack_decoder_assert_api(&(_decoder)->base); \
	dpack_assert_api((_decoder)->capa); \
	dpack_assert_api((_decoder)->head <= (_decoder)->capa); \
	dpack_assert_api((_decoder)->buff)

static __dpack_nonull(1) __dpack_pure __warn_result
size_t
dpack_decoder_fd_left(const struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_fd_api((const struct dpack_decoder_fd *)
	                                decoder);

	const struct dpack_decoder_fd * dec =
		(const struct dpack_decoder_fd *)decoder;

	return dec->capa - dec->head;
}

static __dpack_nonull(1, 2)
int
dpack_decoder_fd_read(struct dpack_decoder * __restrict decoder,
                          uint8_t * __restrict              data,
                          size_t                            size)
{
	dpack_decoder_assert_fd_api((const struct dpack_decoder_fd *)
	                                decoder);
	dpack_assert_intern(data);
	dpack_assert_intern(size);

	struct dpack_decoder_fd * dec = (struct dpack_decoder_fd *)
	                                    decoder;

	if (size <= (dec->capa - dec->head)) {
		memcpy(data, &dec->buff[dec->head], size);
		dec->head += size;
		return 0;
	}

	return -ENODATA;
}

static __dpack_nonull(1)
int
dpack_decoder_fd_skip(struct dpack_decoder * __restrict decoder,
                          size_t                            size)
{
	dpack_decoder_assert_fd_api((const struct dpack_decoder_fd *)
	                                decoder);
	dpack_assert_intern(size);

	struct dpack_decoder_fd * dec = (struct dpack_decoder_fd *)
	                                    decoder;

	if (size <= (dec->capa - dec->head)) {
		dec->head += size;
		return 0;
	}

	return -ENODATA;
}

static __dpack_nonull(1)
void
dpack_decoder_fd_fini(struct dpack_decoder * __restrict decoder __unused)
{
	dpack_decoder_assert_fd_api((const struct dpack_decoder_fd *)
	                                decoder);
}

const struct dpack_decoder_ops dpack_decoder_fd_ops = {
	.left = dpack_decoder_fd_left,
	.read = dpack_decoder_fd_read,
	.skip = dpack_decoder_fd_skip,
	.fini = dpack_decoder_fd_fini
};

int
_dpack_decoder_init_fd(struct dpack_decoder_fd * __restrict decoder,
                       int                                  fd,
                       bool                                 discard)
{
	dpack_assert_api(decoder);
	dpack_assert_api(fd >= 0);

	struct stat st;
	int         err;
	uint8_t *   buff;

	err = ufd_fstat(fd, &st);
	if (err)
		return err;

	buff = malloc((size_t)st->st_blksize);
	if (!buff)
		return -errno;

	err = ufd_ftruncate(fd, 0);
	if (err)
		goto free;

	err = ufd_lseek(fd, 0, SEEK_SET);
	if (err)
		goto free;

	dpack_decoder_init(&decoder->base,
	                   &dpack_decoder_fd_ops,
	                   discard);
	decoder->head = 0;
	decoder->capa = size;
	decoder->buff = file;
	decoder->fd = fd;
	decoder->off = 0;

free:
	free(buff);

	return err;
}

#endif
