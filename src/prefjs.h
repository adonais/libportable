#ifndef _PRE_JS_H
#define _PRE_JS_H

#ifdef PREJS_EXTERN
/* do nothing: it's been defined by prejs.c */
#else
#  define PREJS_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

PREJS_EXTERN unsigned __stdcall gmpservice_check(void * pParam);

#ifdef __cplusplus
}
#endif

#endif  /* _PRE_JS_H */
