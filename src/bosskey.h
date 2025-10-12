#ifndef _BOSS_KEY_H_
#  define _BOSS_KEY_H_

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned __stdcall bosskey_thread(void * lparam);
extern unsigned __stdcall uninstall_bosskey(void);
extern uint32_t __stdcall get_bosskey_id(void);

#ifdef __cplusplus
}
#endif 

#endif   /* end _BOSS_KEY_H_ */
