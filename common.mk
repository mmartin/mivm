CPP:=g++
CFLAGS:=-std=c++11 -Wall -Werror #-Wextra
CFLAGS+=-g -DMIVM_DEBUG

BUILD=$(CPP) $(CFLAGS) -c $< -o $@
LINK_STATIC=ar rcs $@ $^
LINK_SHARED=$(CPP) -shared -o $@ $^

ROOTDIR:=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
VMDIR:=$(ROOTDIR)vm
IRDIR:=$(ROOTDIR)ir
