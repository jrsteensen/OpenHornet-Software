MAKEFILES = $(wildcard */*/Makefile)
SKETCHES := $(dir $(MAKEFILES))
# The sketch template is compiled as a smoke test but is never shipped as firmware.
RELEASE_SKETCHES := $(filter-out templates/%,$(SKETCHES))
ROOTDIR := $(abspath $(CURDIR)/..)
RELEASE_DIR := $(abspath $(CURDIR)/../release)

export

all: $(SKETCHES)

# Every sketch directory is a phony target. The goal name (all, release, clean)
# is passed straight through, so this one rule serves every top-level target.
$(SKETCHES):
	$(MAKE) -C $@ $(MAKECMDGOALS)

prep_release:
	mkdir -p $(RELEASE_DIR)

release: prep_release $(RELEASE_SKETCHES)

clean: $(SKETCHES)

.PHONY: all release prep_release clean $(SKETCHES)
