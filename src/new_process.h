#ifndef _NEW_PROCESS_H
#define _NEW_PROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned __stdcall run_process(void * lparam);
extern HANDLE   __stdcall create_new(WCHAR*, const WCHAR*, int, DWORD*);
extern void     __stdcall kill_trees(void);
extern bool     __stdcall no_gui_boot(void);

#ifdef __cplusplus
}
#endif

#endif  /* _NEW_PROCESS_H */
