CC       = gcc -c 
CFLAGS   = -O3
LD       = gcc -o
BITS	 := 32
CRT_LINK := 1600
MSCRT    =
DLL_MAIN_STDCALL_NAME =

ifeq ($(CRT_LINK),1400)
    MSCRT = -lmsvcr80
else ifeq ($(CRT_LINK),1500)
    MSCRT = -lmsvcr90
else ifeq ($(CRT_LINK),1600)
    MSCRT = -lmsvcr100
else ifeq ($(CRT_LINK),1700)
    MSCRT = -lmsvcr110
else ifeq ($(CRT_LINK),1800)
    MSCRT = -lmsvcr120
else
    MSCRT = -lmsvcrt
endif

ifeq ($(BITS),64)
DLL_MAIN_STDCALL_NAME = DllMain
else
DLL_MAIN_STDCALL_NAME = _DllMain@12
endif

LDFLAGS  = -nodefaultlibs -Wl,-static -lmingw32 -lmingwex -lkernel32 -luser32 -Wl,-s
CFLAGS   += -DCRT_LINK=$(CRT_LINK) -Wall -Wno-unused -Wno-format -msse2 \
	    -fomit-frame-pointer -finline-functions -fno-stack-protector
LDLIBS   = -lshlwapi -lshell32 -lgdi32 -lgcc $(MSCRT)
MD       = mkdir -p
SRC      = src
SUB_DIR  = $(SRC)/mhook-lib
SUBMK    = $(MAKE) -C $(SUB_DIR)
DEP      = .dep
X86FLAG  = -m32
X64FLAG  = -m64
OBJECTS  = $(DEP)/portable.o $(DEP)/inipara.o $(DEP)/ice_error.o \
	   $(DEP)/safe_ex.o $(DEP)/bosskey.o $(DEP)/resource.o
DISTDIR  = Release
OUT1     = $(DISTDIR)/libmhook$(BITS).a
OUT2     = $(DISTDIR)/portable$(BITS).dll
RC       = windres
RCFLAGS  = -l "LANGUAGE 4,2" -J rc -O coff
DLLFLAGS += -shared -Wl,--out-implib,$(DISTDIR)/portable$(BITS).lib --entry=$(DLL_MAIN_STDCALL_NAME)
MKDLL	 += $(LD) $(DLLFLAGS) -shared -L$(DISTDIR) -lmhook

EXEC     = \
    @echo Starting Compile... \
    $(shell $(MD) $(DISTDIR) 2>/dev/null) \
    $(shell $(MD) $(DEP) 2>/dev/null) \

ifeq ($(BITS),32)
    CFLAGS += $(X86FLAG)
    LDFLAGS += $(X86FLAG)
else
    ifeq ($(BITS),64)
        CFLAGS	+= $(X64FLAG)
        LDFLAGS += $(X64FLAG)
	RCFLAGS += -F pe-x86-64
    endif
endif

all		      : $(OUT1) $(OUT2)
$(OUT1)		      : $(SUB_DIR)/Makefile
	$(call SUBMK)
$(OUT2)		      : $(OBJECTS)
	$(LD) $@ $(OBJECTS) $(DLLFLAGS) $(OUT1) $(LDFLAGS) $(LDLIBS)
$(DEP)/portable.o     : $(SRC)/portable.c $(SRC)/portable.h $(SRC)/ttf_list.h
	$(call EXEC)
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/inipara.o      : $(SRC)/inipara.c $(SRC)/inipara.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/safe_ex.o      : $(SRC)/safe_ex.c $(SRC)/safe_ex.h $(SRC)/header.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/ice_error.o    : $(SRC)/ice_error.c $(SRC)/ice_error.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/bosskey.o    : $(SRC)/bosskey.c $(SRC)/bosskey.h
	$(CC) $< $(CFLAGS) -o $@
$(DEP)/resource.o     : $(SRC)/resource.rc
	$(RC) -i $< $(RCFLAGS) -o $@

.PHONY		      : clean
clean                 : 
	-rm -rf $(DISTDIR) $(DEP)


