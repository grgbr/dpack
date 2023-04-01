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

bins                             := test-fix_sample
test-fix_sample-objs             := test-fix_sample.o fix_sample.o test.o
test-fix_sample-cflags           := $(sample-cflags)
test-fix_sample-ldflags          := $(sample-ldflags) -ldpack

bins                             := test-scalar_array_sample
test-scalar_array_sample-objs    := test-scalar_array_sample.o \
                                    scalar_array_sample.o \
                                    test.o
test-scalar_array_sample-cflags  := $(sample-cflags)
test-scalar_array_sample-ldflags := $(sample-ldflags) -ldpack

bins                             := test-map_sample
test-map_sample-objs             := test-map_sample.o map_sample.o test.o
test-map_sample-cflags           := $(sample-cflags)
test-map_sample-ldflags          := $(sample-ldflags) -ldpack

# vim: filetype=make :
