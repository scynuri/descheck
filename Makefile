CC = gcc
LIB_DIR = /usr/lib/
LIBS = -ldwarf -lelf -L$(LIB_DIR)libdwarf.a

all:
	$(CC) -c esb.c
	$(CC) -c dwarf_names.c
	$(CC) -c descheck.c 
	$(CC) esb.o descheck.o dwarf_names.o -o descheck $(LIBS)

test:
	$(CC) -gdwarf-2 test.c -o test


clean:
	rm -f *.o *~ descheck
