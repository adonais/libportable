#include "cpu_info.h"

#if defined _MSC_VER && _MSC_VER > 1500
#pragma intrinsic(_xgetbv)
#endif

bool
xgetbv_mask_as(const uint32_t mask)
{
    return (_xgetbv(0) & mask) == mask;
}

/* using non-temporal avx */
void *__cdecl 
memset_avx256(void* dst, int c, size_t size)
{
    uint8_t *buffer = (uint8_t *)dst;
    size_t head = ALIGN_DOWN(buffer, 32);
    if (head > 0)
    {   /* memory address not aligned, fill head */
        memset_less_align(buffer, c, head);
        buffer += head;
        size -= head;
    }
    if (size >= 32)
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
        while (size >= 32)
        {
            _mm256_stream_si256((__m256i *)(buffer), vals);
            buffer += 32;
            size -= 32;
        }
        _mm_sfence(); // 确保非时态写入完成
    }
    if (size > 0)
    {   /* fill tail */
        memset_less_align(buffer, c, size);
    }
    return dst;
}
