# ted - the suckless-style text editor inspired by vim

include config.mk

SRC = ted.c
OBJ = ${SRC:.c=.o}

all: options ted

options:
	@echo ted build options:
	@echo "VERSION = ${VERSION}"
	@echo "CFLAGS  = ${CFLAGS}"
	@echo "LDFLAGS = ${LDFLAGS}"
	@echo "CC      = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

ted: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f ted ${OBJ}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ted ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/ted

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/ted


.PHONY: all options clean install uninstall
