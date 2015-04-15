#ifndef _BOSS_KEY_H_
#  define _BOSS_KEY_H_

#ifdef BOSS_EXTERN
/* do nothing: it's been defined by bosskey.c */
#else
#  define BOSS_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
BOSS_EXTERN unsigned __stdcall bosskey_thread(void * lparam);
#ifdef __cplusplus
}
#endif 

#endif   /* end _BOSS_KEY_H_ */
