#ifndef _IN_JECT_H
#define _IN_JECT_H
 
#ifdef JECT_EXTERN
/* do nothing: it's been defined by inject.c */
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
