#ifndef _SAFE_EX_H_
#  define _SAFE_EX_H_

#define LIBTBL_LOCK (L"LIBPORTABLE_LAUNCHER_PROCESS=1")

#ifdef __cplusplus
extern "C" {
#endif

extern void __stdcall close_mutex(void);
extern unsigned  __stdcall init_safed(void);

#ifdef __cplusplus
}
#endif 

#endif   /* end _SAFE_EX_H_ */