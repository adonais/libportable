#ifndef LIBPORTABLE_STATIC
#define TETE_BUILD
#endif

#include "portable.h"
#include "inipara.h"
#include "safe_ex.h"
#include "ice_error.h"
#include "bosskey.h"
#include "prefjs.h"
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
typedef void (CALLBACK *user_func)(void);

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

static  WNDINFO ff_info;
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

BOOL WINAPI WaitWriteFile(LPCWSTR app_path)
{
    BOOL  ret = FALSE;
    WCHAR profile_orgi[MAX_PATH+1];   
    if ( !get_mozilla_profile(app_path, profile_orgi, MAX_PATH) )
    {
        return ret;
    }
    if ( PathFileExistsW(profile_orgi) )
    {
        if ( is_thunderbird() )
        {
            ret = WritePrivateProfileStringW(L"Profile0",L"Path",L"../../",profile_orgi);
        }
        else
        {
            ret = WritePrivateProfileStringW(L"Profile0",L"Path",L"../../../",profile_orgi);
        }
    }
    else
    {
        LPWSTR szDir;
        if ( (szDir = (LPWSTR)SYS_MALLOC( sizeof(profile_orgi) ) ) != NULL )
        {
            wcsncpy (szDir, profile_orgi, MAX_PATH);
            PathRemoveFileSpecW( szDir );
            SHCreateDirectoryExW(NULL,szDir,NULL);
            SYS_FREE(szDir);
            WritePrivateProfileSectionW(L"General",L"StartWithLastProfile=1\r\n\0",profile_orgi);
            if ( is_thunderbird() )
            {
                ret = WritePrivateProfileSectionW(L"Profile0",L"Name=default\r\nIsRelative=1\r\nPath=../../\r\nDefault=1\r\n\0" \
                                                  ,profile_orgi);
            }
            else
            {
                ret = WritePrivateProfileSectionW(L"Profile0",L"Name=default\r\nIsRelative=1\r\nPath=../../../\r\nDefault=1\r\n\0" \
                                                  ,profile_orgi);
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
#ifndef LIBPORTABLE_STATIC
    dwFf = is_specialdll(dwCaller, dllname)       ||
           is_specialdll(dwCaller, L"*\\xul.dll") ||
           is_specialdll(dwCaller, L"*\\npswf*.dll");
#else
    dwFf = is_specialdll(dwCaller, L"*\\xul.dll") || is_specialdll(dwCaller, L"*\\npswf*.dll");
#endif
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
    HMODULE hShell32;
    (*init_env)();
    hShell32 = GetModuleHandleW(L"shell32.dll");
    if (hShell32 == NULL)
    {
        return;
    }
    TrueSHGetFolderPathW = (_NtSHGetFolderPathW)GetProcAddress(hShell32,
                           "SHGetFolderPathW");
    TrueSHGetSpecialFolderPathW = (_NtSHGetSpecialFolderPathW)GetProcAddress(hShell32,
                                  "SHGetSpecialFolderPathW");
    TrueSHGetSpecialFolderLocation = (_NtSHGetSpecialFolderLocation)GetProcAddress(hShell32,
                                     "SHGetSpecialFolderLocation"); 
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
    /* 初始化日志记录文件 */
#ifdef _LOGDEBUG
    if ( GetEnvironmentVariableA("APPDATA",logfile_buf,MAX_PATH) > 0 )
    {
        strncat(logfile_buf,"\\",1);
        strncat(logfile_buf,LOG_FILE,strlen((LPCSTR)LOG_FILE));
    }
#endif
    if ( read_appint(L"General", L"Portable") > 0 && (appdata_path[1] == L':' ||
         read_appkey(L"General",L"PortableDataPath",appdata_path,sizeof(appdata_path))) )
    {
        init_portable(&init_global_env);
    }
    if ( read_appint(L"General",L"SafeEx") > 0 )
    {
        init_safed(NULL);
    }
 #ifdef _LOGDEBUG
     logmsg("RunPart = %d, appdata = %ls\n",RunPart, appdata_path);
 #endif
    if ( !RunPart )
    {
        CloseHandle((HANDLE)_beginthreadex(NULL,0,&gmpservice_check,NULL,0,NULL));
        if ( read_appint(L"General",L"ProcessAffinityMask") > 0 )
        {
            CloseHandle((HANDLE)_beginthreadex(NULL,0,&SetCpuAffinity_tt,NULL,0,NULL));
        }
        if ( read_appint(L"General",L"CreateCrashDump") > 0 )
        {
            CloseHandle((HANDLE)_beginthreadex(NULL,0,&init_exeception,NULL,0,NULL));
        }
        if ( read_appint(L"General", L"Bosskey") > 0 )
        {
            ZeroMemory(&ff_info, sizeof(WNDINFO));
            ff_info.hPid = GetCurrentProcessId();
         #ifdef _LOGDEBUG
            logmsg("ff_info.hPid = %lu\n",ff_info.hPid);
         #endif
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
