CC       = $(CROSS_COMPILING)gcc
CFLAGS   = -O2
BITS	 := 32
DFLAGS :=
LD       = $(CROSS_COMPILING)gcc
LDFLAGS  = -Wl,-s
AR       =$(CROSS_COMPILING)ar
CFLAGS   += $(DFLAGS) -Wall -Wno-unused -Wno-format -DNDEBUG -fdata-sections -finline-functions -fomit-frame-pointer -D_LOGDEBUG
CFLAGSEX =

MD       = mkdir -p
SRC      = src
DEP      = ../../.dep
X86FLAG  = -m32
X64FLAG  = -m64
MIN_INC  = include
OBJECTS  = $(DEP)/buffer.o $(DEP)/hde$(BITS).o $(DEP)/hook.o $(DEP)/trampoline.o
DISTDIR  = ../../Release
CFLAGS   += -I$(MIN_INC)
OUT      = libminhook$(BITS).a
EXEC     = \
    @echo coming to minhook subdir... \
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

$(DISTDIR)/$(OUT)      : $(OBJECTS) 
	$(AR) rcs $@ $^
$(DEP)/buffer.o        : $(SRC)/buffer.c $(SRC)/buffer.h
	$(call EXEC)
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/hde$(BITS).o    : $(SRC)/HDE/hde$(BITS).c $(SRC)/HDE/hde$(BITS).h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/hook.o          : $(SRC)/hook.c $(MIN_INC)/MinHook.h
	$(CC) -c $< $(CFLAGS) $(CFLAGSEX) -o $@
$(DEP)/trampoline.o    : $(SRC)/trampoline.c $(SRC)/trampoline.h
	$(CC) -c $< $(CFLAGS) -o $@

.PHONY		       : clean
clean                  : 
	-rm -rf $(DISTDIR) $(DEP)

