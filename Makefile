BUILDDIR := $(CURDIR)/build

DPACK_DEBUG := 1

ifeq ($(DPACK_DEBUG),1)
OPTIM_CFLAGS := -O0
DEBUG_CFLAGS := -ggdb3 -DDPACK_DEBUG
else
OPTIM_CFLAGS := -O2 -DNDEBUG -ffast-math
endif

CFLAGS := -Wall -Wextra -Wformat=2 \
          -D_GNU_SOURCE \
          $(DEBUG_CFLAGS) \
          $(OPTIM_CFLAGS) \
          -include mpack-config.h \
          -Iinclude/ \
          -Impack/.build/amalgamation/src/mpack \
          -I$(CURDIR)

.PHONY: build
build: $(BUILDDIR)/libdpack.a

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)

# Sample libraries
sample_apps := fix_sample scalar_array_sample
sample_libs := $(addsuffix .a,$(addprefix $(BUILDDIR)/lib,$(sample_apps)))
sample_objs := $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(sample_apps)))

.PHONY: check
check: $(BUILDDIR)/test-fix_sample

# Sample test binaries
$(BUILDDIR)/test-%: test/test-%.c $(BUILDDIR)/lib%.a $(BUILDDIR)/libdpack.a
	$(CC) -MD -Itest $(CFLAGS) $(LDFLAGS) -o $(@) $(filter %.c %.a %.o,$(^))

# Sample test libraries
$(sample_libs): $(BUILDDIR)/lib%.a: $(BUILDDIR)/%.o
	$(AR) rcs $(@) $(^)

# Sample test library objects
$(sample_objs): $(BUILDDIR)/%.o: test/%.c | $(BUILDDIR)
	$(CC) -MD -Itest $(CFLAGS) -o $(@) -c $(<)

# Dpack library
$(BUILDDIR)/libdpack.a: $(addprefix $(BUILDDIR)/, \
                                    array.o stdint.o codec.o common.o mpack.o)
	$(AR) rcs $(@) $(^)
$(BUILDDIR)/%.o: src/%.c | $(BUILDDIR)
	$(CC) -MD -Isrc $(CFLAGS) -o $(@) -c $(<)
$(BUILDDIR)/mpack.o: mpack/.build/amalgamation/src/mpack/mpack.c | $(BUILDDIR)
	$(CC) -MD -Isrc $(CFLAGS) -o $(@) -c $(<)

$(BUILDDIR):
	@mkdir -p $(@)

-include $(BUILDDIR)/*.d
