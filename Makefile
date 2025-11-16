# Building: make MOZ_FETCHES_DIR=/builds/worker/fetches
# Building 32bits: make MOZ_FETCHES_DIR=/builds/worker/fetches bits=32
# Install make install LIBPORTABLE_PATH=$MOZ_FETCHES_DIR/clang
CC       = clang-cl
BITS	 ?= 64
LD       = lld-link -DLL
RC       = llvm-rc
RCFLAGS  = -nologo -D "_UNICODE" -D "UNICODE" -FO
CFLAGS   = -O2
LIBPATH  =
LIBPORTABLE_PATH =

ifeq ($(BITS),32)
CFLAGS      += --target=i686-pc-windows-msvc
PROFDATA    = merged32.profdata
RUNTIME_DIR = $(shell echo `$(CC) -clang:--print-runtime-dir --target=i686-pc-windows-msvc`)
else
PROFDATA    = merged64.profdata
RUNTIME_DIR = $(shell echo `$(CC) -clang:--print-runtime-dir`)
endif

ifeq ($(INCLUDE),)
$(error You must define the INCLUDE environment variable for cross-compilation. example: \
  export INCLUDE=";$MOZ_FETCHES_DIR/vs/VC/Tools/MSVC/14.39.33519/atlmfc/include;$MOZ_FETCHES_DIR/vs/VC/Tools/MSVC/14.39.33519/include")
endif

ifneq ($(MOZ_FETCHES_DIR),)
# CFLAGS += -I"$(MOZ_FETCHES_DIR)/vs/VC/Tools/MSVC/14.39.33519/atlmfc/include" -I"$(MOZ_FETCHES_DIR)/vs/VC/Tools/MSVC/14.39.33519/include"

CFLAGS += -Xclang -ivfsoverlay -Xclang $(MOZ_FETCHES_DIR)/vs/overlay.yaml -MD
CFLAGS += -flto=thin -guard:cf -Xclang -MP
CFLAGS += -D_CRT_SECURE_NO_WARNINGS -DCJSON_HIDE_SYMBOLS  -DVC12_CRT -DWINVER=0x0600 -D_WIN32_IE=0x0800 -DINITGUID
CFLAGS += -I"$(MOZ_FETCHES_DIR)/vs/Windows Kits/10/Include/10.0.22621.0/shared" \
          -I"$(MOZ_FETCHES_DIR)/vs/Windows Kits/10/Include/10.0.22621.0/um" \
          -I"$(MOZ_FETCHES_DIR)/vs/Windows Kits/10/Include/10.0.22621.0/ucrt"

CFLAGS += -fms-compatibility-version=19.39 \
          -Wno-nonportable-include-path \
          -Wno-pragma-pack \
          -Wno-unknown-pragmas \
          -Wno-ignored-pragma-intrinsic \
          -Wno-microsoft-anon-tag \
          -Wno-macro-redefined \
          -Wno-ignored-attributes \
          -Wno-missing-prototype-for-cc

ifeq ($(BITS),32)
LIBPATH += -libpath:"$(MOZ_FETCHES_DIR)/vs/VC/Tools/MSVC/14.39.33519/atlmfc/lib/x86"
LIBPATH += -libpath:"$(MOZ_FETCHES_DIR)/vs/VC/Tools/MSVC/14.39.33519/lib/x86"
LIBPATH += -libpath:"$(MOZ_FETCHES_DIR)/vs/Windows Kits/10/Lib/10.0.22621.0/um/x86"
LIBPATH += -libpath:"$(MOZ_FETCHES_DIR)/vs/Windows Kits/10/Lib/10.0.22621.0/ucrt/x86"
else
LIBPATH += -libpath:"$(MOZ_FETCHES_DIR)/vs/VC/Tools/MSVC/14.39.33519/atlmfc/lib/x64"
LIBPATH += -libpath:"$(MOZ_FETCHES_DIR)/vs/VC/Tools/MSVC/14.39.33519/lib/x64"
LIBPATH += -libpath:"$(MOZ_FETCHES_DIR)/vs/Windows Kits/10/Lib/10.0.22621.0/um/x64"
LIBPATH += -libpath:"$(MOZ_FETCHES_DIR)/vs/Windows Kits/10/Lib/10.0.22621.0/ucrt/x64"
endif
else
$(error You must define the MOZ_FETCHES_DIR environment variable)
endif

ifeq ($(PGO_GEN), 1)
CFLAGS  += -clang:-fprofile-generate -mllvm -pgo-temporal-instrumentation -mllvm -enable-name-compression=false
LIBPATH += -libpath:$(RUNTIME_DIR)
else ifeq ($(PGO_USE), 1)
CFLAGS  += -clang:-fprofile-use="$(MOZ_FETCHES_DIR)/$(PROFDATA)"
endif

MD       = mkdir -p
CP       = cp
SRC      = src
SUB_DIR  = $(SRC)/minhook
SUBMK    = $(MAKE) -C $(SUB_DIR)
DEP      = .dep
OBJECTS  = $(DEP)/portable.o $(DEP)/general.o $(DEP)/ice_error.o  $(DEP)/safe_ex.o \
           $(DEP)/inject.o $(DEP)/bosskey.o $(DEP)/new_process.o $(DEP)/set_env.o\
           $(DEP)/cpu_avx.o $(DEP)/cpu_avx512.o $(DEP)/cpu_info.o $(DEP)/balance.o $(DEP)/win_registry.o \
           $(DEP)/on_tabs.o $(DEP)/lz4.o $(DEP)/cjson.o $(DEP)/json_paser.o $(DEP)/ini_parser.o $(DEP)/ice_quit.o

