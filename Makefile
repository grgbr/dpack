BUILDDIR := $(CURDIR)/build

PYANG           := pyang
PLUGIN          := $(CURDIR)/python/pydpack/plugin

DPACK_DEBUG := 0

ifeq ($(DPACK_DEBUG),1)
DEBUG_CFLAGS    := -ggdb3 -DDPACK_DEBUG
OPTIM_CFLAGS    := -O0
else
OPTIM_CFLAGS    := -O2 -DNDEBUG
endif

MACHINE_CFLAGS  := -march=native
OPTIM_CFLAGS    += -ffast-math \
                   -flto=auto -fuse-linker-plugin \
                   -fno-semantic-interposition
HARDEN_CFLAGS   := -D_FORTIFY_SOURCE=3 \
                   -fstack-protector-strong --param=ssp-buffer-size=4 \
                   -fasynchronous-unwind-tables
CFLAGS          := -Wall -Wextra -Wformat=2 \
                   -D_GNU_SOURCE \
                   -include mpack-config.h \
                   -Iinclude/ \
                   -Impack/.build/amalgamation/src/mpack \
                   -I$(CURDIR) \
                   -I$(BUILDDIR) \
                   -fvisibility=hidden \
                   $(DEBUG_CFLAGS) \
                   $(MACHINE_CFLAGS) \
                   $(OPTIM_CFLAGS) \
                   $(HARDEN_CFLAGS)

MACHINE_LDFLAGS := -Wl,-z,ibt -Wl,-z,shstk
OPTIM_LDFLAGS   := -Wl,-z,combreloc -Wl,--hash-style=gnu
HARDEN_LDFLAGS  := -Wl,-z,nodynamic-undefined-weak \
                   -Wl,-z,defs \
                   -Wl,-z,nodlopen -Wl,-z,nodump \
                   -Wl,-z,interpose \
                   -Wl,-z,noexecstack \
                   -Wl,-z,now -Wl,-z,relro -Wl,-z,separate-code
LDFLAGS         := -Wl,-gc-sections -Wl,-print-gc-sections \
                    $(MACHINE_LDFLAGS) \
                    $(OPTIM_LDFLAGS) \
                    $(HARDEN_LDFLAGS)

.PHONY: build
build: $(BUILDDIR)/libdpack.a $(BUILDDIR)/libdpack.so

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)

################################################################################
# Dpack samples
################################################################################

# Sample test binaries
test_objs := test-fix_sample test-scalar_array_sample test-map_sample

ifneq (, $(shell which $(PYANG)))
test_objs += test-sample-fix
# test-sample-scalar_array
endif

.PHONY: check
check: $(addprefix $(BUILDDIR)/,$(test_objs))

$(BUILDDIR)/test-%: sample/test-%.c $(BUILDDIR)/test.o \
                    $(BUILDDIR)/lib%.a $(BUILDDIR)/libdpack.so
	$(CC) -MD -Itest \
	      -pie $(CFLAGS) $(LDFLAGS) \
	      -o $(@) $(filter %.c %.a %.o %.so,$(^))

$(BUILDDIR)/test.o: sample/test.c | $(BUILDDIR)
	$(CC) -MD -Itest -fpie $(CFLAGS) -o $(@) -c $(<)

# Sample libraries
sample_apps := fix_sample scalar_array_sample map_sample
sample_libs := $(addsuffix .a,$(addprefix $(BUILDDIR)/lib,$(sample_apps)))
sample_objs := $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(sample_apps)))

# Sample libraries
pyang_sample_apps := sample-fix sample-scalar_array
pyang_sample_objs := $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(pyang_sample_apps)))
sample_libs       += $(addsuffix .a,$(addprefix $(BUILDDIR)/lib,$(pyang_sample_apps)))

# Sample test libraries
$(sample_libs): $(BUILDDIR)/lib%.a: $(BUILDDIR)/%.o
	$(AR) rcs $(@) $(^)

# Sample test library objects
$(sample_objs): $(BUILDDIR)/%.o: sample/%.c | $(BUILDDIR)
	$(CC) -MD -Itest -fpie $(CFLAGS) -o $(@) -c $(<)

# Sample test library objects
$(pyang_sample_objs): $(BUILDDIR)/%.o: $(BUILDDIR)/%.c | $(BUILDDIR)
	$(CC) -MD -Itest -fpie $(CFLAGS) -o $(@) -c $(<)

$(BUILDDIR)/%.c: sample/%.yang | $(BUILDDIR)
	$(Q)PYTHONPYCACHEPREFIX=$(BUILDDIR) PYTHONPATH=$(PYTHONPATH):$(CURDIR)/python/ \
	      $(PYANG) --plugindir $(PLUGIN) --lint -f dpack \
	      --dpack-output-dir "$(BUILDDIR)" $(<)

################################################################################
# Dpack unit tests
################################################################################

utest_bins := $(addprefix $(BUILDDIR)/test/,string array)

# check: $(utest_bins)

# Unit test binaries
$(utest_bins): $(BUILDDIR)/test/%: test/%.c \
                                   $(BUILDDIR)/libdpack.a \
                                   | $(BUILDDIR)/test
	$(CC) -MD -Itest -pie $(CFLAGS) $(LDFLAGS) \
	      $(shell pkg-config --cflags cmocka) \
	      -o $(@) \
	      $(filter %.c %.o %.so,$(^)) \
	      $(shell pkg-config --libs cmocka)

$(BUILDDIR)/test:
	@mkdir -p $(@)

################################################################################
# Dpack libraries
################################################################################

dpack_objs := map array string scalar codec common mpack

# Dpack static library
statlib_dpack_objs := $(addprefix $(BUILDDIR)/,$(addsuffix .o,$(dpack_objs)))

$(BUILDDIR)/libdpack.a: $(statlib_dpack_objs)
	$(AR) rcs $(@) $(^)
$(BUILDDIR)/%.o: src/%.c | $(BUILDDIR)
	$(CC) -MD -Isrc -fpie $(CFLAGS) -o $(@) -c $(<)
$(BUILDDIR)/mpack.o: mpack/.build/amalgamation/src/mpack/mpack.c | $(BUILDDIR)
	$(CC) -MD -Isrc -fpie $(CFLAGS) -o $(@) -c $(<)

# Dpack shared library
solib_dpack_objs := $(addprefix $(BUILDDIR)/,\
                                $(addsuffix _shared.o,$(dpack_objs)))

$(BUILDDIR)/libdpack.so: $(solib_dpack_objs)
	$(CC) -MD -Isrc $(CFLAGS) $(LDFLAGS) \
	      -fpic -Bsymbolic -Bsymbolic-functions -shared \
	      -Wl,-soname,$(notdir $(@)) \
	      -o $(@) $(^)
$(BUILDDIR)/%_shared.o: src/%.c | $(BUILDDIR)
	$(CC) -MD -Isrc -fpic $(CFLAGS) -o $(@) -c $(<)
$(BUILDDIR)/mpack_shared.o: mpack/.build/amalgamation/src/mpack/mpack.c \
                            | $(BUILDDIR)
	$(CC) -MD -Isrc -fpic $(CFLAGS) -o $(@) -c $(<)

$(BUILDDIR):
	@mkdir -p $(@)

-include $(BUILDDIR)/*.d
