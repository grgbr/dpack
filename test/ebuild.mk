################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of DPack.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

# Enable a bunch of warning options and disable -fno-signed-zeros optimization
# since negative floating point zero is tested.
test-cflags  := -Wall \
                -Wextra \
                -Wformat=2 \
                -Wconversion \
                -Wundef \
                -Wshadow \
                -Wcast-qual \
                -Wcast-align \
                -Wmissing-declarations \
                -D_GNU_SOURCE \
                -DDPACK_VERSION_STRING="\"$(VERSION)\"" \
                -I $(TOPDIR)/include \
                $(filter-out -fno-signed-zeros -fassociative-math, \
                             $(EXTRA_CFLAGS)) \
                -fsigned-zeros -fno-associative-math
test-ldflags := $(test-cflags) \
                -L$(BUILDDIR)/../src \
                $(EXTRA_LDFLAGS) \
                -Wl,-z,start-stop-visibility=hidden \
                -Wl,-whole-archive $(BUILDDIR)/builtin.a -Wl,-no-whole-archive \
                -ldpack

ifneq ($(filter y,$(CONFIG_DPACK_ASSERT_API) $(CONFIG_DPACK_ASSERT_INTERN)),)
test-cflags         := $(filter-out -DNDEBUG,$(test-cflags))
test-ldflags        := $(filter-out -DNDEBUG,$(test-ldflags))
endif # ($(filter y,$(CONFIG_DPACK_ASSERT_API) $(CONFIG_DPACK_ASSERT_INTERN)),)

builtins            := builtin.a
builtin.a-objs      := utest.o $(config-obj)
builtin.a-cflags    := $(test-cflags)

checkbins           := dpack-utest

dpack-utest-objs    += $(call kconf_enabled,DPACK_ARRAY,array.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_BIN,bin.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_DOUBLE,double.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_FLOAT,float.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,bool.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,uint8.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,int8.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,uint16.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,int16.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,uint32.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,int32.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,uint64.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,int64.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_STRING,string.o)
dpack-utest-cflags  := $(test-cflags)
dpack-utest-ldflags := $(test-ldflags)
dpack-utest-pkgconf := libstroll libcute

# ex: filetype=make :
