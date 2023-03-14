#ifndef _DPACK_CDEFS_H
#define _DPACK_CDEFS_H

#include <assert.h>

#define _compile_eval(_expr, _msg) \
	(!!sizeof(struct { static_assert(_expr, _msg); int _dummy; }))

/*
 * Generate a compile error with error message _msg when constant expression
 * _expr is zero / false. "Returns" value of _stmt evaluation otherwise.
 * Borrowed from Gnulib's verify.h header file.
 */
#define compile_eval(_expr, _stmt, _msg) \
	(_compile_eval(_expr, "compile_eval(" # _expr "): " _msg) ? (_stmt) : \
	                                                            (_stmt))

/* Assert that constant expression _expr is nonzero / true at compile time. */
#define compile_assert(_expr) \
	static_assert(_expr, # _expr)

/*
 * Return true if unqualified versions of the types of _a and _b are the same.
 */
#define _is_same_type(_a, _b) \
	__builtin_types_compatible_p(typeof(_a), typeof(_b))

/*
 * Returns true / nonzero when _array is an array wheras typeof(&(_array)[0])
 * degrades to a pointer, i.e., meaning that _array is an "array type".
 * Borrowed from Linux kernel header kernel.h
 */
#define _is_array(_array) \
	(!_is_same_type(_array, &(_array)[0]))

#define array_nr(_array) \
	compile_eval(_is_array(_array), \
	             sizeof(_array) / sizeof((_array)[0]), \
	             "array expected")

#define __unused \
	__attribute__((unused))

#define __dpack_export \
	__attribute__((visibility("default")))

#define dpack_assert(cond) \
	assert(cond)

#define dpack_array_nr(_array) \
	array_nr(_array)

#define dpack_sizeof_member(_type, _member) \
	(sizeof(((_type *)0)->_member))

#endif /* _DPACK_CDEFS_H */
