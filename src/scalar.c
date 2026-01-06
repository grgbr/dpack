/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/scalar.h"
#include "dpack/codec.h"
#include "common.h"
#if defined(CONFIG_DPACK_FLOAT) || defined(CONFIG_DPACK_DOUBLE)
#include <math.h>
#endif /* defined(CONFIG_DPACK_FLOAT) || defined(CONFIG_DPACK_DOUBLE) */

/******************************************************************************
 * Boolean
 ******************************************************************************/

int
dpack_encode_bool(struct dpack_encoder * __restrict encoder, bool value)
{
	dpack_encoder_assert_api(encoder);

	return dpack_write_tag(encoder,
	                       value ? DPACK_TRUE_TAG : DPACK_FALSE_TAG);
}

int
dpack_decode_bool(struct dpack_decoder * __restrict decoder,
                  bool * __restrict                 value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err) {
		switch (tag) {
		case DPACK_FALSE_TAG:
			*value = false;
			return 0;

		case DPACK_TRUE_TAG:
			*value = true;
			return 0;

		default:
			break;
		}

		return -ENOMSG;
	}

	return err;
}

/******************************************************************************
 * 8 bits integer
 ******************************************************************************/

int
dpack_encode_uint8(struct dpack_encoder * __restrict encoder,
                   uint8_t                           value)
{
	dpack_encoder_assert_api(encoder);

	if (value > INT8_MAX) {
		int err;

		err = dpack_write_tag(encoder, DPACK_UINT8_TAG);
		if (err)
			return err;
	}

	return dpack_encoder_write(encoder, &value, sizeof(value));
}

static __dpack_nonull(1, 3) __warn_result
int
dpack_load_uint8(struct dpack_decoder * __restrict decoder,
                 uint8_t                           tag,
                 uint8_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	switch (tag) {
	case DPACK_FIXUINT_TAG:
		*value = tag;
		return 0;

	case DPACK_UINT8_TAG:
		return dpack_decoder_read(decoder, value, sizeof(*value));

	case DPACK_FIXINT_TAG:
		if ((int8_t)tag >= 0) {
			*value = tag;
			return 0;
		}
		break;

	case DPACK_INT8_TAG:
		{
			int err;

			err = dpack_decoder_read(decoder,
			                         value,
			                         sizeof(*value));
			if (!err) {
				if (*(int8_t *)value >= 0)
					return 0;
				break;
			}
			return err;
		}

	default:
		break;
	}

	return -ENOMSG;
}

int
dpack_decode_uint8(struct dpack_decoder * __restrict decoder,
                   uint8_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err)
		return dpack_load_uint8(decoder, tag, value);

	return err;
}

int
dpack_decode_uint8_min(struct dpack_decoder * __restrict decoder,
                       uint8_t                           low,
                       uint8_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low);
	dpack_assert_api(low < UINT8_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_uint8(decoder, value);
	if (err)
		return err;

	if (*value >= low)
		return 0;

	return -ERANGE;
}

int
dpack_decode_uint8_max(struct dpack_decoder * __restrict decoder,
                       uint8_t                           high,
                       uint8_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(high);
	dpack_assert_api(high < UINT8_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_uint8(decoder, value);
	if (err)
		return err;

	if (*value <= high)
		return 0;

	return -ERANGE;
}

int
dpack_decode_uint8_range(struct dpack_decoder * __restrict decoder,
                         uint8_t                           low,
                         uint8_t                           high,
                         uint8_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low);
	dpack_assert_api(high < UINT8_MAX);
	dpack_assert_api(low < high);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_uint8(decoder, value);
	if (err)
		return err;

	if ((*value >= low) && (*value <= high))
		return 0;

	return -ERANGE;
}

int
dpack_encode_int8(struct dpack_encoder * __restrict encoder,
                  int8_t                            value)
{
	dpack_encoder_assert_api(encoder);

	if (value < -32) {
		int err;

		err = dpack_write_tag(encoder, DPACK_INT8_TAG);
		if (err)
			return err;
	}

	return dpack_encoder_write(encoder,
	                           (const uint8_t *)&value,
	                           sizeof(value));
}

