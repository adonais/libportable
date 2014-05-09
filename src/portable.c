#ifndef LIBPORTABLE_STATIC
#define TETE_BUILD
#endif

#include "portable.h"
#include "inipara.h"
#include "safe_ex.h"
#include "ice_error.h"
#include "bosskey.h"
#include "mhook-lib/mhook.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <process.h>
#include <stdio.h>


typedef HRESULT (WINAPI *_NtSHGetFolderPathW)(HWND hwndOwner,
									    int nFolder,
									    HANDLE hToken,
									    DWORD dwFlags,
									    LPWSTR pszPath);
typedef HRESULT (WINAPI *_NtSHGetSpecialFolderLocation)(HWND hwndOwner,
									    int nFolder,
									    LPITEMIDLIST *ppidl);
typedef BOOL (WINAPI *_NtSHGetSpecialFolderPathW)(HWND hwndOwner,
									    LPWSTR lpszPath,
									    int csidl,
									    BOOL fCreate);

static  HANDLE  env_thread;
static  WNDINFO ff_info;
static  WCHAR   appdata_path[VALUE_LEN+1];	
static  WCHAR   localdata_path[VALUE_LEN+1];

static _NtSHGetFolderPathW				TrueSHGetFolderPathW				= NULL;
static _NtSHGetSpecialFolderLocation	TrueSHGetSpecialFolderLocation		= NULL;
static _NtSHGetSpecialFolderPathW		TrueSHGetSpecialFolderPathW			= NULL;

/* Asm replacment for memset */
void * __cdecl memset_nontemporal_tt ( void *dest, int c, size_t count )
{
	return A_memset(dest, c, count);
}

/* Never used,to be compatible with tete's patch */
uint32_t GetNonTemporalDataSizeMin_tt( void )
{
	return 0;
}

intptr_t GetAppDirHash_tt( void )
{
	return 0;
}

BOOL WINAPI WaitWriteFile(LPCWSTR ap_path)
{
	BOOL  ret = FALSE;
	WCHAR profile_path[MAX_PATH+1] = {0};
	BOOL  pname = is_thunderbird();
	if (pname)
	{
		_snwprintf(profile_path,MAX_PATH,L"%ls%ls",ap_path,L"\\Thunderbird\\profiles.ini");
	}
	else
	{
		_snwprintf(profile_path,MAX_PATH,L"%ls%ls",ap_path,L"\\Mozilla\\Firefox\\profiles.ini");
	}
	if ( PathFileExistsW(profile_path) )
	{
		if (pname)
		{
			ret = WritePrivateProfileStringW(L"Profile0",L"Path",L"../../",profile_path);
		}
		else
		{
			ret = WritePrivateProfileStringW(L"Profile0",L"Path",L"../../../",profile_path);
		}
	}
	else
	{
		LPWSTR szDir;
		if ( (szDir = (LPWSTR)SYS_MALLOC( sizeof(profile_path) ) ) != NULL )
		{
			wcsncpy (szDir, profile_path, MAX_PATH);
			PathRemoveFileSpecW( szDir );
			SHCreateDirectoryExW(NULL,szDir,NULL);
			SYS_FREE(szDir);
			WritePrivateProfileSectionW(L"General",L"StartWithLastProfile=1\r\n\0",profile_path);
			if (pname)
			{
				ret = WritePrivateProfileSectionW(L"Profile0",L"Name=default\r\nIsRelative=1\r\nPath=../../\r\nDefault=1\r\n\0" \
												,profile_path);
			}
			else
			{
				ret = WritePrivateProfileSectionW(L"Profile0",L"Name=default\r\nIsRelative=1\r\nPath=../../../\r\nDefault=1\r\n\0" \
												,profile_path);
			}
		}
	}
	return ret;
}

/* 初始化全局变量 */
unsigned WINAPI init_global_env(void * pParam)
{
	BOOL diff = read_appint(L"General", L"Nocompatete") > 0;
	if ( read_appkey(L"General",L"PortableDataPath",appdata_path,sizeof(appdata_path)) )
	{ 
		/* 如果ini文件里的appdata设置路径为相对路径 */
		if (appdata_path[1] != L':')
		{
			PathToCombineW(appdata_path,VALUE_LEN);
		}
		/* 处理localdata变量 */
		if ( !read_appkey(L"Env",L"TmpDataPath",localdata_path,sizeof(appdata_path)) )
		{
			wcsncpy(localdata_path,appdata_path,VALUE_LEN);
		}
		/* 修正相对路径问题 */
		if (localdata_path[1] != L':')
		{
			PathToCombineW(localdata_path,VALUE_LEN);
		}
		/* 为appdata建立目录 */
		charTochar(appdata_path);
		wcsncat(appdata_path,L"\\AppData",VALUE_LEN);
		SHCreateDirectoryExW(NULL,appdata_path,NULL);
		/* 为localdata建立目录 */
		charTochar(localdata_path);
		wcsncat(localdata_path,L"\\LocalAppData\\Temp\\Fx",VALUE_LEN);
		SHCreateDirectoryExW(NULL,localdata_path,NULL);
		if ( diff )
		{
			WaitWriteFile(appdata_path);
		}
	}
	return (unsigned)diff;
}

