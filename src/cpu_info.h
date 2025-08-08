#ifndef _CPU_INFO_H_
#  define _CPU_INFO_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <intrin.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool g_has_avx512;
extern bool cpu_has_avx(void);
extern bool cpu_has_avx512f(const bool mavx);
extern void *memset_avx(void *, int, size_t);
extern uint32_t cpu_features(void);

#ifdef __cplusplus
}
#endif


#endif  /* _CPU_INFO_H_ */
