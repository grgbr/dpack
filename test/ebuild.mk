test-cflags            := -Wall \
                          -Wextra \
                          -Wformat=2 \
                          -Wconversion \
                          -Wundef \
                          -Wshadow \
                          -Wcast-qual \
                          -Wcast-align \
                          -Wmissing-declarations \
                          -D_GNU_SOURCE \
                          -D_STROLL_CMOCKA_UTEST \
                          -I $(TOPDIR)/include \
                          $(EXTRA_CFLAGS)
test-ldflags           := $(common-cflags) \
                          -L$(BUILDDIR)/../src \
                          $(EXTRA_LDFLAGS) \
                          -ldpack

bins                   :=

ifeq ($(CONFIG_DPACK_ARRAY),y)
bins                   += dpack-array-ut
dpack-array-ut-objs    := array.o
dpack-array-ut-cflags  := $(test-cflags)
dpack-array-ut-ldflags := $(test-ldflags)
dpack-array-ut-pkgconf := cmocka
endif # ($(CONFIG_DPACK_ARRAY),y)

ifeq ($(CONFIG_DPACK_STRING),y)
bins                    += dpack-string-ut
dpack-string-ut-objs    := string.o
dpack-string-ut-cflags  := $(test-cflags)
dpack-string-ut-ldflags := $(test-ldflags)
dpack-string-ut-pkgconf := cmocka
endif # ($(CONFIG_DPACK_STRING),y)

# vim: filetype=make :
