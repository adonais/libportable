#ifndef _CPU_INFO_H_
#  define _CPU_INFO_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <intrin.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t cpu_features(void);
extern uint32_t cpu_level_l2(void);
extern bool cpu_has_avx(void);
extern void *memset_avx(void *, int, size_t);

#ifdef __cplusplus
}
#endif


#endif  /* _CPU_INFO_H_ */
