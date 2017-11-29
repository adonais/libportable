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
#include "new_process.h"
#include "cpu_info.h"
#include "balance.h"
#include "set_env.h"
#include "win_registry.h"
#include "on_tabs.h"
#include "MinHook.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <knownfolders.h>
#include <process.h>
#include <stdio.h>

#if defined(VC12_CRT) || defined(__clang__)
#undef _DllMainCRTStartup
#define _DllMainCRTStartup DllMain
#endif

#ifndef SHSTDAPI
#define SHSTDAPI STDAPI
#endif

typedef  LPITEMIDLIST PIDLIST_ABSOLUTE;

SHSTDAPI SHILCreateFromPath (PCWSTR pszPath, PIDLIST_ABSOLUTE *ppidl, DWORD *rgfInOut);

typedef HRESULT (WINAPI *SHGetFolderPathWPtr)(HWND hwndOwner,
        int    nFolder, 
        HANDLE hToken,
        DWORD  dwFlags,
        LPWSTR pszPath);
typedef HRESULT (WINAPI *SHGetSpecialFolderLocationPtr)(HWND hwndOwner,
        int nFolder,
        LPITEMIDLIST *ppidl);
typedef bool (WINAPI *SHGetSpecialFolderPathWPtr)(HWND hwndOwner,
        LPWSTR lpszPath,
        int    csidl,
        bool   fCreate);
typedef HRESULT (WINAPI *SHGetKnownFolderIDListPtr)(REFKNOWNFOLDERID rfid,
        DWORD            dwFlags,
        HANDLE           hToken,
        PIDLIST_ABSOLUTE *ppidl);
typedef HRESULT (WINAPI *SHGetKnownFolderPathPtr)(REFKNOWNFOLDERID rfid,
        DWORD            dwFlags,
        HANDLE           hToken,
        PWSTR            *ppszPath);

static  WNDINFO  ff_info;
static  uintptr_t m_target[EXCLUDE_NUM];
static  SHGetFolderPathWPtr           sSHGetFolderPathWStub;
static  SHGetSpecialFolderLocationPtr sSHGetSpecialFolderLocationStub;
static  SHGetSpecialFolderPathWPtr    sSHGetSpecialFolderPathWStub;
static  SHGetKnownFolderIDListPtr     sSHGetKnownFolderIDListStub;
static  SHGetKnownFolderPathPtr       sSHGetKnownFolderPathStub;

typedef void (*pointer_to_handler)();
typedef struct _dyn_link_desc
{
    const char* name;
    void* hook;
    pointer_to_handler* original;
}dyn_link_desc;

/* Shared data segments(data lock),the running process acquired the lock */
#ifdef _MSC_VER
#pragma data_seg(".shrd")
#endif
volatile long process_cout SHARED = -1;
volatile long run_once SHARED = 0;
volatile uint32_t main_pid SHARED = 0;
volatile uint32_t new_main_pid SHARED = 0;
WCHAR    ini_path[MAX_PATH+1] SHARED = {0};
char     logfile_buf[MAX_PATH+1] SHARED = {0};
WCHAR    appdata_path[VALUE_LEN+1] SHARED = {0};
static   WCHAR localdata_path[VALUE_LEN+1] SHARED = {0} ;
#ifdef _MSC_VER
#pragma data_seg()
#pragma comment(linker,"/SECTION:.shrd,RWS")
#endif

bool
creator_hook(void* target, void* func, void **original)
{
    if ( NULL != target && MH_CreateHook(target, func, original) == MH_OK )
    {
        return ( MH_EnableHook(target) == MH_OK );
    }
    return false;
}

/* AVX memset with non-temporal instructions */
TETE_EXT_CLASS void * __cdecl 
memset_nontemporal_tt (void *dest, int c, size_t count)
{
    return memset_avx(dest, c, count);
}

/* Get the second level cache size */
TETE_EXT_CLASS uint32_t
GetNonTemporalDataSizeMin_tt(void)
{
    return get_level_size();
}

/* Never used,to be compatible with tete's patch */
TETE_EXT_CLASS int
GetCpuFeature_tt(void)
{
    return 0;
}

TETE_EXT_CLASS intptr_t 
GetAppDirHash_tt(void)
{
    return 0;
}

