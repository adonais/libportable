#ifndef _INI_PARA_H_
#  define _INI_PARA_H_

#include <stdbool.h>
#include <windows.h>

#define   VALUE_LEN 128
#define   BUFSIZE   (MAX_PATH*2)
#define   MAX_BUFFER 1024
#define   LOCK_SPIN_COUNT 1500
#define   SIZE_OF_NT_SIGNATURE  sizeof (DWORD)
#define   NAMES_LEN             64

#if defined(__GNUC__)
#define SHARED __attribute__((section(".shrd"), shared))
#define USERED __attribute__ ((__used__))
#else
#define SHARED
#define USERED extern
#endif

#ifdef _LOGDEBUG
#define LOG_FILE L"run_mpv.log"
#endif

#ifdef __cplusplus
extern "C" {
#endif 

#ifdef _LOGDEBUG
void WINAPI  init_logs(void);
void __cdecl logmsg(const char * format, ...);
#endif  /* _LOGDEBUG */

bool WINAPI init_parser(LPWSTR inifull_name, const int len);
bool WINAPI read_appkey(LPCWSTR lpappname,           /* 区段名 */
                        LPCWSTR lpkey,               /* 键名  */
                        LPWSTR  prefstring,          /* 保存值缓冲区 */
                        DWORD   bufsize,             /* 缓冲区大小 */
                        void*   filename             /* 文件名,默认为空 */
                        );
int  WINAPI read_appint(LPCWSTR cat, LPCWSTR name);
bool WINAPI exists_dir(LPCWSTR path);
bool WINAPI create_dir(LPCWSTR full_path);
bool WINAPI get_basedir(LPWSTR lpstrName, const int len);
WCHAR *WINAPI mp_make_u16(const char *utf8, WCHAR *utf16, int len);
char  *WINAPI mp_make_u8(const WCHAR *utf16, char *utf8, int len);

#ifdef __cplusplus
}
#endif 

#endif   /* end _INI_PARA_H_ */