HRESULT WINAPI HookSHGetSpecialFolderLocation(HWND hwndOwner,
											  int nFolder,
											  LPITEMIDLIST *ppidl)								
{  
	int folder = nFolder & 0xff;
	if ( CSIDL_APPDATA == folder || CSIDL_LOCAL_APPDATA == folder )
	{  
		LPITEMIDLIST pidlnew = NULL;
		HRESULT result = 0L;
		if ( appdata_path[0] == L'\0' )
		{
			return TrueSHGetSpecialFolderLocation(hwndOwner, nFolder, ppidl);
		}
		if (CSIDL_LOCAL_APPDATA == folder)
		{
			result = SHILCreateFromPath( localdata_path, &pidlnew, NULL);
		}
		else
		{
			result = SHILCreateFromPath(appdata_path, &pidlnew, NULL);
		}
		if (result == S_OK)
		{
			*ppidl = pidlnew;
			return result;
		}
	}
	return TrueSHGetSpecialFolderLocation(hwndOwner, nFolder, ppidl);
}

HRESULT WINAPI HookSHGetFolderPathW(HWND hwndOwner,int nFolder,HANDLE hToken,
									DWORD dwFlags,LPWSTR pszPath)								
{  
	UINT_PTR	dwCaller;
	BOOL        dwFf = FALSE;
	int			folder = nFolder & 0xff;
	HRESULT     ret = E_FAIL;
#ifndef LIBPORTABLE_STATIC
	WCHAR		dllname[VALUE_LEN+1];
	GetModuleFileNameW(dll_module, dllname, VALUE_LEN);
#endif
#ifdef __GNUC__
	dwCaller = (UINT_PTR)__builtin_return_address(0);
#else
	dwCaller = (UINT_PTR)_ReturnAddress();
#endif
#ifndef LIBPORTABLE_STATIC
	dwFf = is_specialdll(dwCaller, dllname)       ||
		   is_specialdll(dwCaller, L"*\\xul.dll") ||
		   is_specialdll(dwCaller, L"*\\npswf*.dll");
#else
	dwFf = is_specialdll(dwCaller, L"*\\xul.dll") || is_specialdll(dwCaller, L"*\\npswf*.dll");
#endif
	if (env_thread)
	{
		WaitForSingleObject(env_thread,1500);
		CloseHandle(env_thread);
		env_thread = NULL;
	}
	if ( dwFf )
	{
		switch (folder)
		{
			int	 num = 0;
			case CSIDL_APPDATA:
			{
				num = _snwprintf(pszPath,MAX_PATH,L"%ls",appdata_path);
				ret = S_OK;
				break;
			}
			case CSIDL_LOCAL_APPDATA:
			{
				num = _snwprintf(pszPath,MAX_PATH,L"%ls",localdata_path);
				ret = S_OK;
				break;
			}
			default:
				break;
		}
	}
	if (S_OK != ret)
	{
		ret = TrueSHGetFolderPathW(hwndOwner, nFolder, hToken, dwFlags, pszPath);
	}
	return ret;
}

BOOL WINAPI HookSHGetSpecialFolderPathW(HWND hwndOwner,LPWSTR lpszPath,int csidl,BOOL fCreate)                                                      
{
	UINT_PTR	dwCaller;
	BOOL        internal;
#ifdef __GNUC__
	dwCaller = (UINT_PTR)__builtin_return_address(0);
#else
	dwCaller = (UINT_PTR)_ReturnAddress();
#endif
	internal = is_specialdll(dwCaller, L"*\\xul.dll") || is_specialdll(dwCaller, L"*\\npswf*.dll");
	if ( !internal )
	{
		return TrueSHGetSpecialFolderPathW(hwndOwner,lpszPath,csidl,fCreate);
	}
    return (HookSHGetFolderPathW(
			hwndOwner,
			csidl + (fCreate ? CSIDL_FLAG_CREATE : 0),
			NULL,
			0,
			lpszPath)) == S_OK ? TRUE : FALSE;
}


unsigned WINAPI init_portable(void * pParam)
{
	HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
	if (hShell32 != NULL)
	{
		TrueSHGetFolderPathW = (_NtSHGetFolderPathW)GetProcAddress(hShell32,
								"SHGetFolderPathW");
		TrueSHGetSpecialFolderPathW = (_NtSHGetSpecialFolderPathW)GetProcAddress(hShell32,
                                       "SHGetSpecialFolderPathW");
		TrueSHGetSpecialFolderLocation = (_NtSHGetSpecialFolderLocation)GetProcAddress(hShell32,
										 "SHGetSpecialFolderLocation");
	}
	/* hook 下面几个函数 */ 
	if (TrueSHGetSpecialFolderLocation)
	{
		Mhook_SetHook((PVOID*)&TrueSHGetSpecialFolderLocation, (PVOID)HookSHGetSpecialFolderLocation);
	}
	if (TrueSHGetFolderPathW)
	{
		Mhook_SetHook((PVOID*)&TrueSHGetFolderPathW, (PVOID)HookSHGetFolderPathW);
	}
    if (TrueSHGetSpecialFolderPathW)
    {
        Mhook_SetHook((PVOID*)&TrueSHGetSpecialFolderPathW, (PVOID)HookSHGetSpecialFolderPathW);
    }
	return (1);
}

