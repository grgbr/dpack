################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of DPack.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

# Enable a bunch of warning options and disable -ffinite-math-only optimizations
# so that floating point number classification macros such as isnan() works
# properly (since NaN and Infinity values are tested).
common-cflags         := -Wall \
                         -Wextra \
                         -Wformat=2 \
                         -Wconversion \
                         -Wundef \
                         -Wshadow \
                         -Wcast-qual \
                         -Wcast-align \
                         -Wmissing-declarations \
                         -D_GNU_SOURCE \
                         -I $(TOPDIR)/include \
                         $(filter-out -ffinite-math-only,$(EXTRA_CFLAGS)) \
                         -fno-finite-math-only \
                         -fvisibility=hidden
common-ldflags        := $(common-cflags) \
                         $(EXTRA_LDFLAGS) \
                         -Wl,-z,start-stop-visibility=hidden

# When assertions are enabled, ensure NDEBUG macro is not set to enable glibc
# assertions.
ifneq ($(filter y,$(CONFIG_DPACK_ASSERT_API) $(CONFIG_DPACK_ASSERT_INTERN)),)
common-cflags         := $(filter-out -DNDEBUG,$(common-cflags))
common-ldflags        := $(filter-out -DNDEBUG,$(common-ldflags))
endif # ($(filter y,$(CONFIG_DPACK_ASSERT_API) $(CONFIG_DPACK_ASSERT_INTERN)),)

solibs                := libdpack.so
libdpack.so-objs      += shared/codec.o
libdpack.so-objs      += $(call kconf_enabled,DPACK_SCALAR,shared/scalar.o)
libdpack.so-objs      += $(call kconf_enabled,DPACK_STRING,shared/string.o)
libdpack.so-objs      += $(call kconf_enabled,DPACK_LVSTR,shared/lvstr.o)
libdpack.so-objs      += $(call kconf_enabled,DPACK_BIN,shared/bin.o)
libdpack.so-objs      += $(call kconf_enabled,DPACK_MAP,shared/map.o)
libdpack.so-objs      += $(call kconf_enabled,DPACK_ARRAY,shared/array.o)
libdpack.so-cflags    := $(filter-out -fpie -fPIE,$(common-cflags)) -fpic
libdpack.so-ldflags   := $(filter-out -fpie -fPIE,$(common-ldflags)) \
                         -shared -fpic -Bsymbolic -Wl,-soname,libdpack.so
libdpack.so-pkgconf   := libstroll

arlibs                := libdpack.a
libdpack.a-objs       += static/codec.o
libdpack.a-objs       += $(call kconf_enabled,DPACK_SCALAR,static/scalar.o)
libdpack.a-objs       += $(call kconf_enabled,DPACK_STRING,static/string.o)
libdpack.a-objs       += $(call kconf_enabled,DPACK_LVSTR,static/lvstr.o)
libdpack.a-objs       += $(call kconf_enabled,DPACK_BIN,static/bin.o)
libdpack.a-objs       += $(call kconf_enabled,DPACK_MAP,static/map.o)
libdpack.a-objs       += $(call kconf_enabled,DPACK_ARRAY,static/array.o)
libdpack.a-cflags     := $(common-cflags)

# vim: filetype=make :
