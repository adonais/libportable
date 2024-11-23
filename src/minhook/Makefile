CC       = $(CROSS_COMPILING)gcc
CFLAGS   = -O2
BITS	 := 64
DFLAGS :=
LD       = $(CROSS_COMPILING)gcc
LDFLAGS  = -Wl,-s
AR       =$(CROSS_COMPILING)ar
CFLAGS   += $(DFLAGS) -Wall -Wno-unused -ffunction-sections -fdata-sections -fomit-frame-pointer \
            -finline-functions -DWINVER=0x0600 -D_WIN32_IE=0x0601 -DWIN32_LEAN_AND_MEAN -DNDEBUG

LTO      :=

MD       = mkdir -p
SRC      = src
DEP      = ../../.dep
X86FLAG  = -m32 -march=i686
X64FLAG  = -m64
MIN_INC  = include
OBJECTS  = $(DEP)/buffer.o $(DEP)/hde$(BITS).o $(DEP)/hook.o $(DEP)/trampoline.o
DISTDIR  = ../../Release
CFLAGS   += -I$(MIN_INC) -I$(SRC)
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
        CROSS_COMPILING := x86_64-w64-mingw32-
        CFLAGS += $(X64FLAG)
        LDFLAGS += $(X64FLAG)
    endif
endif

ifeq ($(LTO), 1)
AR       := $(filter-out ar,$(AR )) gcc-ar
CFLAGS   := $(filter-out -O2,$(CFLAGS)) -D__LTO__ -Os -fuse-linker-plugin -flto
endif

$(DISTDIR)/$(OUT)      : $(OBJECTS) 
	$(AR) rcs $@ $^
$(DEP)/buffer.o        : $(SRC)/buffer.c $(SRC)/buffer.h
	$(call EXEC)
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/hde$(BITS).o    : $(SRC)/HDE/hde$(BITS).c $(SRC)/HDE/hde$(BITS).h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/hook.o          : $(SRC)/hook.c $(MIN_INC)/MinHook.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/trampoline.o    : $(SRC)/trampoline.c $(SRC)/trampoline.h
	$(CC) -c $< $(CFLAGS) -o $@

.PHONY		       : clean
clean                  : 
	-rm -rf $(DISTDIR) $(DEP)

