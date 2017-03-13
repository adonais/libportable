#ifndef _CPU_INFO_H_
#  define _CPU_INFO_H_

#include "inipara.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t __stdcall get_level_size(void);
extern void*    __cdecl   memset_avx(void*, int, size_t);

#ifdef __cplusplus
}
#endif


#endif  /* _CPU_INFO_H_ */
