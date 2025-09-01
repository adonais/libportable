#include "cpu_info.h"

/* using non-temporal avx 512f */
void *__cdecl
memset_avx512(void* dst, int c, size_t size)
{
    uint8_t *buffer = (uint8_t *)dst;
    size_t head = ALIGN_DOWN(buffer, 64);
    if (head > 0)
    {   /* memory address not aligned, fill head */
        memset_less_align(buffer, c, head);
        buffer += head;
        size -= head;
    }
    if (size >= 64)
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
        while (size >= 64)
        {
            _mm512_stream_si512((__m512i *)buffer, vals);
            buffer += 64;
            size -= 64;
        }
        _mm_sfence();
    }
    if (size > 0)
    {   /* fill tail */
        memset_less_align(buffer, c, size);
    }
    return dst;
}