static __dpack_nonull(1, 3) __warn_result
int
dpack_load_int8(struct dpack_decoder * __restrict decoder,
                uint8_t                           tag,
                int8_t * __restrict               value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	switch (tag) {
	case DPACK_FIXINT_TAG:
		*value = (int8_t)tag;
		return 0;

	case DPACK_INT8_TAG:
		return dpack_decoder_read(decoder,
		                          (uint8_t *)value,
		                          sizeof(*value));

	case DPACK_FIXUINT_TAG:
		if ((int8_t)tag >= 0) {
			*value = (int8_t)tag;
			return 0;
		}
		break;

	case DPACK_UINT8_TAG:
		{
			int err;

			err = dpack_decoder_read(decoder,
			                         (uint8_t *)value,
			                         sizeof(*value));
			if (!err) {
				if (*value >= 0)
					return 0;
				break;
			}
			return err;
		}

	default:
		break;
	}

	return -ENOMSG;
}

int
dpack_decode_int8(struct dpack_decoder * __restrict decoder,
                  int8_t * __restrict               value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err)
		return dpack_load_int8(decoder, tag, value);

	return err;
}

int
dpack_decode_int8_min(struct dpack_decoder * __restrict decoder,
                      int8_t                            low,
                      int8_t * __restrict               value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low > INT8_MIN);
	dpack_assert_api(low < INT8_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_int8(decoder, value);
	if (err)
		return err;

	if (*value >= low)
		return 0;

	return -ERANGE;
}

int
dpack_decode_int8_max(struct dpack_decoder * __restrict decoder,
                      int8_t                            high,
                      int8_t * __restrict               value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(high > INT8_MIN);
	dpack_assert_api(high < INT8_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_int8(decoder, value);
	if (err)
		return err;

	if (*value <= high)
		return 0;

	return -ERANGE;
}

int
dpack_decode_int8_range(struct dpack_decoder * __restrict decoder,
                        int8_t                            low,
                        int8_t                            high,
                        int8_t * __restrict               value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low > INT8_MIN);
	dpack_assert_api(high < INT8_MAX);
	dpack_assert_api(low < high);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_int8(decoder, value);
	if (err)
		return err;

	if ((*value >= low) && (*value <= high))
		return 0;

	return -ERANGE;
}

/******************************************************************************
 * 16 bits integer
 ******************************************************************************/

int
dpack_encode_uint16(struct dpack_encoder * __restrict encoder,
                    uint16_t                          value)
{
	dpack_encoder_assert_api(encoder);

	if (value > UINT8_MAX) {
		int err;

		err = dpack_write_tag(encoder, DPACK_UINT16_TAG);
		if (err)
			return err;

		value = htobe16(value);

		return dpack_encoder_write(encoder,
		                           (const uint8_t *)&value,
		                           sizeof(value));
	}

	return dpack_encode_uint8(encoder, (uint8_t)value);
}

static __dpack_nonull(1, 3) __warn_result
int
dpack_load_uint16(struct dpack_decoder * __restrict decoder,
                  uint8_t                           tag,
                  uint16_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	int err;

	switch (tag) {
	case DPACK_UINT16_TAG:
		err = dpack_decoder_read(decoder,
		                         (uint8_t *)value,
		                         sizeof(*value));
		if (!err) {
			*value = be16toh(*value);
			return 0;
		}
		break;

	case DPACK_INT16_TAG:
		{
			uint16_t val;

			err = dpack_decoder_read(decoder,
			                         (uint8_t *)&val,
			                         sizeof(val));
			if (!err) {
				val = be16toh(val);
				if ((int16_t)val >= 0) {
					*value = val;
					return 0;
				}
				err = -ENOMSG;
			}
			break;
		}

	default:
		{
			uint8_t val;

			err = dpack_load_uint8(decoder, tag, &val);
			if (!err) {
				*value = (uint16_t)val;
				return 0;
			}
			break;
		}
	}

	return err;
}

int
dpack_decode_uint16(struct dpack_decoder * __restrict decoder,
                    uint16_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err)
		return dpack_load_uint16(decoder, tag, value);

	return err;
}

int
dpack_decode_uint16_min(struct dpack_decoder * __restrict decoder,
                        uint16_t                          low,
                        uint16_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low);
	dpack_assert_api(low < UINT16_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_uint16(decoder, value);
	if (err)
		return err;

	if (*value >= low)
		return 0;

	return -ERANGE;
}

