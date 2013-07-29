#define TETE_BUILD

#include "portable.h"
#include "ttf_list.h"
#include "safe_ex.h"
#include "ice_error.h"
#include "bosskey.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <process.h>
#ifdef _MSC_VER
#include "mhook-vs/include/mhook.h"
#else
#include "mhook-lib/mhook.h"
#endif

#ifdef _MSC_VER
#  pragma comment(lib, "kernel32.lib")
#  pragma comment(lib, "user32.lib")
#  pragma comment(lib, "shell32.lib")
#  pragma comment(lib, "shlwapi.lib")
#  pragma comment(lib, "gdi32.lib")
#endif

HMODULE	dll_module				= NULL;             /* dll module entry point */
List    ttf_list 				= NULL;             /* fonts list */
static  WCHAR  appdata_path[VALUE_LEN+1];			/* 自定义的appdata变量路径  */
static  WCHAR  localdata_path[VALUE_LEN+1];

#ifdef _DEBUG
static char  logfile_buf[VALUE_LEN+1];
const  char *logname = "run_hook.log";
#endif

typedef HRESULT (WINAPI *_NtSHGetFolderPath)(HWND hwndOwner,
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
typedef BOOL (WINAPI *_NtSHGetSpecialFolderPathA)(HWND hwndOwner,
									    LPSTR lpszPath,
									    int csidl,
									    BOOL fCreate);

static _NtSHGetFolderPath				TrueSHGetFolderPathW				= NULL;
static _NtSHGetSpecialFolderLocation	TrueSHGetSpecialFolderLocation		= NULL;
static _NtSHGetSpecialFolderPathA       TrueSHGetSpecialFolderPathA			= NULL;
static _NtSHGetSpecialFolderPathW		TrueSHGetSpecialFolderPathW			= NULL;

#ifdef _DEBUG
void __cdecl logmsg(const char * format, ...)
{
	va_list args;
	va_start (args, format);
	int len	 =	_vscprintf(format, args);
	if (len > 0 && strlen(logfile_buf) > 0)
	{
		char	buffer[len+3];
		FILE	*pFile = NULL;
		len = _vsnprintf(buffer,len,format, args);
		buffer[len] = '\0';
		if ( (pFile = fopen(logfile_buf,"a+")) != NULL )
		{
			fprintf(pFile,buffer);
			fclose(pFile);
		}
		va_end (args);
	}
	return;
}
#endif

TETE_EXT_CLASS  
uint32_t GetNonTemporalDataSizeMin_tt( void )
{
	return 0;
}

/* Never used,to be compatible with tete's patch */
TETE_EXT_CLASS 
void * __cdecl memset_nontemporal_tt ( void *dest, int c, size_t count )
{
	return memset(dest, c, count);
}

TETE_EXT_CLASS
intptr_t GetAppDirHash_tt( void )
{
	return 0;
}

void find_fonts_tolist(LPCWSTR parent)
{
    HANDLE h_file = NULL;
    WIN32_FIND_DATAW fd;
    WCHAR filepathname[VALUE_LEN+1] = {0};
    WCHAR sub[VALUE_LEN+1] = {0};
    if( parent[wcslen(parent) -1] != L'\\' )
        _snwprintf(filepathname,VALUE_LEN, L"%ls\\*.*", parent);
    else
        _snwprintf(filepathname,VALUE_LEN, L"%ls*.*", parent);
    h_file = FindFirstFileW(filepathname, &fd);
    if(h_file != INVALID_HANDLE_VALUE)
    {
        do
        {
			if(	wcscmp(fd.cFileName, L".") == 0 ||
				wcscmp(fd.cFileName, L"..")== 0 )
				continue;
            if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                int m = _snwprintf(sub,VALUE_LEN, L"%ls\\%ls",parent, fd.cFileName);
				sub[m] = L'\0';
                find_fonts_tolist(sub);
            }
            else if( PathMatchSpecW(fd.cFileName, L"*.ttf") ||
				     PathMatchSpecW(fd.cFileName, L"*.ttc") ||
				     PathMatchSpecW(fd.cFileName, L"*.otf") )
            {
                WCHAR font_path[VALUE_LEN+1] = {0};
                _snwprintf(font_path, VALUE_LEN, L"%s\\%s", parent, fd.cFileName);
				if (ttf_list)
				{
					add_node(font_path, ttf_list);
				}
            }
        } while(FindNextFileW(h_file, &fd) != 0 || GetLastError() != ERROR_NO_MORE_FILES);
        FindClose(h_file); h_file = NULL;
    }
	return;
}

