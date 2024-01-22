/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/lvstr.h"
#include "dpack/codec.h"
#include "common.h"

size_t
dpack_lvstr_size(size_t len)
{
	dpack_assert_api(len);
	dpack_assert_api(len <= DPACK_LVSTRLEN_MAX);

	switch (len) {
	case 1 ... DPACK_FIXSTR_LEN_MAX:
		return DPACK_FIXSTR_SIZE(len);
#if DPACK_LVSTRLEN_MAX > DPACK_FIXSTR_LEN_MAX
	case (DPACK_FIXSTR_LEN_MAX + 1) ... DPACK_STR8_LEN_MAX:
		return DPACK_STR8_SIZE(len);
#endif
#if DPACK_LVSTRLEN_MAX > DPACK_STR8_LEN_MAX
	case (DPACK_STR8_LEN_MAX + 1) ... DPACK_STR16_LEN_MAX:
		return DPACK_STR16_SIZE(len);
#endif
#if DPACK_LVSTRLEN_MAX > DPACK_STR16_LEN_MAX
	case (DPACK_STR16_LEN_MAX + 1) ... DPACK_LVSTRLEN_MAX:
		return DPACK_STR32_SIZE(len);
#endif
	default:
		dpack_assert_api(0);
	}

	unreachable();
}

int
dpack_encode_lvstr(struct dpack_encoder *      encoder,
                   const struct stroll_lvstr * value)
{
	dpack_assert_api_encoder(encoder);
	dpack_assert_api(value);
	dpack_assert_api(stroll_lvstr_cstr(value));
	dpack_assert_api(stroll_lvstr_len(value));
	dpack_assert_api(strnlen(stroll_lvstr_cstr(value),
	                         DPACK_LVSTRLEN_MAX + 1) ==
	                 stroll_lvstr_len(value));

	return dpack_encode_str_fix(encoder,
	                            stroll_lvstr_cstr(value),
	                            stroll_lvstr_len(value));
}

ssize_t
dpack_decode_lvstr_range(struct dpack_decoder * decoder,
                         size_t                 min_len,
                         size_t                 max_len,
                         struct stroll_lvstr *  value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(min_len);
	dpack_assert_api(min_len < max_len);
	dpack_assert_api(max_len <= DPACK_LVSTRLEN_MAX);
	dpack_assert_api(value);

	ssize_t len;
	char *  cstr;

	len = dpack_decode_strdup_range(decoder, min_len, max_len, &cstr);
	if (len < 0)
		return (int)len;

	dpack_assert_intern(cstr);
	dpack_assert_intern(cstr[0]);
	dpack_assert_intern((size_t)len >= min_len);
	dpack_assert_intern((size_t)len <= max_len);

	stroll_lvstr_ncede(value, cstr, (size_t)len);
	dpack_assert_intern((size_t)len == stroll_lvstr_len(value));

	return len;
}

ssize_t
dpack_decode_lvstr_fix(struct dpack_decoder * decoder,
                       size_t                 len,
                       struct stroll_lvstr *  value)
{
	dpack_assert_api_decoder(decoder);
	dpack_assert_api(len);
	dpack_assert_api(len <= DPACK_LVSTRLEN_MAX);
	dpack_assert_api(value);

	ssize_t ret;
	char *  cstr;

	ret = dpack_decode_strdup_equ(decoder, len, &cstr);
	if (ret < 0)
		return (int)ret;

	dpack_assert_intern(cstr);
	dpack_assert_intern(cstr[0]);
	dpack_assert_intern((size_t)ret == len);

	stroll_lvstr_ncede(value, cstr, len);

	return ret;
}
