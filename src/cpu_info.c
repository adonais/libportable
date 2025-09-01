#include "cpu_info.h"
#include "cpu_avx512.h"
#include "cpu_avx.h"

#define XMM_STATE      (1u << 1)
#define YMM_STATE      (1u << 2)
#define XCR0_OPMASK    (1u << 5)
#define XCR0_ZMM_HI256 (1u << 6)
#define XCR0_HI16_ZMM  (1u << 7)
#define AVX_STATE      (XMM_STATE | YMM_STATE)

#define UU16(x) ((uint8_t)x | (((uint16_t)(x)&0xff) << 8))
#define UU32(y) (UU16(y) | (((uint32_t)(UU16(y))&0xffff) << 16))

#if defined _MSC_VER && _MSC_VER > 1500
#pragma intrinsic(__cpuid)
#endif

memset_ptr optimize_memset = NULL;

typedef enum _cpuid_register
{
    eax = 0,
    ebx = 1,
    ecx = 2,
    edx = 3
}cpuid_register;

static inline bool
has_cpuid_bits(unsigned int level, cpuid_register reg, unsigned int bits)
{
    // Check that the level in question is supported.
    int regs[4];
    __cpuid(regs, level & 0x80000000u);
    if ((unsigned)(regs[0]) < level)
    {
        return false;
    }
    // "The __cpuid intrinsic clears the ECX register before calling the cpuid
    // instruction."
    __cpuid(regs, level);
    return ((unsigned)(regs[reg]) & bits) == bits;
}

static inline bool
cpu_has_sse4_1(void)
{
    return has_cpuid_bits(1u, ecx, (1u << 19));
}

static inline bool
cpu_has_sse4_2(void)
{
    return has_cpuid_bits(1u, ecx, (1u << 20));
}

static inline bool
cpu_has_avx(void)
{
    const unsigned AVX = 1u << 28;
    const unsigned OSXSAVE = 1u << 27;
    const unsigned XSAVE = 1u << 26;
    return has_cpuid_bits(1u, ecx, AVX | OSXSAVE | XSAVE) &&
           // ensure the OS supports XSAVE of YMM registers
           xgetbv_mask_as(AVX_STATE);
}

static inline bool
cpu_has_avx2(void)
{
    return cpu_has_avx() && has_cpuid_bits(7u, ebx, (1u << 5));
}

static inline bool
cpu_has_avx512f(const bool mavx)
{
    if ((!mavx ? cpu_has_avx() : true) && has_cpuid_bits(7u, ebx, (1u << 16)))
    {
        return xgetbv_mask_as(XCR0_OPMASK | XCR0_ZMM_HI256 | XCR0_HI16_ZMM | AVX_STATE);
    }
    return false;
}

void *
memset_less_align(void *dst, const int a, const size_t n)
{
    uint8_t *dt = (uint8_t *)dst;
    if (n & 0x01)
    {
        *dt++ = (uint8_t)a;
    }
    if (n & 0x02)
    {
        *(uint16_t *)dt = UU16(a);
        dt += 2;
    }
    if (n & 0x04)
    {
        *(uint32_t *)dt = UU32(a);
        dt += 4;
    }
    if (n & 0x08)
    {
        _mm_storeu_si64((__m128i *)dt, _mm_set1_epi8((char)a));
        dt += 8;
    }
    if (n & 0x10)
    {
        _mm_storeu_si128((__m128i *)dt, _mm_set1_epi8((char)a));
        dt += 16;
    }
    if (n & 0x20)
    {
        __m128i c = _mm_set1_epi8((char)a);
        _mm_storeu_si128((__m128i *)dt, c);
        _mm_storeu_si128((__m128i *)dt + 1, c);
        dt += 32;
    }
    return dst;
}

static void *__cdecl
memset_sse(void* dst, int c, size_t size)
{
    uint8_t *buffer = (uint8_t *)dst;
    size_t head = ALIGN_DOWN(buffer, 16);
    if (head > 0)
    {
        memset_less_align(buffer, (const uint8_t)c, head);
        buffer += head;
        size -= head;
    }
    if (size >= 16)
    {
        __m128i vals;
        // 使用SSE指令进行快速填充
        if (c)
        {
            vals = _mm_set1_epi8(c);
        }
        else
        {
            vals = _mm_setzero_si128();
        }
        while (size >= 16)
        {
            _mm_stream_si128((__m128i *)(buffer), vals);
            buffer += 16;
            size -= 16;
        }
        _mm_sfence(); // 确保非时态写入完成
    }
    if (size > 0)
    {
        memset_less_align(buffer, (const uint8_t)c, size);
    }
    return dst;
}

/**********************************************************
 * DLL初始化时检测cpu指令集, 函数指针指向将要调用的memset
 * 减少运行时的分支选择, 此项优化是 xunxun1982 提议
 **********************************************************/
bool
initialize_memset(void)
{
    if (cpu_has_avx())
    {
        if (cpu_has_avx512f(true))
        {
            optimize_memset = &memset_avx512;
        }
        else
        {
            optimize_memset = &memset_avx256;
        }
    }
    else
    {
        optimize_memset = &memset_sse;
    }
    return (NULL != optimize_memset);
}

uint32_t
cpu_features(void)
{
    uint32_t mask = 0;
    if (cpu_has_avx512f(false))
    {
        mask |= 0xF8;
    }
    else if (cpu_has_avx2())
    {
        mask |= 0x78;
    }
    else if (cpu_has_avx())
    {
        mask |= 0x38;
    }
    else if (cpu_has_sse4_2())
    {
        mask |= 0x18;
    }
    else if (cpu_has_sse4_1())
    {
        mask |= 0x8;
    }
    return mask;
}
