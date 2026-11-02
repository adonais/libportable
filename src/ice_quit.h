#ifndef _ICE_QUIT_H_
#  define _ICE_QUIT_H_

#define PORTABLE_UP    (WM_USER + 0x4E20)
#define PORTABLE_BOS_0 (PORTABLE_UP + 0x1)
#define PORTABLE_TAB_0 (PORTABLE_UP + 0x2)
#define PORTABLE_TAB_1 (PORTABLE_UP + 0x3)
#define PORTABLE_TAB_2 (PORTABLE_UP + 0x4)
#define PORTABLE_TAB_3 (PORTABLE_UP + 0x5)
#define PORTABLE_TAB_4 (PORTABLE_UP + 0x6)
#define PORTABLE_TAB_5 (PORTABLE_UP + 0x7)
#define PORTABLE_TAB_6 (PORTABLE_UP + 0x8)
#define PORTABLE_TAB_7 (PORTABLE_UP + 0x9)
#define PORTABLE_TAB_8 (PORTABLE_UP + 0x20)
#define PORTABLE_UBO   (PORTABLE_UP + 0x10)
#define PORTABLE_CHR   (PORTABLE_UP + 0x11)
#if defined(DLL_INJECT)
#define PORTABLE_HOOK  (PORTABLE_UP + 0x12)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void __stdcall init_exemsg(uint32_t tid);
extern void __stdcall init_exequit(void);

#ifdef __cplusplus
}
#endif 

#endif   /* end _ICE_QUIT_H_ */