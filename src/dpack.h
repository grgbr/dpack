#ifndef _DPACK_H
#define _DPACK_H

#include <mpack.h>
#include <stdint.h>

#define DPACK_UINT_SIZE_MIN    (1U)

#define DPACK_U8_SIZE_MIN      (DPACK_UINT_SIZE_MIN)
#define DPACK_U8_SIZE_MAX      (2U)

#define DPACK_U16_SIZE_MIN     (DPACK_UINT_SIZE_MIN)
#define DPACK_U16_SIZE_MAX     (3U)

#define DPACK_U32_SIZE_MIN     (DPACK_UINT_SIZE_MIN)
#define DPACK_U32_SIZE_MAX     (5U)

#define DPACK_U64_SIZE_MIN     (DPACK_UINT_SIZE_MIN)
#define DPACK_U64_SIZE_MAX     (9U)

#define DPACK_FIELDID_SIZE_MIN (DPACK_U16_SIZE_MIN)
#define DPACK_FIELDID_SIZE_MAX (DPACK_U16_SIZE_MAX)

/******************************************************************************
 * Encoder / packer
 ******************************************************************************/

struct dpack_encoder {
	struct mpack_writer_t mpack;
};

extern int
dpack_encode_u8(struct dpack_encoder * encoder, uint8_t value);

extern int
dpack_encode_u16(struct dpack_encoder * encoder, uint16_t value);

extern int
dpack_encode_u32(struct dpack_encoder * encoder, uint32_t value);

extern int
dpack_encode_u64(struct dpack_encoder * encoder, uint64_t value);

extern size_t
dpack_encoder_space_used(struct dpack_encoder * encoder);

extern size_t
dpack_encoder_space_left(struct dpack_encoder * encoder);

extern void
dpack_init_buffer_encoder(struct dpack_encoder * encoder,
                          char *                 buffer,
                          size_t                 size);

extern void
dpack_exit_encoder(struct dpack_encoder * encoder);

/******************************************************************************
 * Decoder / unpacker
 ******************************************************************************/

struct dpack_decoder {
	struct mpack_reader_t mpack;
};

extern int
dpack_decode_u8(struct dpack_decoder * decoder, uint8_t * value);

extern int
dpack_decode_u8_min(struct dpack_decoder * decoder,
                    uint8_t                low,
                    uint8_t              * value);

extern int
dpack_decode_u8_max(struct dpack_decoder * decoder,
                    uint8_t                high,
                    uint8_t              * value);

extern int
dpack_decode_u16(struct dpack_decoder * decoder, uint16_t * value);

extern int
dpack_decode_u16_min(struct dpack_decoder * decoder,
                     uint16_t               low,
                     uint16_t             * value);

extern int
dpack_decode_u16_max(struct dpack_decoder * decoder,
                     uint16_t               high,
                     uint16_t             * value);

extern int
dpack_decode_u32(struct dpack_decoder * decoder, uint32_t * value);

extern int
dpack_decode_u32_min(struct dpack_decoder * decoder,
                     uint32_t               low,
                     uint32_t             * value);

extern int
dpack_decode_u32_max(struct dpack_decoder * decoder,
                     uint32_t               high,
                     uint32_t             * value);

extern int
dpack_decode_u64(struct dpack_decoder * decoder, uint64_t * value);

extern int
dpack_decode_u64_min(struct dpack_decoder * decoder,
                     uint64_t               low,
                     uint64_t             * value);

extern int
dpack_decode_u64_max(struct dpack_decoder * decoder,
                     uint64_t               high,
                     uint64_t             * value);

extern int
dpack_decode_field_id(struct dpack_decoder * decoder, uint16_t * id);

extern size_t
dpack_decoder_data_left(struct dpack_decoder * decoder);

extern void
dpack_init_buffer_decoder(struct dpack_decoder * decoder,
                          const char *           buffer,
                          size_t                 size);

extern void
dpack_exit_decoder(struct dpack_decoder * decoder);

#endif /* _DPACK_H */
