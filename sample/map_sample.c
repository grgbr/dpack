#include "map_sample.h"
#include <dpack/codec.h>

int
map_sample_check_ashort(int16_t value)
{
	if ((value < MAP_SAMPLE_ASHORT_MIN) || (value > MAP_SAMPLE_ASHORT_MAX))
		return -EINVAL;

	return 0;
}

static int
map_sample_unpack_ashort(struct dpack_decoder * decoder,
                         struct map_sample    * sample)
{
	map_sample_assert(decoder);
	map_sample_assert(sample);

	int err;

	if (stroll_bmap32_test(sample->filled, MAP_SAMPLE_ASHORT_FLD))
		return -EEXIST;

	err = dpack_decode_int16(decoder, &sample->ashort);
	if (err)
		return err;

	err = map_sample_check_ashort(sample->ashort);
	if (err)
		return err;

	stroll_bmap32_set(&sample->filled, MAP_SAMPLE_ASHORT_FLD);

	return 0;
}

int
map_sample_check_astring(const char * value __unused, size_t len)
{
	/* dpack does not unpack empty strings. */
	map_sample_assert(value);
	map_sample_assert(value[0] != '\0');
	map_sample_assert(len);
	map_sample_assert(value[len] == '\0');

	if ((len < MAP_SAMPLE_ASTRING_LEN_MIN) ||
	    (len > MAP_SAMPLE_ASTRING_LEN_MAX))
		return -EINVAL;

	/*
	 * Some more checks may go here. Character class checking or regex
	 * pattern matching for example...
	 */

	return 0;
}

static int
map_sample_check_intern_astring(const struct stroll_lvstr * value)
{
	return map_sample_check_astring(stroll_lvstr_cstr(value),
	                                stroll_lvstr_len(value));
}

void
map_sample_lend_astring(struct map_sample * sample, const char * value)
{
	map_sample_assert_access(sample);
	map_sample_assert(
		!map_sample_check_astring(
			value,
			strnlen(value, MAP_SAMPLE_ASTRING_LEN_MAX + 1)));

	stroll_lvstr_lend(&sample->astring, value);
	stroll_bmap32_set(&sample->filled, MAP_SAMPLE_ASTRING_FLD);
}

void
map_sample_cede_astring(struct map_sample * sample, char * value)
{
	map_sample_assert_access(sample);
	map_sample_assert(
		!map_sample_check_astring(
			value,
			strnlen(value, MAP_SAMPLE_ASTRING_LEN_MAX + 1)));

	stroll_lvstr_cede(&sample->astring, value);
	stroll_bmap32_set(&sample->filled, MAP_SAMPLE_ASTRING_FLD);
}

int
map_sample_get_astring(const struct map_sample * sample, const char ** value)
{
	map_sample_assert_access(sample);
	map_sample_assert(value);
	map_sample_assert(!map_sample_check_intern_astring(&sample->astring));

	/* This field is optional with a default value. */
	if (!stroll_bmap32_test(sample->filled, MAP_SAMPLE_ASTRING_FLD)) {
		extern const char * const map_sample_dflt_astring;
		*value = map_sample_dflt_astring;
	}
	else {
		*value = stroll_lvstr_cstr(&sample->astring);
		map_sample_assert(*value);
	}

	return 0;
}

static int
map_sample_unpack_astring(struct dpack_decoder * decoder,
                          struct map_sample    * sample)
{
	map_sample_assert(decoder);
	map_sample_assert(sample);

	ssize_t ret;

	if (stroll_bmap32_test(sample->filled, MAP_SAMPLE_ASTRING_FLD)) {
		map_sample_assert(stroll_lvstr_cstr(&sample->astring));
		return -EEXIST;
	}

	ret = dpack_decode_lvstr(decoder, &sample->astring);
	if (ret < 0)
		return (int)ret;

	ret = map_sample_check_intern_astring(&sample->astring);
	if (ret)
		goto free;

	stroll_bmap32_set(&sample->filled, MAP_SAMPLE_ASTRING_FLD);

	return 0;

free:
	stroll_lvstr_drop(&sample->astring);

	/*
	 * No need to clear astring field since the astring field bitmap bit has
	 * not been set.
	 * map_sample_fini() should just check for it before freeing astring
	 * though.
	 */

	return (int)ret;
}

static int
map_sample_unpack_abool(struct dpack_decoder * decoder,
                        struct map_sample    * sample)
{
	map_sample_assert(decoder);
	map_sample_assert(sample);

	int err;

	if (stroll_bmap32_test(sample->filled, MAP_SAMPLE_ABOOL_FLD))
		return -EEXIST;

	err = dpack_decode_bool(decoder, &sample->abool);
	if (err)
		return err;

	/* No need for boolean checker... */

	stroll_bmap32_set(&sample->filled, MAP_SAMPLE_ABOOL_FLD);

	return 0;
}

static int
map_sample_unpack_anuint(struct dpack_decoder * decoder,
                         struct map_sample    * sample)
{
	map_sample_assert(decoder);
	map_sample_assert(sample);

	int err;

	if (stroll_bmap32_test(sample->filled, MAP_SAMPLE_ANUINT_FLD))
		return -EEXIST;

	err = dpack_decode_uint32(decoder, &sample->anuint);
	if (err)
		return err;

	err = map_sample_check_anuint(sample->anuint);
	if (err)
		return err;

	stroll_bmap32_set(&sample->filled, MAP_SAMPLE_ANUINT_FLD);

	return 0;
}

