include common.mk

all: mivm

mivm:
	$(MAKE) -C vm

check: all
	$(MAKE) -C tests

clean:
	$(MAKE) -C vm clean
	$(MAKE) -C tests clean

.PHONY: all mivm check clean