int
dpack_decode_uint16_max(struct dpack_decoder * __restrict decoder,
                        uint16_t                          high,
                        uint16_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(high);
	dpack_assert_api(high < UINT16_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_uint16(decoder, value);
	if (err)
		return err;

	if (*value <= high)
		return 0;

	return -ERANGE;
}

int
dpack_decode_uint16_range(struct dpack_decoder * __restrict decoder,
                          uint16_t                          low,
                          uint16_t                          high,
                          uint16_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low);
	dpack_assert_api(high < UINT16_MAX);
	dpack_assert_api(low < high);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_uint16(decoder, value);
	if (err)
		return err;

	if ((*value >= low) && (*value <= high))
		return 0;

	return -ERANGE;
}

int
dpack_encode_int16(struct dpack_encoder * __restrict encoder,
                   int16_t                           value)
{
	dpack_encoder_assert_api(encoder);

	if ((value < INT8_MIN) || (value > INT8_MAX)) {
		int err;

		err = dpack_write_tag(encoder, DPACK_INT16_TAG);
		if (err)
			return err;

		value = (int16_t)htobe16((uint16_t)value);

		return dpack_encoder_write(encoder,
		                           (const uint8_t *)&value,
		                           sizeof(value));
	}

	return dpack_encode_int8(encoder, (int8_t)value);
}

static __dpack_nonull(1, 3) __warn_result
int
dpack_load_int16(struct dpack_decoder * __restrict decoder,
                 uint8_t                           tag,
                 int16_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	int err;

	switch (tag) {
	case DPACK_INT16_TAG:
		err = dpack_decoder_read(decoder,
		                         (uint8_t *)value,
		                         sizeof(*value));
		if (!err) {
			*value = (int16_t)be16toh(*(uint16_t *)value);
			return 0;
		}
		break;

	case DPACK_UINT16_TAG:
		{
			uint16_t val;

			err = dpack_decoder_read(decoder,
			                         (uint8_t *)&val,
			                         sizeof(val));
			if (!err) {
				val = be16toh(val);
				if ((int16_t)val >= 0) {
					*value = (int16_t)val;
					return 0;
				}
				err = -ENOMSG;
			}
			break;
		}

	default:
		{
			int8_t val;

			err = dpack_load_int8(decoder, tag, &val);
			if (!err) {
				*value = (int16_t)val;
				return 0;
			}
			break;
		}
	}

	return err;
}

int
dpack_decode_int16(struct dpack_decoder * __restrict decoder,
                   int16_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err)
		return dpack_load_int16(decoder, tag, value);

	return err;
}

int
dpack_decode_int16_min(struct dpack_decoder * __restrict decoder,
                       int16_t                           low,
                       int16_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low > INT16_MIN);
	dpack_assert_api(low < INT16_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_int16(decoder, value);
	if (err)
		return err;

	if (*value >= low)
		return 0;

	return -ERANGE;
}

int
dpack_decode_int16_max(struct dpack_decoder * __restrict decoder,
                       int16_t                           high,
                       int16_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(high > INT16_MIN);
	dpack_assert_api(high < INT16_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_int16(decoder, value);
	if (err)
		return err;

	if (*value <= high)
		return 0;

	return -ERANGE;
}

int
dpack_decode_int16_range(struct dpack_decoder * __restrict decoder,
                         int16_t                           low,
                         int16_t                           high,
                         int16_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low > INT16_MIN);
	dpack_assert_api(high < INT16_MAX);
	dpack_assert_api(low < high);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_int16(decoder, value);
	if (err)
		return err;

	if ((*value >= low) && (*value <= high))
		return 0;

	return -ERANGE;
}

/******************************************************************************
 * 32 bits integer
 ******************************************************************************/

int
dpack_encode_uint32(struct dpack_encoder * __restrict encoder,
                    uint32_t                          value)
{
	dpack_encoder_assert_api(encoder);

	if (value > UINT16_MAX) {
		int err;

		err = dpack_write_tag(encoder, DPACK_UINT32_TAG);
		if (err)
			return err;

		value = htobe32(value);

		return dpack_encoder_write(encoder,
		                           (const uint8_t *)&value,
		                           sizeof(value));
	}

	return dpack_encode_uint16(encoder, (uint16_t)value);
}

