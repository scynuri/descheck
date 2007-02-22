RM=rm -f
CC=gcc
ECHO=echo
ECHO_NO_NEWLINE=echo -n
CFLAGS= -c -Wall -gdwarf-2
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
		fi \
	done


bintest_00:test_00_simple.o
	$(CC) $(LDFLAGS) test_00_simple.o -o $@

bintest_01:
	@touch $@ && chmod -c 777 $@

cleantests clean:
	@$(RM) *.o bintest*  *~
