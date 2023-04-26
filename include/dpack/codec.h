/**
 * @file
 * Encoder / Decoder interface
 *
 * @author       Grégor Boirie <gregor.boirie@free.fr>
 * @date         23 Apr 2023
 * @copyright    Copyright (C) 2017-2023 Grégor Boirie.
 * @licensestart GNU Lesser General Public License (LGPL) v3
 *
 * This file is part of libdpack
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, If not, see <http://www.gnu.org/licenses/>.
 * @licenseend
 */
#ifndef _DPACK_CODEC_H
#define _DPACK_CODEC_H

#include <dpack/cdefs.h>
#include <dpack/mpack.h>

/******************************************************************************
 * Encoder / packer
 ******************************************************************************/

/**
 * A MessagePack encoder.
 *
 * An opaque structure allowing to encode data into an existing in-memory buffer
 * according to the @rstsubst{MessagePack} format.
 */
struct dpack_encoder {
	struct mpack_writer_t mpack;
};

/**
 * Return buffer space used by encoded bytes.
 *
 * @param[in] encoder encoder
 *
 * @return Size of buffer space used in bytes
 *
 * Compute the number of bytes already encoded / packed / serialized into the
 * buffer assigned to @p encoder at initialization time.
 *
 * @warning
 * @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 * before calling this function. Result is undefined otherwise.
 *
 * @see
 * - dpack_encoder_space_left()
 * - dpack_encoder_init_buffer()
 */
extern size_t
dpack_encoder_space_used(struct dpack_encoder * encoder) __dpack_nonull(1)
                                                         __dpack_pure
                                                         __dpack_nothrow
                                                         __leaf
                                                         __warn_result
                                                         __dpack_export;

/**
 * Return buffer space left for encoding purpose.
 *
 * @param[in] encoder encoder
 *
 * @return Size of buffer space left in bytes
 *
 * Compute the number of bytes available for encoding / packing / serialization
 * into the buffer assigned to @p encoder at initialization time.
 *
 * @warning
 * @p encoder *MUST* have been initialized using dpack_encoder_init_buffer()
 * before calling this function. Result is undefined otherwise.
 *
 * @see
 * - dpack_encoder_space_used()
 * - dpack_encoder_init_buffer()
 */
extern size_t
dpack_encoder_space_left(struct dpack_encoder * encoder) __dpack_nonull(1)
                                                         __dpack_pure
                                                         __dpack_nothrow
                                                         __leaf
                                                         __warn_result
                                                         __dpack_export;

/**
 * Initialize a MessagePack encoder with buffer space
 *
 * @param[in]    encoder encoder
 * @param[inout] buffer  memory buffer
 * @param[in]    size of @p buffer
 *
 * Initialize a MessagePack encoder for encoding / packing / serialization
 * purpose.
 *
 * @p buffer is a previously allocated memory area of size @p size and owned by
 * the caller. It is the responsibility of the caller to manage allocation and
 * free of @p buffer.
 *
 * Owner of @p buffer *MUST* call dpack_encoder_fini() before @man{free(3)}'ing
 * it.
 *
 * @warning
 * When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 * @p size is zero, result is undefined. An assertion is triggered otherwise.
 *
 * @see
 * dpack_encoder_fini()
 */
extern void
dpack_encoder_init_buffer(struct dpack_encoder * encoder,
                          char *                 buffer,
                          size_t                 size)
	__dpack_nonull(1, 2) __dpack_nothrow __leaf __dpack_export;

/**
 * Finalize a MessagePack encoder
 *
 * @param[in] encoder encoder
 *
 * Release resources allocated for @p encoder.
 *
 * @p buffer previously registered at dpack_encoder_init_buffer() time may
 * safely be @man{free(3)}'ed once dpack_encoder_fini() has been called only.
 *
 * @see
 * dpack_encoder_init_buffer()
 */
extern void
dpack_encoder_fini(struct dpack_encoder * encoder)
	__dpack_nonull(1) __dpack_nothrow __leaf __dpack_export;

/******************************************************************************
 * Decoder / unpacker
 ******************************************************************************/

struct dpack_decoder;

typedef int (dpack_decode_item_fn)(struct dpack_decoder * decoder,
                                   unsigned int           id,
                                   void * __restrict      data);

typedef void (dpack_decoder_intr_fn)(struct dpack_decoder * decoder,
                                     enum mpack_type_t      type,
                                     unsigned int           nr);

/**
 * A MessagePack decoder.
 *
 * An opaque structure allowing to decode data packed according to the
 * @rstsubst{MessagePack} format from an existing in-memory buffer.
 */
struct dpack_decoder {
	struct mpack_reader_t   mpack;
	dpack_decoder_intr_fn * intr;
};

extern size_t
dpack_decoder_data_left(struct dpack_decoder * decoder) __dpack_nonull(1)
#if !defined(MPACK_READ_TRACKING) || (MPACK_READ_TRACKING == 0)
	                                                __dpack_pure
#endif /* !defined(MPACK_READ_TRACKING) || (MPACK_READ_TRACKING == 0) */
	                                                __dpack_nothrow
	                                                __leaf
	                                                __warn_result
	                                                __dpack_export;

extern int
dpack_decoder_skip(struct dpack_decoder * decoder)
	__dpack_nonull(1) __dpack_nothrow __warn_result __dpack_export;

extern void
dpack_decoder_init_buffer(struct dpack_decoder * decoder,
                          const char *           buffer,
                          size_t                 size)
	__dpack_nonull(1, 2) __dpack_nothrow __leaf __dpack_export;

extern void
dpack_decoder_init_skip_buffer(struct dpack_decoder * decoder,
                               const char *           buffer,
                               size_t                 size)
	__dpack_nonull(1, 2) __dpack_nothrow __leaf __dpack_export;

extern void
dpack_decoder_fini(struct dpack_decoder * decoder) __dpack_nonull(1)
                                                   __dpack_nothrow
						   __leaf
						   __dpack_export;

#endif /* _DPACK_CODEC_H */