static __dpack_nonull(1, 3) __warn_result
int
dpack_load_uint32(struct dpack_decoder * __restrict decoder,
                  uint8_t                           tag,
                  uint32_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	int err;

	switch (tag) {
	case DPACK_UINT32_TAG:
		err = dpack_decoder_read(decoder,
		                         (uint8_t *)value,
		                         sizeof(*value));
		if (!err) {
			*value = be32toh(*value);
			return 0;
		}
		break;

	case DPACK_INT32_TAG:
		{
			uint32_t val;

			err = dpack_decoder_read(decoder,
			                         (uint8_t *)&val,
			                         sizeof(val));
			if (!err) {
				val = be32toh(val);
				if ((int32_t)val >= 0) {
					*value = val;
					return 0;
				}
				err = -ENOMSG;
			}
			break;
		}

	default:
		{
			uint16_t val;

			err = dpack_load_uint16(decoder, tag, &val);
			if (!err) {
				*value = (uint32_t)val;
				return 0;
			}
			break;
		}
	}

	return err;
}

int
dpack_decode_uint32(struct dpack_decoder * __restrict decoder,
                    uint32_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err)
		return dpack_load_uint32(decoder, tag, value);

	return err;
}

int
dpack_decode_uint32_min(struct dpack_decoder * __restrict decoder,
                        uint32_t                          low,
                        uint32_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low);
	dpack_assert_api(low < UINT32_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_uint32(decoder, value);
	if (err)
		return err;

	if (*value >= low)
		return 0;

	return -ERANGE;
}

int
dpack_decode_uint32_max(struct dpack_decoder * __restrict decoder,
                        uint32_t                          high,
                        uint32_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(high);
	dpack_assert_api(high < UINT32_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_uint32(decoder, value);
	if (err)
		return err;

	if (*value <= high)
		return 0;

	return -ERANGE;
}

int
dpack_decode_uint32_range(struct dpack_decoder * __restrict decoder,
                          uint32_t                          low,
                          uint32_t                          high,
                          uint32_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low);
	dpack_assert_api(high < UINT32_MAX);
	dpack_assert_api(low < high);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_uint32(decoder, value);
	if (err)
		return err;

	if ((*value >= low) && (*value <= high))
		return 0;

	return -ERANGE;
}

int
dpack_encode_int32(struct dpack_encoder * __restrict encoder,
                   int32_t                           value)
{
	dpack_encoder_assert_api(encoder);

	if ((value < INT16_MIN) || (value > INT16_MAX)) {
		int err;

		err = dpack_write_tag(encoder, DPACK_INT32_TAG);
		if (err)
			return err;

		value = (int32_t)htobe32((uint32_t)value);

		return dpack_encoder_write(encoder,
		                           (const uint8_t *)&value,
		                           sizeof(value));
	}

	return dpack_encode_int16(encoder, (int16_t)value);
}

static __dpack_nonull(1, 3) __warn_result
int
dpack_load_int32(struct dpack_decoder * __restrict decoder,
                 uint8_t                           tag,
                 int32_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	int err;

	switch (tag) {
	case DPACK_INT32_TAG:
		err = dpack_decoder_read(decoder,
		                         (uint8_t *)value,
		                         sizeof(*value));
		if (!err) {
			*value = (int32_t)be32toh(*(uint32_t *)value);
			return 0;
		}
		break;

	case DPACK_UINT32_TAG:
		{
			uint32_t val;

			err = dpack_decoder_read(decoder,
			                         (uint8_t *)&val,
			                         sizeof(val));
			if (!err) {
				val = be32toh(val);
				if ((int32_t)val >= 0) {
					*value = (int32_t)val;
					return 0;
				}
				err = -ENOMSG;
			}
			break;
		}

	default:
		{
			int16_t val;

			err = dpack_load_int16(decoder, tag, &val);
			if (!err) {
				*value = (int32_t)val;
				return 0;
			}
			break;
		}
	}

	return err;
}

int
dpack_decode_int32(struct dpack_decoder * __restrict decoder,
                   int32_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err)
		return dpack_load_int32(decoder, tag, value);

	return err;
}

int
dpack_decode_int32_min(struct dpack_decoder * __restrict decoder,
                       int32_t                           low,
                       int32_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low > INT32_MIN);
	dpack_assert_api(low < INT32_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_int32(decoder, value);
	if (err)
		return err;

	if (*value >= low)
		return 0;

	return -ERANGE;
}

