/* 优化的CPU指令集检测与调度系统 */
#include <windows.h> // 用于原子操作和内存屏障
#include "cpu_info.h"
#include "cpu_avx512.h"
#include "cpu_avx.h"

// XSAVE状态位定义
#define XMM_STATE      (1u << 1)
#define YMM_STATE      (1u << 2)
#define XCR0_OPMASK    (1u << 5)
#define XCR0_ZMM_HI256 (1u << 6)
#define XCR0_HI16_ZMM  (1u << 7)
#define AVX_STATE      (XMM_STATE | YMM_STATE)

// 字节操作宏定义 - 用于非对齐内存操作
#define UU16(x) ((uint8_t)x | (((uint16_t)(x)&0xff) << 8))
#define UU32(y) (UU16(y) | (((uint32_t)(UU16(y))&0xffff) << 16))
#define UU64(z) (UU32(z) | (((uint64_t)(UU32(z))&0xffffffff) << 32))

// 内存对齐计算宏 - 计算到对齐边界的字节数
#define ALIGN_DOWN(_s, _a) (((_a) - (size_t)_s & ((_a) - 1)) & ((_a) - 1))

#if defined _MSC_VER && _MSC_VER > 1500
#pragma intrinsic(__cpuid)
#endif

// 全局CPU特征缓存
static cpu_features_cache_t g_cpu_cache = {0};
static volatile LONG g_cpu_cache_lock = 0;

// 导出的CPU特性缓存指针的定义
TETE_EXT_CLASS const cpu_features_cache_t* g_cpu_features;

// 函数指针类型定义 - 用于运行时调度
typedef void* (*memset_func_ptr)(void* dst, int c, size_t size);
typedef void (*memset_avx_func_ptr)(uint8_t** buffer, int c, size_t* size, bool use_avx512);

// 优化的函数指针 - 启动时初始化一次，避免运行时分支
static memset_func_ptr optimized_memset = NULL;
static memset_avx_func_ptr optimized_memset_avx = NULL;

// 前向声明 - 不同指令集的memset实现
static void* memset_sse(void* dst, int c, size_t size);
static void* memset_avx256(void* dst, int c, size_t size);
static void* memset_avx512(void* dst, int c, size_t size);

// 保持向后兼容性
bool g_has_avx512 = false;

typedef enum _cpuid_register
{
    eax = 0,
    ebx = 1,
    ecx = 2,
    edx = 3
}cpuid_register;

