#ifndef _SET_ENV_H_
#  define _SET_ENV_H_

#define CRT_LEN 64

#ifdef __cplusplus
extern "C" {
#endif

extern void     __stdcall set_envp(char *crt_names, int len);

#ifdef __cplusplus
}
#endif 

#endif   /* end _SET_ENV_H_ */