void add_fonts_toapp(List *Li_header)
{
	PtrToNode *curr;
	for (curr = Li_header; *curr; )
	{
		Position ttf_element = *curr;
		DWORD	 numFonts = 0;
		if ( AddFontResourceExW(ttf_element->Element,FR_PRIVATE,&numFonts) )
		{
		#ifdef _DEBUG
			logmsg("AddFontResourceW ok\n");
		#endif
		}
		*curr = ttf_element->Next; 
	}
}

unsigned WINAPI install_fonts(void * pParam)
{
	WCHAR fonts_path[VALUE_LEN+1];
	if ( read_appkey(L"Env",L"DiyFontPath",fonts_path,sizeof(fonts_path)) )
	{
		PathToCombineW(fonts_path,VALUE_LEN);
		if ( PathFileExistsW(fonts_path) )
		{
			/* 初始化字体存储链表 */
			struct	Node fonts_header;
			ttf_list = init_listing( &fonts_header );
			if (ttf_list)
			{
				find_fonts_tolist(fonts_path);
				add_fonts_toapp(&ttf_list);
			}
		}
	}
	return (1);
}

void WINAPI uninstall_fonts(List *PtrLi)
{
	PtrToNode *curr;
	for (curr = PtrLi; *curr; )
	{
		Position pfonts = *curr; 
		if ( wcslen(pfonts->Element)>0 )
		{
			RemoveFontResourceExW(pfonts->Element,FR_PRIVATE,NULL);
		}
        *curr = pfonts->Next; 
        if (pfonts)
        {
			SYS_FREE(pfonts);
        }
	}
}

unsigned WINAPI init_global_env(void * pParam)
{
	if ( !read_appkey(L"General",L"PortableDataPath",appdata_path,sizeof(appdata_path)) )
	{
		return (0);
	} 
	/* 如果ini文件里的appdata设置路径为相对路径 */
	if (appdata_path[1] != L':')
	{
		if ( !PathToCombineW(appdata_path,VALUE_LEN) )
		{
			appdata_path[0] = L'\0';
			return (0);
		}
	}
	/* 处理localdata变量 */
	if ( !read_appkey(L"Env",L"TmpDataPath",localdata_path,sizeof(appdata_path)) )
	{
		wcsncpy(localdata_path,appdata_path,VALUE_LEN);
	}
	/* 修正相对路径问题 */
	if (localdata_path[1] != L':')
	{
		if ( !PathToCombineW(localdata_path,VALUE_LEN) )
		{
			localdata_path[0] = L'\0';
			return (0);
		}
	}
	/* 为appdata建立目录 */
	charTochar(appdata_path);
	wcsncat(appdata_path,L"\\AppData",VALUE_LEN);
	SHCreateDirectoryExW(NULL,appdata_path,NULL);
	/* 为localdata建立目录 */
	charTochar(localdata_path);
	wcsncat(localdata_path,L"\\LocalAppData",VALUE_LEN);
	SHCreateDirectoryExW(NULL,localdata_path,NULL);
	return (1);
}

HRESULT WINAPI HookSHGetSpecialFolderLocation(HWND hwndOwner,
											  int nFolder,
											  LPITEMIDLIST *ppidl)								
{  
	int folder = nFolder & 0xff;
	if (CSIDL_APPDATA == folder || CSIDL_LOCAL_APPDATA == folder)
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
	int folder = nFolder & 0xff;
	if( CSIDL_APPDATA == folder			||
		CSIDL_LOCAL_APPDATA == folder
	  )
	{  
		UINT_PTR	dwCaller;
		int			num = 0;
	#ifdef __GNUC__
		dwCaller = (UINT_PTR)__builtin_return_address(0);
	#else
		dwCaller = (UINT_PTR)_ReturnAddress();
	#endif
		if ( ( CSIDL_LOCAL_APPDATA == folder && !is_nplugins() ) ||
			 ( IsSpecialDll(dwCaller, L"*\\np*.dll") )
			)
		{
			num = _snwprintf(pszPath,MAX_PATH,L"%ls",localdata_path);
			pszPath[num] = L'\0';
			return S_OK;
		}
	}
	return TrueSHGetFolderPathW(hwndOwner, nFolder, hToken,dwFlags,pszPath);
}

