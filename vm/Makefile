include ../common.mk

BIN:=libmivm.a libmivm.so mivm

all: $(BIN)

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
	$(CPP) -o $@ $^ -L . -lmivm -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system -lboost_program_options

clean:
	rm -f *.o *.a *.so mivm

.PHONY: all clean
