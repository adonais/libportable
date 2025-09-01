#ifndef _CPU_AVX512_H_
#  define _CPU_AVX512_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void *__cdecl memset_avx512(void *pdst, int c, size_t size);

#ifdef __cplusplus
}
#endif


#endif  /* _CPU_AVX512_H_ */
