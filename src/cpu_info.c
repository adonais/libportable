#include "cpu_info.h"
#include <stdio.h>

#if defined _MSC_VER && _MSC_VER > 1500
#pragma intrinsic(__cpuid, __stosb, _mm256_set1_epi8, \
        _mm256_stream_si256, _xgetbv)
#elif defined(__GNUC__)
extern void __cpuid(int CPUInfo[4], int info_type);
extern void __stosb(unsigned char *,unsigned char,size_t);
#else
#error Unsupported compiler
#endif

#define CPUID(func, a, b, c, d) do {\
    int regs[4];\
    __cpuid(regs, func); \
    *a = regs[0]; *b = regs[1];  *c = regs[2];  *d = regs[3];\
  } while(0)

static __inline bool
cpu_has_avx(void)
{
    bool has_avx = false;
    bool has_avx_hardware = false;
    int  eax, ebx, ecx, edx;
    CPUID(0x1, &eax, &ebx, &ecx, &edx);
    if ( eax >= 0x2)
    {
        /* check OSXSAVE flags */
        has_avx_hardware = (ecx & 0x10000000) != 0;
    }
    /* check AVX feature flags and XSAVE enabled by kernel */
    has_avx = has_avx_hardware && (ecx & 0x08000000) != 0 \
              &&(_xgetbv(0) & 6) == 6;
    return has_avx;
}

static __inline uint32_t 
get_cache_size(void) 
{
    int eax, ebx, ecx, edx;
    int size = 0;  
    CPUID(0x80000000, &eax, &ebx, &ecx, &edx);
    if ((uint32_t)eax >= 0x80000006) 
    {
        CPUID(0x80000006, &eax, &ebx, &ecx, &edx);
        size = (ecx >> 16) & 0xffff;
    }
    return size * 1024;
}

/* using non-temporal avx */
void* __cdecl 
memset_avx(void* dest, int c, unsigned long count)
{
    uint32_t i;
    uint8_t* end_dst = NULL;
    const    uint32_t count256 = count / sizeof(__m256i);
    const    uint8_t  non_aligned = count % sizeof(__m256i);
    __m256i  vals;
    if ( !cpu_has_avx() )
    {
        return memset(dest, c, count);
    }    
    if ( non_aligned )
    {
        end_dst = (uint8_t*)dest;
    }
    vals = _mm256_set1_epi8(c);
    for (i = 0; i < count256; i++)
    {
        _mm256_stream_si256((__m256i*)dest+i, vals);
    }
    if ( end_dst )
    {
        __stosb(end_dst+i*32, (uint8_t)c, non_aligned);
    }
    return dest;
}

uint32_t __stdcall 
get_level_size(void)
{
    return cpu_has_avx()?get_cache_size():0;
}
