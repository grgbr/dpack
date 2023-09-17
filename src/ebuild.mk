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
                         $(EXTRA_CFLAGS) \
                         -fvisibility=hidden
common-ldflags        := $(common-cflags) \
                         $(EXTRA_LDFLAGS) \
                         -Wl,-z,start-stop-visibility=hidden

# When assertions are enabled:
# * ensure NDEBUG macro is not set to enable glibc assertions ;
# * disable -ffinite-math-only optimizations so that floating point number
#   assertion checking works properly (since NaN and Infinity values are
#   tested).
ifneq ($(filter y,$(CONFIG_DPACK_ASSERT_API) $(CONFIG_DPACK_ASSERT_INTERN)),)
common-cflags         := $(filter-out -DNDEBUG,$(common-cflags))
common-cflags         := $(filter-out -ffinite-math-only,$(common-cflags)) \
                         -fno-finite-math-only
common-ldflags        := $(filter-out -DNDEBUG,$(common-ldflags))
endif # ($(filter y,$(CONFIG_DPACK_ASSERT_API) $(CONFIG_DPACK_ASSERT_INTERN)),)

solibs                := libdpack.so
libdpack.so-objs      += shared/codec.o shared/common.o shared/mpack.o
shared/mpack.o-cflags := $(common-cflags) -fpic
libdpack.so-objs      += $(call kconf_enabled,DPACK_SCALAR,shared/scalar.o)
libdpack.so-objs      += $(call kconf_enabled,DPACK_STRING,shared/string.o)
libdpack.so-objs      += $(call kconf_enabled,DPACK_LVSTR,shared/lvstr.o)
libdpack.so-objs      += $(call kconf_enabled,DPACK_BIN,shared/bin.o)
libdpack.so-objs      += $(call kconf_enabled,DPACK_MAP,shared/map.o)
libdpack.so-objs      += $(call kconf_enabled,DPACK_ARRAY,shared/array.o)
libdpack.so-cflags    := $(filter-out -fpie -fPIE,$(common-cflags)) -fpic
libdpack.so-ldflags   := $(filter-out -fpie -fPIE,$(common-cflags)) \
                         -shared -fpic -Bsymbolic -Wl,-soname,libdpack.so
libdpack.so-pkgconf   := libstroll

arlibs                := libdpack.a
libdpack.a-objs       += static/codec.o static/common.o static/mpack.o
static/mpack.o-cflags := $(common-cflags)
libdpack.a-objs       += $(call kconf_enabled,DPACK_SCALAR,static/scalar.o)
libdpack.a-objs       += $(call kconf_enabled,DPACK_STRING,static/string.o)
libdpack.a-objs       += $(call kconf_enabled,DPACK_LVSTR,static/lvstr.o)
libdpack.a-objs       += $(call kconf_enabled,DPACK_BIN,static/bin.o)
libdpack.a-objs       += $(call kconf_enabled,DPACK_MAP,static/map.o)
libdpack.a-objs       += $(call kconf_enabled,DPACK_ARRAY,static/array.o)
libdpack.a-cflags     := $(common-cflags)

# Install generated mpack header into top-level build include directory, i.e.,
# where package kconfig config.h is generated.
# If modifying this, ensure that the $(headers) definition matches with the
# $(mpack-header) definition below.
HEADERDIR             := $(BUILDDIR)/../include
headers               := $(PACKAGE)/mpack.h

################################################################################
# MPack construction logic
################################################################################

# The generated mpack source implementation file.
mpack.o-src   := $(BUILDDIR)/mpack/mpack.c
# The generated mpack source header file. If modifying this, ensure this matches
# with the $(headers) definition above.
mpack-header  := $(HEADERDIR)/$(PACKAGE)/mpack.h

# Make all mpack.o targets (static/mpack.o and shared/mpack.o) depend on mpack.c
# source file generated thanks to the mpack amalgamation target defined below,
# i.e., $(BUILDDIR)/mpack/mpack.c
libdpack_objs  = $(addprefix $(BUILDDIR)/,$(filter-out %/mpack.o,$(1)))

# Make all static library objects depend on static mpack.o object.
$(call libdpack_objs,$(libdpack.a-objs)): $(BUILDDIR)/static/mpack.o
# Static mpack object requires the mpack header...
$(BUILDDIR)/static/mpack.o: $(mpack-header)

# Make all shared library objects depend on shared mpack.o object.
$(call libdpack_objs,$(libdpack.so-objs)): $(BUILDDIR)/shared/mpack.o
# Shared mpack object requires the mpack header...
$(BUILDDIR)/shared/mpack.o: $(mpack-header)

# Build mpack library sources thanks to the amalgamation script shipped with the
# mpack source subtree.
# To prevent from polluting the original mpack source subtree, it is first
# recursively copied under the $(BUILDDIR)/mpack build directory so that the
# generation process may run from under it.
$(BUILDDIR)/mpack/mpack.c: $(wildcard $(TOPDIR)/mpack/src/mpack/*.c) \
                           $(wildcard $(TOPDIR)/mpack/src/mpack/*.h) \
                           $(TOPDIR)/mpack/tools/amalgamate.sh \
                           | $(BUILDDIR)/mpack
	@echo "  GEN     $(@)"
	$(Q)$(RSYNC) -a --delete $(TOPDIR)/mpack/ $(BUILDDIR)/mpack/
	$(Q)cd $(BUILDDIR)/mpack && tools/amalgamate.sh $(if $(Q),>/dev/null)
	$(Q)sed 's@#include[[:blank:]]\+"mpack.h"@#include <$(PACKAGE)/mpack.h>@' \
	        $(BUILDDIR)/mpack/.build/amalgamation/src/mpack/mpack.c \
	        > $(@)

$(mpack-header): $(BUILDDIR)/mpack/mpack.c
	@echo "  GEN     $(@)"
	$(Q)sed -e 's@#include[[:blank:]]\+"mpack-config.h"@#include <$(PACKAGE)/mpack-config.h>@' \
	        -e '/^#define MPACK_H 1/a \\n#define MPACK_HAS_CONFIG 1' \
	        $(BUILDDIR)/mpack/.build/amalgamation/src/mpack/mpack.h \
	        > $(@)

$(BUILDDIR)/mpack:
	@mkdir -p $(@)

# Hookup generated mpack sources cleanup logic with main clean target.
clean: clean-mpack

# Recursively delete the mpack source generation build directory mentioned
# above.
.PHONY: clean-mpack
clean-mpack:
	$(call rmr_recipe,$(BUILDDIR)/mpack)
	$(call rm_recipe,$(mpack-header))

# vim: filetype=make :
