/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of DPack.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "dpack/codec.h"
#include "common.h"
#include <stroll/page.h>
#include <utils/file.h>
#include <sys/mman.h>

#define dpack_decoder_assert_file_api(_decoder) \
	dpack_assert_api(_decoder); \
	dpack_decoder_assert_api(&(_decoder)->base); \
	dpack_assert_api((_decoder)->fsize > 0); \
	dpack_assert_api((uint64_t)(_decoder)->fsize <= (uint64_t)SIZE_MAX); \
	dpack_assert_api((_decoder)->foff >= 0); \
	dpack_assert_api((_decoder)->foff <= (_decoder)->fsize); \
	dpack_assert_api((_decoder)->moff >= 0); \
	dpack_assert_api((_decoder)->moff <= (_decoder)->foff); \
	dpack_assert_api((uint64_t)DPACK_DECODER_FILE_MSIZE_MAX < \
	                 (uint64_t)OFF_MAX); \
	dpack_assert_api((uint64_t)DPACK_DECODER_FILE_MSIZE_MAX < \
	                 (uint64_t)SIZE_MAX); \
	dpack_assert_api((_decoder)->msize); \
	dpack_assert_api((_decoder)->msize <= \
	                 (size_t)DPACK_DECODER_FILE_MSIZE_MAX); \
	dpack_assert_api(stroll_aligned((_decoder)->msize, \
	                                stroll_page_size())); \
	dpack_assert_api((_decoder)->moff <= \
	                 stroll_align_lower((_decoder)->fsize, \
	                                    (off_t)(_decoder)->msize)); \
	dpack_assert_api((_decoder)->fd >= 0)

static __dpack_nonull(1) __dpack_pure __warn_result
size_t
dpack_decoder_file_left(const struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_file_api((const struct dpack_decoder_file *)
	                              decoder);

	const struct dpack_decoder_file * dec =
		(const struct dpack_decoder_file *)decoder;

	return (size_t)(dec->fsize - dec->foff);
}

static __dpack_nonull(1) __dpack_nothrow __warn_result
int
dpack_decoder_file_remap(struct dpack_decoder_file * __restrict decoder,
                         off_t                                  offset)
{
	dpack_assert_intern(decoder);
	dpack_decoder_assert_intern(&decoder->base);
	dpack_assert_intern(decoder->fsize > 0);
	dpack_assert_intern((uint64_t)decoder->fsize <= (uint64_t)SIZE_MAX);
	dpack_assert_intern(decoder->foff >= 0);
	dpack_assert_intern(decoder->foff <= decoder->fsize);
	dpack_assert_intern(decoder->moff >= 0);
	dpack_assert_intern(decoder->moff < decoder->fsize);
	dpack_assert_intern(decoder->msize);
	dpack_assert_intern(decoder->msize <=
	                    (size_t)DPACK_DECODER_FILE_MSIZE_MAX);
	dpack_assert_intern(stroll_aligned(decoder->msize, stroll_page_size()));
	dpack_assert_intern(decoder->map);
	dpack_assert_intern(decoder->map != MAP_FAILED);
	dpack_assert_intern(decoder->fd >= 0);
	dpack_assert_intern(offset >= 0);
	dpack_assert_intern(stroll_aligned(offset, decoder->msize));
	dpack_assert_intern(offset <=
	                    stroll_align_lower(decoder->fsize,
	                                       (off_t)decoder->msize));

STROLL_IGNORE_WARN("-Wcast-qual")
	decoder->map = mmap((void *)decoder->map,
	                    decoder->msize,
	                    PROT_READ,
	                    MAP_FIXED | MAP_PRIVATE | MAP_POPULATE,
	                    decoder->fd,
	                    offset);
STROLL_RESTORE_WARN
	if (decoder->map != MAP_FAILED)
		return 0;

	dpack_assert_intern(errno != EACCES);
	dpack_assert_intern(errno != EBADF);
	dpack_assert_intern(errno != EEXIST);
	dpack_assert_intern(errno != EINVAL);
	dpack_assert_intern(errno != ETXTBSY);

	return -errno;
}

static __dpack_nonull(1, 2) __dpack_nothrow __warn_result
int
dpack_decoder_file_read(struct dpack_decoder * __restrict decoder,
                        uint8_t * __restrict              data,
                        size_t                            size)
{
	dpack_decoder_assert_file_api((const struct dpack_decoder_file *)
	                              decoder);
	dpack_assert_api(((struct dpack_decoder_file *)decoder)->map);
	dpack_assert_api(((struct dpack_decoder_file *)decoder)->map !=
	                 MAP_FAILED);
	dpack_assert_intern(size);
	dpack_assert_api((uint64_t)size <= (uint64_t)OFF_MAX);

	struct dpack_decoder_file * dec = (struct dpack_decoder_file *)decoder;
	off_t                       foff;

	if (!__builtin_add_overflow(dec->foff, (off_t)size, &foff) &&
	    (foff <= dec->fsize)) {
		size_t msz = dec->msize;
		off_t  msk = (off_t)(msz - 1);
		off_t  moff = dec->foff & ~msk;
		int    err;
		size_t start = (size_t)(dec->foff & msk);
		size_t bytes = stroll_min(size, msz - start);

		if (moff != dec->moff) {
			/* Relocate the data mapping window if required. */
			err = dpack_decoder_file_remap(dec, moff);
			if (err)
				return err;
		}
		memcpy(data, &dec->map[start], bytes);
		size -= bytes;

		while (size >= msz) {
			moff += (off_t)msz;
			err = dpack_decoder_file_remap(dec, moff);
			if (err)
				return err;

			data += msz;
			memcpy(data, dec->map, msz);
			size -= msz;
		}

		if (size) {
			moff += (off_t)msz;
			err = dpack_decoder_file_remap(dec, moff);
			if (err)
				return err;

			memcpy(data + msz, dec->map, size);
		}

		dec->foff = foff;
		dec->moff = moff;

		return 0;
	}

	return -ENODATA;
}

