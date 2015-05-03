#ifndef _SET_ENV_H_
#  define _SET_ENV_H_

#define CRT_LEN 16

#ifdef __cplusplus
extern "C" {
#endif

extern void     __stdcall set_envp(char *crt_names, int len);
extern unsigned __stdcall pentadactyl_fixed(void * pParam);

#ifdef __cplusplus
}
#endif 

#endif   /* end _SET_ENV_H_ */