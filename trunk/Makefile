RM=rm -f
CC=gcc
CD=cd
MAKE=make
TARGET=descheck
NO_PRINT_DIR=--no-print-directory
CFLAGS= -c -Wall -gdwarf-2
LIB_DIR = /usr/lib
TESTDIR = tests
LDFLAGS = -lelf  -ldwarf -L$(LIB_DIR)/libdwarf.a -lbfd -lopcodes
DEPS=esb.o descheck.o dwarf_names.o call_graph.o log.o


all: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) $<

build $(TARGET):$(DEPS)
	$(CC) $(DEPS) -o $(TARGET) $(LDFLAGS)

test: $(TARGET)
	@$(CD) $(TESTDIR) && $(MAKE) maketests $(NO_PRINT_DIR) && $(MAKE) runtests $(NO_PRINT_DIR)

clean:
	@$(RM) $(DEPS) *~ $(TARGET)

clean-all:clean
	@$(CD) $(TESTDIR) && $(MAKE) cleantests $(NO_PRINT_DIR)
