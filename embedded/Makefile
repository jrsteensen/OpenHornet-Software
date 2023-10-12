MAKEFILES = $(wildcard **/Makefile)
SKETCHES := $(dir $(MAKEFILES))

all: $(SKETCHES)

$(SKETCHES):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: all $(SKETCHES)
