#include "cpu_info.h"

#define XMM_STATE (1u << 1)
#define YMM_STATE (1u << 2)
#define AVX_STATE (XMM_STATE | YMM_STATE)

#define UU16(x) ((uint8_t)x | (((uint16_t)(x)&0xff) << 8))
#define UU32(y) (UU16(y) | (((uint32_t)(UU16(y))&0xffff) << 16))

typedef enum _cpuid_register
{
    eax = 0,
    ebx = 1,
    ecx = 2,
    edx = 3
}cpuid_register;

static void*
memset_less32(void *dst, int a, size_t n)
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
        _mm_stream_si32((int *)dt, UU32(a));
        dt += 4;
    }
    if (n & 0x08)
    {
        uint32_t c = UU32(a);
        _mm_stream_si32((int *)dt+0, c);
        _mm_stream_si32((int *)dt+1, c);
        dt += 8;
    }
    if (n & 0x10)
    {
        uint32_t c = UU32(a);
        _mm_stream_si32((int *)dt+0, c);
        _mm_stream_si32((int *)dt+1, c);
        _mm_stream_si32((int *)dt+2, c);
        _mm_stream_si32((int *)dt+3, c);
        dt += 16;
    }
    return dst;
}

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
cpu_has_avx2(void)
{
    return cpu_has_avx() && has_cpuid_bits(7u, ebx, (1u << 5));
}

static inline bool
cpu_has_avx512f(void)
{
    if (cpu_has_avx() && has_cpuid_bits(7u, ebx, (1u << 16)))
    {
        const unsigned XCR0_OPMASK = 1u << 5;
        const unsigned XCR0_ZMM_HI256 = 1u << 6;
        const unsigned XCR0_HI16_ZMM = 1u << 7;
        const unsigned XCRO_STATE = XCR0_OPMASK | XCR0_ZMM_HI256 | XCR0_HI16_ZMM | AVX_STATE;
        return (_xgetbv(0) & XCRO_STATE) == XCRO_STATE;
    }
    return false;
}

bool
cpu_has_avx(void)
{
    const unsigned AVX = 1u << 28;
    const unsigned OSXSAVE = 1u << 27;
    const unsigned XSAVE = 1u << 26;
    return has_cpuid_bits(1u, ecx, AVX | OSXSAVE | XSAVE) &&
           // ensure the OS supports XSAVE of YMM registers
           (_xgetbv(0) & AVX_STATE) == AVX_STATE;
}

int
cpu_level_l2(void)
{
    int regs[4];
    int size = 0;
    __cpuid(regs, 0x80000000u);
    if (regs[0] >= 0x80000006u)
    {
        __cpuid(regs, 0x80000006u);
        size = (regs[2] >> 16) & 0xffff;
    }
    return size * 1024;
}

uint32_t
cpu_features(void)
{
    uint32_t mask = 0;
    if (cpu_has_avx512f())
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

/* using non-temporal avx */
void*
memset_avx(void* dst, int c, size_t size)
{
    __m256i   vals;
    uint8_t   *buffer = (uint8_t *)dst;
    const uint8_t non_aligned = (uintptr_t)buffer % 32;
     /* memory address not 32-byte aligned */
    if ( non_aligned )
    {
        /* fill head */
        uintptr_t head = 32 - non_aligned;
        memset_less32(buffer, c, head);
        buffer += head;
        size -= head;
    }
    if ( c )
    {
        vals = _mm256_set1_epi8(c);
    }
    else
    {
        vals = _mm256_setzero_si256();
    }
    while (size >= 32)
    {
        _mm256_stream_si256((__m256i*)buffer, vals);
        buffer += 32;
        size -= 32;
    }
    if ( size > 0 )
    {
        /* fill tail */
        memset_less32(buffer, c, size);
    }
    return dst;
}
