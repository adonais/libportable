#ifndef _SAFE_EX_H_
#  define _SAFE_EX_H_

#ifdef SAFE_EXTERN
#  undef SAFE_EXTERN
#  define SAFE_EXTERN
#else
#  define SAFE_EXTERN extern
#endif

#include <windows.h>

#ifdef _MSC_VER
#if defined(__cplusplus)
extern "C" {
#endif
void* _ReturnAddress(void);
#if defined(__cplusplus)
}
#endif 
#pragma intrinsic(_ReturnAddress)
#endif

#ifdef __cplusplus
extern "C" {
#endif
SAFE_EXTERN unsigned WINAPI init_safed(void * pParam);
SAFE_EXTERN void safe_end(void);
SAFE_EXTERN BOOL WINAPI IsSpecialDll(UINT_PTR callerAddress,LPCWSTR dll_file);
#ifdef __cplusplus
}
#endif 

#endif   /* end _SAFE_EX_H_ */