#ifndef _BOSS_KEY_H_
#  define _BOSS_KEY_H_

#ifdef BOSS_EXTERN
/* do nothing: it's been defined by bosskey.c */
#else
#  define BOSS_EXTERN extern
#endif

#include <windows.h>

typedef struct tagWNDINFO
{
	int	     atom_str;
	int      key_mod;
	int      key_vk;
    DWORD    hPid;
    HWND     hFF;
} WNDINFO;

typedef WNDINFO *LPWNDINFO;

#ifdef __cplusplus
extern "C" {
#endif
BOSS_EXTERN unsigned WINAPI bosskey_thread(void * lparam);
#ifdef __cplusplus
}
#endif 

#endif   /* end _BOSS_KEY_H_ */
