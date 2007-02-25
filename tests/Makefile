RM=rm -f
CC=gcc
ECHO=echo
ECHO_NO_NEWLINE=echo -n
CFLAGS= -c -Wall -O -gdwarf-2
LDFLAGS=
BIN_TESTS=bintest_00 bintest_01

maketests all: build

.c.o:
	$(CC) $(CFLAGS) $<

build:$(BIN_TESTS)
runtests run:build
	@for TESTCASE in $(BIN_TESTS); \
	do \
		$(ECHO_NO_NEWLINE) Running ./$$TESTCASE; \
		./$$TESTCASE; \
		if [ $$? != 0 ]; then \
			$(ECHO) "\t..... Failure"; \
		else \
			$(ECHO) "\t..... Success"; \
		fi; \
		../descheck ./$$TESTCASE; \
	done
	@true


bintest_00:test_00_simple.o
	$(CC) $(LDFLAGS) test_00_simple.o -o $@

bintest_01:test_01_one_func.o
	$(CC) $(LDFLAGS) test_01_one_func.o -o $@

cleantests clean:
	@$(RM) *.o bintest*  *~
