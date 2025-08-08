#include "cpu_avx512.h"

void
memset_avx512_as(uint8_t **pdst, int c, size_t *psize)
{
    __m512i vals;
    if (c)
    {
        vals = _mm512_set1_epi8(c);
    }
    else
    {
        vals = _mm512_setzero_si512();
    }
    while (*psize >= 64)
    {
        _mm512_stream_si512((__m512i*)(*pdst), vals);
        *pdst += 64;
        *psize -= 64;
    }
}
