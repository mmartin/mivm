include common.mk

all: mivm #mivm-ir

mivm-ir:
	$(MAKE) -C ir

mivm:
	$(MAKE) -C vm

check: all
	$(MAKE) -C tests

clean:
	$(MAKE) -C ir clean
	$(MAKE) -C vm clean
	$(MAKE) -C tests clean

.PHONY: all mivm mivm-ir check clean
