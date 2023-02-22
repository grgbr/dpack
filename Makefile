BUILDDIR := $(CURDIR)/build

OPTIM_CFLAGS := -O2 -DNDEBUG
#OPTIM_CFLAGS := -O0
#DEBUG_CFLAGS := -ggdb3
CFLAGS := -Wall -Wextra -Wformat=2 \
          $(DEBUG_CFLAGS) \
          $(OPTIM_CFLAGS) \
          -Isrc -Impack/.build/amalgamation/src/mpack

.PHONY: build
build: $(BUILDDIR)/dpack.a

.PHONY: check
check: $(BUILDDIR)/test-fix_sample

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)

# Sample test binaries
$(BUILDDIR)/test-%: test/test-%.c $(BUILDDIR)/lib%.a $(BUILDDIR)/libdpack.a
	$(CC) -MD -Itest $(CFLAGS) $(LDFLAGS) -o $(@) $(filter %.c %.a %.o,$(^))

# Sample libraries
sample_libs := fix_sample

$(addsuffix .a,$(addprefix $(BUILDDIR)/lib,$(sample_libs))): \
	$(BUILDDIR)/lib%.a: $(BUILDDIR)/%.o
		$(AR) rcs $(@) $(^)

$(addsuffix .o,$(addprefix $(BUILDDIR)/,$(sample_libs))): \
	$(BUILDDIR)/%.o: test/%.c | $(BUILDDIR)
		$(CC) -MD -Itest $(CFLAGS) -o $(@) -c $(<)

# Dpack library
$(BUILDDIR)/libdpack.a: $(BUILDDIR)/dpack.o $(BUILDDIR)/mpack.o
	$(AR) rcs $(@) $(^)
$(BUILDDIR)/dpack.o: src/dpack.c | $(BUILDDIR)
	$(CC) -MD $(CFLAGS) -o $(@) -c $(<)
$(BUILDDIR)/mpack.o: mpack/.build/amalgamation/src/mpack/mpack.c | $(BUILDDIR)
	$(CC) -MD $(CFLAGS) -o $(@) -c $(<)

$(BUILDDIR):
	@mkdir -p $(@)

-include $(BUILDDIR)/*.d
