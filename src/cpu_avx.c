#include "cpu_avx.h"

#if defined _MSC_VER && _MSC_VER > 1500
#pragma intrinsic(_xgetbv)
#endif

bool
xgetbv_mask_as(const uint32_t mask)
{
    return (_xgetbv(0) & mask) == mask;
}

void
memset_avx256_as(uint8_t **pdst, int c, size_t *psize)
{
    __m256i vals;
    if (c)
    {
        vals = _mm256_set1_epi8(c);
    }
    else
    {
        vals = _mm256_setzero_si256();
    }
    while (*psize >= 32)
    {
        _mm256_stream_si256((__m256i*)(*pdst), vals);
        *pdst += 32;
        *psize -= 32;
    }
}
