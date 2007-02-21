CC = gcc
CFLAGS = -Wall
LIB_DIR = /usr/lib/
LIBS = -ldwarf -lelf -L$(LIB_DIR)libdwarf.a

all:
	$(CC) -c esb.c $(CFLAGS)
	$(CC) -c dwarf_names.c $(CFLAGS)
	$(CC) -c descheck.c $(CFLAGS)
	$(CC) esb.o descheck.o dwarf_names.o -o descheck $(LIBS)

test:
	$(CC) -gdwarf-2 test.c -o test


clean:
	rm -f *.o *~ descheck
