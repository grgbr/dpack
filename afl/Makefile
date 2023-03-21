TOPDIR     := $(CURDIR)/..
BUILDDIR   := $(TOPDIR)/build
PYANG      := $(BUILDDIR)/venv/bin/pyang
PLUGIN     := $(TOPDIR)/python/pydpack/plugin
CC         := afl-cc
MPACK_SRC  := $(TOPDIR)/mpack/.build/amalgamation/src/mpack
dpack_objs := map array string scalar codec common

.PHONY: all
all: $(BUILDDIR)/test-afl | $(BUILDDIR)

$(BUILDDIR)/afl-fix.c: | $(BUILDDIR)
	@PYTHONPYCACHEPREFIX=$(BUILDDIR) PYTHONPATH=$(PYTHONPATH):$(TOPDIR)/python/ \
	      $(PYANG) --plugindir $(PLUGIN) --lint -f dpack \
	      --dpack-output-dir "$(BUILDDIR)" $(TOPDIR)/afl/afl-fix.yang

$(BUILDDIR)/test-afl: $(TOPDIR)/afl/test-afl.c \
                      $(BUILDDIR)/afl-fix.c \
                      $(addprefix $(TOPDIR)/src/,$(addsuffix .c,$(dpack_objs))) \
                      $(MPACK_SRC)/mpack.c
	@$(CC) -Wall -include $(TOPDIR)/mpack-config.h \
	       -I$(TOPDIR)/include \
	       -I$(TOPDIR) \
	       -I$(MPACK_SRC) \
	       -I$(BUILDDIR) \
	       -DDPACK_DEBUG -D_GNU_SOURCE \
	       -o $(@) $(filter %.c %.a %.o %.so,$(^))

$(BUILDDIR):
	@mkdir -p $(@)

.PHONY: clean
clean:
	@rm -f $(BUILDDIR)/*afl*.*