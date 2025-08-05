#ifndef _CPU_INFO_H_
#  define _CPU_INFO_H_

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void* __cdecl memset_avx(void*, int, size_t);
extern bool __cdecl cpu_has_avx(void);

#ifdef __cplusplus
}
#endif


#endif  /* _CPU_INFO_H_ */
