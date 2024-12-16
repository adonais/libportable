#ifndef _LIB_UMPV_H_
#define _LIB_UMPV_H_

#ifndef UMPV_CLASS_EXPORT
#  define UMPV_CLASS_EXPORT __declspec(dllexport)
#endif

#ifndef UMPV_CLASS_IMPORT
#  define UMPV_CLASS_IMPORT __declspec(dllimport)
#endif

#if defined(LIBUMPV_STATIC)
#  define UMPV_EXT_CLASS extern
#elif defined(LIBUMPV_BUILDING)
#  define UMPV_EXT_CLASS    UMPV_CLASS_EXPORT
#else
#  define UMPV_EXT_CLASS    UMPV_CLASS_IMPORT
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/* Force link this DLL */
UMPV_EXT_CLASS int umpv_init_status(void);

#if defined(LIBUMPV_STATIC)
UMPV_EXT_CLASS void __stdcall do_it(void);
UMPV_EXT_CLASS void __stdcall undo_it(void);
#endif

#ifdef  __cplusplus
}
#endif

#endif   /* end _LIB_UMPV_H_ */
