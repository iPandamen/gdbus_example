
CC ?= gcc

# CFLAGS = $(shell pkg-config --cflags glib-2.0 gio-2.0)
# LDFLAGS = $(shell pkg-config --libs-only-L glib-2.0 gio-2.0)
# LIBS = $(shell pkg-config --libs-only-l glib-2.0 gio-2.0)

C_SRCS = $(wildcard *.c)
C_OBJS = ${C_SRCS:.c=.o}
C_BINS = ${C_OBJS:.o=}

.PHONY:  all before ${C_BINS} after clean

all: before ${C_BINS} after

before:
	@echo CFLAGS: ${CFLAGS}
	@echo LDFLAGS: ${LDFALGS}
	@echo LIBS: ${LIBS}
	@echo C_SRCS: ${C_SRCS}
	@echo C_OBJS: ${C_OBJS}
	@echo C_BINS: ${C_BINS}

after:

${C_BINS}: 
	${CC} -g $@.c -o $@ ${CFLAGS} ${LDFLAGS} ${LIBS}

# ${C_BINS}: ${C_OBJS}
#   ${CC} -g $@.o -o $@ ${CFLAGS} ${LDFLAGS} ${LIBS}

# %.o: %.c
#   ${CC} -g -c $< -o $@ ${CFLAGS} ${LDFLAGS} ${LIBS}

clean:
	-rm -f ${C_OBJS} ${C_BINS}


