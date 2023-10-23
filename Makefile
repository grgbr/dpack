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

define list_check_confs_cmds :=
nr=$$($(PYTHON) $(TOPDIR)/scripts/gen_check_confs.py $(config-in) count); \
c=0; \
while [ $$c -lt $$nr ]; do \
	echo $$c; \
	c=$$((c + 1)); \
done
endef

check_conf_list       := $(shell $(list_check_confs_cmds))
check_conf_targets    := $(addprefix check-conf,$(check_conf_list))
checkall_builddir     := $(BUILDDIR)/checkall
check_lib_search_path := \
	$(BUILDDIR)/src$(if $(LD_LIBRARY_PATH),:$(LD_LIBRARY_PATH))

.PHONY: check
check: build
	$(Q)env LD_LIBRARY_PATH="$(check_lib_search_path)" \
	        $(BUILDDIR)/test/dpack-utest \
	        $(if $(Q),--terse,--verbose) \
	        --xml=$(BUILDDIR)/test/dpack-utest.xml \
	        run

.PHONY: checkall
checkall: $(check_conf_targets)

$(check_conf_targets): check-conf%: $(checkall_builddir)/conf%/.config FORCE
	$(Q)$(MAKE) -C $(TOPDIR) check BUILDDIR:='$(abspath $(dir $(<)))'

$(addprefix $(checkall_builddir)/conf,$(addsuffix /.config,$(check_conf_list))): \
$(checkall_builddir)/conf%/.config: $(checkall_builddir)/conf%/test.config
	$(Q)env KCONFIG_ALLCONFIG='$(<)' \
	        KCONFIG_CONFIG='$(@)' \
	        $(KCONF) --allnoconfig '$(config-in)' >/dev/null
	$(Q)$(MAKE) -C $(TOPDIR) olddefconfig BUILDDIR:='$(abspath $(dir $(@)))'

$(addprefix $(checkall_builddir)/conf,$(addsuffix /test.config,$(check_conf_list))): \
$(checkall_builddir)/conf%/test.config: $(config-in) \
                                      $(TOPDIR)/scripts/gen_check_confs.py
	@mkdir -p $(dir $(@))
	$(Q)$(PYTHON) $(TOPDIR)/scripts/gen_check_confs.py $(config-in) \
	                                                   genone \
	                                                   $(*) \
	                                                   $(@)

clean: clean-checkall

.PHONY: clean-checkall
clean-checkall:
	$(call rmr_recipe,$(checkall_builddir))

.PHONY: FORCE
FORCE:
	@:
