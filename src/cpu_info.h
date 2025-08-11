#ifndef _CPU_INFO_H_
#  define _CPU_INFO_H_

#include <stdbool.h>
#include <stdint.h>
#include <intrin.h>
#include "portable.h" // For TETE_EXT_CLASS

// CPU特征缓存结构 - 避免重复检测
// 将其定义在头文件中，以便其他模块可以理解其结构
typedef struct {
    uint32_t features_mask;          // CPU特征位掩码
    bool has_avx;                    // 支持AVX指令集
    bool has_avx2;                   // 支持AVX2指令集
    bool has_avx512f;                // 支持AVX512F指令集
    bool has_sse4_1;                 // 支持SSE4.1指令集
    bool has_sse4_2;                 // 支持SSE4.2指令集
    bool initialized;                // 是否已初始化
} cpu_features_cache_t;

#ifdef __cplusplus
extern "C" {
#endif

// 导出的CPU特性缓存指针，供外部模块只读访问
TETE_EXT_CLASS const cpu_features_cache_t* g_cpu_features;

// 全局初始化函数，应在程序启动时尽早调用
TETE_EXT_CLASS void cpu_info_initialize(void);

// 旧的API保持不变，以实现向后兼容
extern bool g_has_avx512;
extern bool cpu_has_avx(void);
extern bool cpu_has_avx512f(const bool mavx);
extern void *memset_avx(void *, int, size_t);
extern uint32_t cpu_features(void);

#ifdef __cplusplus
}
#endif

#endif  /* _CPU_INFO_H_ */
