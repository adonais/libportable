CC       = $(CROSS_COMPILING)gcc -c 
CFLAGS   = -O2
LD       = $(CROSS_COMPILING)gcc -o
BITS	 := 32
DFLAGS   :=
MSCRT    := -lmsvcrt
DLL_MAIN_STDCALL_NAME =

ifeq ($(CROSS_COMPILING),)
YASM     := msrt/yasm
else
YASM     := yasm
endif

ifeq ($(BITS),64)
DLL_MAIN_STDCALL_NAME = DllMain
else
DLL_MAIN_STDCALL_NAME = _DllMain@12
endif

LDFLAGS  = -nodefaultlibs -Wl,-static -lmingw32 -lmingwex -lgcc -lkernel32 -luser32 -Wl,-s
CFLAGS   += $(DFLAGS)  -Wall -Wno-unused -Wno-format -Wno-int-to-pointer-cast -msse2 \
	    -fomit-frame-pointer -finline-functions -fno-stack-protector
LDLIBS   = -lshlwapi -lshell32 -lgdi32 $(MSCRT)
MD       = mkdir -p
CP       = cp
SRC      = src
SUB_DIR  = $(SRC)/mhook-lib
SUBMK    = $(MAKE) -C $(SUB_DIR)
DEP      = .dep
X86FLAG  = -D_WIN32 -m32
X64FLAG  =  -D_WIN64 -m64
OBJECTS  = $(DEP)/portable.o $(DEP)/inipara.o $(DEP)/ttf_list.o $(DEP)/ice_error.o  $(DEP)/safe_ex.o \
           $(DEP)/inject.o $(DEP)/bosskey.o  $(DEP)/cachesize$(BITS).o $(DEP)/instrset$(BITS).o \
	   $(DEP)/unalignedisfaster$(BITS).o $(DEP)/memset$(BITS).o $(DEP)/cputype$(BITS).o \
	   $(DEP)/inject$(BITS).o $(DEP)/resource.o

DISTDIR  = Release
OUT1     = $(DISTDIR)/libmhook$(BITS).a
OUT2     = $(DISTDIR)/portable$(BITS).dll
RC       = $(CROSS_COMPILING)windres
RCFLAGS  = -l "LANGUAGE 4,2" -J rc -O coff
DLLFLAGS += -shared -Wl,--out-implib,$(DISTDIR)/libportable$(BITS).dll.a --entry=$(DLL_MAIN_STDCALL_NAME)
MKDLL	 += $(LD) $(DLLFLAGS) -shared -L$(DISTDIR) -lmhook(BITS)

EXEC     = \
    @echo Starting Compile... \
    $(shell $(MD) $(DISTDIR) 2>/dev/null) \
    $(shell $(MD) $(DEP) 2>/dev/null) \

ifeq ($(BITS),32)
    CFLAGS += $(X86FLAG)
    LDFLAGS += $(X86FLAG)
    ASMFLAGS = -fwin32 -DWINDOWS -D__i386__ -DWIN32 -Worphan-labels
else
    ifeq ($(BITS),64)
        CFLAGS	+= $(X64FLAG)
        LDFLAGS += $(X64FLAG)
	ASMFLAGS = -fwin64 -DWINDOWS -D__x86_64__ -Worphan-labels
	RCFLAGS += -F pe-x86-64
    endif
endif

all		      : $(OUT1) $(OUT2)
$(OUT1)		      : $(SUB_DIR)/Makefile
	$(call SUBMK)
$(OUT2)		      : $(OBJECTS)
	$(LD) $@ $(OBJECTS) $(DLLFLAGS) $(OUT1) $(LDFLAGS) $(LDLIBS)
	-$(CP) $(DISTDIR)/libportable$(BITS).dll.a $(DISTDIR)/portable$(BITS).lib 2>/dev/null
$(DEP)/portable.o     : $(SRC)/portable.c $(SRC)/portable.h  $(SRC)/header.h
	$(call EXEC)
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/inipara.o      : $(SRC)/inipara.c $(SRC)/inipara.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/ttf_list.o   : $(SRC)/ttf_list.c $(SRC)/ttf_list.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/inject.o      : $(SRC)/inject.c $(SRC)/inject.h $(SRC)/header.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/safe_ex.o      : $(SRC)/safe_ex.c $(SRC)/safe_ex.h $(SRC)/header.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/ice_error.o    : $(SRC)/ice_error.c $(SRC)/ice_error.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/bosskey.o    : $(SRC)/bosskey.c $(SRC)/bosskey.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/cachesize$(BITS).o	  : $(SRC)/asm/cachesize$(BITS).asm
	$(YASM) -o $@ $(ASMFLAGS) $<
$(DEP)/instrset$(BITS).o	  : $(SRC)/asm/instrset$(BITS).asm
	$(YASM) -o $@ $(ASMFLAGS) $<
$(DEP)/memset$(BITS).o	          : $(SRC)/asm/memset$(BITS).asm
	$(YASM) -o $@ $(ASMFLAGS) $<
$(DEP)/cputype$(BITS).o	          : $(SRC)/asm/cputype$(BITS).asm
	$(YASM) -o $@ $(ASMFLAGS) $<
$(DEP)/unalignedisfaster$(BITS).o : $(SRC)/asm/unalignedisfaster$(BITS).asm
	$(YASM) -o $@ $(ASMFLAGS) $<
$(DEP)/inject$(BITS).o	          : $(SRC)/asm/inject$(BITS).asm
	$(YASM) -o $@ $(ASMFLAGS) $<
$(DEP)/resource.o                 : $(SRC)/resource.rc
	$(RC) -i $< $(RCFLAGS) -o $@

.PHONY		                  : clean
clean                             : 
	-rm -rf $(DISTDIR) $(DEP)


