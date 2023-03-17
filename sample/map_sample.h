#ifndef _MAP_SAMPLE_H
#define _MAP_SAMPLE_H

#include <dpack/map.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define map_sample_assert(condition) \
	assert(condition)

enum map_sample_field {
	MAP_SAMPLE_ASHORT_FLD  = 0,
	MAP_SAMPLE_ASTRING_FLD = 1,
	MAP_SAMPLE_ABOOL_FLD   = 2,
	MAP_SAMPLE_ANUINT_FLD  = 3,
	MAP_SAMPLE_FLD_NR
};

struct map_sample {
	unsigned int   filled;
	int16_t        ashort;
	char         * astring;
	bool           abool;
	uint32_t       anuint;
};

#define MAP_SAMPLE_MAND_FLD_NR     (2U)
#define MAP_SAMPLE_VALID_FLD_MSK   ((1U << MAP_SAMPLE_FLD_NR) - 1)
#define MAP_SAMPLE_MAND_FLD_MSK    (MAP_SAMPLE_ASHORT_FLD | \
                                    MAP_SAMPLE_ABOOL_FLD)

#define MAP_SAMPLE_ASHORT_MIN      (-10)
#define MAP_SAMPLE_ASHORT_MAX      (200)

#define MAP_SAMPLE_ASTRING_LEN_MIN (4U)
#define MAP_SAMPLE_ASTRING_LEN_MAX (63U)

extern const struct map_sample map_sample_dflt;

#define MAP_SAMPLE_INIT \
	map_sample_dflt

#define MAP_SAMPLE_DECL(_name) \
	struct map_sample _name = MAP_SAMPLE_INIT

#define MAP_SAMPLE_PACKED_SIZE_MIN \
	(DPACK_MAP_HEAD_SIZE(MAP_SAMPLE_MAND_FLD_NR) + \
	 DPACK_MAP_UINT16_SIZE_MIN + \
	 DPACK_MAP_BOOL_SIZE_MIN)

#define MAP_SAMPLE_PACKED_SIZE_MAX \
	(DPACK_MAP_HEAD_SIZE(MAP_SAMPLE_MAND_FLD_NR) + \
	 DPACK_MAP_UINT16_SIZE_MAX + \
	 DPACK_MAP_STR_SIZE_MIN(MAP_SAMPLE_ASTRING_LEN_MAX) + \
	 DPACK_MAP_BOOL_SIZE_MAX + \
	 DPACK_MAP_UINT32_SIZE_MAX)

#define map_sample_assert_access(_sample) \
	map_sample_assert(_sample); \
	map_sample_assert(!((_sample)->filled & ~MAP_SAMPLE_VALID_FLD_MSK))

static inline int
map_sample_check_anuint(uint32_t value __unused)
{
	/* No particular invalid values... */
	return 0;
}

extern int
map_sample_check_ashort(int16_t value);

extern int
map_sample_check_astring(const char * value, size_t len);

extern int
map_sample_check(const struct map_sample * sample);

extern int
map_sample_pack(struct dpack_encoder    * encoder,
                const struct map_sample * sample);

extern int
map_sample_unpack(struct dpack_decoder * decoder, struct map_sample * sample);

static inline void
map_sample_set_ashort(struct map_sample * sample, int16_t value)
{
	map_sample_assert_access(sample);
	map_sample_assert(!map_sample_check_ashort(value));

	sample->ashort = value;
	sample->filled |= (1U << MAP_SAMPLE_ASHORT_FLD);
}

static inline int
map_sample_get_ashort(const struct map_sample * sample, int16_t * value)
{
	map_sample_assert_access(sample);
	map_sample_assert(value);

	/*
	 * Despite being a mandatory field, we still want the caller to be able
	 * to retrieve the current value. Tell him by returning -ENOENT.
	 */
	if (!(sample->filled & (1U << MAP_SAMPLE_ASHORT_FLD)))
		return -ENOENT;

	map_sample_assert(!map_sample_check_ashort(sample->ashort));

	*value = sample->ashort;

	return 0;
}

static inline void
map_sample_set_astring(struct map_sample * sample, char * value)
{
	map_sample_assert_access(sample);
	map_sample_assert(
		!map_sample_check_astring(
			value,
			strnlen(value, MAP_SAMPLE_ASTRING_LEN_MAX + 1)));

	sample->astring = value;
	sample->filled |= (1U << MAP_SAMPLE_ASTRING_FLD);
}

static inline int
map_sample_get_astring(const struct map_sample * sample, const char ** value)
{
	map_sample_assert_access(sample);
	map_sample_assert(value);

	map_sample_assert(
		!map_sample_check_astring(
			sample->astring,
			strnlen(sample->astring,
			        MAP_SAMPLE_ASTRING_LEN_MAX + 1)));

	/* This field is optional with a default value. */
	if (!(sample->filled & (1U << MAP_SAMPLE_ASTRING_FLD)))
		*value = sample->astring;
	else
		*value = map_sample_dflt.astring;

	return 0;
}

static inline void
map_sample_set_abool(struct map_sample * sample, bool value)
{
	map_sample_assert_access(sample);

	sample->abool = value;
	sample->filled |= (1U << MAP_SAMPLE_ABOOL_FLD);
}

static inline int
map_sample_get_abool(const struct map_sample * sample, bool * value)
{
	map_sample_assert_access(sample);
	map_sample_assert(value);

	/*
	 * Despite being a mandatory field, we still want the caller to be able
	 * to retrieve the current value. Tell him by returning -ENOENT.
	 */
	if (!(sample->filled & (1U << MAP_SAMPLE_ABOOL_FLD)))
		return -ENOENT;

	*value = sample->abool;

	return 0;
}

static inline void
map_sample_set_anuint(struct map_sample * sample, uint32_t value)
{
	map_sample_assert_access(sample);
	map_sample_assert(!map_sample_check_anuint(value));

	sample->anuint = value;
	sample->filled |= (1U << MAP_SAMPLE_ANUINT_FLD);
}

static inline int
map_sample_get_anuint(const struct map_sample * sample, unsigned int * value)
{
	map_sample_assert_access(sample);
	map_sample_assert(value);

	if (!(sample->filled & (1U << MAP_SAMPLE_ANUINT_FLD)))
		return -ENOENT;

	map_sample_assert(!map_sample_check_anuint(sample->anuint));

	*value = sample->anuint;

	return 0;
}

static inline void
map_sample_init(struct map_sample * sample)
{
	map_sample_assert(sample);

	*sample = map_sample_dflt;
}

extern void
map_sample_fini(struct map_sample * sample);

static inline struct map_sample *
map_sample_alloc(void)
{
	return malloc(sizeof(struct map_sample));
}

static inline void
map_sample_free(struct map_sample * sample)
{
	map_sample_assert(sample);

	free(sample);
}

extern struct map_sample *
map_sample_create(void);

extern void
map_sample_destroy(struct map_sample * sample);

#endif /* _MAP_SAMPLE_H */
