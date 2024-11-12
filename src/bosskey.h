#ifndef _BOSS_KEY_H_
#  define _BOSS_KEY_H_

typedef struct _WNDINFO
{
    DWORD pid;
    HWND  h;
    WCHAR key_str[VALUE_LEN+1];
    int   atom_str;
    int   key_mod;
    int   key_vk;
} WNDINFO, *LPWNDINFO;

#ifdef __cplusplus
extern "C" {
#endif

void __stdcall undo_bosskey(void);
void __stdcall init_bosskey(void);

#ifdef __cplusplus
}
#endif 

#endif   /* end _BOSS_KEY_H_ */