static __dpack_nonull(1) __dpack_nothrow __warn_result
int
dpack_decoder_file_skip(struct dpack_decoder * __restrict decoder,
                        size_t                            size)
{
	dpack_decoder_assert_file_api((struct dpack_decoder_file *)decoder);
	dpack_assert_intern(size);
	dpack_assert_api((uint64_t)size <= (uint64_t)OFF_MAX);

	struct dpack_decoder_file * dec = (struct dpack_decoder_file *)decoder;
	off_t                       foff;

	if (!__builtin_add_overflow(dec->foff, (off_t)size, &foff) &&
	    (foff <= dec->fsize)) {
		dec->foff = foff;
		return 0;
	}

	/* Out of file data boundaries. */
	return -ENODATA;
}

static __dpack_nonull(1) __warn_result
int
dpack_decoder_file_fini(struct dpack_decoder * __restrict decoder)
{
	dpack_decoder_assert_file_api((const struct dpack_decoder_file *)
	                              decoder);

	struct dpack_decoder_file * dec = (struct dpack_decoder_file *)decoder;
	int                         ret;

	/*
	 * As stated into munmap(2), closing the file descriptor does not unmap
	 * mappings implicitly.
	 */
	if (dec->map) {
STROLL_IGNORE_WARN("-Wcast-qual")
		if (munmap((void *)dec->map, dec->msize)) {
STROLL_RESTORE_WARN
			dpack_assert_intern(errno != EACCES);
			dpack_assert_intern(errno != EBADF);
			dpack_assert_intern(errno != EEXIST);
			dpack_assert_intern(errno != EINVAL);
			dpack_assert_intern(errno != ENFILE);
			dpack_assert_intern(errno != ENODEV);
			dpack_assert_intern(errno != EOVERFLOW);
			dpack_assert_intern(errno != ETXTBSY);

			ret = -errno;
		}
	}

	if (!ret)
		return ufile_close(dec->fd);

	ufile_close(dec->fd);

	return ret;
}

static const struct dpack_decoder_ops dpack_decoder_file_ops = {
	.left = dpack_decoder_file_left,
	.read = dpack_decoder_file_read,
	.skip = dpack_decoder_file_skip,
	.fini = dpack_decoder_file_fini
};

int
_dpack_decoder_init_file_at(struct dpack_decoder_file * __restrict decoder,
                            int                                    dir,
                            const char * __restrict                path,
                            size_t                                 map_size,
                            int                                    flags,
                            bool                                   discard)
{
	dpack_assert_api(decoder);
	dpack_assert_api(dir >= 0);
	dpack_assert_api(upath_validate_path_name(path) > 0);
	dpack_assert_api(map_size);
	dpack_assert_api((uint64_t)DPACK_DECODER_FILE_MSIZE_MAX <
	                 (uint64_t)OFF_MAX);
	dpack_assert_api((uint64_t)DPACK_DECODER_FILE_MSIZE_MAX <
	                 (uint64_t)SIZE_MAX);
	dpack_assert_api(map_size <= (size_t)DPACK_DECODER_FILE_MSIZE_MAX);
	dpack_assert_api(!(flags & (O_WRONLY | O_RDWR)));
	dpack_assert_api(!(flags & O_APPEND));
	dpack_assert_api(!(flags & O_CREAT));
	dpack_assert_api(!(flags & O_DIRECTORY));
	dpack_assert_api(!(flags & O_EXCL));
	dpack_assert_api(!(flags & O_NONBLOCK));
	dpack_assert_api(!(flags & O_PATH));

	int         fd;
	struct stat st;
	int         err;
	void *      map;

	fd = ufile_open_at(dir, path, O_RDONLY | flags);
	if (fd)
		return fd;

	err = ufile_fstat(fd, &st);
	if (err)
		goto close;

	if (!st.st_size) {
		err = -ENODATA;
		goto close;
	}
	else if ((off64_t)st.st_size > (off64_t)SIZE_MAX) {
		/*
		 *  Do not support files larger than SIZE_MAX since the API
		 * returns / manipulates size_t arguments...
		 */
		err = -EOVERFLOW;
		goto close;
	}

	map_size = stroll_min(map_size, (size_t)st.st_size);
	map_size = stroll_align_upper(map_size, stroll_page_size());
	map = mmap(0, map_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
	if (map == MAP_FAILED) {
		dpack_assert_intern(errno != EBADF);
		dpack_assert_intern(errno != EEXIST);
		dpack_assert_intern(errno != EINVAL);
		dpack_assert_intern(errno != EOVERFLOW);
		dpack_assert_intern(errno != ETXTBSY);

		err = -errno;
		goto close;
	}

	dpack_decoder_init(&decoder->base,
	                   &dpack_decoder_file_ops,
	                   discard);
	decoder->foff = 0;
	decoder->moff = 0;
	decoder->fsize = st.st_size;
	decoder->msize = map_size;
	decoder->map = map;
	decoder->fd = fd;

	return 0;

close:
	ufile_close(fd);

	return err;
}
