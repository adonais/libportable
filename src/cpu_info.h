#ifndef _CPU_INFO_H_
#  define _CPU_INFO_H_

#ifdef CPU_INFO
/* do nothing: it's been defined by cpuinfo.c */
#else
#  define CPU_INFO extern
#endif

#include "inipara.h"

#ifdef __cplusplus
extern "C" {
#endif
CPU_INFO uint32_t __stdcall get_level_size(void);
CPU_INFO void*    __cdecl   memset_avx(void*, int, unsigned long);
#ifdef __cplusplus
}
#endif


#endif  /* _CPU_INFO_H_ */
