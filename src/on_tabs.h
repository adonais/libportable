#ifndef _ON_TABS_H_
#  define _ON_TABS_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void __stdcall threads_on_win7(void);
extern unsigned __stdcall threads_on_win10(void *);
extern void __stdcall un_uia(void);

#ifdef __cplusplus
}
#endif 

#endif   /* end _ON_TABS_H_ */