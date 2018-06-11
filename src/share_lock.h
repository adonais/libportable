#ifndef _SHARE_LOCK_H_
#  define _SHARE_LOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

extern bool   WINAPI share_create(bool read, uint32_t); 
extern HANDLE WINAPI share_open(bool ready);
extern LPVOID WINAPI share_map(size_t bytes, bool read);
extern bool   WINAPI share_unmap(LPVOID memory);
extern void   WINAPI share_close(void);
extern HANDLE WINAPI get_share_handle(void);
extern void   WINAPI set_share_handle(HANDLE handle);
extern bool   WINAPI get_ini_path(WCHAR *ini, int len);
extern bool   WINAPI get_appdt_path(WCHAR *ini, int len);
extern bool   WINAPI get_localdt_path(WCHAR *ini, int len);
extern bool   WINAPI get_process_path(WCHAR *path, int len);
extern bool   WINAPI get_process_flags(void);
extern DWORD  WINAPI get_process_pid(void);
extern bool   WINAPI get_process_remote(void);
extern void   WINAPI set_process_remote(bool flags);
extern void   WINAPI set_process_flags(bool flags);
extern void   WINAPI set_process_pid(uint32_t pid);

#ifdef __cplusplus
}
#endif 

#endif   /* end _SHARE_LOCK_H_ */