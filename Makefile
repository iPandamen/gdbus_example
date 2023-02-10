

CC = gcc

CFLAGS = $(shell pkg-config --cflags glib-2.0 gio-2.0)
LDFLAGS = $(shell pkg-config --libs-only-L glib-2.0 gio-2.0)
LIBS = $(shell pkg-config --libs-only-l glib-2.0 gio-2.0)


all:
	echo CFLAGS: ${CFLAGS}
	echo LDFLAGS: ${LDFALGS}
	echo LIBS: ${LIBS}
	${CC} -g -o g_variant g_variant.c ${CFLAGS} ${LDFLAGS} ${LIBS}
	${CC} -g -o hostname_sync hostname_sync.c ${CFLAGS} ${LDFLAGS} ${LIBS}
	${CC} -g -o hostname_async hostname_async.c ${CFLAGS} ${LDFLAGS} ${LIBS}
	${CC} -g -o signal_sync signal_sync.c ${CFLAGS} ${LDFLAGS} ${LIBS}

clean:
	rm g_variant hostname_sync hostname_async signal_sync

