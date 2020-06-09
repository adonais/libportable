CC       := $(CROSS_COMPILING)gcc
CFLAGS   = -O2
DFLAGS   ?=
LD       = $(CC) -o
AR       = $(CROSS_COMPILING)ar
LDFLAGS  = -Wl,-s
CFLAGS   += $(DFLAGS) -Wall -Wno-unused -Wno-format -DNDEBUG \
            -finline-functions -DWINVER=0x0501 -D_WIN32_IE=0x0601 -mavx
LTO      ?=
OUT      = libminhook$(BITS).a
USE_GCC  =

ifeq ($(findstring clang,$(CC)),)
BUILD = $(shell file `which $(CC)` 2>/dev/null | grep x86-64)
USE_GCC  = 1
else
AR = llvm-ar.exe
BUILD = $(shell file `which $(CC)` 2>/dev/null | grep x86-64)
endif

ifneq ($(CROSS_COMPILING),)
ifneq ($(BITS),32)
BITS	 ?= 64
endif
else
ifeq ($(BUILD),)
BITS	 ?= 32
else
BITS	 ?= 64
endif
endif

ifeq ($(findstring clang,$(CC)),clang)
CFLAGS   += -Wno-ignored-attributes -Wno-unknown-attributes -Wno-deprecated-declarations
ifneq (,$(filter $(DFLAGS),--target=x86_64-pc-windows --target=x86_64-pc-windows-msvc --target=i686-pc-windows --target=i686-pc-windows-msvc))
CFLAGS   += -D_CRT_SECURE_NO_WARNINGS -DVC12_CRT
OUT      = minhook$(BITS).lib
endif   #target x86_64 or i686
ifneq (,$(findstring i686,$(DFLAGS)))
BITS	 = 32
else 
BITS	 = 64
endif
endif   # clang

MD       = mkdir -p
SRC      = src
DEP      = ../../.dep
X86FLAG  = -m32 -march=i686
X64FLAG  = -m64
MIN_INC  = include
OBJECTS  = $(DEP)/buffer.o $(DEP)/hde$(BITS).o $(DEP)/hook.o $(DEP)/trampoline.o
DISTDIR  = ../../Release
CFLAGS   += -I$(MIN_INC) -I$(SRC)

EXEC     = \
    @echo coming to minhook subdir... \
    $(shell $(MD) $(DISTDIR) 2>/dev/null) \
    $(shell $(MD) $(DEP) 2>/dev/null) \

ifeq ($(BITS),32)
    CFLAGS += $(X86FLAG)
    LDFLAGS += $(X86FLAG)
else ifeq ($(BITS),64)
    CFLAGS += $(X64FLAG)
    LDFLAGS += $(X64FLAG)
endif

ifeq ($(USE_GCC),1)
ifeq ($(LTO), 1)
AR       := $(filter-out ar,$(AR )) gcc-ar
CFLAGS   := $(filter-out -O2,$(CFLAGS)) -D__LTO__ -Os -fno-use-linker-plugin -flto
endif
endif

$(DISTDIR)/$(OUT)      : $(OBJECTS) 
	$(AR) rcs $@ $^
$(DEP)/buffer.o        : $(SRC)/buffer.c $(SRC)/buffer.h
	$(call EXEC)
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/hde$(BITS).o    : $(SRC)/hde/hde$(BITS).c $(SRC)/hde/hde$(BITS).h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/hook.o          : $(SRC)/hook.c $(MIN_INC)/MinHook.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/trampoline.o    : $(SRC)/trampoline.c $(SRC)/trampoline.h
	$(CC) -c $< $(CFLAGS) -o $@

.PHONY		       : clean
clean                  : 
	-rm -rf $(DISTDIR) $(DEP)

