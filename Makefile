CC       := $(CROSS_COMPILING)gcc 
CFLAGS   = -O2
LD       = $(CC) -o
MSCRT    := -mcrtdll=ucrt
MSVC     = 0
DFLAGS   ?=
LTO      ?=
AR       ?= ar
RC       ?= windres -i

RC_TAR32 = -F pe-i386
RC_TAR64 = -F pe-x86-64
RCFLAGS  = --define UNICODE -J rc -O coff
USE_GCC  =

ifeq ($(findstring clang,$(CC)),)
USE_GCC  = 1
endif

BUILD    = $(shell file `which $(CC) 2>/dev/null` 2>/dev/null | grep x86-64)
T_CC     = $(findstring x86_64-w64-mingw32-clang,$(CC))

ifeq ($(BUILD),)
BITS	 = 32
else
BITS	 = 64
endif

ifneq ($(CROSS_COMPILING),)
ifneq ($(BITS),32)
BITS	 = 64
endif
endif

CFLAGS   += $(DFLAGS) -Wall -Wno-unused -Wno-format -Wno-int-to-pointer-cast \
            -Wno-unknown-pragmas -finline-functions -DINITGUID \
            -DWINVER=0x0600 -D_WIN32_IE=0x0601 -D_UCRT

ifeq ($(findstring clang,$(CC)),clang)
CXX      = $(CC)++
CFLAGS   += -Wno-ignored-attributes -Wno-unknown-attributes -Wno-deprecated-declarations
ifneq (,$(filter $(DFLAGS),--target=x86_64-pc-windows --target=x86_64-pc-windows-msvc --target=i686-pc-windows --target=i686-pc-windows-msvc)) 
CFLAGS   += -D_CRT_SECURE_NO_WARNINGS -DVC12_CRT
MSCRT    =
MSVC     = 1
LDFLAGS := $(filter-out -s,$(LDFLAGS)) -fuse-ld=lld
endif   #target x86_64 or i686
ifneq (,$(findstring i686,$(DFLAGS)))
BITS	 = 32
else 
BITS	 = 64
endif
endif   # clang

#llvm-rc not support clang-gnu-ld Toolchains,we use windres
ifeq ($(USE_GCC),)
AR       = llvm-ar.exe
ifneq ($(filter $(MSVC)-$(T_CC),1-$(T_CC) 0-$(CC)),)
RC       = llvm-rc.exe
RCFLAGS  = -nologo -D "_UNICODE" -D "UNICODE" -FO
else
USE_RES  = 1
endif
else
USE_RES  = 1
endif

MD       = mkdir -p
CP       = cp
SRC      = src
SUB_DIR  = $(SRC)/minhook
SUBMK    = $(MAKE) -C $(SUB_DIR)
DEP      = .dep
X86FLAG  = -D_WIN32 -m32
X64FLAG  =  -D_WIN64 -m64
OBJECTS  = $(DEP)/portable.o $(DEP)/general.o $(DEP)/ice_error.o  $(DEP)/safe_ex.o \
           $(DEP)/inject.o $(DEP)/bosskey.o $(DEP)/new_process.o $(DEP)/set_env.o\
           $(DEP)/cpu_info.o $(DEP)/balance.o $(DEP)/win_registry.o $(DEP)/on_tabs.o \
           $(DEP)/lz4.o $(DEP)/cjson.o $(DEP)/json_paser.o $(DEP)/ini_parser.o
MIN_INC  = $(SRC)/minhook/include
CFLAGS   += -fvisibility=hidden -DCJSON_HIDE_SYMBOLS -I$(MIN_INC) -I$(SRC)
DISTDIR  = Release
OUT1     = $(DISTDIR)/libminhook$(BITS).a
ifeq ($(MSVC),1)	
OBJECTS  += $(DEP)/on_tabs.o
endif	

EXEC     = \
    @echo Starting Compile... \
    $(shell $(MD) $(DISTDIR) 2>/dev/null) \
    $(shell $(MD) $(DEP) 2>/dev/null) \

ifeq ($(BITS),32)
    CFLAGS  += $(X86FLAG)
    LDFLAGS += -m32
    ASMFLAGS = -fwin32 -DWINDOWS -D__i386__ -DWIN32 -Worphan-labels
else ifeq ($(BITS),64)
    CFLAGS	+= $(X64FLAG)
    LDFLAGS += -m64
    ASMFLAGS = -fwin64 -DWINDOWS -D__x86_64__ -Worphan-labels
endif