OUT1     = $(DISTDIR)/minhook$(BITS).lib
MIN_INC  = $(SRC)/minhook/include
DISTDIR  = Release
CFLAGS   += -I$(MIN_INC) -I$(SRC)

EXEC     = \
    @echo Starting Compile... \
    $(shell $(MD) $(DISTDIR) 2>/dev/null) \
    $(shell $(MD) $(DEP) 2>/dev/null) \

ifeq ($(DLL_INJECT),1)
CFLAGS  += -DDLL_INJECT
endif

OUT      = $(DISTDIR)/portable$(BITS).dll
TETE     = $(DISTDIR)/tmemutil.dll
DEPLIBS  = kernel32.lib user32.lib shlwapi.lib shell32.lib ole32.lib
LDFLAGS  += $(OUT1) -OPT:REF,ICF -guard:cf,nolongjmp -mllvm:-mcpu=nocona $(LIBPATH) $(DEPLIBS)

ifeq ($(BITS),32)
LDFLAGS += -machine:X86
else
LDFLAGS += -machine:X64
endif

OBJECTS  += $(DEP)/resource.o
OBJS     = $(OBJECTS)

all                   : $(OUT1) $(OUT)
$(OUT1)               : $(SUB_DIR)/Makefile
	$(call SUBMK)
$(OUT)                : $(OBJECTS) $(OUT1)
	$(LD) -out:$@ $(OBJS) $(LDFLAGS)
	-$(CP) $(OUT) $(TETE) 2>/dev/null
$(DEP)/portable.o     : $(SRC)/portable.c $(SRC)/portable.h $(SRC)/general.h
	$(call EXEC)
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/ini_parser.o   : $(SRC)/ini_parser.c $(SRC)/ini_parser.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/general.o      : $(SRC)/general.c $(SRC)/general.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/inject.o       : $(SRC)/inject.c $(SRC)/inject.h $(SRC)/winapis.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/safe_ex.o      : $(SRC)/safe_ex.c $(SRC)/safe_ex.h $(SRC)/winapis.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/ice_error.o    : $(SRC)/ice_error.c $(SRC)/ice_error.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/bosskey.o      : $(SRC)/bosskey.c $(SRC)/bosskey.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/new_process.o  : $(SRC)/new_process.c $(SRC)/new_process.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/cpu_avx.o      : $(SRC)/cpu_avx.c $(SRC)/cpu_avx.h
	$(CC) $(CFLAGS) -c $< -mavx -o $@
$(DEP)/cpu_avx512.o   : $(SRC)/cpu_avx512.c $(SRC)/cpu_avx512.h
	$(CC) $(CFLAGS) -c $< -mavx512f -o $@
$(DEP)/cpu_info.o     : $(SRC)/cpu_info.c $(SRC)/cpu_info.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/balance.o      : $(SRC)/balance.c $(SRC)/balance.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/win_registry.o : $(SRC)/win_registry.c $(SRC)/win_registry.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/set_env.o      : $(SRC)/set_env.c $(SRC)/set_env.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/on_tabs.o      : $(SRC)/on_tabs.c $(SRC)/on_tabs.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/json_paser.o   : $(SRC)/json_paser.c $(SRC)/json_paser.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/lz4.o          : $(SRC)/lz4.c $(SRC)/lz4.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/cjson.o        : $(SRC)/cjson.c $(SRC)/cjson.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/ice_quit.o     : $(SRC)/ice_quit.c $(SRC)/ice_quit.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/resource.o     : $(SRC)/resource.rc
	$(RC) $< $(RCFLAGS) $@

.PHONY             : install
install                 : all
	@if test -z "$(LIBPORTABLE_PATH)" ; then echo install in ... $(LIBPORTABLE_PATH); fi
	@if [ ! -d "$(LIBPORTABLE_PATH)" ]; then $(MD) $(LIBPORTABLE_PATH); fi
	@if [ ! -d "$(LIBPORTABLE_PATH)/bin" ]; then $(MD) "$(LIBPORTABLE_PATH)/bin"; fi
	@if [ ! -d "$(LIBPORTABLE_PATH)/lib" ]; then $(MD) "$(LIBPORTABLE_PATH)/lib"; fi
	@if [ ! -d "$(LIBPORTABLE_PATH)/include" ]; then $(MD) "$(LIBPORTABLE_PATH)/include"; fi
	-$(CP) "$(SRC)/portable.h" "$(LIBPORTABLE_PATH)/include"
	-$(CP) "$(DISTDIR)/portable$(BITS).lib" "$(LIBPORTABLE_PATH)/lib"
	-$(CP) "$(OUT)" "$(LIBPORTABLE_PATH)/bin"
	-$(CP) "$(SRC)/portable(example).ini" "$(LIBPORTABLE_PATH)/bin"

.PHONY                : clean
clean                 :
	-rm -rf $(DISTDIR) $(DEP) *.pdb

