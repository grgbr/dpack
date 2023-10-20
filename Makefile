################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of DPack.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

override PACKAGE := dpack
override VERSION := 1.0
EXTRA_CFLAGS     := -O2 -DNDEBUG
EXTRA_LDFLAGS    := $(EXTRA_CFLAGS)

export VERSION EXTRA_CFLAGS EXTRA_LDFLAGS

ifeq ($(strip $(EBUILDDIR)),)
ifneq ($(realpath ebuild/main.mk),)
EBUILDDIR := $(realpath ebuild)
else  # ($(realpath ebuild/main.mk),)
EBUILDDIR := $(realpath /usr/share/ebuild)
endif # !($(realpath ebuild/main.mk),)
endif # ($(strip $(EBUILDDIR)),)

ifeq ($(realpath $(EBUILDDIR)/main.mk),)
$(error '$(EBUILDDIR)': no valid eBuild install found !)
endif # ($(realpath $(EBUILDDIR)/main.mk),)

include $(EBUILDDIR)/main.mk

chkall_builddir  := $(BUILDDIR)/checkall
chkconf_builddir  = $(chkall_builddir)/conf$(strip $(1))
chkconf_infile    = $(chkall_builddir)/confs/conf$(strip $(1)).in

.PHONY: check
check: build
	$(Q)$(BUILDDIR)/test/dpack-utest \
		$(if $(Q),--silent,--terse) \
		--xml=$(BUILDDIR)/test/dpack-utest.xml \
		run

$(chkall_builddir)/confs/.generated: $(config-in) \
                                     $(TOPDIR)/scripts/gen_check_confs.py \
                                     | $(chkall_builddir)/confs
	$(Q)$(PYTHON) $(TOPDIR)/scripts/gen_check_confs.py $(config-in) $(|)
	@touch $(@)

$(chkall_builddir)/conf%/.config: $(chkall_builddir)/confs/.generated \
                                  | $(chkall_builddir)/conf%/
	$(Q)env KCONFIG_ALLCONFIG='$(call chkconf_infile,$(*))' \
	        KCONFIG_CONFIG='$(call chkconf_builddir,$(*))/.config' \
	        $(KCONF) --allnoconfig '$(config-in)' >/dev/null
	$(Q)$(MAKE) -C $(TOPDIR) \
	            olddefconfig \
	            BUILDDIR:='$(call chkconf_builddir,$(*))'

check-conf%: $(chkall_builddir)/conf%/.config
	$(Q)$(MAKE) -C $(TOPDIR) check BUILDDIR:='$(call chkconf_builddir,$(*))'

.PHONY: checkall
checkall: $(chkall_builddir)/confs/.generated
	$(Q)for c in $(wildcard $(chkall_builddir)/confs/conf*.in); do \
		$(MAKE) -C $(TOPDIR) check-$$(basename $$c .in) || exit 1; \
	done

$(chkall_builddir)/conf%/:
	@mkdir -p $(@)

$(chkall_builddir)/confs:
	@mkdir -p $(@)
