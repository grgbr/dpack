/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Encoder / Decoder interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      23 Apr 2023
 * @copyright Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _DPACK_CODEC_H
#define _DPACK_CODEC_H

#include <dpack/cdefs.h>
#include <stdint.h>
#include <errno.h>

/******************************************************************************
 * Encoder / packer
 ******************************************************************************/

struct dpack_encoder;

typedef size_t dpack_encoder_space_fn(const struct dpack_encoder * __restrict)
	__dpack_nonull(1) __warn_result;


typedef void dpack_encoder_write_fn(struct dpack_encoder * __restrict,
                                    const uint8_t * __restrict,
                                    size_t)
	__dpack_nonull(1, 2);

typedef void dpack_encoder_fini_fn(struct dpack_encoder * __restrict)
	__dpack_nonull(1);

struct dpack_encoder_ops {
	dpack_encoder_space_fn * left;
	dpack_encoder_space_fn * used;
	dpack_encoder_write_fn * write;
	dpack_encoder_fini_fn *  fini;
};

#define dpack_encoder_assert_ops_api(_ops) \
	dpack_assert_api(_ops); \
	dpack_assert_api((_ops)->left); \
	dpack_assert_api((_ops)->used); \
	dpack_assert_api((_ops)->write); \
	dpack_assert_api((_ops)->fini)

/**
 * A MessagePack encoder.
 *
 * An opaque structure allowing to encode data according to the
 * @rstsubst{MessagePack format}.
 */
struct dpack_encoder {
	const struct dpack_encoder_ops * ops;
};

#define DPACK_ENCODER_INIT(_ops) \
	{ .ops = _ops }

#define dpack_encoder_assert_api(_encoder) \
	dpack_assert_api(_encoder); \
	dpack_encoder_assert_ops_api((_encoder)->ops)

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
static inline __dpack_nonull(1) __dpack_pure __warn_result
size_t
dpack_encoder_space_left(const struct dpack_encoder * __restrict encoder)
{
	dpack_encoder_assert_api(encoder);

	return encoder->ops->left(encoder);
}

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
static inline __dpack_nonull(1) __dpack_pure __warn_result
size_t
dpack_encoder_space_used(const struct dpack_encoder * __restrict encoder)
{
	dpack_encoder_assert_api(encoder);

	return encoder->ops->used(encoder);
}

static inline __dpack_nonull(1, 2)
void
dpack_encoder_init(struct dpack_encoder * __restrict           encoder,
                   const struct dpack_encoder_ops * __restrict ops)
{
	dpack_assert_api(encoder);
	dpack_encoder_assert_ops_api(ops);

	encoder->ops= ops;
}

/**
 * Finalize a MessagePack encoder
 *
 * @param[inout] encoder encoder
 * @param[in]    abort   request abortion
 *
 * Release resources allocated for @p encoder.
 *
 * When specified as #DPACK_ABORT, @p abort argument request abortion of current
 * serialization and content of @p buffer previously registered at
 * dpack_encoder_init_buffer() time is undefined. Passing #DPACK_DONE instead
 * completes serialization successfully.
 *
 * @p buffer previously registered at dpack_encoder_init_buffer() time may
 * safely be @man{free(3)}'ed once dpack_encoder_fini() has been called only.
 *
 * @see
 * - dpack_encoder_init_buffer()
 */
static inline __dpack_nonull(1)
void
dpack_encoder_fini(struct dpack_encoder * __restrict encoder)
{
	dpack_encoder_assert_api(encoder);

	encoder->ops->fini(encoder);
}

struct dpack_encoder_buffer {
	struct dpack_encoder base;
	size_t               tail;
	size_t               capa;
	uint8_t *            buff;
};

extern const struct dpack_encoder_ops dpack_encoder_buffer_ops;

#define DPACK_ENCODER_INIT_BUFFER(_buff, _size) \
	{ \
		.base = DPACK_ENCODER_INIT(&dpack_encoder_buffer_ops), \
		.tail = 0, \
		.capa = _size, \
		.buff = _buff \
	}

/**
 * Initialize a MessagePack encoder with buffer space
 *
 * @param[inout] encoder encoder
 * @param[inout] buffer  memory buffer
 * @param[in]    size of @p buffer
 *
 * Initialize a @rstsubst{MessagePack} encoder for encoding / packing /
 * serialization purpose.
 *
 * @p buffer is a previously allocated memory area of size @p size and owned by
 * the caller. It is the responsibility of the caller to manage allocation and
 * release of @p buffer.
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
dpack_encoder_init_buffer(struct dpack_encoder_buffer * __restrict encoder,
                          uint8_t * __restrict                     buffer,
                          size_t                                   size)
	__dpack_nonull(1, 2) __dpack_export;

/******************************************************************************
 * Decoder / unpacker
 ******************************************************************************/

