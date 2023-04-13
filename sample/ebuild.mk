sample-cflags  := -Wall \
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
sample-ldflags := $(common-cflags) \
                  -L$(BUILDDIR)/../src \
                  $(EXTRA_LDFLAGS)

ifneq ($(filter y,$(CONFIG_DPACK_ASSERT_API) $(CONFIG_DPACK_ASSERT_INTERN)),)
sample-cflags  := $(filter-out -DNDEBUG,$(sample-cflags))
sample-ldflags := $(filter-out -DNDEBUG,$(sample-ldflags))
endif # ($(filter y,$(CONFIG_DPACK_ASSERT_API) $(CONFIG_DPACK_ASSERT_INTERN)),)

builtins                         := builtin.a
builtin.a-objs                   := test.o
builtin.a-cflags                 := $(sample-cflags)

bins                             := test-fix_sample
test-fix_sample-objs             := test-fix_sample.o fix_sample.o
test-fix_sample-cflags           := $(sample-cflags)
test-fix_sample-ldflags          := $(sample-ldflags) -l:builtin.a -ldpack
test-fix_sample-pkgconf          := libstroll

bins                             += test-scalar_array_sample
test-scalar_array_sample-objs    := test-scalar_array_sample.o \
                                    scalar_array_sample.o
test-scalar_array_sample-cflags  := $(sample-cflags)
test-scalar_array_sample-ldflags := $(sample-ldflags) -l:builtin.a -ldpack
test-scalar_array_sample-pkgconf := libstroll

bins                             += test-map_sample
test-map_sample-objs             := test-map_sample.o map_sample.o
test-map_sample-cflags           := $(sample-cflags)
test-map_sample-ldflags          := $(sample-ldflags) -l:builtin.a -ldpack
test-map_sample-pkgconf          := libstroll

# vim: filetype=make :