int
dpack_decode_int32_max(struct dpack_decoder * __restrict decoder,
                       int32_t                           high,
                       int32_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(high > INT32_MIN);
	dpack_assert_api(high < INT32_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_int32(decoder, value);
	if (err)
		return err;

	if (*value <= high)
		return 0;

	return -ERANGE;
}

int
dpack_decode_int32_range(struct dpack_decoder * __restrict decoder,
                         int32_t                           low,
                         int32_t                           high,
                         int32_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low > INT32_MIN);
	dpack_assert_api(high < INT32_MAX);
	dpack_assert_api(low < high);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_int32(decoder, value);
	if (err)
		return err;

	if ((*value >= low) && (*value <= high))
		return 0;

	return -ERANGE;
}

/******************************************************************************
 * 64 bits integer
 ******************************************************************************/

int
dpack_encode_uint64(struct dpack_encoder * __restrict encoder,
                    uint64_t                          value)
{
	dpack_encoder_assert_api(encoder);

	if (value > UINT32_MAX) {
		int err;

		err = dpack_write_tag(encoder, DPACK_UINT64_TAG);
		if (err)
			return err;

		value = htobe64(value);

		return dpack_encoder_write(encoder,
		                           (const uint8_t *)&value,
		                           sizeof(value));
	}

	return dpack_encode_uint32(encoder, (uint32_t)value);
}

static __dpack_nonull(1, 3) __warn_result
int
dpack_load_uint64(struct dpack_decoder * __restrict decoder,
                  uint8_t                           tag,
                  uint64_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	int err;

	switch (tag) {
	case DPACK_UINT64_TAG:
		err = dpack_decoder_read(decoder,
		                         (uint8_t *)value,
		                         sizeof(*value));
		if (!err) {
			*value = be64toh(*value);
			return 0;
		}
		break;

	case DPACK_INT64_TAG:
		{
			uint64_t val;

			err = dpack_decoder_read(decoder,
			                         (uint8_t *)&val,
			                         sizeof(val));
			if (!err) {
				val = be64toh(val);
				if ((int64_t)val >= 0) {
					*value = val;
					return 0;
				}
				err = -ENOMSG;
			}
			break;
		}

	default:
		{
			uint32_t val;

			err = dpack_load_uint32(decoder, tag, &val);
			if (!err) {
				*value = (uint64_t)val;
				return 0;
			}
			break;
		}
	}

	return err;
}

int
dpack_decode_uint64(struct dpack_decoder * __restrict decoder,
                    uint64_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err)
		return dpack_load_uint64(decoder, tag, value);

	return err;
}

int
dpack_decode_uint64_min(struct dpack_decoder * __restrict decoder,
                        uint64_t                          low,
                        uint64_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low);
	dpack_assert_api(low < UINT64_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_uint64(decoder, value);
	if (err)
		return err;

	if (*value >= low)
		return 0;

	return -ERANGE;
}

int
dpack_decode_uint64_max(struct dpack_decoder * __restrict decoder,
                        uint64_t                          high,
                        uint64_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(high);
	dpack_assert_api(high < UINT64_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_uint64(decoder, value);
	if (err)
		return err;

	if (*value <= high)
		return 0;

	return -ERANGE;
}

int
dpack_decode_uint64_range(struct dpack_decoder * __restrict decoder,
                          uint64_t                          low,
                          uint64_t                          high,
                          uint64_t * __restrict             value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low);
	dpack_assert_api(high < UINT64_MAX);
	dpack_assert_api(low < high);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_uint64(decoder, value);
	if (err)
		return err;

	if ((*value >= low) && (*value <= high))
		return 0;

	return -ERANGE;
}

int
dpack_encode_int64(struct dpack_encoder * __restrict encoder,
                   int64_t                           value)
{
	dpack_encoder_assert_api(encoder);

	if ((value < INT32_MIN) || (value > INT32_MAX)) {
		int err;

		err = dpack_write_tag(encoder, DPACK_INT64_TAG);
		if (err)
			return err;

		value = (int64_t)htobe64((uint64_t)value);

		return dpack_encoder_write(encoder,
		                           (const uint8_t *)&value,
		                           sizeof(value));
	}

	return dpack_encode_int32(encoder, (int32_t)value);
}

