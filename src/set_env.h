#ifndef _SET_ENV_H_
#  define _SET_ENV_H_

#ifdef ENV_EXTERN
/* do nothing: it's been defined by set_env.c */
#else
#  define ENV_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif 
ENV_EXTERN unsigned __stdcall pentadactyl_fixed(void * pParam);
ENV_EXTERN unsigned __stdcall foreach_env(void *pParam);
ENV_EXTERN unsigned __stdcall set_envp(void *pParam);
#ifdef __cplusplus
}
#endif 

#endif   /* end _SET_ENV_H_ */