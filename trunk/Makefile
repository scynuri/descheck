all: build



TARGET:=objdump
LIBDIR:=./libs

GCC=gcc
ECHO=echo
RM=rm -f


# ############################################################
#			CFLAGS
# ############################################################
CFLAGS_DEBUG:=-g -O2
CFLAGS_DEFINES:=-DHAVE_CONFIG_H -DLOCALEDIR="\"/usr/local/share/locale\"" -Dbin_dummy_emulation=bin_vanilla_emulation -D_GNU_SOURCE
CFLAGS_INCLUDES:= -I.  -I../bfd -I../include -I../intl
CFLAGS_WARNINGS:= -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Werror 

CFLAGS_COMPILE:= $(CFLAGS_DEFINES) $(CFLAGS_INCLUDES) $(CFLAGS_WARNINGS) $(CFLAGS_DEBUG)
CFLAGS_LINK:= $(CFLAGS_DEBUG) $(CFLAGS_WARNINGS)





# ############################################################
#			DEPS
# ############################################################
OBJDUMP_BIN_DEPS:=objdump.o dwarf.o budemang.o prdbg.o rddbg.o debug.o stabs.o ieee.o rdcoff.o bucomm.o version.o filemode.o
OBJDUMP_LIB_DEPS:=$(LIBDIR)/libopcodes.a $(LIBDIR)/libbfd.a $(LIBDIR)/libiberty.a

DESCHECK_BIN_DEPS:=descheck.o
DESCHECK_LIB_DEPS:=

BIN_DEPS:=$(DESCHECK_BIN_DEPS) $(OBJDUMP_BIN_DEPS)
LIB_DEPS:=$(DESCHECK_LIB_DEPS) $(OBJDUMP_LIB_DEPS)

DEPS:=$(BIN_DEPS) $(LIB_DEPS)




# ############################################################
#			MAKE directives
# ############################################################
.c.o:
	$(GCC) $(CFLAGS_COMPILE)      -c $<

build: $(BIN_DEPS)
	@$(ECHO) " ================================== "
	@$(ECHO) " BUILT all dependencies ... LINKING "
	$(GCC) $(CFLAGS_LINK) -o $(TARGET) $(DEPS)
	@$(ECHO) "             ALL DONE               "
	@$(ECHO) " ================================== "

clean:
	$(RM) $(DESCHECK_BIN_DEPS) $(TARGET)
	
clean-all:
	$(RM) $(BIN_DEPS) $(TARGET)
