#ifndef __INTRIN_C_H_
#define __INTRIN_C_H_

/* do not have a C99 compiler */
#if (defined _MSC_VER && _MSC_VER < 1800 && !(defined __cplusplus)) ||\
    (defined __GNUC__&& __STDC_VERSION__ < 199901L && __GNUC__ < 3)
typedef unsigned char _Bool;
#  define bool _Bool
#  define true 1
#  define false 0
#  define __bool_true_false_are_defined 1
#else
#  include <stdbool.h>
#endif

#if defined(__GNUC__)
/*** Stack frame juggling ***/
#define _ReturnAddress() (__builtin_return_address(0))
#define _AddressOfReturnAddress() (&(((void **)(__builtin_frame_address(0)))[1]))
#if defined(__AVX__)
#include <x86intrin.h>
extern __inline__ __attribute__((__gnu_inline__, __always_inline__, __artificial__)) 
unsigned long long _xgetbv(unsigned int __xcr_no)
{
    unsigned int __eax, __edx;
    __asm__ ("xgetbv" : "=a" (__eax), "=d" (__edx) : "c" (__xcr_no));
    return ((unsigned long long)__edx << 32) | __eax;
}
#endif
#endif

#ifndef __INTRIN_H_
#define __INTRIN_H_   /* 不支持mingw64,它已经从windows.h引入 */

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#include <intrin.h>
#pragma intrinsic(_InterlockedCompareExchange, _InterlockedExchange, \
                  __stosb, _ReturnAddress, strlen, wcslen, \
                  wcscpy, wcscmp, memcpy, memset)
#elif defined(__GNUC__)
#include "intrin_gcc.h"
#else
#error Unsupported compiler
#endif

#endif   /* __INTRIN_H_   */
#endif   /* __INTRIN_C_H_ */
