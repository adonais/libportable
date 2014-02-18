#ifndef _IN_JECT_H
#define _IN_JECT_H
 
#ifdef JECT_EXTERN
#  undef JECT_EXTERN
#  define JECT_EXTERN
#else
#  define JECT_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
JECT_EXTERN unsigned __stdcall InjectDll(void *mpara);
#ifdef __cplusplus
}
#endif

#endif   /* _IN_JECT_H */
