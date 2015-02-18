#ifndef __INTRIN_C_H_
#define __INTRIN_C_H_

#if defined(__GNUC__)
/*** Stack frame juggling ***/
#define _ReturnAddress() (__builtin_return_address(0))
#define _AddressOfReturnAddress() (&(((void **)(__builtin_frame_address(0)))[1]))
#endif

#ifndef __INTRIN_H_
#define __INTRIN_H_   /* 不支持mingw64,它已经从windows.h引入 */

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#include <intrin.h>
#pragma intrinsic(_InterlockedCompareExchange, _InterlockedExchange, \
                  __stosb, __movsb, _ReturnAddress, strlen, wcslen, \
                   wcscpy, wcscmp, memcpy, memset)
#elif defined(__GNUC__)
#include <intrin_gcc.h>
#else
#error Unsupported compiler
#endif

#endif   /* __INTRIN_H_   */
#endif   /* __INTRIN_C_H_ */
