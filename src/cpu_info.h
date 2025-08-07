#ifndef _CPU_INFO_H_
#  define _CPU_INFO_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <intrin.h>

#if defined _MSC_VER && _MSC_VER > 1500
#pragma intrinsic(__cpuid, _xgetbv)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void* memset_avx(void *, int, size_t);
extern uint32_t cpu_features(void);
extern bool cpu_has_avx(void);
extern int cpu_level_l2(void);

#ifdef __cplusplus
}
#endif


#endif  /* _CPU_INFO_H_ */
