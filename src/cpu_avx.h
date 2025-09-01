#ifndef _CPU_AVX_H_
#  define _CPU_AVX_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool xgetbv_mask_as(const uint32_t mask);
extern void *__cdecl memset_avx256(void *pdst, int c, size_t size);

#ifdef __cplusplus
}
#endif

#endif  /* _CPU_AVX_H_ */
