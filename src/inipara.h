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
#define LIB_INLINE inline __attribute__((__gnu_inline__))
#define SHARED __attribute__((section(".shrd"), shared))
#define USERED __attribute__ ((__used__))
#define ALIGNED32 __attribute((aligned (32)))
#else
#define LIB_INLINE __inline
#define SHARED
#define USERED extern
#define ALIGNED32 __declspec(align(32))
#endif

#ifdef _LOGDEBUG
#define LOG_FILE    "run_hook.log"
#endif

#define fzero(b,len)  (memset((LPBYTE)(b), '\0', (len)))
extern LIB_INLINE bool is_wow64() {int wow64=0; return IsWow64Process(GetCurrentProcess(),&wow64)?(wow64==1?true:false):false;}

typedef HMODULE (WINAPI *LoadLibraryExPtr)(LPCWSTR lpFileName,HANDLE hFile,DWORD dwFlags);
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

extern LoadLibraryExPtr sLoadLibraryExStub;
extern HMODULE          dll_module;
extern bool creator_hook(void* target, void* func, void **original);

#ifdef _LOGDEBUG
extern void     __cdecl logmsg(const char * format, ...);
extern void     WINAPI  init_logs(void);
#endif  /* _LOGDEBUG */

extern DWORD    WINAPI get_os_version(void);
extern bool     WINAPI path_to_absolute(LPWSTR lpfile, int str_len);
extern LPWSTR   WINAPI wstr_replace(LPWSTR in, size_t in_size, LPCWSTR pattern, LPCWSTR by);
extern bool     WINAPI read_appkey(LPCWSTR lpappname,           /* 区段名 */
                                   LPCWSTR lpkey,               /* 键名  */
                                   LPWSTR  prefstring,          /* 保存值缓冲区 */
                                   DWORD   bufsize,             /* 缓冲区大小 */
                                   void*   filename             /* 文件名,默认为空 */
                                   );
extern int      WINAPI read_appint(LPCWSTR cat, LPCWSTR name);
extern int      WINAPI get_file_version(void);
extern bool     WINAPI foreach_section(LPCWSTR cat,                     /* ini 区段 */
                                       wchar_t(*lpdata)[VALUE_LEN+1],   /* 二维数组首地址,保存多个段值 */
                                       int line                         /* 二维数组行数 */
                                       );
extern bool     WINAPI is_specialapp(LPCWSTR appname);
extern bool     WINAPI is_browser(void* path);
extern bool     WINAPI is_flash_plugins(uintptr_t caller);
extern bool     WINAPI getw_cwd(LPWSTR lpstrName, DWORD wlen);
extern unsigned WINAPI write_file(void * p);
extern bool     WINAPI is_specialdll(uintptr_t callerAddress,LPCWSTR dll_file);
extern HWND     WINAPI get_moz_hwnd(LPWNDINFO pInfo);
extern bool     WINAPI is_gui(LPCWSTR lpFileName);
extern bool     WINAPI exists_dir(LPCWSTR path);
extern bool     WINAPI create_dir(LPCWSTR full_path);
extern bool     WINAPI print_process_module(DWORD pid);
extern bool     WINAPI get_env_status(LPCWSTR env);
extern bool     WINAPI get_ini_path(WCHAR *ini, int len);
extern bool     WINAPI get_appdt_path(WCHAR *ini, int len);
extern bool     WINAPI get_localdt_path(WCHAR *ini, int len);

#ifdef __cplusplus
}
#endif 

#endif   /* end _INI_PARA_H_ */
