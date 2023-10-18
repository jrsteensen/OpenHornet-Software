MAKEFILES = $(wildcard **/Makefile)
SKETCHES := $(dir $(MAKEFILES))
RELEASE_DIR := $(abspath $(CURDIR)/../release)

all: $(SKETCHES)

$(SKETCHES):
	$(MAKE) -C $@ $(MAKECMDGOALS)

prep_release:
	mkdir -p $(RELEASE_DIR)

release: prep_release $(SKETCHES)

clean:
	$(MAKE) -C $(SKETCHES) clean

.PHONY: all $(SKETCHES)
