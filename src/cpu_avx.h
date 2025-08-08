#ifndef _CPU_AVX_H_
#  define _CPU_AVX_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <intrin.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool xgetbv_mask_as(const uint32_t mask);
extern void memset_avx256_as(uint8_t **pdst, int c, size_t *psize);

#ifdef __cplusplus
}
#endif


#endif  /* _CPU_AVX_H_ */
