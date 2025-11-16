CC       := clang-cl
BITS	 ?= 64
AR       = llvm-lib
CFLAGS   = -O2

ifeq ($(BITS),32)
CFLAGS += --target=i686-pc-windows-msvc
endif

ifeq ($(INCLUDE),)
$(error You must define the INCLUDE environment variable for cross-compilation. example: \
  export INCLUDE=";$MOZ_FETCHES_DIR/vs/VC/Tools/MSVC/14.39.33519/atlmfc/include;$MOZ_FETCHES_DIR/vs/VC/Tools/MSVC/14.39.33519/include")
endif

ifneq ($(MOZ_FETCHES_DIR),)
# CFLAGS += -I"$(MOZ_FETCHES_DIR)/vs/VC/Tools/MSVC/14.39.33519/atlmfc/include" -I"$(MOZ_FETCHES_DIR)/vs/VC/Tools/MSVC/14.39.33519/include"

CFLAGS += -Xclang -ivfsoverlay -Xclang $(MOZ_FETCHES_DIR)/vs/overlay.yaml -MD
CFLAGS += -flto=thin -guard:cf -Xclang -MP
CFLAGS += -D_CRT_SECURE_NO_WARNINGS -DVC12_CRT -DWINVER=0x0600 -D_WIN32_IE=0x0800
CFLAGS += -I"$(MOZ_FETCHES_DIR)/vs/Windows Kits/10/Include/10.0.22621.0/shared" \
          -I"$(MOZ_FETCHES_DIR)/vs/Windows Kits/10/Include/10.0.22621.0/um" \
          -I"$(MOZ_FETCHES_DIR)/vs/Windows Kits/10/Include/10.0.22621.0/ucrt"

CFLAGS += -fms-compatibility-version=19.39 \
          -Wno-nonportable-include-path \
          -Wno-pragma-pack \
          -Wno-unknown-pragmas \
          -Wno-microsoft-anon-tag \
          -Wno-ignored-pragma-intrinsic \
          -Wno-macro-redefined \
          -Wno-ignored-attributes
else
$(error You must define the MOZ_FETCHES_DIR environment variable)
endif

ifeq ($(PGO_GEN), 1)
CFLAGS += -clang:-fprofile-generate -mllvm -pgo-temporal-instrumentation -mllvm -enable-name-compression=false
else ifeq ($(PGO_USE), 1)
CFLAGS += -clang:-fprofile-use="$(MOZ_FETCHES_DIR)/merged64.profdata"
endif

OUT      = minhook$(BITS).lib
MD       = mkdir -p
SRC      = src
DEP      = ../../.dep
MIN_INC  = include
OBJECTS  = $(DEP)/buffer.o $(DEP)/hde$(BITS).o $(DEP)/hook.o $(DEP)/trampoline.o
DISTDIR  = ../../Release
CFLAGS   += -I$(MIN_INC) -I$(SRC)

EXEC     = \
    @echo coming to minhook subdir... \
    $(shell $(MD) $(DISTDIR) 2>/dev/null) \
    $(shell $(MD) $(DEP) 2>/dev/null) \

$(DISTDIR)/$(OUT)      : $(OBJECTS) 
	$(AR) -out:$@ $^
$(DEP)/buffer.o        : $(SRC)/buffer.c $(SRC)/buffer.h
	$(call EXEC)
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/hde$(BITS).o    : $(SRC)/hde/hde$(BITS).c $(SRC)/hde/hde$(BITS).h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/hook.o          : $(SRC)/hook.c $(MIN_INC)/MinHook.h
	$(CC) $(CFLAGS) -c $< -o $@
$(DEP)/trampoline.o    : $(SRC)/trampoline.c $(SRC)/trampoline.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY		       : clean
clean                  : 
	-rm -rf $(DISTDIR) $(DEP)

