<<<<<<< .mine
RM=rm -f
CC=gcc
TARGET=descheck
CFLAGS= -c -Wall
LIB_DIR = /usr/lib
LDFLAGS = -lelf  -ldwarf -L$(LIB_DIR)/libdwarf.a
DEPS=esb.o descheck.o dwarf_names.o
=======
CC = gcc
CFLAGS = -Wall
LIB_DIR = /usr/lib/
LIBS = -ldwarf -lelf -L$(LIB_DIR)libdwarf.a
>>>>>>> .r15

<<<<<<< .mine
all: build
=======
all:
	$(CC) -c esb.c $(CFLAGS)
	$(CC) -c dwarf_names.c $(CFLAGS)
	$(CC) -c descheck.c $(CFLAGS)
	$(CC) esb.o descheck.o dwarf_names.o -o descheck $(LIBS)
>>>>>>> .r15

.c.o:
	$(CC) $(CFLAGS) $<

build:$(DEPS)
	$(CC) $(DEPS) -o $(TARGET) $(LDFLAGS)

test:
	$(CC) -gdwarf-2 test.c -o test

clean:
	@$(RM) $(DEPS) *~ $(TARGET)
