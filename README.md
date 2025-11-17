## How to build libportable source code?

- C compiler  
Microsoft Visual Studio/Windows .   
 
- or

- Gcc/Linux  

## Build!
- vc14 or above  (cmd shell)  

	nmake -f Makefile.msvc clean  
	nmake -f Makefile.msvc  
	
	clang:  
	nmake -f Makefile.msvc CC=clang-cl clean  
	nmake -f Makefile.msvc CC=clang-cl  

- Gcc/Linux Cross compiling:

	make clean  
	make PGO_GEN=1 BITS=32  
	make PGO_USE=1 BITS=32  

## Add libportable into Firefox?
**Binary static injection,example:**
	
	setdll32 /d:portable32.dll mozglue.dll       // 32 bits firefox    
	setdll64 /d:portable64.dll mozglue.dll       // 64 bits firefox    

**Compiled from firefox's source code,patches example:**   
```
diff --git a/mozglue/misc/WindowsDllMain.cpp b/mozglue/misc/WindowsDllMain.cpp
--- a/mozglue/misc/WindowsDllMain.cpp
+++ b/mozglue/misc/WindowsDllMain.cpp
@@ -6,6 +6,16 @@
 
 #include <libloaderapi.h>
 
+#if defined(_MSC_VER) && defined(TT_MEMUTIL)
+#if defined(_M_IX86)
+#pragma comment(lib,"portable32.lib")
+#pragma comment(linker, "/include:_GetCpuFeature_tt")
+#elif defined(_M_AMD64) || defined(_M_X64)
+#pragma comment(lib,"portable64.lib")
+#pragma comment(linker, "/include:GetCpuFeature_tt")
+#endif
+#endif /* _MSC_VER && TT_MEMUTIL */
+
 BOOL WINAPI DllMain(HINSTANCE aInstDll, DWORD aReason, LPVOID) {
   if (aReason == DLL_PROCESS_ATTACH) {
     ::DisableThreadLibraryCalls(aInstDll);
```