static void*
memset_less_align(void *dst, int a, size_t n)
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
    if (n & 0x20)
    {
    #if defined(_M_X64) || defined(__x86_64__)
        uint64_t c = UU64(a);
        _mm_stream_si64((int64_t *)dt+0, c);
        _mm_stream_si64((int64_t *)dt+1, c);
        _mm_stream_si64((int64_t *)dt+2, c);
        _mm_stream_si64((int64_t *)dt+3, c);
    #elif defined(_M_IX86) || defined(__i386__)
        uint32_t c = UU32(a);
        _mm_stream_si32((int *)dt+0, c);
        _mm_stream_si32((int *)dt+1, c);
        _mm_stream_si32((int *)dt+2, c);
        _mm_stream_si32((int *)dt+3, c);
        _mm_stream_si32((int *)dt+4, c);
        _mm_stream_si32((int *)dt+5, c);
        _mm_stream_si32((int *)dt+6, c);
        _mm_stream_si32((int *)dt+7, c);
    #else
        #error Unsupported compilers
    #endif
        dt += 32;
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

static void
memset_avx_as(uint8_t **pdst, int c, size_t *psize, const bool avx512)
{
    if (!avx512)
    {
        memset_avx256_as(pdst, c, psize);
    }
    else
    {
        memset_avx512_as(pdst, c, psize);
    }
}

// 优化的CPU特征检测函数 - 使用缓存避免重复CPUID调用
bool
cpu_has_avx512f(const bool mavx)
{
    if (!g_cpu_cache.initialized) {
        cpu_info_initialize();
    }
    return g_cpu_cache.has_avx512f;
}

bool
cpu_has_avx(void)
{
    if (!g_cpu_cache.initialized) {
        cpu_info_initialize();
    }
    return g_cpu_cache.has_avx;
}

// 更新cpu_features函数使用缓存
uint32_t
cpu_features(void)
{
    if (!g_cpu_cache.initialized) {
        cpu_info_initialize();
    }
    return g_cpu_cache.features_mask;
}

// 优化的CPU特征初始化 - 使用自旋锁确保线程安全的一次性初始化
void cpu_info_initialize(void)
{
    // 自旋锁等待其他线程完成初始化
    while (InterlockedCompareExchange(&g_cpu_cache_lock, 1, 0) != 0)
    {
        _mm_pause(); // CPU指令提示，减少自旋锁功耗
    }
    
    // 双重检查锁定模式 - 避免重复初始化
    if (!g_cpu_cache.initialized)
    {
        // 原始CPU特征检测逻辑 - 一次性检测所有特征
        
        // SSE4.1检测
        g_cpu_cache.has_sse4_1 = has_cpuid_bits(1u, ecx, (1u << 19));
        
        // SSE4.2检测
        g_cpu_cache.has_sse4_2 = has_cpuid_bits(1u, ecx, (1u << 20));
        
        // AVX检测 - 需要同时检测CPUID和XSAVE支持
        const unsigned AVX = 1u << 28;
        const unsigned OSXSAVE = 1u << 27;
        const unsigned XSAVE = 1u << 26;
        g_cpu_cache.has_avx = has_cpuid_bits(1u, ecx, AVX | OSXSAVE | XSAVE) &&
                             xgetbv_mask_as(AVX_STATE);
        
        // AVX2检测 - 需要先支持AVX
        g_cpu_cache.has_avx2 = g_cpu_cache.has_avx && has_cpuid_bits(7u, ebx, (1u << 5));
        
        // AVX512F检测 - 需要AVX支持和完整的AVX512状态
        g_cpu_cache.has_avx512f = g_cpu_cache.has_avx &&
                                  has_cpuid_bits(7u, ebx, (1u << 16)) &&
                                  xgetbv_mask_as(XCR0_OPMASK | XCR0_ZMM_HI256 | XCR0_HI16_ZMM | AVX_STATE);
        
        // 计算特征掩码 - 按优先级设置最高级别支持
        g_cpu_cache.features_mask = 0;
        if (g_cpu_cache.has_avx512f) {
            g_cpu_cache.features_mask |= 0xF8;
        } else if (g_cpu_cache.has_avx2) {
            g_cpu_cache.features_mask |= 0x78;
        } else if (g_cpu_cache.has_avx) {
            g_cpu_cache.features_mask |= 0x38;
        } else if (g_cpu_cache.has_sse4_2) {
            g_cpu_cache.features_mask |= 0x18;
        } else if (g_cpu_cache.has_sse4_1) {
            g_cpu_cache.features_mask |= 0x8;
        }
        
        // 选择最优的memset实现 - 避免运行时分支判断
        if (g_cpu_cache.has_avx512f) {
            optimized_memset = memset_avx512;
            optimized_memset_avx = memset_avx512_as;
        } else if (g_cpu_cache.has_avx) {
            optimized_memset = memset_avx256;
            optimized_memset_avx = memset_avx_as;
        } else {
            optimized_memset = memset_sse;
            optimized_memset_avx = NULL; // SSE版本不需要AVX特定函数
        }
        
        // 更新全局变量以保持向后兼容性
        g_has_avx512 = g_cpu_cache.has_avx512f;

        // 将导出的指针指向内部缓存
        g_cpu_features = &g_cpu_cache;
        
        // 内存屏障确保所有写操作在标记初始化完成前完成
        MemoryBarrier();
        g_cpu_cache.initialized = true;
    }
    
    // 释放锁
    InterlockedExchange(&g_cpu_cache_lock, 0);
}

// 快速的CPU特征查询函数 - 使用缓存避免重复检测
static inline bool get_cpu_feature_cached(int feature_type)
{
    if (!g_cpu_cache.initialized) {
        init_cpu_features_cache();
    }
    
    switch (feature_type) {
        case 1: return g_cpu_cache.has_sse4_1;
        case 2: return g_cpu_cache.has_sse4_2;
        case 3: return g_cpu_cache.has_avx;
        case 4: return g_cpu_cache.has_avx2;
        case 5: return g_cpu_cache.has_avx512f;
        default: return false;
    }
}

/* 优化的memset实现 - 使用函数指针避免运行时分支判断 */
void*
memset_avx(void* dst, int c, size_t size)
{
    // 确保CPU特征已初始化
    if (!g_cpu_cache.initialized) {
        cpu_info_initialize();
    }
    
    // 直接调用预选的最优实现，避免分支判断开销
    return optimized_memset(dst, c, size);
}

// SSE版本的memset实现 - 用于不支持AVX的CPU
static void*
memset_sse(void* dst, int c, size_t size)
{
    uint8_t *buffer = (uint8_t *)dst;
    const int align = 16; // SSE 128位对齐
    size_t head = ALIGN_DOWN(buffer, align);
    
    if (head > 0)
    {
        memset_less_align(buffer, c, head);
        buffer += head;
        size -= head;
    }
    
    // 使用SSE指令进行快速填充
    if (size >= 16)
    {
        __m128i value = _mm_set1_epi8((char)c);
        while (size >= 16)
        {
            _mm_stream_si128((__m128i*)buffer, value);
            buffer += 16;
            size -= 16;
        }
        _mm_sfence(); // 确保非时态写入完成
    }
    
    if (size > 0)
    {
        memset_less_align(buffer, c, size);
    }
    
    return dst;
}

// AVX256版本的memset实现 - 用于支持AVX但不支持AVX512的CPU
static void*
memset_avx256(void* dst, int c, size_t size)
{
    uint8_t *buffer = (uint8_t *)dst;
    const int align = 32; // AVX 256位对齐
    size_t head = ALIGN_DOWN(buffer, align);
    
    if (head > 0)
    {
        memset_less_align(buffer, c, head);
        buffer += head;
        size -= head;
    }
    
    // 调用已有的AVX实现，但确保不使用AVX512
    memset_avx_as(&buffer, c, &size, false);
    
    if (size > 0)
    {
        memset_less_align(buffer, c, size);
    }
    
    return dst;
}

// AVX512版本的memset实现 - 用于支持AVX512的CPU
static void*
memset_avx512(void* dst, int c, size_t size)
{
    uint8_t *buffer = (uint8_t *)dst;
    const int align = 64; // AVX512 512位对齐
    size_t head = ALIGN_DOWN(buffer, align);
    
    if (head > 0)
    {
        memset_less_align(buffer, c, head);
        buffer += head;
        size -= head;
    }
    
    // 调用已有的AVX512实现
    memset_avx_as(&buffer, c, &size, true);
    
    if (size > 0)
    {
        memset_less_align(buffer, c, size);
    }
    
    return dst;
}
