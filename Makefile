CC       = $(CROSS_COMPILING)gcc -c 
CFLAGS   = -O2
AR       = $(CROSS_COMPILING)ar
LD       = $(CROSS_COMPILING)gcc -o
BITS	 := 32
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

CFLAGS   += $(DFLAGS) -D_LOGDEBUG -Wall -Wno-unused -Wno-format -Wno-int-to-pointer-cast \
            -Wno-unknown-pragmas -finline-functions -DINITGUID \
            -DWINVER=0x0501 -D_WIN32_IE=0x0601  -mavx

MD       = mkdir -p
CP       = cp
SRC      = src
SUB_DIR  = $(SRC)/minhook
SUBMK    = $(MAKE) -C $(SUB_DIR)
DEP      = .dep
X86FLAG  = -D_WIN32 -m32
X64FLAG  =  -D_WIN64 -m64
OBJECTS  = $(DEP)/portable.o $(DEP)/inipara.o $(DEP)/ice_error.o  $(DEP)/safe_ex.o \
           $(DEP)/inject.o $(DEP)/bosskey.o $(DEP)/new_process.o $(DEP)/set_env.o\
           $(DEP)/cpu_info.o $(DEP)/balance.o $(DEP)/load_module.o $(DEP)/win_registry.o
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
OUT      = $(DISTDIR)/portable$(BITS)_s.lib
TETE     = $(DISTDIR)/tmemutil_s.lib
LDFLAGS  =
OBJS     = $(DEP)/*.o
else
OUT      = $(DISTDIR)/portable$(BITS).dll
TETE     = $(DISTDIR)/tmemutil.dll
DEPLIBS  = -Wl,-static -lminhook$(BITS)
LDLIBS   = -lshlwapi -lshell32 -lole32 $(MSCRT)
LDFLAGS  += -L$(DISTDIR) -nostdlib $(DEPLIBS) -lmingw32 -lmingwex -lgcc -lkernel32 -luser32 \
            -static-libgcc --entry=$(DLL_MAIN_STDCALL_NAME) -Wl,-s
DLLFLAGS += -fPIC -shared -Wl,--out-implib,$(DISTDIR)/libportable$(BITS).dll.a
RC       = $(CROSS_COMPILING)windres
RCFLAGS  = --define UNICODE -J rc -O coff
ifeq ($(BITS),32)
RCFLAGS  += -F pe-i386
endif
OBJECTS  += $(DEP)/resource.o
OBJS     = $(OBJECTS)
endif

ifeq ($(LTO), 1)
AR       := $(filter-out ar,$(AR )) gcc-ar
CFLAGS   := $(filter-out -O2,$(CFLAGS)) -D__LTO__ -Os -fuse-linker-plugin -flto
#warning is only during the LTO link, debug(--verbose --save-temps )
DLLFLAGS := $(filter-out -fPIC,$(DLLFLAGS)) -fuse-linker-plugin -flto
endif

all                   : $(OUT1) $(OUT)
$(OUT1)               : $(SUB_DIR)/Makefile
	$(call SUBMK)
$(OUT)                : $(OBJECTS) $(OUT1)
	$(LD) $@ $(OBJS) $(DLLFLAGS) $(LDFLAGS) $(LDLIBS)
	-$(CP) $(OUT) $(TETE) 2>/dev/null
$(DEP)/portable.o     : $(SRC)/portable.c $(SRC)/portable.h $(SRC)/inipara.h
	$(call EXEC)
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/inipara.o      : $(SRC)/inipara.c $(SRC)/inipara.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/inject.o       : $(SRC)/inject.c $(SRC)/inject.h $(SRC)/winapis.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/safe_ex.o      : $(SRC)/safe_ex.c $(SRC)/safe_ex.h $(SRC)/winapis.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/ice_error.o    : $(SRC)/ice_error.c $(SRC)/ice_error.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/bosskey.o      : $(SRC)/bosskey.c $(SRC)/bosskey.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/new_process.o  : $(SRC)/new_process.c $(SRC)/new_process.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/cpu_info.o     : $(SRC)/cpu_info.c $(SRC)/cpu_info.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/balance.o      : $(SRC)/balance.c $(SRC)/balance.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/load_module.o  : $(SRC)/load_module.c $(SRC)/load_module.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/win_registry.o : $(SRC)/win_registry.c $(SRC)/win_registry.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/set_env.o      : $(SRC)/set_env.c $(SRC)/set_env.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/resource.o     : $(SRC)/resource.rc
	$(RC) -i $< $(RCFLAGS) -o $@

.PHONY                : clean
clean                 : 
	-rm -rf $(DISTDIR) $(DEP)

