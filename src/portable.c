#define TETE_BUILD

#include "portable.h"
#include "ttf_list.h"
#include "safe_ex.h"
#include "ice_error.h"
#include "bosskey.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <process.h>
#include "mhook-lib/mhook.h"

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

unsigned WINAPI init_global_env(void * pParam)
{
	BOOL diff = (char *)pParam?TRUE:FALSE;
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
	wcsncat(localdata_path,L"\\LocalAppData\\Temp\\Fx",VALUE_LEN);
	SHCreateDirectoryExW(NULL,localdata_path,NULL);
	if ( diff )
	{
		WaitWriteFile(appdata_path);
	}
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
	typedef			 int (__cdecl *_pwrite_env)(LPCWSTR envstring);
	int				 ret = 0;
	HMODULE	 hCrt;
	_pwrite_env   Truewrite_env = NULL;
	LPWSTR		 lpstring;
#if (_MSC_VER == 1400) || (CRT_LINK == 1400)
	hCrt = GetModuleHandleW(L"msvcr80.dll");
#elif (_MSC_VER == 1500) || (CRT_LINK == 1500)
	hCrt = GetModuleHandleW(L"msvcr90.dll");
#elif (_MSC_VER == 1600) || (CRT_LINK == 1600)
	hCrt = GetModuleHandleW(L"msvcr100.dll");
#elif (_MSC_VER == 1700) || (CRT_LINK == 1700)
	hCrt = GetModuleHandleW(L"msvcr110.dll");
#elif (_MSC_VER == 1800) || (CRT_LINK == 1800)
	hCrt = GetModuleHandleW(L"msvcr120.dll");
#else
	#error MSCRT version is too low,not exist _wputenv function.
	hCrt = NULL;
#endif
	if ( hCrt )
	{
		Truewrite_env = (_pwrite_env)GetProcAddress(hCrt,"_wputenv");
	}
	if ( Truewrite_env )
	{
		if ( profile_path[1] != L':' )
		{
			if (!ini_ready(profile_path,MAX_PATH))
			{
				return ((unsigned)ret);
			}
		}
		if ( (lpstring = (LPWSTR)SYS_MALLOC(32767)) != NULL )
		{
			if ( (ret = GetPrivateProfileSectionW(L"Env", lpstring, 32767, profile_path)) > 0 )
			{
				LPWSTR	strKey = lpstring;
				while(*strKey != L'\0') 
				{
					if ( stristrW(strKey, L"NpluginPath") )
					{
						WCHAR lpfile[VALUE_LEN+1];
						if ( read_appkey(L"Env",L"NpluginPath",lpfile,sizeof(lpfile)) )
						{
							WCHAR env_string[VALUE_LEN+1] = {0};
							PathToCombineW(lpfile, VALUE_LEN);
							if ( _snwprintf(env_string,VALUE_LEN,L"%ls%ls",L"MOZ_PLUGIN_PATH=",lpfile) > 0)
							{
								ret = Truewrite_env( (LPCWSTR)env_string );
							}
						}
					}
					else if	(stristrW(strKey, L"TmpDataPath") ||
							 stristrW(strKey, L"DiyFontPath") )
					{
						;
					}
					else
					{
						ret = Truewrite_env( (LPCWSTR)strKey );
					}
					strKey += wcslen(strKey)+1;
				}
			}
			SYS_FREE(lpstring);
		}
	}
	return ( (unsigned)ret );
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
				BOOL		diff = read_appint(L"General", L"Nocompatete") > 0;
				HANDLE	h_thread = (HANDLE)_beginthreadex(NULL,0,&init_global_env,diff?&diff:NULL,0,NULL);
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
