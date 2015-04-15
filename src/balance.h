#ifndef _BAL_ANCE_H_
#  define _BAL_ANCE_H_

#ifdef BAL_EXTERN
/* do nothing: it's been defined by balance.c */
#else
#  define BAL_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
BAL_EXTERN unsigned __stdcall SetCpuAffinity_tt(void * pParam);
#ifdef __cplusplus
}
#endif 

#endif   /* end _BAL_ANCE_H_ */