ifeq ($(LIBPORTABLE_STATIC),1)
CFLAGS   += -DLIBPORTABLE_STATIC
LD       = $(AR) rcs
OUT      = $(DISTDIR)/portable$(BITS)_s.lib
TETE     = $(DISTDIR)/tmemutil_s.lib
LDFLAGS  =
OBJS     = $(DEP)/*.o
else  # not build LIBPORTABLE_STATIC
OUT      = $(DISTDIR)/portable$(BITS).dll
TETE     = $(DISTDIR)/tmemutil.dll
DEPLIBS  = -lminhook$(BITS)
LDLIBS   = -lshlwapi -lshell32 -lole32
LDFLAGS  += -L$(DISTDIR) $(DEPLIBS) -lkernel32 -luser32 -s
DLLFLAGS += -fPIC -shared

ifdef USE_RES
ifeq ($(BITS),32)
RCFLAGS  += $(RC_TAR32) -o
else
RCFLAGS  += $(RC_TAR64) -o
endif
endif  # USE_GCC

OBJECTS  += $(DEP)/resource.o
OBJS     = $(OBJECTS)

ifeq ($(findstring clang,$(CC)), clang)
LDFLAGS += -static -Wno-unused-command-line-argument -v
else ifeq ($(USE_GCC),1)
LDLIBS  += $(MSCRT)
LDFLAGS += -static-libgcc -Wl,--out-implib,$(DISTDIR)/libportable$(BITS).dll.a
ifeq ($(LTO), 1)
AR       := $(filter-out ar,$(AR )) gcc-ar
CFLAGS   := $(filter-out -O2,$(CFLAGS)) -D__LTO__ -Os -flto
#warning is only during the LTO link, debug(--verbose --save-temps )
DLLFLAGS := $(filter-out -fPIC,$(DLLFLAGS)) -flto
endif
else
$(error "unknown compiler")
endif
endif  # LIBPORTABLE_STATIC end

all                   : $(OUT1) $(OUT)
$(OUT1)               : $(SUB_DIR)/Makefile
	$(call SUBMK)
$(OUT)                : $(OBJECTS) $(OUT1)
ifeq ($(LIBPORTABLE_STATIC),1)
	$(LD) $@ $(OBJS)
else	
	$(LD) $@ $(OBJS) $(DFLAGS) $(DLLFLAGS) $(LDFLAGS) $(LDLIBS)
endif	
	-$(CP) $(OUT) $(TETE) 2>/dev/null
$(DEP)/portable.o     : $(SRC)/portable.c $(SRC)/portable.h $(SRC)/general.h
	$(call EXEC)
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/ini_parser.o   : $(SRC)/ini_parser.c $(SRC)/ini_parser.h
	$(CC) -c $< $(CFLAGS) -o $@	
$(DEP)/general.o      : $(SRC)/general.c $(SRC)/general.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/inject.o       : $(SRC)/inject.c $(SRC)/inject.h $(SRC)/winapis.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/safe_ex.o      : $(SRC)/safe_ex.c $(SRC)/safe_ex.h $(SRC)/winapis.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/ice_error.o    : $(SRC)/ice_error.c $(SRC)/ice_error.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/bosskey.o      : $(SRC)/bosskey.c $(SRC)/bosskey.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/new_process.o  : $(SRC)/new_process.c $(SRC)/new_process.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/cpu_info.o     : $(SRC)/cpu_info.c $(SRC)/cpu_info.h
	$(CC) -c $< $(CFLAGS) -mavx -o $@
$(DEP)/balance.o      : $(SRC)/balance.c $(SRC)/balance.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/win_registry.o : $(SRC)/win_registry.c $(SRC)/win_registry.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/set_env.o      : $(SRC)/set_env.c $(SRC)/set_env.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/on_tabs.o      : $(SRC)/on_tabs.c $(SRC)/on_tabs.h $(SRC)/win_automation.h
	$(CC) -c $< $(CFLAGS) -o $@	
$(DEP)/json_paser.o   : $(SRC)/json_paser.c $(SRC)/json_paser.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/lz4.o          : $(SRC)/lz4.c $(SRC)/lz4.h
	$(CC) -c $< $(CFLAGS) -o $@
$(DEP)/cjson.o        : $(SRC)/cjson.c $(SRC)/cjson.h
	$(CC) -c $< $(CFLAGS) -o $@
ifeq ($(LIBPORTABLE_STATIC),)	
$(DEP)/resource.o     : $(SRC)/resource.rc
	$(RC) $< $(RCFLAGS) $@
endif

.PHONY                : clean
clean                 : 
	-rm -rf $(DISTDIR) $(DEP) *.pdb

