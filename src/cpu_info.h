#ifndef _CPU_INFO_H_
#  define _CPU_INFO_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <intrin.h>

#if defined _MSC_VER && _MSC_VER > 1500 && !defined(__clang__)
#pragma intrinsic(__cpuid, _xgetbv)
#elif defined(__GNUC__)
extern void __cpuid(int CPUInfo[4], int info_type);
#else
// none
#endif

#define CPUID(func, a, b, c, d) do {\
    int regs[4];\
    __cpuid(regs, func); \
    *a = regs[0]; *b = regs[1];  *c = regs[2];  *d = regs[3];\
  } while(0)


#ifdef __cplusplus
extern "C" {
#endif

extern void* __cdecl memset_avx(void*, int, size_t);
extern bool __cdecl cpu_has_avx(void);

#ifdef __cplusplus
}
#endif


#endif  /* _CPU_INFO_H_ */
