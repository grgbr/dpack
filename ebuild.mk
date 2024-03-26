################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of DPack.
# Copyright (C) 2023-2024 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

config-in   := Config.in
config-h    := $(PACKAGE)/config.h
config-obj  := config.o

ifneq ($(realpath $(kconf_config)),)
ifeq ($(call kconf_is_enabled,DPACK_HAS_BASIC_ITEMS),)
$(error Invalid build configuration !)
endif # ($(call kconf_is_enabled,DPACK_HAS_BASIC_ITEMS),)
endif # ($(realpath $(kconf_config)),)

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
Version: $(VERSION)
Requires.private: libstroll
Cflags: -I$${includedir}
Libs: -L$${libdir} -ldpack
endef

pkgconfigs       := libdpack.pc
libdpack.pc-tmpl := libdpack_pkgconf_tmpl

################################################################################
# Source code tags generation
################################################################################

tagfiles := $(shell find $(addprefix $(CURDIR)/,$(subdirs) mpack) \
                    $(HEADERDIR) \
                    -type f)

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
	$(if $(strip $(CUTEDOC_TARGET_PATH)), \
	     CUTEDOC_TARGET_PATH="$(strip $(CUTEDOC_TARGET_PATH))") \
	$(if $(strip $(CUTEDOC_INVENTORY_PATH)), \
	     CUTEDOC_INVENTORY_PATH="$(strip $(CUTEDOC_INVENTORY_PATH))") \
	$(if $(strip $(STROLLDOC_TARGET_PATH)), \
	     STROLLDOC_TARGET_PATH="$(strip $(STROLLDOC_TARGET_PATH))") \
	$(if $(strip $(STROLLDOC_INVENTORY_PATH)), \
	     STROLLDOC_INVENTORY_PATH="$(strip $(STROLLDOC_INVENTORY_PATH))")

################################################################################
# Source distribution generation
################################################################################

# Declare the list of files under revision control to include into final source
# distribution tarball.
override distfiles = $(list_versioned_recipe)

# Override InterSphinx eBuild base documentation URI and make it point to online
# GitHub pages when building final source distribution tarball
dist: export EBUILDDOC_TARGET_PATH := http://grgbr.github.io/ebuild/
dist: export CUTEDOC_TARGET_PATH := http://grgbr.github.io/cute/
dist: export STROLLDOC_TARGET_PATH := http://grgbr.github.io/stroll/

# ex: filetype=make :
