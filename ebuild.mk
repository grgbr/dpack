################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of DPack.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

config-in   := Config.in
config-h    := $(PACKAGE)/config.h

HEADERDIR   := $(CURDIR)/include
headers      = $(PACKAGE)/cdefs.h $(PACKAGE)/codec.h $(PACKAGE)/mpack-config.h
headers     += $(call kconf_enabled,DPACK_SCALAR,$(PACKAGE)/scalar.h)
headers     += $(call kconf_enabled,DPACK_STRING,$(PACKAGE)/string.h)
headers     += $(call kconf_enabled,DPACK_LVSTR,$(PACKAGE)/lvstr.h)
headers     += $(call kconf_enabled,DPACK_BIN,$(PACKAGE)/bin.h)
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
Requires.private: libstroll
Cflags: -I$${includedir}
Libs: -L$${libdir} -ldpack
endef

pkgconfigs       := libdpack.pc
libdpack.pc-tmpl := libdpack_pkgconf_tmpl

################################################################################
# Documentation generation
################################################################################

doxyconf  := $(CURDIR)/sphinx/Doxyfile
doxyenv   := SRCDIR="$(HEADERDIR) $(SRCDIR)/src"

sphinxsrc := $(CURDIR)/sphinx
sphinxenv := \
	VERSION="$(VERSION)" \
	$(if $(strip $(EBUILDDOC_TARGET_PATH)), \
	     EBUILDDOC_TARGET_PATH="$(strip $(EBUILDDOC_TARGET_PATH))") \
	$(if $(strip $(EBUILDDOC_INVENTORY_PATH)), \
	     EBUILDDOC_INVENTORY_PATH="$(strip $(EBUILDDOC_INVENTORY_PATH))") \
	$(if $(strip $(STROLLDOC_TARGET_PATH)), \
	     STROLLDOC_TARGET_PATH="$(strip $(STROLLDOC_TARGET_PATH))") \
	$(if $(strip $(STROLLDOC_INVENTORY_PATH)), \
	     STROLLDOC_INVENTORY_PATH="$(strip $(STROLLDOC_INVENTORY_PATH))")

# vim: filetype=make :
