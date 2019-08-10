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
#include "share_lock.h"
#include "updates.h"
#include "MinHook.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <knownfolders.h>
#include <process.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>

#if defined(VC12_CRT) 
#undef _DllMainCRTStartup
#define _DllMainCRTStartup DllMain
#endif

#ifndef SHSTDAPI
#define SHSTDAPI STDAPI
#endif

#define _UPDATE L"upcheck.exe "

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
                WCHAR appdata_path[MAX_PATH+1] = {0};
                if (!get_appdt_path(appdata_path, MAX_PATH))
                {
                    return result;
                }
                if ( appdata_path[0] != L'\0' )
                {
                    result = SHILCreateFromPath(appdata_path, &pidlnew, NULL);
                }
                break;
            }
            case CSIDL_LOCAL_APPDATA:
            {
                WCHAR localdt_path[MAX_PATH+1] = {0};
                if (!get_localdt_path(localdt_path, MAX_PATH))
                {
                    break;
                }
                if ( localdt_path[0] != L'\0' && create_dir(localdt_path) )
                {
                    result = SHILCreateFromPath(localdt_path, &pidlnew, NULL);
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
    uintptr_t dwCaller;
    bool      dwFf = false;
    int       folder = nFolder & 0xff;
    HRESULT   ret = E_FAIL;
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
            WCHAR appdata_path[MAX_PATH+1] = {0};
            if (!get_appdt_path(appdata_path, MAX_PATH))
            {
                break;
            }
            if ( appdata_path[0] != L'\0' )
            {
                int	 num = wnsprintfW(pszPath,MAX_PATH,L"%ls",appdata_path);
                if ( num>0 && num<MAX_PATH ) ret = S_OK;
            }
            break;
        }
        case CSIDL_LOCAL_APPDATA:
        {
            WCHAR localdt_path[MAX_PATH+1] = {0};
            if (!get_localdt_path(localdt_path, MAX_PATH))
            {
                break;
            }
            if (localdt_path[0] != L'\0' && create_dir(localdt_path))
            {
                int	 num = wnsprintfW(pszPath,MAX_PATH,L"%ls",localdt_path);
                if (num>0 && num<MAX_PATH ) 
                    ret = S_OK;
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
        WCHAR appdata_path[MAX_PATH+1] = {0};
        if (!get_appdt_path(appdata_path, MAX_PATH))
        {
            return S_FALSE;
        }
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
        WCHAR localdt_path[MAX_PATH+1] = {0};
        if (!get_localdt_path(localdt_path, MAX_PATH))
        {
            return S_FALSE;
        }
        *ppszPath = CoTaskMemAlloc((wcslen(localdt_path) + 1) * sizeof(WCHAR));
        if (!*ppszPath)
        {
            return E_OUTOFMEMORY;
        }
        wcscpy(*ppszPath, localdt_path);
        PathRemoveBackslashW(*ppszPath);
        return S_OK;
    }
    return sSHGetKnownFolderPathStub(rfid,dwFlags,hToken,ppszPath);
}

static void 
init_portable(void)
{
#define DLD(s,h) {#s, (void*)(Hook##s), (pointer_to_handler*)(void*)(&h)}
    int i;
    HMODULE h_shell32;
    const dyn_link_desc api_tables[] = 
    {
          DLD(SHGetSpecialFolderLocation, sSHGetSpecialFolderLocationStub)
        , DLD(SHGetFolderPathW, sSHGetFolderPathWStub)
        , DLD(SHGetSpecialFolderPathW, sSHGetSpecialFolderPathWStub)
        , DLD(SHGetKnownFolderIDList, sSHGetKnownFolderIDListStub)
        , DLD(SHGetKnownFolderPath, sSHGetKnownFolderPathStub)
    };
    int func_num = sizeof(api_tables)/sizeof(api_tables[0]);
    if ( (h_shell32 = GetModuleHandleW(L"shell32.dll")) == NULL )
    {
        return;
    }
    if ( !m_target[0] )
    {
        for ( i = 0 ; i<func_num; i++)
        {
            m_target[i] = (uintptr_t)GetProcAddress(h_shell32, api_tables[i].name);
        }
        for ( i = 0 ; m_target[i]!=0&&i<func_num; i++ )
        {
            creator_hook((void*)m_target[i], api_tables[i].hook, (void **)api_tables[i].original);
        }
    }
#undef DLD
}

/* 初始化全局变量 */
static bool
init_global_env(LPWSTR appdt, LPWSTR localdt, LPWSTR ini, int len)
{   
    /* 如果ini文件里的appdata设置路径为相对路径 */
    if (appdt[1] != L':')
    {
        path_to_absolute(appdt,len);
    }
    if (read_appkey(L"Env",L"TmpDataPath",localdt,sizeof(WCHAR)*len,ini))
    {
        /* 修正相对路径问题 */
        if (localdt[1] != L':')
        {
            path_to_absolute(localdt,len);
        }
    }
    else
    {
        wcsncpy(localdt,appdt,len);
    }
    if ( appdt[0] != L'\0' )
    {
        wcsncat(appdt,L"\\AppData",len);
    }
    if ( localdt[0] != L'\0' )
    {
        wcsncat(localdt,L"\\LocalAppData\\Temp\\Fx",len);
    }
    if ( profile_boot() || get_process_profile() )
    {   
        return true;
    }
#ifdef _LOGDEBUG
    logmsg("%lu ready to write_file!\n", GetCurrentProcessId());
#endif    
    return write_file(appdt);
}

#if defined(__GNUC__) && defined(__LTO__)
#pragma GCC push_options
#pragma GCC optimize ("O3")
#endif

static uint64_t
read_long(LPCWSTR cat,LPCWSTR name)
{
    WCHAR buf[NAMES_LEN+1] = {0};
    if (!read_appkey(cat, name, buf, sizeof(buf), NULL))
    {
        return 0;
    }
    return _wcstoui64(buf, NULL, 10);   
}

static bool
diff_days(void)
{
    uint64_t diff = 3600*24;
    uint64_t time1 = read_long(L"update",L"last_check");
    uint64_t time2 = (uint64_t)time(NULL);
    if (time2 - time1 > diff)
    {
        return true;
    }
    return false;
}

unsigned WINAPI
update_thread(void *lparam)
{
    DWORD pid = 0;
    WCHAR *pos = NULL;
    WCHAR temp[MAX_PATH+1] = {0};
    WCHAR path[VALUE_LEN+1] = {0};
    WCHAR wcmd[MAX_PATH+1] = {0};
    if (!get_localdt_path(temp, MAX_PATH))
    {
    #ifdef _LOGDEBUG
        logmsg("get_localdt_path return false!\n");
    #endif          
        return (0);
    }
    if ((pid = get_process_pid()) < 5)
    {
        return (0);
    }
    if (!GetModuleFileNameW(NULL, path, VALUE_LEN))
    {
        return (0);
    }  
    if (!get_process_comp(path))
    {
        return (0);
    }
    if ((pos = wcsrchr(path, L'\\')) == NULL)
    {
        return (0);
    }
    if (true)
    {
        wcsncpy(wcmd, path, ++pos-path);
        wcsncat(temp, L"\\Mozilla\\updates", MAX_PATH);
    }
    if (read_appint(L"update", L"be_ready") > 0)
    {
        wnsprintfW(wcmd, MAX_PATH, L"%ls"_UPDATE L"-k %lu -e %ls -s %ls -u 1", wcmd, pid, temp, path);
        CloseHandle(create_new(wcmd, NULL, 2, NULL));
    }
    else if (diff_days())
    {
        Sleep(8000);
        wnsprintfW(wcmd, MAX_PATH, L"%ls"_UPDATE L"-i auto -k %lu -e %ls", wcmd, pid, temp);
        CloseHandle(create_new(wcmd, NULL, 2, NULL));
    }
    return (1);
}

static bool 
init_share_data(void)
{
    if (!init_parser(sdata.ini, MAX_PATH))
    {
        return false;
    }
    if ((sdata.main = GetCurrentProcessId()) < 0x4)
    {
        return false;
    }
    if (!GetModuleFileNameW(NULL,sdata.process,MAX_PATH))
    {
        return false;
    }
    if (get_profile_boot(sdata.appdt,MAX_PATH) || get_process_profile())
    {
        set_process_profile(true);
    #ifdef _LOGDEBUG
        logmsg("profile_boot, appdata[%ls]!\n", sdata.appdt);
    #endif
    }
    else if (!read_appkey(L"General",L"PortableDataPath",sdata.appdt,sizeof(sdata.appdt),sdata.ini))
    {
        wnsprintfW(sdata.appdt, MAX_PATH, L"%ls", L"../Profiles");
    #ifdef _LOGDEBUG
        logmsg("appdata[%ls]!\n", sdata.appdt);
    #endif        
    }
    else
    {
    #ifdef _LOGDEBUG
        logmsg("appdata[%ls]!\n", sdata.appdt);
    #endif           
    }
    if (!init_global_env(sdata.appdt, sdata.localdt, sdata.ini, MAX_PATH))
    {
    #ifdef _LOGDEBUG
        logmsg("init_global_env return false!\n");
    #endif
        return false;
    }
    set_envp(NULL);
    return true;
}

static bool 
init_share_locks(void)
{
    if (share_create(false, sizeof(sdata)))
    {
        s_data *memory = share_map(sizeof(s_data), false);
        if (memory != NULL)
        {
            memcpy(memory,&sdata, sizeof(sdata));
            share_unmap(memory);
            return true;
        }
    }
    return false;
}

static void 
local_hook(void)
{
    if (MH_Initialize() != MH_OK)
    {
    #ifdef _LOGDEBUG
        logmsg("MH_Initialize false!!!!\n");
    #endif 
        return;
    }
    if (read_appint(L"General", L"Portable") > 0)
    {
        init_portable();
    }
#ifndef DISABLE_SAFE
    if (read_appint(L"General",L"SafeEx") > 0)
    {
        init_safed(NULL);
    }
#endif
}

static void 
other_hook(void)
{
    DWORD ver = get_os_version();
    if (ver > 503 && 
        read_appint(L"General", L"Update") > 0 &&
        read_appint(L"General", L"Portable") > 0)
    {
        CloseHandle((HANDLE)_beginthreadex(NULL,0,&update_thread,NULL,0,NULL));
    }
    if (ff_info.hPid == 0)
    {
        ff_info.hPid = GetCurrentProcessId();
    }
    if (read_appint(L"General",L"CreateCrashDump") != 0)
    {
        init_exeception(NULL);
    }
#if defined(_MSC_VER)    /* mingw-w64 crt does not implement IUIAutomation interface */
    if (read_appint(L"General",L"OnTabs") > 0)
    {
        if (ver > 601)
        {
            CloseHandle((HANDLE)_beginthreadex(NULL,0,&threads_on_win10,NULL,0,NULL));
        #ifdef _LOGDEBUG
            logmsg("win8--win10!\n");
        #endif
        }
        else if (ver > 503 && ver < 602)
        {
            threads_on_win7();
        #ifdef _LOGDEBUG
            logmsg("vista--win7!\n");
        #endif
        }
    }
#endif
    if (!init_watch())
    {
    #ifdef _LOGDEBUG
        logmsg("init_watch return false!\n");
    #endif
    }
    if (read_appint(L"General", L"DisableScan") > 0)
    {
        init_winreg(NULL);
    }
    if (read_appint(L"General",L"ProcessAffinityMask") > 0)
    {
        CloseHandle((HANDLE)_beginthreadex(NULL,0,&set_cpu_balance,&ff_info,0,NULL)); 
    }
    if (read_appint(L"General", L"Bosskey") > 0)
    {
        CloseHandle((HANDLE)_beginthreadex(NULL,0,&bosskey_thread,&ff_info,0,NULL));
    }
    if (read_appint(L"General", L"ProxyExe") > 0)
    {
        CloseHandle((HANDLE)_beginthreadex(NULL,0,&run_process,NULL,0,NULL));
    }
}

/* uninstall hook and clean up */
void WINAPI 
undo_it(void)
{
    if (get_process_pid() == GetCurrentProcessId())
    {
        share_close();
    #ifdef _LOGDEBUG
        logmsg("main process[%lu] exit\n", GetCurrentProcessId());
    #endif
    }
    /* 解除快捷键 */
    if ( ff_info.atom_str )
    {
        UnregisterHotKey(NULL, ff_info.atom_str);
        GlobalDeleteAtom(ff_info.atom_str);
        memset(&ff_info, 0, sizeof(ff_info));
    }
    /* 清理启动过的进程树 */
    kill_trees();
    jmp_end();
    MH_Uninitialize();
#if !(__GNUC__ || __clang__)
    /* 反注册IUIAutomation接口 */
    un_uia();
#endif
    return;
}

void WINAPI 
do_it(void)
{
    bool   res = false;
    HANDLE map = NULL;
    if ((map = share_open(false)) == NULL)
    {
        res = init_share_data();   
        if (res)
        {
            res = init_share_locks();
            if (res)
            {
                local_hook();
                other_hook();
            }
            return;          
        }
    }
    else
    {
    #ifdef _LOGDEBUG
        logmsg("share_open succeed!\n");
    #endif        
        set_share_handle(map);
    }
    if (is_browser(NULL))
    {   /* multiple firefox's processes */
        uint32_t m_pid = get_process_pid();
        if (m_pid == _getppid())
        {
            if (pie_boot() || profile_boot())
            {
                set_process_pid(GetCurrentProcessId());
            #ifdef _LOGDEBUG
                logmsg("process_pie[id = %lu] create\n", GetCurrentProcessId());
            #endif
                local_hook();
                other_hook();
            }
        }
        else if (get_process_remote())
        {
            set_envp(NULL);
            local_hook();
            other_hook();
        #ifdef _LOGDEBUG
            logmsg("%lu get_process_remote() true.\n", GetCurrentProcessId());
        #endif            
        }
        else if (get_process_flags())
        {
            set_process_flags(false);
            local_hook();
            other_hook();
        #ifdef _LOGDEBUG
            logmsg("%lu flags restart.\n", GetCurrentProcessId());
        #endif               
        }        
        else if (get_file_version() >= 670)
        {
            set_envp(NULL);
            local_hook();
            other_hook();            
        #ifdef _LOGDEBUG
            logmsg("%lu with fx67 or high.\n", GetCurrentProcessId());
        #endif
        }         
        else if (NULL != map)
        {
            share_close();
        #ifdef _LOGDEBUG
            logmsg("%lu nothing to do, we closed the share mem.\n", GetCurrentProcessId());
        #endif
        }
    }
    else if (is_specialapp(L"plugin-container.exe"))
    {
        local_hook();
    }
    else
    {
        res = init_share_data();
        if (res)
        {
        #ifdef _LOGDEBUG
            logmsg("other fx process[%lu] create\n", GetCurrentProcessId());
        #endif            
            local_hook();
            other_hook();
        }
        else
        {
    #ifdef _LOGDEBUG
        logmsg("other process[%lu] create hook false\n", GetCurrentProcessId());
    #endif            
        }
    }
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
        SetDllDirectoryW(L"");
        const HMODULE hlib = GetModuleHandleW(L"kernel32.dll");
        if (hlib)
        {
            typedef BOOL (WINAPI * SSPM) (DWORD);
            const SSPM fnSetSearchPathMode = (SSPM)GetProcAddress (hlib, "SetSearchPathMode");
            if (fnSetSearchPathMode)
            {
          	    fnSetSearchPathMode(BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE | BASE_SEARCH_PATH_PERMANENT);
            }
        }
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
