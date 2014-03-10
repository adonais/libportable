CC       = $(CROSS_COMPILING)gcc
CFLAGS   = -O2
BITS	 := 32
DFLAGS :=
LD       = $(CROSS_COMPILING)gcc
LDFLAGS  = -Wl,-s
AR       =$(CROSS_COMPILING)ar
CFLAGS   += $(DFLAGS) -Wall -Wno-unused -Wno-format -DNDEBUG -finline-functions -fomit-frame-pointer
CFLAGSEX =
LDLIBS   = -lmhook$(BITS) -municode
MD       = mkdir -p
SRC      = src
DEP      = ../../.dep
X86FLAG  = -msse2 -m32
X64FLAG  = -m64

OBJECTS  = $(DEP)/cpu.o $(DEP)/disasm.o $(DEP)/disasm_x86.o \
	   $(DEP)/mhook.o $(DEP)/misc.o
TESTFILE = mhook-test.exe
TESTOBJ  = $(DEP)/mhook-test.o 
DISTDIR  = ../../Release
OUT      = libmhook$(BITS).a
EXEC     = \
    @echo coming to mhook subdir... \
    $(shell $(MD) $(DISTDIR) 2>/dev/null) \
    $(shell $(MD) $(DEP) 2>/dev/null) \

ifeq ($(BITS),32)
    CFLAGS += $(X86FLAG)
    LDFLAGS += $(X86FLAG)
else
    ifeq ($(BITS),64)
        CFLAGS += $(X64FLAG)
        LDFLAGS += $(X64FLAG)
    endif
endif

$(DISTDIR)/$(OUT)    : $(OBJECTS) 
	$(AR) rcs $@ $^
$(DEP)/cpu.o         : cpu.c cpu.h
	$(call EXEC)
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/disasm.o      : disasm.c disasm.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/disasm_x86.o  : disasm_x86.c disasm_x86.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/mhook.o       : mhook.c mhook.h
	$(CC) -c $< $(CFLAGS) $(CFLAGSEX) -o $@
$(DEP)/misc.o        : misc.c misc.h
	$(CC) -c $< $(CFLAGS) -o $@

test                   : $(DISTDIR)/$(TESTFILE)
$(DISTDIR)/$(TESTFILE) : $(TESTOBJ) $(DISTDIR)/$(OUT)
	$(LD) -o $@ $< -L$(DISTDIR) $(LDLIBS) $(LDFLAGS)	
$(TESTOBJ)	       : mhook-test.c mhook.h
	$(shell $(MD) $(DEP) 2>/dev/null)
	$(CC) -c $< $(CFLAGS) -o $@

.PHONY		       : clean
clean                  : 
	-rm -rf $(DISTDIR) $(DEP)

