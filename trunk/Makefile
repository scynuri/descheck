RM=rm -f
CC=gcc
TARGET=descheck
CFLAGS= -c -Wall
LIB_DIR = /usr/lib
LDFLAGS = -lelf  -ldwarf -L$(LIB_DIR)/libdwarf.a
DEPS=esb.o descheck.o dwarf_names.o


all: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) $<

build $(TARGET):$(DEPS)
	$(CC) $(DEPS) -o $(TARGET) $(LDFLAGS)

test:
	$(CC) -gdwarf-2 test.c -o test

clean:
	@$(RM) $(DEPS) *~ $(TARGET)