BOOL WINAPI HookSHGetSpecialFolderPathW(HWND hwndOwner,LPWSTR lpszPath,int csidl,BOOL fCreate)                                                      
{
	if ( !is_nplugins() )
	{
		BOOL ret =  TrueSHGetSpecialFolderPathW(hwndOwner,lpszPath,csidl,fCreate);
		if( CSIDL_APPDATA == csidl          ||
			(CSIDL_APPDATA|CSIDL_FLAG_CREATE)  == csidl
		   )
		{
		#ifdef _DEBUG
			logmsg("SHGetSpecialFolderPath hook off.\n");
		#endif
			int num = _snwprintf(lpszPath,MAX_PATH,L"%ls",appdata_path);
			lpszPath[num] = L'\0';
			if (TrueSHGetSpecialFolderPathW && ret)
			{
					Mhook_Unhook((PVOID*)&TrueSHGetSpecialFolderPathW);
					TrueSHGetSpecialFolderPathW = NULL;
			}
		}
		return ret;
	}
	return TrueSHGetSpecialFolderPathW(hwndOwner, lpszPath, csidl,fCreate);
}

unsigned WINAPI SetPluginPath(void * pParam)
{
	typedef		 errno_t (__cdecl *_pwrite_env)(LPCWSTR name,LPCWSTR value);
	typedef		 errno_t (__cdecl *_pfn_itow_s)(int value,wchar_t *buffer,size_t size,int radix);
	wchar_t		 lpfile[VALUE_LEN+1];
	wchar_t		 wstr[16];
	int          value;
	HMODULE		 hCrt;
	_pwrite_env  Truewrite_env = NULL;
	_pfn_itow_s  True_itow_s = NULL;
#if (_MSC_VER == 1600) || (CRT_LINK == 1600)
	hCrt = GetModuleHandleW(L"msvcr100.dll");
#elif (_MSC_VER == 1700) || (CRT_LINK == 1700)
	hCrt = GetModuleHandleW(L"msvcr110.dll");
#else
	#error MSCRT version is too low,not exist _wputenv_s function.
	hCrt = NULL;
#endif
	if (!hCrt)
	{
		return (0);
	}
	Truewrite_env = (_pwrite_env)GetProcAddress(hCrt,"_wputenv_s");
	True_itow_s = (_pfn_itow_s)GetProcAddress(hCrt,"_itow_s");
	if ( !Truewrite_env || !True_itow_s )
	{
		return (0);
	}
	if ( read_appkey(L"Env",L"NpluginPath",lpfile,sizeof(lpfile)) ||
		 read_appkey(L"Env",L"MOZ_PLUGIN_PATH",lpfile,sizeof(lpfile))
	    )
	{
		PathToCombineW(lpfile, VALUE_LEN);
		Truewrite_env(L"MOZ_PLUGIN_PATH", lpfile);
	}
	/* add a  MOZILLA_DISABLE_PLUGINS environment variable to Iceweasel
	   see Don-t-register-plugins-if-the-MOZILLA_DISABLE_PLUGIN.patch */
	if ( (value = read_appint(L"Env",L"MOZILLA_DISABLE_PLUGINS"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"MOZILLA_DISABLE_PLUGINS", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"MOZ_NO_REMOTE"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"MOZ_NO_REMOTE", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"MOZ_SAFE_MODE_RESTART"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"MOZ_SAFE_MODE_RESTART", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"MOZ_PURGE_CACHES"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"MOZ_PURGE_CACHES", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"MOZ_DISABLE_OOP_TABS"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"MOZ_DISABLE_OOP_TABS", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"MOZ_DISABLE_OOP_PLUGINS"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"MOZ_DISABLE_OOP_PLUGINS", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"MOZ_GFX_SPOOF_VENDOR_ID"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,10,10) == 0 )
		{
			Truewrite_env(L"MOZ_GFX_SPOOF_VENDOR_ID", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"MOZ_GFX_SPOOF_DEVICE_ID"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,10,10) == 0 )
		{
			Truewrite_env(L"MOZ_GFX_SPOOF_DEVICE_ID", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"MOZ_GFX_SPOOF_WINDOWS_VERSION"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,10,10) == 0 )
		{
			Truewrite_env(L"MOZ_GFX_SPOOF_WINDOWS_VERSION", wstr);
		}
	}
	if ( read_appkey(L"Env",L"MOZ_GFX_SPOOF_DRIVER_VERSION",lpfile,sizeof(lpfile)) )
	{
		Truewrite_env(L"MOZ_GFX_SPOOF_DRIVER_VERSION", lpfile);
	}
	if ( (value = read_appint(L"Env",L"MOZ_WEBGL_PREFER_EGL"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"MOZ_WEBGL_PREFER_EGL", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"MOZ_WEBGL_FORCE_OPENGL"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"MOZ_WEBGL_FORCE_OPENGL", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"JSIMD_FORCEMMX"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"JSIMD_FORCEMMX", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"JSIMD_FORCE3DNOW"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"JSIMD_FORCE3DNOW", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"JSIMD_FORCESSE"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"JSIMD_FORCESSE", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"JSIMD_FORCESSE2"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"JSIMD_FORCESSE2", wstr);
		}
	}
	if ( (value = read_appint(L"Env",L"XRE_START_OFFLINE"))>=0 ) 
	{
		if ( True_itow_s(value,wstr,2,10) == 0 )
		{
			Truewrite_env(L"XRE_START_OFFLINE", wstr);
		}
	}
	if ( read_appkey(L"Env",L"XRE_PROFILE_NAME",lpfile,sizeof(lpfile)) )
	{
		Truewrite_env(L"XRE_PROFILE_NAME", lpfile);
	}
	if ( read_appkey(L"Env",L"XRE_PROFILE_PATH",lpfile,sizeof(lpfile)) )
	{
		PathToCombineW(lpfile, VALUE_LEN);
		Truewrite_env(L"XRE_PROFILE_PATH", lpfile);
	}
	if ( read_appkey(L"Env",L"XRE_PROFILE_LOCAL_PATH",lpfile,sizeof(lpfile)) )
	{
		PathToCombineW(lpfile, VALUE_LEN);
		Truewrite_env(L"XRE_PROFILE_LOCAL_PATH", lpfile);
	}
	if ( read_appkey(L"Env",L"XUL_APP_FILE",lpfile,sizeof(lpfile)) )
	{
		PathToCombineW(lpfile, VALUE_LEN);
		Truewrite_env(L"XUL_APP_FILE", lpfile);
	}
	return (1);
}

