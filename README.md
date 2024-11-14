HOW TO BUILD umpv.dll
=================

## System requirements
------------------

- C compiler 

mingw/gcc, msys2

mingw/gcc on:
https://sourceforge.net/projects/libportable/files/Tools/

msys2 project on
https://sourceforge.net/projects/msys2/

 or

Microsoft Visual Studio .

## Build!
------------------
```
mingw/gcc compiler 

build x64:

make clean
make

build x86:

make clean
make BITS=32

--------------------
vc compiler 

nmake -f Makefile.msvc clean
nmake -f Makefile.msvc
```

## Injecting mpv.exe's import table
```
setdll32 /d:umpv32.dll mpv.exe       // 32 bits mpv player
setdll64 /d:umpv64.dll mpv.exe       // 64 bits mpv player
```

## Compiled with mpv's source code,patches example:
```
@@ -1,3 +1,7 @@
+#if defined(_WIN32) && defined(__GNUC__)
+#include <umpv.h>
+#endif
+
 #include <windows.h>
 #include <shellapi.h>
 
@@ -61,6 +65,10 @@ int main(void)
 {
     microsoft_nonsense();
 
+#if defined(_WIN32) && defined(__GNUC__)
+    umpv_init_status();
+#endif
+
     // If started from the console wrapper (see osdep/win32-console-wrapper.c),
     // attach to the console and set up the standard IO handles
     bool has_console = terminal_try_attach();
```