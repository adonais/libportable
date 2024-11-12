#ifndef _POR_TABLE_H_
#define _POR_TABLE_H_

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

#ifdef  __cplusplus
extern "C" {
#endif

/* compatibility with tete's patches */
TETE_EXT_CLASS int umpv_init_status(void);

#if defined(LIBPORTABLE_STATIC)
TETE_EXT_CLASS void __stdcall do_it(void);
TETE_EXT_CLASS void __stdcall undo_it(void);
#endif

#ifdef  __cplusplus
}
#endif

#endif   /* end _POR_TABLE_H_ */