MAKEFILES = $(wildcard **/Makefile)
SKETCHES := $(dir $(MAKEFILES))

all: $(SKETCHES)

$(SKETCHES):
	$(MAKE) -C $@ $(MAKECMDGOALS)

clean:
	$(MAKE) -C $(SKETCHES) clean

.PHONY: all $(SKETCHES)
