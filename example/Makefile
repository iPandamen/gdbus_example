
CC ?= gcc
CFLAGS ?= $(shell pkg-config --cflags glib-2.0 gio-2.0)
LDFLAGS ?= $(shell pkg-config --libs-only-L glib-2.0 gio-2.0)
LIBS ?= $(shell pkg-config --libs-only-l glib-2.0 gio-2.0)

C_SRCS = $(wildcard *.c)
C_OBJS = ${C_SRCS:.c=.o}
C_BINS = ${C_OBJS:.o=}

ifdef BUILD_DIR
example_BUILD_DIR=${BUILD_DIR}/example
else 
example_BUILD_DIR=${CURDIR}/build
endif

.PHONY:  all before ${C_BINS} after clean

all: before ${C_BINS} after

before:
	if [ ! -d ${example_BUILD_DIR} ]; then mkdir ${example_BUILD_DIR}; fi

after:
	cp ${C_BINS} ${example_BUILD_DIR}

${C_BINS}: ${C_OBJS}
	${CC} -g $@.o -o $@ ${CFLAGS} ${LDFLAGS} ${LIBS}

%.o: %.c
	${CC} -g -c $< -o $@ ${CFLAGS} ${LDFLAGS} ${LIBS}

clean:
	${RM} ${C_OBJS} ${C_BINS}
	${RM} -r ${example_BUILD_DIR}


