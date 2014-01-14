#define TETE_BUILD

#include "portable.h"
#include "header.h"
#include "ttf_list.h"
#include "safe_ex.h"
#include "ice_error.h"
#include "bosskey.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <process.h>
#include "mhook-lib/mhook.h"
#ifdef _MSC_VER
  #include <stdarg.h>
#endif

#define SIZE_OF_NT_SIGNATURE		sizeof (DWORD)
#define CRT_LEN								100
#define MAX_ENV_SIZE					32767

HMODULE	dll_module				= NULL;

/* fonts list */
List    ttf_list 									= NULL;

static  WCHAR  appdata_path[VALUE_LEN+1];	
static  WCHAR  localdata_path[VALUE_LEN+1];

static _NtSHGetFolderPathW				TrueSHGetFolderPathW					= NULL;
static _NtSHGetSpecialFolderLocation	TrueSHGetSpecialFolderLocation		= NULL;
static _NtSHGetSpecialFolderPathW		TrueSHGetSpecialFolderPathW			= NULL;

#ifdef _LOGDEBUG
static char  logfile_buf[VALUE_LEN+1];
LPCSTR logname = "run_hook.log";

void __cdecl logmsg(const char * format, ...)
{
	va_list args;
	int		len	 ;
	char	  buffer[VALUE_LEN+3];
	va_start (args, format);
	len	 =	_vscprintf(format, args);
	if (len > 0 && len < VALUE_LEN && strlen(logfile_buf) > 0)
	{
		FILE	*pFile = NULL;
		len = _vsnprintf(buffer,len,format, args);
		buffer[len++] = '\n';
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
		#ifdef _LOGDEBUG
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
		int num ;
		BOOL ret =  TrueSHGetSpecialFolderPathW(hwndOwner,lpszPath,csidl,fCreate);
		if( CSIDL_APPDATA == csidl          ||
			(CSIDL_APPDATA|CSIDL_FLAG_CREATE)  == csidl
		   )
		{
		#ifdef _LOGDEBUG
			logmsg("SHGetSpecialFolderPath hook off.\n");
		#endif
			num = _snwprintf(lpszPath,MAX_PATH,L"%ls",appdata_path);
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

/* 从输入表查找CRT版本 */
BOOL find_msvcrt(char *crt_name,int len)
{
	BOOL			ret = FALSE;
	IMAGE_DOS_HEADER      *pDos;
	IMAGE_OPTIONAL_HEADER *pOptHeader;
	IMAGE_IMPORT_DESCRIPTOR    *pImport ;
	HMODULE hMod=GetModuleHandleW(NULL);
	if (!hMod)
	{
	#ifdef _LOGDEBUG
		logmsg("GetModuleHandleW false,hMod = 0\n");
	#endif
		return ret;
	}
	pDos = (IMAGE_DOS_HEADER *)hMod;
	pOptHeader = (IMAGE_OPTIONAL_HEADER *)( 
										(BYTE *)hMod 
                                       + pDos->e_lfanew
                                       + SIZE_OF_NT_SIGNATURE
									   + sizeof(IMAGE_FILE_HEADER)
                             );
	pImport = (IMAGE_IMPORT_DESCRIPTOR * )(
                                             (BYTE *)hMod
                                             + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
                      ) ;
	while( TRUE )
	{
		char*	pszDllName = NULL;
		char		name[CRT_LEN+1] = {0};
		IMAGE_THUNK_DATA *pThunk  = (PIMAGE_THUNK_DATA)(pImport->Characteristics);
		IMAGE_THUNK_DATA *pThunkIAT = (PIMAGE_THUNK_DATA)(pImport->FirstThunk);
		if(pThunk == 0 && pThunkIAT == 0) break;
		pszDllName = (char*)((BYTE*)hMod+pImport->Name);
	#ifdef _LOGDEBUG
		logmsg("dllname:  [%s]\n",(const char *)pszDllName);
	#endif
		if ( PathMatchSpecA(pszDllName,"msvcr*.dll") )
		{
			strncpy(name,pszDllName,CRT_LEN);
			strncpy(crt_name,CharLowerA(name),len);
			ret = TRUE;
			break;
		}
		pImport++;
	}
	return ret;
}

unsigned WINAPI SetPluginPath(void * pParam)
{
	typedef			 int (__cdecl *_pwrite_env)(LPCWSTR envstring);
	int				 ret = 0;
	HMODULE	 hCrt =NULL;
	_pwrite_env   write_env = NULL;
	char              *msvc_crt = (char *)pParam;
	LPWSTR		 lpstring;
	if ( (hCrt = GetModuleHandleA(msvc_crt)) == NULL )
	{
	#ifdef _LOGDEBUG
		logmsg("GetModuleHandleA false,error code [%lu] \n",GetLastError());
	#endif
		return (0);
	}
	write_env = (_pwrite_env)GetProcAddress(hCrt,"_wputenv");
	if ( write_env )
	{
		if ( profile_path[1] != L':' )
		{
			if (!ini_ready(profile_path,MAX_PATH))
			{
				return ((unsigned)ret);
			}
		}
		if ( (lpstring = (LPWSTR)SYS_MALLOC(MAX_ENV_SIZE)) != NULL )
		{
			if ( (ret = GetPrivateProfileSectionW(L"Env", lpstring, MAX_ENV_SIZE-1, profile_path)) > 0 )
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
								ret = write_env( (LPCWSTR)env_string );
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
						ret = write_env( (LPCWSTR)strKey );
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

#if defined(LIBPORTABLE_EXPORTS) && defined(_MSC_VER)
int CALLBACK _DllMainCRTStartup(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
#else
BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
#endif
{
	static WNDINFO ff_info;
    switch(dwReason) 
	{
		static char msvc_crt[CRT_LEN+1];
		case DLL_PROCESS_ATTACH:
		{
			HANDLE		 hc = NULL;
			dll_module = (HMODULE)hModule;
			DisableThreadLibraryCalls(hModule);
		#ifdef _LOGDEBUG
			if ( SHGetSpecialFolderPathA(NULL,logfile_buf,CSIDL_APPDATA,FALSE) )
			{
				strncat(logfile_buf,"\\",1);
				strncat(logfile_buf,logname,strlen(logname));
			}
		#endif
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
			if ( find_msvcrt(msvc_crt,CRT_LEN) )
			{
				CloseHandle((HANDLE)_beginthreadex(NULL,0,&SetPluginPath,(void *)msvc_crt,0,NULL));
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
