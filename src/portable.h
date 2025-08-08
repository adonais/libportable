#ifndef _POR_TABLE_H_
#define _POR_TABLE_H_

#include <stdint.h>       /* for uint32_t define */

#ifndef TETE_CLASS_EXPORT
#  define TETE_CLASS_EXPORT __declspec(dllexport)
#endif

#ifndef TETE_CLASS_IMPORT
#  define TETE_CLASS_IMPORT __declspec(dllimport)
#endif

#if defined(LIBPORTABLE_STATIC)
#  define TETE_EXT_CLASS extern
#elif defined(TETE_BUILD)
#  define TETE_EXT_CLASS       TETE_CLASS_EXPORT
#else
#  define TETE_EXT_CLASS       TETE_CLASS_IMPORT
#endif

#define NON_TEMPORAL_STORES_NOT_SUPPORTED 0
#define CPU_AVX512F_MASK 0x80
#define CPU_AVX2_MASK    0x40
#define CPU_AVX_MASK     0x20
#define CPU_SSE42_MASK   0x10
#define CPU_SSE41_MASK   0x8

#ifdef  __cplusplus
extern "C" {
#endif

/* compatibility with tete's patches */
TETE_EXT_CLASS intptr_t GetAppDirHash_tt(void);
TETE_EXT_CLASS uint32_t GetCpuFeature_tt(void);
TETE_EXT_CLASS uint32_t GetNonTemporalDataSizeMin_tt(void);

TETE_EXT_CLASS void *__cdecl memset_nontemporal_tt(void *, int, size_t);
#if defined(LIBPORTABLE_STATIC)
TETE_EXT_CLASS void __stdcall do_it(void);
#endif
TETE_EXT_CLASS void __stdcall undo_it(void);

#ifdef  __cplusplus
}
#endif

#endif   /* end _POR_TABLE_H_ */