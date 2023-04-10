#ifndef _MAP_SAMPLE_H
#define _MAP_SAMPLE_H

#include <dpack/map.h>
#include <stroll/bmap.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define map_sample_assert(_cond) \
	stroll_assert("map sample", _cond)

enum map_sample_field {
	MAP_SAMPLE_ASHORT_FLD  = 0,
	MAP_SAMPLE_ASTRING_FLD = 1,
	MAP_SAMPLE_ABOOL_FLD   = 2,
	MAP_SAMPLE_ANUINT_FLD  = 3,
	MAP_SAMPLE_FLD_NR
};

struct map_sample {
	uint32_t            filled;
	int16_t             ashort;
	struct stroll_lvstr astring;
	bool                abool;
	uint32_t            anuint;
};

#define MAP_SAMPLE_MAND_FLD_NR     (2U)
#define MAP_SAMPLE_VALID_FLD_MSK   ((UINT32_C(1) << MAP_SAMPLE_FLD_NR) - 1)
#define MAP_SAMPLE_MAND_FLD_MSK    (MAP_SAMPLE_ASHORT_FLD | \
                                    MAP_SAMPLE_ABOOL_FLD)

#define MAP_SAMPLE_ASHORT_MIN      (INT16_MIN + 2)
#define MAP_SAMPLE_ASHORT_MAX      (INT16_MAX - 2)

#define MAP_SAMPLE_ASTRING_LEN_MIN (4U)
#define MAP_SAMPLE_ASTRING_LEN_MAX (63U)

#define MAP_SAMPLE_INIT \
	{ .filled = STROLL_BMAP32_INIT_CLEAR }

#define MAP_SAMPLE_DECL(_name) \
	struct map_sample _name = MAP_SAMPLE_INIT

#define MAP_SAMPLE_PACKED_SIZE_MIN \
	(DPACK_MAP_HEAD_SIZE(MAP_SAMPLE_MAND_FLD_NR) + \
	 DPACK_MAP_UINT16_SIZE_MIN + \
	 DPACK_MAP_BOOL_SIZE_MIN)

#define MAP_SAMPLE_PACKED_SIZE_MAX \
	(DPACK_MAP_HEAD_SIZE(MAP_SAMPLE_FLD_NR) + \
	 DPACK_MAP_UINT16_SIZE_MAX + \
	 DPACK_MAP_LVSTR_SIZE_MIN(MAP_SAMPLE_ASTRING_LEN_MAX) + \
	 DPACK_MAP_BOOL_SIZE_MAX + \
	 DPACK_MAP_UINT32_SIZE_MAX)

#define map_sample_assert_access(_sample) \
	map_sample_assert(_sample); \
	map_sample_assert(!stroll_bmap32_test_mask((_sample)->filled, \
	                                           ~MAP_SAMPLE_VALID_FLD_MSK))

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
	stroll_bmap32_set(&sample->filled, MAP_SAMPLE_ASHORT_FLD);
}

static inline int
map_sample_get_ashort(const struct map_sample * sample, int16_t * value)
{
	map_sample_assert_access(sample);
	map_sample_assert(value);

	/*
	 * Despite being a mandatory field, we still want the caller to be able
	 * to retrieve the current value. Tell him by returning -EPERM.
	 */
	if (!stroll_bmap32_test(sample->filled, MAP_SAMPLE_ASHORT_FLD))
		return -EPERM;

	map_sample_assert(!map_sample_check_ashort(sample->ashort));

	*value = sample->ashort;

	return 0;
}

extern void
map_sample_lend_astring(struct map_sample * sample, const char * value);

extern void
map_sample_cede_astring(struct map_sample * sample, char * value);

extern int
map_sample_get_astring(const struct map_sample * sample, const char ** value);

static inline void
map_sample_set_abool(struct map_sample * sample, bool value)
{
	map_sample_assert_access(sample);

	sample->abool = value;
	stroll_bmap32_set(&sample->filled, MAP_SAMPLE_ABOOL_FLD);
}

static inline int
map_sample_get_abool(const struct map_sample * sample, bool * value)
{
	map_sample_assert_access(sample);
	map_sample_assert(value);

	/*
	 * Despite being a mandatory field, we still want the caller to be able
	 * to retrieve the current value. Tell him by returning -EPERM.
	 */
	if (!stroll_bmap32_test(sample->filled, MAP_SAMPLE_ABOOL_FLD))
		return -EPERM;

	*value = sample->abool;

	return 0;
}

static inline void
map_sample_set_anuint(struct map_sample * sample, uint32_t value)
{
	map_sample_assert_access(sample);
	map_sample_assert(!map_sample_check_anuint(value));

	sample->anuint = value;
	stroll_bmap32_set(&sample->filled, MAP_SAMPLE_ANUINT_FLD);
}

static inline int
map_sample_get_anuint(const struct map_sample * sample, unsigned int * value)
{
	map_sample_assert_access(sample);
	map_sample_assert(value);

	if (!stroll_bmap32_test(sample->filled, MAP_SAMPLE_ANUINT_FLD))
		return -EPERM;

	map_sample_assert(!map_sample_check_anuint(sample->anuint));

	*value = sample->anuint;

	return 0;
}

static inline void
map_sample_init(struct map_sample * sample)
{
	map_sample_assert(sample);

	stroll_bmap32_setup_clear(&sample->filled);
	stroll_lvstr_init(&sample->astring);
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
