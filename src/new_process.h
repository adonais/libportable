#ifndef _NEW_PROCESS_H
#define _NEW_PROCESS_H

#define	  PROCESS_NUM 10

#ifdef __cplusplus
extern "C" {
#endif

extern void*    g_handle[PROCESS_NUM];     /* process tree handle */
extern unsigned __stdcall run_process(void * pParam);
extern void     __stdcall refresh_tray(void);

#ifdef __cplusplus
}
#endif

#endif  /* _NEW_PROCESS_H */
