#ifndef _ICE_ERROR_H_
#  define _ICE_ERROR_H_

#ifdef ERROR_EXTERN
/* do nothing: it's been defined by ice_error.c */
#else
#  define ERROR_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif 
ERROR_EXTERN unsigned __stdcall init_exeception(void * pParam);
ERROR_EXTERN void jmp_end(void);
#ifdef __cplusplus
}
#endif 

#endif   /* end _ICE_ERROR_H_ */