static __dpack_nonull(1, 3) __warn_result
int
dpack_load_int64(struct dpack_decoder * __restrict decoder,
                 uint8_t                           tag,
                 int64_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	int err;

	switch (tag) {
	case DPACK_INT64_TAG:
		err = dpack_decoder_read(decoder,
		                         (uint8_t *)value,
		                         sizeof(*value));
		if (!err) {
			*value = (int64_t)be64toh(*(uint64_t *)value);
			return 0;
		}
		break;

	case DPACK_UINT64_TAG:
		{
			uint64_t val;

			err = dpack_decoder_read(decoder,
			                         (uint8_t *)&val,
			                         sizeof(val));
			if (!err) {
				val = be64toh(val);
				if ((int64_t)val >= 0) {
					*value = (int64_t)val;
					return 0;
				}
				err = -ENOMSG;
			}
			break;
		}

	default:
		{
			int32_t val;

			err = dpack_load_int32(decoder, tag, &val);
			if (!err) {
				*value = (int64_t)val;
				return 0;
			}
			break;
		}
	}

	return err;
}

int
dpack_decode_int64(struct dpack_decoder * __restrict decoder,
                   int64_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err)
		return dpack_load_int64(decoder, tag, value);

	return err;
}

int
dpack_decode_int64_min(struct dpack_decoder * __restrict decoder,
                       int64_t                           low,
                       int64_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low > INT64_MIN);
	dpack_assert_api(low < INT64_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_int64(decoder, value);
	if (err)
		return err;

	if (*value >= low)
		return 0;

	return -ERANGE;
}

int
dpack_decode_int64_max(struct dpack_decoder * __restrict decoder,
                       int64_t                           high,
                       int64_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(high > INT64_MIN);
	dpack_assert_api(high < INT64_MAX);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_int64(decoder, value);
	if (err)
		return err;

	if (*value <= high)
		return 0;

	return -ERANGE;
}

int
dpack_decode_int64_range(struct dpack_decoder * __restrict decoder,
                         int64_t                           low,
                         int64_t                           high,
                         int64_t * __restrict              value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(low > INT64_MIN);
	dpack_assert_api(high < INT64_MAX);
	dpack_assert_api(low < high);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_int64(decoder, value);
	if (err)
		return err;

	if ((*value >= low) && (*value <= high))
		return 0;

	return -ERANGE;
}

/******************************************************************************
 * Single precision floating point
 ******************************************************************************/

#if defined(CONFIG_DPACK_FLOAT)

int
dpack_encode_float(struct dpack_encoder * __restrict encoder, float value)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(!isnanf(value));

	int err;

	err = dpack_write_tag(encoder, DPACK_FLOAT32_TAG);
	if (!err) {
		union { float f; uint32_t u; } val = { .f = value };

		val.u = htobe32(val.u);

		return dpack_encoder_write(encoder,
		                           (const uint8_t *)&val.u,
		                           sizeof(val.u));
	}

	return err;
}

static __dpack_nonull(1, 3) __warn_result
int
dpack_load_float(struct dpack_decoder * __restrict decoder,
                 uint8_t                           tag,
                 float * __restrict                value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	if (tag == DPACK_FLOAT32_TAG) {
		union { uint32_t u; float f; } val;
		int                            err;

		err = dpack_decoder_read(decoder,
		                         (uint8_t *)&val.u,
		                         sizeof(val.u));
		if (!err) {
			val.u = be32toh(val.u);
			if (isnanf(val.f))
				return -EBADMSG;
			*value = val.f;
			return 0;
		}

		return err;
	}

	return -ENOMSG;
}

int
dpack_decode_float(struct dpack_decoder * __restrict decoder,
                   float * __restrict                value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);

	return (!err) ? dpack_load_float(decoder, tag, value) : err;
}

int
dpack_decode_float_min(struct dpack_decoder * __restrict decoder,
                       float                             low,
                       float * __restrict                value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(isfinite(low));
	dpack_assert_api(value);

	int err;

	err = dpack_decode_float(decoder, value);
	if (!err)
		return (*value >= low) ? 0 : -ERANGE;

	return err;
}

