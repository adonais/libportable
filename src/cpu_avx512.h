#ifndef _CPU_AVX512_H_
#  define _CPU_AVX512_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <intrin.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void memset_avx512_as(uint8_t **pdst, int c, size_t *psize);

#ifdef __cplusplus
}
#endif


#endif  /* _CPU_AVX512_H_ */
