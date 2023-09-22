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
                $(EXTRA_CFLAGS)
test-ldflags := $(test-cflags) \
                -L$(BUILDDIR)/../src \
                $(EXTRA_LDFLAGS) \
                -Wl,-whole-archive $(BUILDDIR)/builtin.a -Wl,-no-whole-archive \
                -ldpack

ifneq ($(filter y,$(CONFIG_DPACK_ASSERT_API) $(CONFIG_DPACK_ASSERT_INTERN)),)
test-cflags         := $(filter-out -DNDEBUG,$(test-cflags))
test-ldflags        := $(filter-out -DNDEBUG,$(test-ldflags))
endif # ($(filter y,$(CONFIG_DPACK_ASSERT_API) $(CONFIG_DPACK_ASSERT_INTERN)),)

builtins            := builtin.a
builtin.a-objs      := utest.o
builtin.a-cflags    := $(test-cflags)

bins                := dpack-utest

dpack-utest-objs    += $(call kconf_enabled,DPACK_ARRAY,array.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_BIN,bin.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_DOUBLE,double.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_FLOAT,float.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,bool.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,uint8.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,int8.o)
ifeq (0,1)
dpack-utest-objs    += $(call kconf_enabled,DPACK_SCALAR,scalar.o)
dpack-utest-objs    += $(call kconf_enabled,DPACK_STRING,string.o)
endif
dpack-utest-cflags  := $(test-cflags)
dpack-utest-ldflags := $(test-ldflags)
dpack-utest-pkgconf := libstroll libcute

# ex: filetype=make :
