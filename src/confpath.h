#ifndef _SET_ENV_H_
#  define _SET_ENV_H_

#define CRT_LEN 64

#ifdef __cplusplus
extern "C" {
#endif

bool __stdcall find_mpv_conf(wchar_t* PATH, const int len);

#ifdef __cplusplus
}
#endif 

#endif   /* end _SET_ENV_H_ */