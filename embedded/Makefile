MAKEFILES = $(wildcard */*/Makefile)
SKETCHES := $(dir $(MAKEFILES))
ROOTDIR := $(abspath $(CURDIR)/..)
RELEASE_DIR := $(abspath $(CURDIR)/../release)

export

all: $(SKETCHES)

$(SKETCHES):
	$(MAKE) -C $@ $(MAKECMDGOALS)

prep_release:
	mkdir -p $(RELEASE_DIR)

release: prep_release $(SKETCHES)

clean:
	$(MAKE) -C $(SKETCHES) clean

.PHONY: all $(SKETCHES)
