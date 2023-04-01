/******************************************************************************
 * Setup mpack build configuration. For available features, see:
 * <mpack>/src/mpack/mpack-platform.h
 *
 * Documentation is available online:
 * https://ludocode.github.io/mpack/group__config.html
 ******************************************************************************/

#ifndef _DPACK_MPACK_CONFIG_H
#define _DPACK_MPACK_CONFIG_H

#include <dpack/config.h>

/* Enable compile time configuration */
#ifndef MPACK_HAS_CONFIG
#define MPACK_HAS_CONFIG 1
#endif /* MPACK_HAS_CONFIG */

/* Enable compiler intrinsics and other built-in functions. */
/* #define MPACK_NO_BUILTINS 0 */

/* Enable compilation of the base Tag Reader. */
/* #define MPACK_READER 1 */

/* Disable compilation of the Expect API. */
#define MPACK_EXPECT 0

/* Disable compilation of the dynamic Node API. */
#define MPACK_NODE 0

/* Enables compilation of the Writer. */
/* #define MPACK_WRITER 1 */

/*
 * Enables compilation of the Writer builder API providing additional functions
 * to the Writer for automatically determining the element count of compound
 * elements. Functions are:
 *     mpack_build_map()
 *     mpack_build_array()
 *     mpack_complete_map()
 *     mpack_complete_array()
 */
/* #define MPACK_BUILDER 1 */

/* Disable compatibility with older versions of mpack. */
/* #define MPACK_COMPATIBILITY 0 */

/* Disable the use of extension types. */
/* #define MPACK_EXTENSIONS 0 */

/* Enables the inclusion of C headers defining standard types and macros. */
/* #define MPACK_CONFORMING 1 */

/* Enables the use of the C stdlib. */
/* #define MPACK_STDLIB 1 */

/*
 * Disable the use of the C stdio by default. Enable this if debugging so that
 * mpack may provide helpers to ease debugging and reading/writing to/from
 * files.
 */
#if defined(CONFIG_DPACK_DEBUG)
#define MPACK_STDIO 1
#else
#define MPACK_STDIO 0
#endif

/* Enables float support. */
/* #define MPACK_FLOAT 1 */

/* Enables double support. */
/* #define MPACK_DOUBLE 1 */

/* Enable mpack debugging if dpack debugging support is enabled. */
#if defined(CONFIG_DPACK_DEBUG)
#define MPACK_DEBUG 1
#endif

/* Enable mpack error strings when debugging support is enabled. */
#if defined(CONFIG_DPACK_DEBUG)
#define MPACK_STRINGS 1
#else
#define MPACK_STRINGS 0
#endif

/*
 * Enable custom mpack assertions so that dpack provides a mpack_assert_fail()
 * function when debugging is enabled.
 */
#if defined(CONFIG_DPACK_DEBUG)
#define MPACK_CUSTOM_ASSERT 1
#else
#define MPACK_CUSTOM_ASSERT 0
#endif

/* Optimize for speed. */
#define MPACK_OPTIMIZE_FOR_SIZE 0

/* 
 * Enable a small amount of internal storage within the writer to avoid some
 * allocations when using builders.
 *
 * Setup MPACK_BUILDER_INTERNAL_STORAGE_SIZE to configure its size.
 *
 * Warning: writers are typically placed on the stack so make sure you have
 * sufficient stack space.
 *
 * Keep this disabled by default. Uncomment to enable.
 */
/* #define MPACK_BUILDER_INTERNAL_STORAGE      1 */
/* #define MPACK_BUILDER_INTERNAL_STORAGE_SIZE 256 */

#endif /* _DPACK_MPACK_CONFIG_H */
