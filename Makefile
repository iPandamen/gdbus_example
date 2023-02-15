
CC = gcc

CFLAGS = $(shell pkg-config --cflags glib-2.0 gio-2.0)
LDFLAGS = $(shell pkg-config --libs-only-L glib-2.0 gio-2.0)
LIBS = $(shell pkg-config --libs-only-l glib-2.0 gio-2.0)


.PHONY: all example clean

all: example
	echo CFLAGS: ${CFLAGS}
	echo LDFLAGS: ${LDFALGS}
	echo LIBS: ${LIBS}

	${CC} -g -o gatt_server gatt_server.c ${CFLAGS} ${LDFLAGS} ${LIBS}
	${CC} -g -o test test.c ${CFLAGS} ${LDFLAGS} ${LIBS}
	${CC} -g -o test_advertisement test_advertisement.c ${CFLAGS} ${LDFLAGS} ${LIBS}
	${CC} -g -o gdbus_gatt_battery_service gdbus_gatt_battery_service.c ${CFLAGS} ${LDFLAGS} ${LIBS}
	# example
	${CC} -g -o example_advertisement example_advertisement.c ${CFLAGS} ${LDFLAGS} ${LIBS}

example:
	${MAKE} -C ./example

clean:
	${MAKE} -C ./example clean
	-rm -f gatt_server test gdbus_gatt_battery_service example_advertisement test_advertisement

