include ../common.mk
CFLAGS+=-I $(VMDIR)

BIN:=link_static link_shared basic_opcodes

all: $(BIN)
	./link_static
	LD_LIBRARY_PATH=$(VMDIR) ./link_shared
	@echo linking OK
	
	LD_LIBRARY_PATH=$(VMDIR) ./basic_opcodes --log_level=test_suite
	@echo basic opcodes OK

link_test_static.o: link_test.cpp $(VMDIR)/mivm.hpp
	$(BUILD)
link_static: link_test_static.o $(VMDIR)/libmivm.a
	$(CPP) -o $@ $^ -static

link_test_shared.o: link_test.cpp $(VMDIR)/mivm.hpp
	$(BUILD)
link_shared: link_test_shared.o
	$(CPP) -o $@ $^ -L $(VMDIR) -lmivm

basic_opcodes.o: basic_opcodes.cpp $(VMDIR)/mivm.hpp
	$(BUILD)
basic_opcodes: basic_opcodes.o
	$(CPP) -o $@ $^ -L $(VMDIR) -lmivm -lboost_unit_test_framework

clean:
	rm -f *.o $(BIN)

.PHONY: all clean
