#ifndef _NEW_PROCESS_H
#define _NEW_PROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned __stdcall run_process(void * lparam);
extern void     __stdcall kill_trees(void);
extern bool     __stdcall no_gui_boot(void);
extern HANDLE   __stdcall create_new(LPCWSTR, LPCWSTR, LPCWSTR, int, DWORD*);

#ifdef __cplusplus
}
#endif

#endif  /* _NEW_PROCESS_H */