/* uninstall hook and clean up */
void WINAPI undo_it(void)
{
	if (ff_info.atom_str)
	{
		UnregisterHotKey(NULL, ff_info.atom_str);
		GlobalDeleteAtom(ff_info.atom_str);
	}
	if (g_handle[0]>0)
	{
		int i;
		for ( i =0 ; i<PROCESS_NUM && g_handle[i]>0 ; ++i )
		{
			TerminateProcess(g_handle[i], (DWORD)-1);
			CloseHandle(g_handle[i]);
		}
		refresh_tray();
	}
	if (TrueSHGetFolderPathW)
	{
		Mhook_Unhook((PVOID*)&TrueSHGetFolderPathW);
	}
    if (TrueSHGetSpecialFolderPathW)
    {
        Mhook_Unhook((PVOID*)&TrueSHGetSpecialFolderPathW);
    }
	if (TrueSHGetSpecialFolderLocation)
	{
		Mhook_Unhook((PVOID*)&TrueSHGetSpecialFolderLocation);
	}
	jmp_end();
	safe_end();
	return;
}

void WINAPI do_it(void)
{
	HANDLE		 hc = NULL;
	UINT_PTR	 dwCaller = 0;
#ifdef _LOGDEBUG
	if ( GetEnvironmentVariableA("APPDATA",logfile_buf,MAX_PATH) > 0 )
	{
		strncat(logfile_buf,"\\",1);
		strncat(logfile_buf,LOG_FILE,strlen((LPCSTR)LOG_FILE));
	}
#endif
	if (!dll_module)
	{
	#ifdef __GNUC__
		dwCaller = (UINT_PTR)__builtin_return_address(0);
	#else
		dwCaller = (UINT_PTR)_ReturnAddress();
	#endif
	}
	if (dwCaller)
	{
		GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)dwCaller, &dll_module);
	}
	if ( read_appint(L"General",L"SafeEx") > 0 )
	{
		init_safed(NULL);
	}
	if ( read_appint(L"General", L"Portable") > 0 )
	{
		env_thread = (HANDLE)_beginthreadex(NULL,0,&init_global_env,NULL,0,NULL);
		if (env_thread) 
		{
			SetThreadPriority(env_thread,THREAD_PRIORITY_HIGHEST);
			init_portable(NULL);
		}
	}
	if ( is_browser() || is_thunderbird() )
	{
		if ( read_appint(L"General",L"GdiBatchLimit") > 0 )
		{
			hc = OpenThread(THREAD_ALL_ACCESS, 0, GetCurrentThreadId());
			if (hc)
			{
				CloseHandle((HANDLE)_beginthreadex(NULL,0,&GdiSetLimit_tt,hc,0,NULL));
			}
		}
		if ( read_appint(L"General",L"ProcessAffinityMask") > 0 )
		{
			hc = OpenThread(THREAD_ALL_ACCESS, 0, GetCurrentThreadId());
			if (hc)
			{
				CloseHandle((HANDLE)_beginthreadex(NULL,0,&SetCpuAffinity_tt,hc,0,NULL));
			}
		}
		if ( read_appint(L"General",L"CreateCrashDump") > 0 )
		{
			CloseHandle((HANDLE)_beginthreadex(NULL,0,&init_exeception,NULL,0,NULL));
		}
		if ( read_appint(L"General", L"Bosskey") > 0 )
		{
			CloseHandle((HANDLE)_beginthreadex(NULL,0,&bosskey_thread,&ff_info,0,NULL));
		}
		CloseHandle((HANDLE)_beginthreadex(NULL,0,&SetPluginPath,NULL,0,NULL));
		if ( read_appint(L"General", L"ProxyExe") > 0 )
		{
			CloseHandle((HANDLE)_beginthreadex(NULL,0,&run_process,NULL,0,NULL));
		}
	}
}

/* This is standard DllMain function. */
#ifdef __cplusplus
extern "C" {
#endif 

#if defined(LIBPORTABLE_EXPORTS) || !defined(LIBPORTABLE_STATIC)
int CALLBACK _DllMainCRTStartup(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
    switch(dwReason) 
	{
		case DLL_PROCESS_ATTACH:
			dll_module = (HMODULE)hModule;
			DisableThreadLibraryCalls(hModule);
			do_it();
			break;
		case DLL_PROCESS_DETACH:
			undo_it();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		default:
			return FALSE;
	}
	return TRUE;
}
#endif  /* LIBPORTABLE_EXPORTS */

#ifdef __cplusplus
}
#endif 
