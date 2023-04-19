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
                -I $(TOPDIR)/include \
                $(EXTRA_CFLAGS)
test-ldflags := $(test-cflags) \
                -L$(BUILDDIR)/../src \
                $(EXTRA_LDFLAGS) \
                -Wl,-whole-archive $(BUILDDIR)/builtin.a -Wl,-no-whole-archive \
                -ldpack

ifneq ($(filter y,$(CONFIG_DPACK_ASSERT_API) $(CONFIG_DPACK_ASSERT_INTERN)),)
test-cflags            := $(filter-out -DNDEBUG,$(test-cflags))
test-ldflags           := $(filter-out -DNDEBUG,$(test-ldflags))
endif # ($(filter y,$(CONFIG_DPACK_ASSERT_API) $(CONFIG_DPACK_ASSERT_INTERN)),)

builtins         := builtin.a
builtin.a-objs   := utest.o
builtin.a-cflags := $(test-cflags)

bins :=

ifeq ($(CONFIG_DPACK_SCALAR),y)
bins                    += dpack-scalar-ut
dpack-scalar-ut-objs    := scalar.o
dpack-scalar-ut-cflags  := $(test-cflags)
dpack-scalar-ut-ldflags := $(test-ldflags)
dpack-scalar-ut-pkgconf := libstroll cmocka
endif # ($(CONFIG_DPACK_SCALAR),y)

ifeq ($(CONFIG_DPACK_ARRAY),y)
bins                    += dpack-array-ut
dpack-array-ut-objs     := array.o
dpack-array-ut-cflags   := $(test-cflags)
dpack-array-ut-ldflags  := $(test-ldflags)
dpack-array-ut-pkgconf  := libstroll cmocka
endif # ($(CONFIG_DPACK_ARRAY),y)

ifeq ($(CONFIG_DPACK_STRING),y)
bins                    += dpack-string-ut
dpack-string-ut-objs    := string.o
dpack-string-ut-cflags  := $(test-cflags)
dpack-string-ut-ldflags := $(test-ldflags)
dpack-string-ut-pkgconf := libstroll cmocka
endif # ($(CONFIG_DPACK_STRING),y)

# vim: filetype=make :
