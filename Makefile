CC       = $(CROSS_COMPILING)gcc -c 
CFLAGS   = -O2
AR       = $(CROSS_COMPILING)ar
LD       = $(CROSS_COMPILING)gcc -o
BITS	 := 64
DFLAGS   :=
MSCRT    := -lmsvcrt
LTO      :=
DLL_MAIN_STDCALL_NAME =
LIBPORTABLE_STATIC :=

ifeq ($(CROSS_COMPILING),)
YASM     := msrt/yasm
else
YASM     := yasm
endif

ifeq ($(BITS),64)
CROSS_COMPILING       := x86_64-w64-mingw32-
DLL_MAIN_STDCALL_NAME = _DllMainCRTStartup
else
DLL_MAIN_STDCALL_NAME = __DllMainCRTStartup@12
endif

CFLAGS   += $(DFLAGS) -Wall -Wno-unused -ffunction-sections -fdata-sections -finline-functions \
            -fomit-frame-pointer -DWINVER=0x0600 -D_WIN32_IE=0x0601 -DWIN32_LEAN_AND_MEAN -DNDEBUG

MD       = mkdir -p
CP       = cp
SRC      = src
SUB_DIR  = $(SRC)/minhook
SUBMK    = $(MAKE) -C $(SUB_DIR)
DEP      = .dep
X86FLAG  = -D_WIN32 -m32
X64FLAG  =  -D_WIN64 -m64
OBJECTS  = $(DEP)/umpv.o $(DEP)/inipara.o $(DEP)/chk.o $(DEP)/internal_exp.o \
           $(DEP)/bosskey.o $(DEP)/oneinst.o $(DEP)/confpath.o
MIN_INC  = $(SRC)/minhook/include
CFLAGS   += -I$(MIN_INC) -I$(SRC)
DISTDIR  = Release
OUT1     = $(DISTDIR)/libminhook$(BITS).a

EXEC     = \
    @echo Starting Compile... \
    $(shell $(MD) $(DISTDIR) 2>/dev/null) \
    $(shell $(MD) $(DEP) 2>/dev/null) \

ifeq ($(BITS),32)
    CFLAGS  += $(X86FLAG)
    LDFLAGS := -m32
    ASMFLAGS = -fwin32 -DWINDOWS -D__i386__ -DWIN32 -Worphan-labels
    RCFLAGS += -F pe-i386
else
    ifeq ($(BITS),64)
        CFLAGS	+= $(X64FLAG)
        LDFLAGS := -m64
	ASMFLAGS = -fwin64 -DWINDOWS -D__x86_64__ -Worphan-labels
	RCFLAGS += -F pe-x86-64
    endif
endif

ifeq ($(LIBPORTABLE_STATIC),1)
CFLAGS   += -DLIBPORTABLE_STATIC
LD       =$(AR) rcs
OUT      = $(DISTDIR)/umpv_s.lib
LDFLAGS  =
OBJS     = $(DEP)/*.o
else
OUT      = $(DISTDIR)/umpv.dll
DEPLIBS  = -lminhook$(BITS)
LDLIBS   = -lshlwapi -lshell32 -lcomctl32 -luser32 -lkernel32
LDFLAGS  += -L$(DISTDIR) $(DEPLIBS) \
            -nostdlib --entry=$(DLL_MAIN_STDCALL_NAME) -Wl,--gc-sections -Wl,-s
DLLFLAGS += -fPIC -shared -Wl,--out-implib,$(DISTDIR)/libumpv.dll.a
RC       = $(CROSS_COMPILING)windres
RCFLAGS  += --define UNICODE -J rc -O coff
OBJECTS  += $(DEP)/resource.o
OBJS     = $(OBJECTS)
endif

ifeq ($(LTO), 1)
AR       := $(filter-out ar,$(AR )) gcc-ar
CFLAGS   := $(filter-out -O2,$(CFLAGS)) -D__LTO__ -Os -fuse-linker-plugin -flto
#warning is only during the LTO link, debug(--verbose --save-temps )
DLLFLAGS := $(filter-out -fPIC,$(DLLFLAGS)) -fuse-linker-plugin -flto
else
CFLAGS   += -D_LOGDEBUG
endif

all                   : $(OUT1) $(OUT)
$(OUT1)               : $(SUB_DIR)/Makefile $(SUB_DIR)/src/*.c $(SUB_DIR)/src/*.h
	$(call SUBMK)
$(OUT)                : $(OBJECTS) $(OUT1)
	$(LD) $@ $(OBJS) $(DLLFLAGS) $(LDFLAGS) $(LDLIBS)
$(DEP)/umpv.o         : $(SRC)/umpv.c $(SRC)/umpv.h $(SRC)/inipara.h $(SRC)/internal_crt.h $(SRC)/bosskey.h $(SRC)/oneinst.h $(SRC)/confpath.h $(SRC)/resource.rc
	$(call EXEC)
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/inipara.o      : $(SRC)/inipara.c $(SRC)/inipara.h $(SRC)/internal_crt.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/internal_exp.o : $(SRC)/internal_exp.c $(SRC)/internal_exp.h $(SRC)/inipara.h $(SRC)/internal_crt.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/bosskey.o      : $(SRC)/bosskey.c $(SRC)/bosskey.h $(SRC)/inipara.h $(SRC)/internal_crt.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/oneinst.o      : $(SRC)/oneinst.c $(SRC)/oneinst.h $(SRC)/inipara.h $(SRC)/internal_crt.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/confpath.o     : $(SRC)/confpath.c $(SRC)/confpath.h $(SRC)/inipara.h $(SRC)/internal_crt.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/chk.o          : $(SRC)/chk.s
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/resource.o     : $(SRC)/resource.rc
	$(RC) -i $< $(RCFLAGS) -o $@

.PHONY                : clean
clean                 : 
	-rm -rf $(DISTDIR) $(DEP)