int
map_sample_check(const struct map_sample * sample)
{
	/* Assert that sample has no invalid filled fields. */
	map_sample_assert(sample);
	map_sample_assert(!stroll_bmap32_test_mask(sample->filled,
	                                           ~MAP_SAMPLE_VALID_FLD_MSK));

	/* Now ensure all mandatory fields are set. */
	if (stroll_bmap32_and(sample->filled, MAP_SAMPLE_MAND_FLD_MSK) !=
	    MAP_SAMPLE_MAND_FLD_MSK)
		return -EPERM;

	/*
	 * If we are really paranoid, perform assertion checking for all
	 * mandatory fields. Despite abool field being mandatory, there is no
	 * validation logic for boolean values...
	 */
	map_sample_assert(!map_sample_check_ashort(sample->ashort));

	/* Now assert optional fields. Start by checking astring field: */
	if (stroll_bmap32_test(sample->filled, MAP_SAMPLE_ASTRING_FLD)) {
		map_sample_assert(
			!map_sample_check_intern_astring(&sample->astring));
	}

	/* ... and finish by asserting auint field. */
	if (stroll_bmap32_test(sample->filled, MAP_SAMPLE_ANUINT_FLD)) {
		map_sample_assert(!map_sample_check_anuint(sample->anuint));
	}

	/*
	 * If required, more complex inter-field checking may happen here...
	 * Let's say we want ashort field to be > 50 if the optional astring
	 * field is not set:
	 */
	if (!stroll_bmap32_test(sample->filled, MAP_SAMPLE_ASTRING_FLD) &&
	    sample->ashort <= 50)
		return -EINVAL;

	/* Right ! */
	return 0;
}

int
map_sample_pack(struct dpack_encoder    * encoder,
                const struct map_sample * sample)
{
	/* Caller MUST call us with large enough encoding buffer space. */
	map_sample_assert(encoder);
	map_sample_assert(dpack_encoder_space_left(encoder) >=
	                  MAP_SAMPLE_PACKED_SIZE_MIN);

	/* Assert that sample is consistent. */
	map_sample_assert(!map_sample_check(sample));

	int err;

	dpack_map_begin_encode(encoder, stroll_bmap32_hweight(sample->filled));

	/* ashort field is mandatory. */
	err = dpack_map_encode_int16(encoder,
	                             MAP_SAMPLE_ASHORT_FLD,
	                             sample->ashort);
	if (err)
		return err;

	/* astring field is optional. */
	if (stroll_bmap32_test(sample->filled, MAP_SAMPLE_ASTRING_FLD)) {
		err = dpack_map_encode_lvstr(encoder,
		                             MAP_SAMPLE_ASTRING_FLD,
		                             &sample->astring);
		if (err)
			return err;
	}

	/* abool field is mandatory. */
	err = dpack_map_encode_bool(encoder,
	                            MAP_SAMPLE_ABOOL_FLD,
	                            sample->abool);
	if (err)
		return err;

	/* anuint field is optional. */
	if (stroll_bmap32_test(sample->filled, MAP_SAMPLE_ANUINT_FLD)) {
		err = dpack_map_encode_uint32(encoder,
		                              MAP_SAMPLE_ANUINT_FLD,
		                              sample->anuint);
		if (err)
			return err;
	}

	dpack_map_end_encode(encoder);

	return 0;
}

static int
map_sample_unpack_field(struct dpack_decoder * decoder,
                        unsigned int           fid,
                        void                 * data)
{
	struct map_sample * spl = data;

	map_sample_assert(decoder);
	map_sample_assert(spl);
	map_sample_assert(!stroll_bmap32_test_mask(spl->filled,
	                                           ~MAP_SAMPLE_VALID_FLD_MSK));

	switch (fid) {
	case MAP_SAMPLE_ASHORT_FLD:
		return map_sample_unpack_ashort(decoder, spl);
	case MAP_SAMPLE_ASTRING_FLD:
		return map_sample_unpack_astring(decoder, spl);
	case MAP_SAMPLE_ABOOL_FLD:
		return map_sample_unpack_abool(decoder, spl);
	case MAP_SAMPLE_ANUINT_FLD:
		return map_sample_unpack_anuint(decoder, spl);
	default:
		return -EBADMSG;
	}
}

int
map_sample_unpack(struct dpack_decoder * decoder, struct map_sample * sample)
{
	/* Caller MUST call us with large enough decoding buffer space. */
	map_sample_assert(decoder);
	map_sample_assert(dpack_decoder_data_left(decoder) >=
	                  MAP_SAMPLE_PACKED_SIZE_MIN);

	/* Assert that sample is a freshly initialized object. */
	map_sample_assert(sample);
	map_sample_assert(!stroll_bmap32_test_all(sample->filled));

	int err;

	err = dpack_map_decode_range(decoder,
	                             MAP_SAMPLE_MAND_FLD_NR,
	                             MAP_SAMPLE_FLD_NR,
	                             map_sample_unpack_field,
	                             sample);
	if (err)
		return err;

	return map_sample_check(sample);
}

const char * const map_sample_dflt_astring = "default astring field string";

void
map_sample_fini(struct map_sample * sample)
{
	map_sample_assert(sample);

	stroll_lvstr_fini(&sample->astring);
}

struct map_sample *
map_sample_create(void)
{
	struct map_sample * spl;

	spl = map_sample_alloc();
	if (!spl)
		return NULL;

	map_sample_init(spl);

	return spl;
}

void
map_sample_destroy(struct map_sample * sample)
{
	map_sample_assert(sample);

	map_sample_fini(sample);
	map_sample_free(sample);
}
