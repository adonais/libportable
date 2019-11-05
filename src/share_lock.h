#ifndef _SHARE_LOCK_H_
#  define _SHARE_LOCK_H_

typedef struct _s_data
{
    WCHAR    appdt[MAX_PATH+1];
    WCHAR    localdt[MAX_PATH+1];
    WCHAR    process[MAX_PATH+1];
    WCHAR    ini[MAX_PATH+1];
} s_data;

#ifdef __cplusplus
extern "C" {
#endif

extern s_data sdata;
extern bool   WINAPI get_env_status(LPCWSTR env);
extern bool   WINAPI get_ini_path(WCHAR *ini, int len);
extern bool   WINAPI get_appdt_path(WCHAR *ini, int len);
extern bool   WINAPI get_localdt_path(WCHAR *ini, int len);
extern bool   WINAPI get_process_path(WCHAR *path, int len);

#ifdef __cplusplus
}
#endif 

#endif   /* end _SHARE_LOCK_H_ */