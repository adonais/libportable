#ifndef _INI_PARA_H_
#  define _INI_PARA_H_

#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include <intrin.h>

#define   SYS_MALLOC(x) (HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (x)))
#define   SYS_FREE(x)   (HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, (x)),(x = NULL))

#define   EXCLUDE_NUM 32                    /* 白名单个数(数组最大行数) */
#define   VALUE_LEN 128                     /* 保存值的最大长度 */
#define   BUFSIZE   (MAX_PATH*2)
#define   MAX_BUFF  1024
#define   LOCK_SPIN_COUNT 1500
#define   SIZE_OF_NT_SIGNATURE  sizeof (DWORD)
#define   NAMES_LEN             64
#define   goodHandle(m_handle) ((m_handle != NULL) && (m_handle != INVALID_HANDLE_VALUE))

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

#if defined(_MSC_VER) && (_MSC_VER >= 1500)
#pragma intrinsic(_InterlockedCompareExchange, _InterlockedExchange, \
                  __stosb, _ReturnAddress, strlen, wcslen, \
                  wcscmp, memcpy, memset)
#elif defined(__GNUC__)
#define _ReturnAddress() (__builtin_return_address(0))
#define _AddressOfReturnAddress() (&(((void **)(__builtin_frame_address(0)))[1]))
extern __inline__ __attribute__((__gnu_inline__, __always_inline__, __artificial__)) 
void __nop(void)
{
    __asm__ __volatile__("nop");
}
#else
#error Unsupported compiler
#endif

#define fzero(b, len) (__stosb((LPBYTE)(b), '\0', (len)))

extern LIB_INLINE bool is_wow64() { \
       int wow64 = 0; return IsWow64Process(GetCurrentProcess(), &wow64) ? (wow64 == 1 ? true : false) : false; \
}

typedef HMODULE (WINAPI *LoadLibraryExPtr)(LPCWSTR lpFileName,HANDLE hFile,DWORD dwFlags);
typedef struct tagWNDINFO
{
    int   atom_str;
    int   key_mod;
    int   key_vk;
    DWORD hPid;
    HWND  hFF;
} WNDINFO, *LPWNDINFO;

typedef enum
{
    MOZ_UNKOWN = 0,
    MOZ_ICEWEASEL,
    MOZ_ZEN,
    MOZ_FIREFOX,
    MOZ_BETA,
    MOZ_DEV,
    MOZ_NIGHTLY
} m_family;

#ifdef __cplusplus
extern "C" {
#endif 

extern m_family e_browser;
extern char ini_portable_path[MAX_PATH + 1];
extern WCHAR xre_profile_path[MAX_BUFF];
extern WCHAR xre_profile_local_path[MAX_BUFF];
extern LoadLibraryExPtr sLoadLibraryExStub;
extern HMODULE          dll_module;
extern bool creator_hook(void *target, void *func, void **original);
extern bool disable_hook(void **target);
extern bool remove_hook(void **target);

#ifdef _LOGDEBUG
extern void     __cdecl logmsg(const char * format, ...);
extern void     WINAPI  init_logs(void);
#endif  /* _LOGDEBUG */

extern int      WINAPI get_file_version(void);
extern LPWSTR   WINAPI wstr_replace(LPWSTR in, size_t in_size, LPCWSTR pattern, LPCWSTR by);
extern bool     WINAPI wget_process_directory(LPWSTR lpstrName, DWORD len);
extern bool     WINAPI wcreate_dir(LPCWSTR dir);
extern bool     WINAPI wexist_file(LPCWSTR path);
extern bool     WINAPI exists_dir(const char *path, int mode);
extern bool     WINAPI exists_file(const char *path, int mode);
extern bool     WINAPI create_dir(const char *dir);
extern bool     WINAPI path_to_absolute(LPWSTR lpfile, int str_len);
extern HWND     WINAPI get_moz_hwnd(LPWNDINFO pInfo);
extern bool     WINAPI is_gui(LPCWSTR lpFileName);
extern bool     WINAPI get_process_directory(char *name, uint32_t len);
extern bool     WINAPI is_specialdll(uintptr_t callerAddress,LPCWSTR dll_file);
extern bool     WINAPI is_flash_plugins(uintptr_t caller);
extern m_family WINAPI is_ff_official(void);
extern bool     WINAPI write_file(LPCWSTR appdt);
extern void     WINAPI write_json_file(LPCWSTR appdt);
extern bool     WINAPI check_arg(LPCWSTR warg, LPCWSTR sub1, LPCWSTR sub2);
extern bool     WINAPI get_appdt_path(WCHAR *ini, int len);
extern bool     WINAPI get_localdt_path(WCHAR *ini, int len);
extern DWORD    WINAPI get_os_version(void);
extern bool     WINAPI cmd_has_setup(void);
extern bool     WINAPI cmd_has_profile(char *pout, const int size);
extern bool     WINAPI is_specialapp(LPCWSTR appname);
extern bool     WINAPI get_process_name(LPWSTR name, DWORD wlen);
extern bool     WINAPI is_specialapp(LPCWSTR appname);

#ifdef __cplusplus
}
#endif 

#endif   /* end _INI_PARA_H_ */
