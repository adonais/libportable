#ifndef _CPU_INFO_H_
#  define _CPU_INFO_H_

#include "general.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void* __cdecl memset_avx(void*, int, size_t);

#ifdef __cplusplus
}
#endif


#endif  /* _CPU_INFO_H_ */
