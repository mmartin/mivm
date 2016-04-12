include ../common.mk

BIN:=libmivm.a libmivm.so mivm

all: $(BIN)

mivm.hpp.inc.tmp: $(ROOTDIR)/opcodes.csv
	sed -n 's/ .*//g;1!p' $< > $@

mivm.hpp: mivm.hpp.inc.tmp

mivm.cpp.inc.tmp: $(ROOTDIR)/opcodes.csv
	sed -n '1!p' $< | \
	while IFS=, read name argc; do \
		echo "#define OPC_CODE $$name"; \
		echo "#define OPC_ARGC $$argc"; \
		echo "#define OPC_IP_INC (instructionPtr + OPC_ARGC + 1)"; \
		echo "#define OPC_NAME \"$$name\""; \
		echo "OPC_$$name"; \
		echo "#undef OPC_CODE"; \
		echo "#undef OPC_ARGC"; \
		echo "#undef OPC_IP_INC"; \
		echo "#undef OPC_NAME"; \
	done > $@

mivm.cpp: mivm.cpp.inc.tmp

mivm_static.o: mivm.cpp mivm.hpp
	$(BUILD)
mivm_shared.o: mivm.cpp mivm.hpp
	$(BUILD)

libmivm.a: mivm_static.o
	$(LINK_STATIC)

libmivm.so: CFLAGS+=-fPIC
libmivm.so: mivm_shared.o
	$(LINK_SHARED)

mivm.o: main.cpp mivm.hpp
	$(BUILD)

mivm: mivm.o |libmivm.so
	$(CPP) -o $@ $^ -L . -lmivm #-lboost_program_options

clean:
	rm -f *.o *.a *.so mivm
	rm -f *.inc.tmp

.PHONY: all clean
