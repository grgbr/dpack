#include "common.h"

int
dpack_decode_tag(struct mpack_reader_t * reader,
                 enum mpack_type_t       type,
                 struct mpack_tag_t    * tag)
{
	dpack_assert(reader);
	dpack_assert(tag);
	dpack_assert(mpack_reader_error(reader) == mpack_ok);

	enum mpack_error_t err;

	*tag = mpack_read_tag(reader);
	err = mpack_reader_error(reader);
	if (err != mpack_ok)
		return dpack_errno_from_mpack(err);

	if (mpack_tag_type(tag) != type) {
		mpack_reader_flag_error(reader, mpack_error_type);
		return -ENOMSG;
	}

	return 0;
}