struct dpack_decoder;

typedef size_t dpack_decoder_left_fn(const struct dpack_decoder * __restrict)
	__dpack_nonull(1) __warn_result;

typedef void dpack_decoder_read_fn(struct dpack_decoder * __restrict,
                                   uint8_t * __restrict,
                                   size_t)
	__dpack_nonull(1, 2);

typedef void dpack_decoder_discard_fn(struct dpack_decoder * __restrict,
                                      size_t)
	__dpack_nonull(1);

typedef void dpack_decoder_fini_fn(struct dpack_decoder * __restrict)
	__dpack_nonull(1);

struct dpack_decoder_ops {
	dpack_decoder_left_fn *    left;
	dpack_decoder_read_fn *    read;
	dpack_decoder_discard_fn * disc;
	dpack_decoder_fini_fn *    fini;
};

#define DPACK_DECODER_INIT_OPS(_left, _read, _disc, _fini) \
	{ .left = _left, .read = _read, .disc = _disc, .fini = _fini }

#define dpack_decoder_assert_ops_api(_ops) \
	dpack_assert_api(_ops); \
	dpack_assert_api((_ops)->left); \
	dpack_assert_api((_ops)->read); \
	dpack_assert_api((_ops)->disc); \
	dpack_assert_api((_ops)->fini)

/**
 * A MessagePack decoder.
 *
 * An opaque structure allowing to decode data packed according to the
 * @rstsubst{MessagePack format} from an existing in-memory buffer.
 */
struct dpack_decoder {
	const struct dpack_decoder_ops * ops;
};

#define DPACK_DECODER_INIT(_ops) \
	{ .ops = _ops }

#define dpack_decoder_assert_api(_decoder) \
	dpack_assert_api(_decoder); \
	dpack_decoder_assert_ops_api((_decoder)->ops)

/**
 * Return size of data left to decode.
 *
 * @param[in] decoder decoder
 *
 * @return Size of encoded data left in bytes
 *
 * Compute the number of bytes of unconsumed encoded / packed / serialized data
 * remaining into the buffer assigned to @p decoder at initialization time.
 *
 * @warning
 * @p decoder *MUST* have been initialized using dpack_decoder_init_buffer() or
 * dpack_decoder_init_skip_buffer() before calling this function. Result is
 * undefined otherwise.
 *
 * @see
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
static inline __dpack_nonull(1) __dpack_pure __warn_result
size_t
dpack_decoder_data_left(const struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_api(decoder);

	return decoder->ops->left(decoder);
}

static inline __dpack_nonull(1, 2)
void
dpack_decoder_init(struct dpack_decoder * __restrict           decoder,
                   const struct dpack_decoder_ops * __restrict ops)
{
	dpack_assert_api(decoder);
	dpack_decoder_assert_ops_api(ops);

	decoder->ops= ops;
}

/**
 * Finalize a MessagePack decoder
 *
 * @param[inout] decoder decoder
 *
 * Release resources allocated for @p decoder.
 *
 * @p buffer previously registered at dpack_decoder_init_buffer() /
 * dpack_decoder_init_skip_buffer() time may safely be @man{free(3)}'ed once
 * dpack_decoder_fini() has been called only.
 *
 * @see
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
static inline __dpack_nonull(1)
void
dpack_decoder_fini(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_api(decoder);

	decoder->ops->fini(decoder);
}

struct dpack_decoder_buffer {
	struct dpack_decoder base;
	size_t               head;
	size_t               capa;
	const uint8_t *      buff;
};

extern const struct dpack_decoder_ops dpack_decoder_buffer_ops;

#define DPACK_DECODER_INIT_BUFFER(_buff, _size) \
	{ \
		.base = DPACK_DECODER_INIT(&dpack_decoder_buffer_ops), \
		.head = 0, \
		.capa = _size, \
		.buff = _buff \
	}

/**
 * Initialize a MessagePack decoder with buffer space
 *
 * @param[inout] decoder decoder
 * @param[in]    buffer  memory buffer
 * @param[in]    size of @p buffer
 *
 * Initialize a @rstsubst{MessagePack} decoder for decoding / unpacking /
 * deserialization purpose.
 *
 * @p buffer is a previously allocated memory area of size @p size and owned by
 * the caller. It is the responsibility of the caller to manage allocation and
 * release of @p buffer.
 *
 * Owner of @p buffer *MUST* call dpack_decoder_fini() before @man{free(3)}'ing
 * it.
 *
 * @warning
 * When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 * @p size is zero, result is undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decoder_fini()
 * - dpack_decoder_init_skip_buffer()
 */
