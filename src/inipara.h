#ifndef _INI_PARA_H_
#  define _INI_PARA_H_

#include <windows.h>
#include <stdint.h>
#include "intrin_c.h"

#define   SYS_MALLOC(x)		 HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (x))
#define   SYS_FREE(x)		 (HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, (x)),(x = NULL))

#define   EXCLUDE_NUM 32                    /* 白名单个数(数组最大行数) */
#define   VALUE_LEN 128                     /* 保存值的最大长度 */
#define   BUFSIZE   (MAX_PATH*2)
#define   LOCK_SPIN_COUNT 1500
#define   SIZE_OF_NT_SIGNATURE  sizeof (DWORD)
#define   NAMES_LEN             64
#define   goodHandle(m_handle) ( (m_handle != NULL) && (m_handle != INVALID_HANDLE_VALUE) )

#if defined(__GNUC__)
#define SHARED __attribute__((section(".shrd"), shared))
#define USERED __attribute__ ((__used__))
#else
#define SHARED
#define USERED extern
#endif

#ifdef _LOGDEBUG
#define LOG_FILE    "run_hook.log"
#endif

#define fzero(b,len)  (__stosb((LPBYTE)(b), '\0', (len)))

typedef HMODULE (WINAPI *LoadLibraryExWPtr)(LPCWSTR lpFileName,HANDLE hFile,DWORD dwFlags);
typedef struct tagWNDINFO
{
    int   atom_str;
    int   key_mod;
    int   key_vk;
    DWORD hPid;
    HWND  hFF;
} WNDINFO, *LPWNDINFO;

#ifdef __cplusplus
extern "C" {
#endif 

extern LoadLibraryExWPtr sLoadLibraryExWStub;
extern HMODULE           dll_module; 
#ifdef _LOGDEBUG
extern void     __cdecl logmsg(const char * format, ...);
#endif  /* _LOGDEBUG */
extern DWORD    WINAPI GetOsVersion(void);
extern bool     WINAPI PathToCombineW(LPWSTR lpfile, int str_len);
extern LPWSTR   WINAPI stristrW(LPCWSTR Str, LPCWSTR Pat);
extern bool     WINAPI init_parser(LPWSTR inifull_name,DWORD buf_len);
extern bool     WINAPI read_appkey(LPCWSTR lpappname,           /* 区段名 */
                                   LPCWSTR lpkey,               /* 键名  */
                                   LPWSTR  prefstring,          /* 保存值缓冲区 */
                                   DWORD   bufsize,             /* 缓冲区大小 */
                                   void*   filename             /* 文件名,默认为空 */
                                   );
extern int      WINAPI read_appint(LPCWSTR cat, LPCWSTR name);
extern bool     WINAPI foreach_section(LPCWSTR cat,                     /* ini 区段 */
                                       wchar_t(*lpdata)[VALUE_LEN+1],   /* 二维数组首地址,保存多个段值 */
                                       int line                         /* 二维数组行数 */
                                       );
extern bool     WINAPI is_specialapp(LPCWSTR appname);
extern bool     WINAPI is_browser(void);
extern bool     WINAPI get_mozprofiles_path(LPCWSTR, wchar_t*, int);
extern bool     WINAPI GetCurrentWorkDir(LPWSTR lpstrName, DWORD wlen);
extern bool     WINAPI WaitWriteFile(LPCWSTR app_path);
extern bool     WINAPI is_specialdll(uintptr_t callerAddress,LPCWSTR dll_file);
extern HWND     WINAPI get_moz_hwnd(LPWNDINFO pInfo);
extern bool     WINAPI IsGUI(LPCWSTR lpFileName);
extern bool     WINAPI exists_dir(LPCWSTR path);
extern bool     WINAPI create_dir(LPCWSTR full_path);

#ifdef __cplusplus
}
#endif 

#endif   /* end _INI_PARA_H_ */
