#ifndef __LOAD_MODULE_H__
#define __LOAD_MODULE_H__

#include <windows.h>

typedef void *HMEMORYMODULE;
typedef void *HMEMORYRSRC;
typedef void *HCUSTOMMODULE;

#ifdef __cplusplus
extern "C" {
#endif

typedef LPVOID (*CustomAllocFunc)(LPVOID, SIZE_T, DWORD, DWORD, void*);
typedef BOOL (*CustomFreeFunc)(LPVOID, SIZE_T, DWORD, void*);
typedef HCUSTOMMODULE (*CustomLoadLibraryFunc)(LPCSTR, void *);
typedef FARPROC (*CustomGetProcAddressFunc)(HCUSTOMMODULE, LPCSTR, void *);
typedef void (*CustomFreeLibraryFunc)(HCUSTOMMODULE, void *);

HMEMORYMODULE memLoadLibrary(const void *, size_t);
HMEMORYMODULE memLoadLibraryEx(const void *, size_t,
    CustomAllocFunc,
    CustomFreeFunc,
    CustomLoadLibraryFunc,
    CustomGetProcAddressFunc,
    CustomFreeLibraryFunc,
    void *);

FARPROC memGetProcAddress(HMEMORYMODULE, LPCSTR);
void memFreeLibrary(HMEMORYMODULE);
LPVOID memDefaultAlloc(LPVOID, SIZE_T, DWORD, DWORD, void *);
BOOL memDefaultFree(LPVOID, SIZE_T, DWORD, void *);
HCUSTOMMODULE memDefaultLoadLibrary(LPCSTR, void *);
FARPROC memDefaultGetProcAddress(HCUSTOMMODULE, LPCSTR, void *);
void memDefaultFreeLibrary(HCUSTOMMODULE, void *);

#ifdef __cplusplus
}
#endif


#endif  // __LOAD_MODULE_H__