HRESULT WINAPI 
HookSHGetSpecialFolderLocation(HWND hwndOwner, int nFolder, LPITEMIDLIST *ppidl)
{
    int folder = nFolder & 0xff;
    if ( CSIDL_APPDATA == folder || CSIDL_LOCAL_APPDATA == folder )
    {
        LPITEMIDLIST pidlnew = NULL;
        HRESULT result = E_FAIL;
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
                if ( localdata_path[0] != L'\0' && create_dir(localdata_path) )
                {
                    result = SHILCreateFromPath(localdata_path, &pidlnew, NULL);
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
    return sSHGetSpecialFolderLocationStub(hwndOwner, nFolder, ppidl);
}

HRESULT WINAPI 
HookSHGetFolderPathW(HWND hwndOwner,int nFolder,HANDLE hToken,
                     DWORD dwFlags,LPWSTR pszPath)
{
    uintptr_t   dwCaller;
    bool        dwFf = false;
    int         folder = nFolder & 0xff;
    HRESULT     ret = E_FAIL;
#ifndef LIBPORTABLE_STATIC
    WCHAR		dllname[VALUE_LEN+1];
    GetModuleFileNameW(dll_module, dllname, VALUE_LEN);
#endif
    dwCaller = (uintptr_t)_ReturnAddress();
    dwFf = is_specialdll(dwCaller, L"*\\xul.dll")                 ||
        #ifndef LIBPORTABLE_STATIC
           is_specialdll(dwCaller, dllname)                       ||
        #endif
           is_flash_plugins(dwCaller);
    if ( !dwFf )
    {
        return sSHGetFolderPathWStub(hwndOwner, nFolder, hToken, dwFlags, pszPath);
    }

    switch (folder)
    {
        case CSIDL_APPDATA:
        {
            if ( appdata_path[0] != L'\0' )
            {
                int	 num = wnsprintfW(pszPath,MAX_PATH,L"%ls",appdata_path);
                if ( num>0 && num<MAX_PATH ) ret = S_OK;
            }
            break;
        }
        case CSIDL_LOCAL_APPDATA:
        {
            if ( localdata_path[0] != L'\0' && create_dir(localdata_path) )
            {
                int	 num = wnsprintfW(pszPath,MAX_PATH,L"%ls",localdata_path);
                if ( num>0 && num<MAX_PATH ) ret = S_OK;
            }
            break;
        }
        default:
            break;
    }
    
    if (S_OK != ret)
    {
        ret = sSHGetFolderPathWStub(hwndOwner, nFolder, hToken, dwFlags, pszPath);
    }
    return ret;
}

bool WINAPI 
HookSHGetSpecialFolderPathW(HWND hwndOwner,LPWSTR lpszPath,int csidl,bool fCreate)
{
    bool       internal;
    uintptr_t  dwCaller = (uintptr_t)_ReturnAddress();
    internal = is_specialdll(dwCaller, L"*\\xul.dll") || is_flash_plugins(dwCaller);
    if ( !internal )
    {
        return sSHGetSpecialFolderPathWStub(hwndOwner,lpszPath,csidl,fCreate);
    }
    return (HookSHGetFolderPathW(
            hwndOwner,
            csidl + (fCreate ? CSIDL_FLAG_CREATE : 0),
            NULL,
            0,
            lpszPath)) == S_OK ? true : false;
}

HRESULT WINAPI 
HookSHGetKnownFolderIDList(REFKNOWNFOLDERID rfid,DWORD dwFlags,HANDLE hToken,PIDLIST_ABSOLUTE *ppidl)
{
    if ( IsEqualGUID(rfid, &FOLDERID_RoamingAppData) )
    {
        return HookSHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, ppidl);
    }
    else if ( IsEqualGUID(rfid, &FOLDERID_LocalAppData) )
    {
        return HookSHGetSpecialFolderLocation(NULL, CSIDL_LOCAL_APPDATA, ppidl);
    }
    return sSHGetKnownFolderIDListStub(rfid,dwFlags,hToken,ppidl);
}

HRESULT WINAPI 
HookSHGetKnownFolderPath(REFKNOWNFOLDERID rfid,DWORD dwFlags,HANDLE hToken,PWSTR *ppszPath)
{
    *ppszPath = NULL;
    if ( IsEqualGUID(rfid, &FOLDERID_RoamingAppData) )
    {
        *ppszPath = CoTaskMemAlloc((wcslen(appdata_path) + 1) * sizeof(WCHAR));
        if (!*ppszPath) 
        {
            return E_OUTOFMEMORY;
        #ifdef _LOGDEBUG
            logmsg("return E_OUTOFMEMORY\n");
        #endif 
        }
        wcscpy(*ppszPath, appdata_path);
        PathRemoveBackslashW(*ppszPath);
        return S_OK;
    }
    else if ( IsEqualGUID(rfid, &FOLDERID_LocalAppData) )
    {
        *ppszPath = CoTaskMemAlloc((wcslen(localdata_path) + 1) * sizeof(WCHAR));
        if (!*ppszPath)
        {
            return E_OUTOFMEMORY;
        }
        wcscpy(*ppszPath, localdata_path);
        PathRemoveBackslashW(*ppszPath);
        return S_OK;
    }
    return sSHGetKnownFolderPathStub(rfid,dwFlags,hToken,ppszPath);
}

static void 
init_portable(void)
{
#define FUNC_NUM 5
#define DLD(s,h) {#s, (void*)(Hook##s), (pointer_to_handler*)(void*)(&h)}
    int i;
    HMODULE h_shell32;
    const dyn_link_desc api_tables[FUNC_NUM] = 
    {
          DLD(SHGetSpecialFolderLocation, sSHGetSpecialFolderLocationStub)
        , DLD(SHGetFolderPathW, sSHGetFolderPathWStub)
        , DLD(SHGetSpecialFolderPathW, sSHGetSpecialFolderPathWStub)
        , DLD(SHGetKnownFolderIDList, sSHGetKnownFolderIDListStub)
        , DLD(SHGetKnownFolderPath, sSHGetKnownFolderPathStub)
    };
    if ( (h_shell32 = GetModuleHandleW(L"shell32.dll")) == NULL )
    {
        return;
    }
    if ( !m_target[0] )
    {
        for ( i = 0 ; i<FUNC_NUM; i++)
        {
            m_target[i] = (uintptr_t)GetProcAddress(h_shell32, api_tables[i].name);
        }
        for ( i = 0 ; m_target[i]!=0&&i<FUNC_NUM; i++ )
        {
            creator_hook((void*)m_target[i], api_tables[i].hook, (void **)api_tables[i].original);
        }
    }
#undef FUNC_NUM
#undef DLD
}


/* 初始化全局变量 */
static bool
init_global_env(void)
{   
    /* 如果ini文件里的appdata设置路径为相对路径 */
    if ( appdata_path[1] != L':' )
    {
        PathToCombineW(appdata_path,VALUE_LEN);
    }
    if ( read_appkey(L"Env",L"TmpDataPath",localdata_path,sizeof(appdata_path),NULL) )
    {
        /* 修正相对路径问题 */
        if ( localdata_path[1] != L':' )
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
        wcsncat(appdata_path,L"\\AppData",VALUE_LEN);
    }
    if ( localdata_path[0] != L'\0' )
    {
        wcsncat(localdata_path,L"\\LocalAppData\\Temp\\Fx",VALUE_LEN);
    }
    return WaitWriteFile(NULL);
}

#if defined(__GNUC__) && defined(__LTO__)
#pragma GCC push_options
#pragma GCC optimize ("O3")
#endif

/* uninstall hook and clean up */
void WINAPI 
undo_it(void)
{
    if ( process_cout == -1 )
    {
    #ifdef _LOGDEBUG
        logmsg("all process clear. \n");
    #endif
        _InterlockedExchange(&run_once, 0);
    }
    else if ( main_pid == GetCurrentProcessId() )
    {
    #ifdef _LOGDEBUG
        logmsg("main_pid = %u exit. \n", main_pid);
    #endif
        main_pid = new_main_pid;
        new_main_pid = 0;
    }
    /* 解除快捷键 */
    if ( ff_info.atom_str )
    {
        UnregisterHotKey(NULL, ff_info.atom_str);
        GlobalDeleteAtom(ff_info.atom_str);
    }
    /* 清理启动过的进程树 */
    kill_trees();
    jmp_end();
    MH_Uninitialize();
    /* 反注册IUIAutomation接口 */
    un_uia();
    return;
}

void WINAPI 
do_it(void)
{
    if ( process_cout>2048 || !run_once )
    {
        if ( !init_parser(ini_path, MAX_PATH) )
        {
            return;
        }
        if ( (main_pid = GetCurrentProcessId()) < 0x4 )
        {
            return;
        }  
        if ( read_appint(L"General", L"Portable") <= 0 )
        {
            return;
        }
        if ( true )
        {
            /* 如果存在MOZ_NO_REMOTE宏,环境变量需要优先导入 */
            set_envp(NULL); 
        }
        if ( !read_appkey(L"General",L"PortableDataPath",appdata_path,sizeof(appdata_path),NULL) )
        {
            /* 预设默认的配置文件所在路径 */
            wnsprintfW(appdata_path, VALUE_LEN, L"%ls", L"../Profiles");
        }
        if ( !init_global_env() )
        {
        #ifdef _LOGDEBUG
            logmsg("init_global_env return false!\n");
        #endif 
        }
    }
    if ( true )
    {
        if ( ff_info.hPid == 0 )
        {
            ff_info.hPid = GetCurrentProcessId();
        }
        if ( MH_Initialize() != MH_OK )
        {
        #ifdef _LOGDEBUG
            logmsg("MH_Initialize false!!!!\n");
        #endif 
            return;
        }
        if ( appdata_path[1] == L':' )
        {
            init_portable();
        }
    #ifndef DISABLE_SAFE
        if ( read_appint(L"General",L"SafeEx") > 0 )
        {
            init_safed(NULL);
        }
    #endif
        if ( read_appint(L"General",L"CreateCrashDump") != 0 )
        {
            init_exeception(NULL);
        }

    }
    if ( new_main_pid?!is_child_of(new_main_pid):!is_child_of(main_pid) )
    {
        if ( run_once )
        {
            new_main_pid = ff_info.hPid;
        }
        if ( new_main_pid )
        {
        #ifdef _LOGDEBUG
            logmsg("set new environment!\n");
        #endif 
            set_envp(NULL);
        }
        if ( read_appint(L"General",L"OnTabs") > 0 )
        {
            DWORD ver = GetOsVersion();
            if (ver > 601)
            {
                CloseHandle((HANDLE)_beginthreadex(NULL,0,&threads_on_win10,NULL,0,NULL));
            #ifdef _LOGDEBUG
                logmsg("win8--win10!\n");
            #endif
            }
            else if ( ver > 503 && ver < 602 )
            {
                threads_on_win7();
            #ifdef _LOGDEBUG
                logmsg("vista--win7!\n");
            #endif
            }
        }
        if ( read_appint(L"General", L"DisableScan") > 0 )
        {
            init_winreg(NULL);
        }
    }
    /* 使用计数器方式判断是否浏览器重启? */
    if ( !run_once )
    {
        if ( read_appint(L"General",L"ProcessAffinityMask") > 0 )
        {
            CloseHandle((HANDLE)_beginthreadex(NULL,0,&set_cpu_balance,&ff_info,0,NULL)); 
        }
        if ( read_appint(L"General", L"Bosskey") > 0 )
        {
            CloseHandle((HANDLE)_beginthreadex(NULL,0,&bosskey_thread,&ff_info,0,NULL));
        }
        if ( read_appint(L"General", L"ProxyExe") > 0 )
        {
            CloseHandle((HANDLE)_beginthreadex(NULL,0,&run_process,NULL,0,NULL));
        }
    }
    *(long volatile*)&run_once = 1;
}

#if defined(__GNUC__) && defined(__LTO__)
#pragma GCC pop_options
#endif

/* This is standard DllMain function. */
#ifdef __cplusplus
extern "C" {
#endif

#if defined(LIBPORTABLE_EXPORTS) || !defined(LIBPORTABLE_STATIC)
int CALLBACK 
_DllMainCRTStartup(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
    switch(dwReason)
    {
    case DLL_PROCESS_ATTACH:
        dll_module = (HMODULE)hModule;
        DisableThreadLibraryCalls(hModule);
    #ifdef _LOGDEBUG
        init_logs();
    #endif
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
        return false;
    }
    return true;
}
#endif  /* LIBPORTABLE_EXPORTS */

#ifdef __cplusplus
}
#endif
