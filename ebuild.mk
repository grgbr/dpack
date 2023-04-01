config-in   := Config.in
config-h    := $(PACKAGE)/config.h

HEADERDIR   := $(CURDIR)/include
headers      = $(PACKAGE)/cdefs.h $(PACKAGE)/codec.h $(PACKAGE)/mpack-config.h
headers     += $(call kconf_enabled,DPACK_SCALAR,$(PACKAGE)/scalar.h)
headers     += $(call kconf_enabled,DPACK_STRING,$(PACKAGE)/string.h)
headers     += $(call kconf_enabled,DPACK_MAP,$(PACKAGE)/map.h)
headers     += $(call kconf_enabled,DPACK_ARRAY,$(PACKAGE)/array.h)

subdirs     := src

ifeq ($(CONFIG_DPACK_SAMPLE),y)
subdirs     += sample
sample-deps := src
endif # ($(CONFIG_DPACK_SAMPLE),y)

ifeq ($(CONFIG_DPACK_UTEST),y)
subdirs   += test
test-deps := src
endif # ($(CONFIG_DPACK_UTEST),y)

define libdpack_pkgconf_tmpl
prefix=$(PREFIX)
exec_prefix=$${prefix}
libdir=$${exec_prefix}/lib
includedir=$${prefix}/include

Name: libdpack
Description: dpack library
Version: %%PKG_VERSION%%
Requires:
Cflags: -I$${includedir}
Libs: -L$${libdir} -ldpack
endef

pkgconfigs       := libdpack.pc
libdpack.pc-tmpl := libdpack_pkgconf_tmpl

################################################################################
# Documentation generation
################################################################################

#doxyconf  := $(CURDIR)/sphinx/Doxyfile
#doxyenv   := INCDIR="$(patsubst -I%,%,$(filter -I%,$(common-cflags)))" \
#             VERSION="$(VERSION)"
#
#sphinxsrc := $(CURDIR)/sphinx
#sphinxenv := VERSION="$(VERSION)"

# vim: filetype=make :
