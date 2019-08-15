## How to build libportable source code?

- C compiler  
Microsoft Visual Studio .   
 
- or

- gcc, msys/msys2  
gcc for windows:  
https://sourceforge.net/projects/libportable/files/Tools/mingw9.2.1-clang-win64_32.7z  
msys msys2 project on:  
https://sourceforge.net/projects/mingw/files/MSYS  
https://sourceforge.net/projects/msys2/

## Build!
- vc compiler  (cmd shell)  

	nmake -f Makefile.msvc clean  
	nmake -f Makefile.msvc
	
	dynamic lininking the old msvcrt.dll:  
	nmake -f Makefile.msvc clean  
	nmake -f Makefile.msvc MSVC_CRT=1 
	
	enable tcmalloc memory allocator:  
	nmake -f Makefile.msvc TCMALLOC=1  

- gcc/mingw64 compiler (msys shell)

	make clean  
	make
	
	enable GCC Link-time optimization(LTO):   
	make LTO=1
	
	If gcc is a cross-compiler, use the CROSS_COMPILING option:
	
	make clean  
	64bits:  
	make CROSS_COMPILING=x86_64-w64-mingw32-  
	32bits:  
	make CROSS_COMPILING=x86_64-w64-mingw32- BITS=32  

- clang compiler,If you have MSVC compiler installed   
	**(cmd shell):**
	
	nmake -f Makefile.msvc CC=clang-cl clean  
	nmake -f Makefile.msvc CC=clang-cl  
	
	**(msys shell):**  
	
	make clean  
	make CC=clang DFLAGS=--target=x86_64-w64-windows-gnu        // (64bits target build)  
	make CC=clang DFLAGS=--target=i686-w64-windows-gnu         // (32bits target build)  
	
	make clean  
	make CC=clang DFLAGS=--target=i686-pc-windows-msvc  
	make CC=clang DFLAGS=--target=x86_64-pc-windows-msvc  


## Add libportable into Firefox?
**Binary static injection,example:**
	
	setdll32 /d:portable32.dll xul.dll       // 32 bits firefox    
	setdll64 /d:portable64.dll xul.dll       // 64 bits firefox    

**Compiled from firefox's source code,patches example:**   
```
diff --git a/toolkit/library/moz.build b/toolkit/library/moz.build
--- a/toolkit/library/moz.build
+++ b/toolkit/library/moz.build
@@ -290,16 +290,22 @@ if CONFIG['OS_ARCH'] == 'SunOS':
         ]
 
 if CONFIG['OS_ARCH'] == 'FreeBSD':
     OS_LIBS += [
         'util',
     ]
 
 if CONFIG['OS_ARCH'] == 'WINNT':
+    if CONFIG['CPU_ARCH'] == 'x86_64':
+        OS_LIBS += ['portable64']
+    else:
+        OS_LIBS += ['portable32']
+    
+if CONFIG['OS_ARCH'] == 'WINNT':
     OS_LIBS += [
         'shell32',
         'ole32',
         'version',
         'winspool',
         'comdlg32',
         'imm32',
         'msimg32',
```