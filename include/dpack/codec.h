#ifndef _DPACK_CODEC_H
#define _DPACK_CODEC_H

#include <dpack/cdefs.h>
#include <mpack.h>

/******************************************************************************
 * Encoder / packer
 ******************************************************************************/

struct dpack_encoder {
	struct mpack_writer_t mpack;
};

extern size_t
dpack_encoder_space_used(struct dpack_encoder * encoder) __dpack_export;

extern size_t
dpack_encoder_space_left(struct dpack_encoder * encoder) __dpack_export;

extern void
dpack_init_buffer_encoder(struct dpack_encoder * encoder,
                          char *                 buffer,
                          size_t                 size) __dpack_export;

extern void
dpack_exit_encoder(struct dpack_encoder * encoder) __dpack_export;

/******************************************************************************
 * Decoder / unpacker
 ******************************************************************************/

struct dpack_decoder {
	struct mpack_reader_t mpack;
};

extern size_t
dpack_decoder_data_left(struct dpack_decoder * decoder) __dpack_export;

extern void
dpack_init_buffer_decoder(struct dpack_decoder * decoder,
                          const char *           buffer,
                          size_t                 size) __dpack_export;

extern void
dpack_exit_decoder(struct dpack_decoder * decoder) __dpack_export;

#endif /* _DPACK_CODEC_H */
