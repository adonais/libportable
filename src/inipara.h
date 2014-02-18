#ifndef _INI_PARA_H_
#  define _INI_PARA_H_

#ifdef INI_EXTERN
#  undef INI_EXTERN
#  define INI_EXTERN
#else
#  define INI_EXTERN extern
#endif

#include <windows.h>

#define   SYS_MALLOC(x)		 HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (x))
#define   SYS_FREE(x)		 HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, (x))

#define	  EXCLUDE_NUM 26					/* 白名单个数(数组最大行数) */
#define   VALUE_LEN 128                     /* 保存值的最大长度 */
#define   BUFSIZE   (MAX_PATH*2)

INI_EXTERN wchar_t profile_path[MAX_PATH+1];               /* only init once */
INI_EXTERN HMODULE dll_module;                             /* portable module addr */

#ifdef _LOGDEBUG
#define LOG_FILE	"run_hook.log"
INI_EXTERN  char    logfile_buf[VALUE_LEN+1];
extern
#ifdef __cplusplus
"C"
#endif
void __cdecl logmsg(const char * format, ...);
#endif

extern 
#ifdef __cplusplus
"C"
#endif
void * __cdecl A_memset(void * dest, int c, size_t count); /*asm memset funciton */

#define fzero(b,len) (A_memset((b),'\0',(len)),(void)0)

#ifdef __cplusplus
extern "C" {
#endif 

INI_EXTERN DWORD WINAPI GetOsVersion(void);
INI_EXTERN void WINAPI charTochar(LPWSTR path);
INI_EXTERN BOOL PathToCombineW(IN LPWSTR lpfile, IN size_t str_len);
INI_EXTERN LPWSTR stristrW(LPCWSTR Str, LPCWSTR Pat);
INI_EXTERN BOOL WINAPI ini_ready(LPWSTR inifull_name,DWORD buf_len);
INI_EXTERN BOOL read_appkey(LPCWSTR lpappname,              /* 区段名 */
				 LPCWSTR lpkey,							    /* 键名  */	
				 LPWSTR  prefstring,						/* 保存值缓冲区 */	
				 DWORD   bufsize							/* 缓冲区大小 */
				 );
INI_EXTERN int read_appint(LPCWSTR cat, LPCWSTR name);
INI_EXTERN BOOL foreach_section(LPCWSTR cat, wchar_t (*lpdata)[VALUE_LEN+1], int m);
INI_EXTERN BOOL is_nplugins(void);
INI_EXTERN BOOL is_thunderbird(void);
INI_EXTERN BOOL is_browser(void);
INI_EXTERN BOOL WINAPI is_specialdll(UINT_PTR callerAddress,LPCWSTR dll_file);
INI_EXTERN unsigned WINAPI SetCpuAffinity_tt(void * pParam);
INI_EXTERN unsigned WINAPI GdiSetLimit_tt(void * pParam);
INI_EXTERN BOOL WINAPI IsGUI(LPCWSTR lpFileName);
INI_EXTERN BOOL WINAPI GetCurrentProcessName(LPWSTR lpstrName, DWORD wlen);

#ifdef __cplusplus
}
#endif 

#endif   /* end _INI_PARA_H_ */