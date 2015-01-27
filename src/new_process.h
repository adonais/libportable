#ifndef _NEW_PROCESS_H
#define _NEW_PROCESS_H

#ifdef PROCESS_EXTERN
/* do nothing: it's been defined by new_process.c */
#else
#  define PROCESS_EXTERN extern
#endif

#define	  PROCESS_NUM           10

PROCESS_EXTERN void*  g_handle[PROCESS_NUM];                  /* process tree handle */

#ifdef __cplusplus
extern "C" {
#endif

PROCESS_EXTERN unsigned __stdcall run_process(void * pParam);
PROCESS_EXTERN void     __stdcall refresh_tray(void);

#ifdef __cplusplus
}
#endif

#endif  /* _NEW_PROCESS_H */
