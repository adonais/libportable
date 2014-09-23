#ifndef LIBPORTABLE_STATIC
#define TETE_BUILD
#endif

#include "portable.h"
#include "inipara.h"
#ifndef DISABLE_SAFE
#include "safe_ex.h"
#endif
#include "ice_error.h"
#include "bosskey.h"
#include "minhook.h"
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
typedef void (CALLBACK *user_func)(void);

static  WNDINFO ff_info;
static _NtSHGetFolderPathW				TrueSHGetFolderPathW				= NULL;
static _NtSHGetSpecialFolderLocation	TrueSHGetSpecialFolderLocation		= NULL;
static _NtSHGetSpecialFolderPathW		TrueSHGetSpecialFolderPathW			= NULL;

/* 数据段共享锁,保证进程生存周期内只运行一次 */
#ifdef _MSC_VER
#pragma data_seg(".shrd")
#endif
int     RunOnce SHARED = 0;
int     RunPart SHARED = 0;
WCHAR   appdata_path[VALUE_LEN+1] SHARED = {0};
WCHAR   localdata_path[VALUE_LEN+1] SHARED = {0} ;
#ifdef _MSC_VER
#pragma data_seg()
#endif

/* Asm replacment for memset */
void * __cdecl memset_nontemporal_tt ( void *dest, int c, size_t count )
{
    return memset(dest, c, count);
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

BOOL WINAPI WaitWriteFile(LPCWSTR app_path)
{
    BOOL  ret = FALSE;
    WCHAR moz_profile[MAX_PATH+1];   
    if ( !get_mozilla_profile(app_path, moz_profile, MAX_PATH) )
    {
        return ret;
    }
    if ( PathFileExistsW(moz_profile) )
    {
        if ( is_thunderbird() )
        {
            ret = WritePrivateProfileStringW(L"Profile0",L"Path",L"../../",moz_profile);
        }
        else
        {
            ret = WritePrivateProfileStringW(L"Profile0",L"Path",L"../../../",moz_profile);
        }
    }
    else
    {
        LPWSTR szDir;
        if ( (szDir = (LPWSTR)SYS_MALLOC( sizeof(moz_profile) ) ) != NULL )
        {
            wcsncpy (szDir, moz_profile, MAX_PATH);
            PathRemoveFileSpecW( szDir );
            SHCreateDirectoryExW(NULL,szDir,NULL);
            SYS_FREE(szDir);
            WritePrivateProfileSectionW(L"General",L"StartWithLastProfile=1\r\n\0",moz_profile);
            if ( is_thunderbird() )
            {
                ret = WritePrivateProfileSectionW(L"Profile0",L"Name=default\r\nIsRelative=1\r\nPath=../../\r\nDefault=1\r\n\0" \
                                                  ,moz_profile);
            }
            else
            {
                ret = WritePrivateProfileSectionW(L"Profile0",L"Name=default\r\nIsRelative=1\r\nPath=../../../\r\nDefault=1\r\n\0" \
                                                  ,moz_profile);
            }
        }
    }
    return ret;
}

/* 初始化全局变量 */
void CALLBACK init_global_env(void)
{
    if ( RunOnce )
    {
        return;
    }
    /* 如果ini文件里的appdata设置路径为相对路径 */
    if (appdata_path[1] != L':')
    {
        PathToCombineW(appdata_path,VALUE_LEN);
    }
    if ( read_appkey(L"Env",L"TmpDataPath",localdata_path,sizeof(appdata_path)) )
    {
        /* 修正相对路径问题 */
        if (localdata_path[1] != L':')
        {
            PathToCombineW(localdata_path,VALUE_LEN);
        }
    }
    else
    {
        wcsncpy(localdata_path,appdata_path,VALUE_LEN);
    }
    if ( appdata_path[0] != L'\0' )
    {
        /* 为appdata建立目录 */
        wcsncat(appdata_path,L"\\AppData",VALUE_LEN);
        SHCreateDirectoryExW(NULL,appdata_path,NULL);
    }
    if ( localdata_path[0] != L'\0' )
    {
        /* 为localdata建立目录 */
        wcsncat(localdata_path,L"\\LocalAppData\\Temp\\Fx",VALUE_LEN);
        SHCreateDirectoryExW(NULL,localdata_path,NULL);
    }
    if ( read_appint(L"General", L"Nocompatete") > 0 )
    {
        WaitWriteFile(appdata_path);
    }
    RunOnce = 1;
    return;
}

HRESULT WINAPI HookSHGetSpecialFolderLocation(HWND hwndOwner,
        int nFolder,
        LPITEMIDLIST *ppidl)
{
    int folder = nFolder & 0xff;
    if ( CSIDL_APPDATA == folder || CSIDL_LOCAL_APPDATA == folder )
    {
        LPITEMIDLIST pidlnew = NULL;
        HRESULT result = S_FALSE;
        switch (folder)
        {
        case CSIDL_APPDATA:
        {
            if ( appdata_path[0] != L'\0' )
            {
                result = SHILCreateFromPath(appdata_path, &pidlnew, NULL);
            }
            break;
        }
        case CSIDL_LOCAL_APPDATA:
        {
            if (localdata_path[0] != L'\0' )
            {
                result = SHILCreateFromPath( localdata_path, &pidlnew, NULL);
            }
            break;
        }
        default:
            break;
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
    dwFf = is_specialdll(dwCaller, L"*\\xul.dll") ||
        #ifndef LIBPORTABLE_STATIC
           is_specialdll(dwCaller, dllname)       ||
        #endif
           is_specialdll(dwCaller, L"*\\npswf*.dll");
    if ( dwFf )
    {
        switch (folder)
        {
            int	 num = 0;
        case CSIDL_APPDATA:
        {
            if ( PathIsDirectoryW(appdata_path) )
            {
                num = _snwprintf(pszPath,MAX_PATH,L"%ls",appdata_path);
                ret = S_OK;
            }
            break;
        }
        case CSIDL_LOCAL_APPDATA:
        {
            if ( PathIsDirectoryW(localdata_path) )
            {
                num = _snwprintf(pszPath,MAX_PATH,L"%ls",localdata_path);
                ret = S_OK;
            }
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

void WINAPI init_portable(user_func init_env)
{
    /* 回调函数运行,初始化全局变量 */
    (*init_env)();                 
    /* hook 下面几个函数 */
    if ( MH_CreateHook(&SHGetSpecialFolderLocation, &HookSHGetSpecialFolderLocation, (LPVOID*)&TrueSHGetSpecialFolderLocation) == MH_OK )
    {
        if ( MH_EnableHook(&SHGetSpecialFolderLocation) != MH_OK )
        {
        #ifdef _LOGDEBUG
            logmsg("SHGetSpecialFolderLocation hook failed!\n");
        #endif
        }
    }
    if ( MH_CreateHook(&SHGetFolderPathW, &HookSHGetFolderPathW, (LPVOID*)&TrueSHGetFolderPathW) == MH_OK )
    {
        if ( MH_EnableHook(&SHGetFolderPathW) != MH_OK )
        {
        #ifdef _LOGDEBUG
            logmsg("SHGetFolderPathW hook failed!\n");
        #endif
        }
    }
    if ( MH_CreateHook(&SHGetSpecialFolderPathW, &HookSHGetSpecialFolderPathW, (LPVOID*)&TrueSHGetSpecialFolderPathW) == MH_OK )
    {
        if ( MH_EnableHook(&SHGetSpecialFolderPathW) != MH_OK )
        {
        #ifdef _LOGDEBUG
            logmsg("SHGetSpecialFolderPathW hook failed!\n");
        #endif
        }
    }
    return;
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
        MH_DisableHook(&SHGetFolderPathW);
    }
    if (TrueSHGetSpecialFolderPathW)
    {
        MH_DisableHook(&SHGetSpecialFolderPathW);
    }
    if (TrueSHGetSpecialFolderLocation)
    {
        MH_DisableHook(&SHGetSpecialFolderLocation);
    }
    jmp_end();
#ifndef DISABLE_SAFE
	safe_end();
#endif
	MH_Uninitialize();
    return;
}

void WINAPI do_it(void)
{
    /* 初始化日志记录文件 */
#ifdef _LOGDEBUG
    if ( GetEnvironmentVariableA("APPDATA",logfile_buf,MAX_PATH) > 0 )
    {
        strncat(logfile_buf,"\\",1);
        strncat(logfile_buf,LOG_FILE,strlen((LPCSTR)LOG_FILE));
    }
#endif
    if (MH_Initialize() != MH_OK) return;
    if ( read_appint(L"General", L"Portable") > 0 && (appdata_path[1] == L':' ||
         read_appkey(L"General",L"PortableDataPath",appdata_path,sizeof(appdata_path))) )
    {
        init_portable(&init_global_env);
    }
#ifndef DISABLE_SAFE
    if ( read_appint(L"General",L"SafeEx") > 0 )
    {
        init_safed(NULL);
    }
#endif
    if ( !RunPart && inifile_exist() )
    {
        if ( read_appint(L"General",L"ProcessAffinityMask") > 0 )
        {
            CloseHandle((HANDLE)_beginthreadex(NULL,0,&SetCpuAffinity_tt,NULL,0,NULL));
        }
        if ( read_appint(L"General",L"CreateCrashDump") )
        {
            CloseHandle((HANDLE)_beginthreadex(NULL,0,&init_exeception,NULL,0,NULL));
        }
        if ( read_appint(L"General", L"Bosskey") > 0 )
        {
            ZeroMemory(&ff_info, sizeof(WNDINFO));
            ff_info.hPid = GetCurrentProcessId();
            CloseHandle((HANDLE)_beginthreadex(NULL,0,&bosskey_thread,&ff_info,0,NULL));
        }
        CloseHandle((HANDLE)_beginthreadex(NULL,0,&SetPluginPath,NULL,0,NULL));
        if ( read_appint(L"General", L"ProxyExe") > 0 )
        {
            CloseHandle((HANDLE)_beginthreadex(NULL,0,&run_process,NULL,0,NULL));
        }
        RunPart=1;
    }
}

/* This is standard DllMain function. */
#ifdef __cplusplus
extern "C" {
#endif

#if defined(VC12_CRT)
#undef _DllMainCRTStartup
#define _DllMainCRTStartup DllMain
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