unsigned WINAPI init_portable(void * pParam)
{
	HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
	if (hShell32 != NULL)
	{
		TrueSHGetFolderPathW = (_NtSHGetFolderPath)GetProcAddress(hShell32,
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

/* uninstall hook */
void WINAPI hook_end(void)
{
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
	if (ttf_list)
	{
		uninstall_fonts(&ttf_list);
	}
	return;
}

/* This is standard DllMain function. */
#ifdef __cplusplus
extern "C" {
#endif 

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
	static WNDINFO ff_info;
    switch(dwReason) 
	{
		case DLL_PROCESS_ATTACH:
		{
			HANDLE		 hc = NULL;
			dll_module = (HMODULE)hModule;
			DisableThreadLibraryCalls(hModule);
		#ifdef _DEBUG
			TrueSHGetSpecialFolderPathA = (_NtSHGetSpecialFolderPathA)GetProcAddress
										  (GetModuleHandleW(L"shell32.dll"),"SHGetSpecialFolderPathA");
			if ( TrueSHGetSpecialFolderPathA && *logfile_buf == '\0' )
			{
				if ( TrueSHGetSpecialFolderPathA(NULL,logfile_buf,CSIDL_APPDATA,FALSE) )
				{
					strncat(logfile_buf,"\\",1);
					strncat(logfile_buf,logname,strlen(logname));
				}
			}
		#endif
			SetPluginPath(NULL);
			if ( read_appint(L"General",L"SafeEx") > 0 )
			{
				init_safed(NULL);
			}
			if ( read_appint(L"General", L"Portable") > 0 )
			{
				HANDLE h_thread = (HANDLE)_beginthreadex(NULL,0,&init_global_env,NULL,0,NULL);
				if (h_thread)
				{
					SetThreadPriority(h_thread,THREAD_PRIORITY_HIGHEST);
					CloseHandle(h_thread);
				}
				init_portable(NULL);
			}
			if ( read_appint(L"General",L"GdiBatchLimit") > 0 )
			{
				hc = OpenThread(THREAD_ALL_ACCESS, 0, GetCurrentThreadId());
				if (hc)
				{
					CloseHandle((HANDLE)_beginthreadex(NULL,0,&GdiSetLimit_tt,hc,0,NULL));
				}
			}
			CloseHandle((HANDLE)_beginthreadex(NULL,0,&install_fonts,NULL,0,NULL));
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
		}
			break;
		case DLL_PROCESS_DETACH:
			if (ff_info.atom_str)
			{
				UnregisterHotKey(NULL, ff_info.atom_str);
				GlobalDeleteAtom(ff_info.atom_str);
			}
			hook_end();
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

#ifdef __cplusplus
}
#endif 
