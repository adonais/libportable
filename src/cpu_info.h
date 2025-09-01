#ifndef _CPU_INFO_H_
#  define _CPU_INFO_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <intrin.h>

#define ALIGN_DOWN(_s, _a) ((((size_t)(_a) - (size_t)(_s)) & ((_a) - 1)) & ((_a) - 1))

typedef void *(__cdecl *memset_ptr)(void *dest, int c, size_t count);

#ifdef __cplusplus
extern "C" {
#endif

extern memset_ptr optimize_memset;
extern bool initialize_memset(void);
extern uint32_t cpu_features(void);
extern void *memset_less_align(void *dst, const int a, const size_t n);

#ifdef __cplusplus
}
#endif

#endif  /* _CPU_INFO_H_ */