extern void
dpack_decoder_init_buffer(struct dpack_decoder_buffer * __restrict decoder,
                          const uint8_t * __restrict               buffer,
                          size_t                                   size)
	 __dpack_nonull(1, 2) __dpack_nothrow __leaf __dpack_export;

#if 0
#include <dpack/mpack.h>

/******************************************************************************
 * Decoder / unpacker
 ******************************************************************************/

struct dpack_decoder;

/**
 * MessagePack collection item callback.
 *
 * @param[inout] decoder decoder
 * @param[in]    id      index of collection item
 * @param[inout] data    arbitrary location to client data
 *
 * @return 0 in case of success, a negative errno like error code otherwise.
 *
 * Signature of callback functions called by decoding functions to decode items
 * of @rstref{sect-api-array} or @rstref{sect-api-map} collections.
 *
 * @p decoder shall be given as a decoder initialized using
 * dpack_decoder_init_buffer() or dpack_decoder_init_skip_buffer().
 *
 * @p id shall be given as the index of the next item within the current
 * collection that is being decoded, starting from zero.
 *
 * The @p data argument passed to the collection decoding function
 * (dpack_array_decode(), dpack_map_decode(), ...) shall be given as the @p data
 * argument to the callback function. It should point to an optional arbitrary
 * location owned by the client application.
 *
 * The callback function should **return** ``0`` in case of success. When
 * returning a *negative error* code, current collection decoding process is
 * interrupted and the error code is returned to the caller of the collection
 * decoding function (dpack_array_decode(), dpack_map_decode(), ...).
 *
 * @see
 * - dpack_array_decode()
 * - dpack_map_decode()
 * - dpack_decoder_init_buffer()
 * - dpack_decoder_init_skip_buffer()
 */
typedef int dpack_decode_item_fn(struct dpack_decoder * decoder,
                                 unsigned int           id,
                                 void * __restrict      data);

typedef void dpack_decoder_intr_fn(struct dpack_decoder * decoder,
                                   enum mpack_type_t      type,
                                   unsigned int           nr);

/**
 * Skip next item of encoded data
 *
 * @param[inout] decoder decoder
 *
 * While decoding, this function skip the next item of encoded data. When at the
 * start of a structured data set / data items collection decoding, the entire
 * data aggregate is skipped.
 *
 * @warning
 * When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 * @p decoder is in an error state, result is undefined. An assertion is
 * triggered otherwise.
 *
 * @see
 * - dpack_decoder_fini()
 * - dpack_decoder_init_skip_buffer()
 */
extern int
dpack_decoder_skip(struct dpack_decoder * decoder)
	__dpack_nonull(1) __dpack_nothrow __warn_result __dpack_export;

/**
 * Initialize a MessagePack decoder with buffer space and data skipping ability
 *
 * @param[inout] decoder decoder
 * @param[in]    buffer  memory buffer
 * @param[in]    size of @p buffer
 *
 * Initialize a @rstsubst{MessagePack} decoder for decoding / unpacking /
 * deserialization purpose. The decoder will maintain internal state required to
 * skip encoded aggregates / structured data set (such as @rstref{sect-api-map}
 * or @rstref{sect-api-array}).
 *
 * @p buffer is a previously allocated memory area of size @p size and owned by
 * the caller. It is the responsibility of the caller to manage allocation and
 * release of @p buffer.
 *
 * Owner of @p buffer *MUST* call dpack_decoder_fini() before @man{free(3)}'ing
 * it.
 *
 * @note
 * Skipping ability is useful when decoding byte streams which size or end is
 * unknown.
 * While in the middle of the decoding process of a complex structured data sets
 * and / or collections of data items, @p decoder has the ability to ignore the
 * current data aggregate which resulted in data consistency failure.
 * See @rstref{sect-api-map} and @rstref{sect-api-array} for further
 * informations.
 *
 * @warning
 * When compiled with the #CONFIG_DPACK_ASSERT_API build option disabled and
 * @p size is zero, result is undefined. An assertion is triggered otherwise.
 *
 * @see
 * - dpack_decoder_fini()
 * - dpack_decoder_init_buffer()
 */
extern void
dpack_decoder_init_skip_buffer(struct dpack_decoder * decoder,
                               const char *           buffer,
                               size_t                 size)
	__dpack_nonull(1, 2) __dpack_nothrow __leaf __dpack_export;

#endif

#endif /* _DPACK_CODEC_H */
