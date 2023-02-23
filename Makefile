
ifeq (${ARCH}, arm)
CC = arm-linux-gnueabihf-gcc
CFLAGS = $(shell pkg-config --cflags glib-2.0 gio-2.0)
LDFLAGS = -L/home/asd/WorkSpace/nfs_rootfs/rootfs/usr/lib -lffi -lpcre -lz -lgmodule-2.0
LIBS = $(shell pkg-config --libs-only-l glib-2.0 gio-2.0)
BUILD_DIR=${CURDIR}/build_for_arm/
else 
CC = gcc
CFLAGS = $(shell pkg-config --cflags glib-2.0 gio-2.0)
LDFLAGS = $(shell pkg-config --libs-only-L glib-2.0 gio-2.0)
LIBS = $(shell pkg-config --libs-only-l glib-2.0 gio-2.0)
BUILD_DIR=${CURDIR}/build
endif 


export CC
export CFLAGS
export LDFLAGS
export LIBS
export BUILD_DIR

.PHONY: all before tests example after clean

all: before build after

before:
	if [ ! -d ${BUILD_DIR} ]; then mkdir ${BUILD_DIR}; fi

after:

build: tests example

tests:
	${MAKE} -C ./tests

example:
	${MAKE} -C ./example

clean:
	${MAKE} -C ./tests clean
	${MAKE} -C ./example clean
	${RM} -r ${BUILD_DIR}