int
dpack_decode_float_max(struct dpack_decoder * __restrict decoder,
                       float                             high,
                       float * __restrict                value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(isfinite(high));
	dpack_assert_api(value);

	int err;

	err = dpack_decode_float(decoder, value);
	if (!err)
		return (*value <= high) ? 0 : -ERANGE;

	return err;
}

int
dpack_decode_float_range(struct dpack_decoder * __restrict decoder,
                         float                             low,
                         float                             high,
                         float * __restrict                value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(isfinite(low));
	dpack_assert_api(isfinite(high));
	dpack_assert_api(low < high);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_float(decoder, value);
	if (!err)
		return ((*value >= low) && (*value <= high)) ? 0 : -ERANGE;

	return err;
}

#endif /* defined(CONFIG_DPACK_FLOAT) */

/******************************************************************************
 * Double precision floating point
 ******************************************************************************/

#if defined(CONFIG_DPACK_DOUBLE)

int
dpack_encode_double(struct dpack_encoder * __restrict encoder, double value)
{
	dpack_encoder_assert_api(encoder);
	dpack_assert_api(!isnan(value));

	int err;

	err = dpack_write_tag(encoder, DPACK_FLOAT64_TAG);
	if (!err) {
		union { double d; uint64_t u; } val = { .d = value };

		val.u = htobe64(val.u);

		return dpack_encoder_write(encoder,
		                           (const uint8_t *)&val.u,
		                           sizeof(val.u));
	}

	return err;
}

static __dpack_nonull(1, 3) __warn_result
int
dpack_load_double(struct dpack_decoder * __restrict decoder,
                  uint8_t                           tag,
                  double * __restrict               value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	int err;

	switch (tag) {
	case DPACK_FLOAT64_TAG:
		{
			union { uint64_t u; double d; } val;

			err = dpack_decoder_read(decoder,
			                         (uint8_t *)&val.u,
			                         sizeof(val.u));
			if (!err) {
				val.u = be64toh(val.u);
				if (isnan(val.d))
					return -EBADMSG;
				*value = val.d;
				return 0;
			}
			break;
		}

	default:
		{
			float val;

			err = dpack_load_float(decoder, tag, &val);
			if (!err) {
				*value = (double)val;
				return 0;
			}
		}
	}

	return err;
}

int
dpack_decode_double(struct dpack_decoder * __restrict decoder,
                    double * __restrict               value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(value);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);

	return (!err) ? dpack_load_double(decoder, tag, value) : err;
}

int
dpack_decode_double_min(struct dpack_decoder * __restrict decoder,
                        double                            low,
                        double * __restrict               value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(isfinite(low));
	dpack_assert_api(value);


	int err;

	err = dpack_decode_double(decoder, value);
	if (!err)
		return (*value >= low) ? 0 : -ERANGE;

	return err;
}

int
dpack_decode_double_max(struct dpack_decoder * __restrict decoder,
                        double                            high,
                        double * __restrict               value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(isfinite(high));
	dpack_assert_api(value);

	int err;

	err = dpack_decode_double(decoder, value);
	if (!err)
		return (*value <= high) ? 0 : -ERANGE;

	return err;
}

int
dpack_decode_double_range(struct dpack_decoder * __restrict decoder,
                          double                            low,
                          double                            high,
                          double * __restrict               value)
{
	dpack_decoder_assert_api(decoder);
	dpack_assert_api(isfinite(low));
	dpack_assert_api(isfinite(high));
	dpack_assert_api(low < high);
	dpack_assert_api(value);

	int err;

	err = dpack_decode_double(decoder, value);
	if (!err)
		return ((*value >= low) && (*value <= high)) ? 0 : -ERANGE;

	return err;
}

#endif /* defined(CONFIG_DPACK_DOUBLE) */

/******************************************************************************
 * Nil / NULL
 ******************************************************************************/

int
dpack_encode_nil(struct dpack_encoder * __restrict encoder)
{
	dpack_encoder_assert_api(encoder);

	return dpack_write_tag(encoder, DPACK_NIL_TAG);
}

int
dpack_decode_nil(struct dpack_decoder * decoder)
{
	dpack_decoder_assert_api(decoder);

	uint8_t tag;
	int     err;

	err = dpack_read_tag(decoder, &tag);
	if (!err) {
		switch (tag) {
		case DPACK_NIL_TAG:
			return 0;

		default:
			break;
		}

		return -ENOMSG;
	}

	return err;
}
