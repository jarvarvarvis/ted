# ted version
VERSION = 0.1

# TODO: custom definitions
DEFINITIONS = 

# ncurses
NCURSESLIBS = -lncurses

# paths
PREFIX = /usr/local/
MANPREFIX = ${PREFIX}/share/man

# include and libs
INCS = 
LIBS = ${NCURSESLIBS}

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -DVERSION=\"${VERSION}\"
CFLAGS   = -ggdb -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os ${INCS} ${DEFINITIONS} ${CPPFLAGS}
LDFLAGS  = ${LIBS}

# compiler and linker
CC = gcc
