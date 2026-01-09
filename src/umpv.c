#ifndef LIBUMPV_STATIC
#define LIBUMPV_BUILDING
#endif

#include "umpv.h"
#include "inipara.h"
#include "bosskey.h"
#include "oneinst.h"
#include "internal_exp.h"
#include "MinHook.h"
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>

/* Shared data segments(data lock),the running process acquired the lock */
#ifdef _MSC_VER
#pragma data_seg(".shmpv")
#endif
volatile DWORD lib_pid SHARED = 0;
volatile long lib_init_once SHARED = 0;
WCHAR    ini_path[MAX_PATH+1] SHARED = {0};
WCHAR    logfile_buf[MAX_PATH+1] SHARED = {0};
volatile INT_PTR mpv_window_hwnd SHARED = 0;
#ifdef _MSC_VER
#pragma data_seg()
#endif

int
umpv_init_status(void)
{
    return 0;
}

#if defined(__GNUC__) && defined(__LTO__)
#pragma GCC push_options
#pragma GCC optimize ("O3")
#endif

static bool
judg_args(void)
{
    int  argc = 0;
    HMODULE hcrt = GetModuleHandleW(L"shell32.dll");
    CommandLineToArgvWptr crt_CommandLineToArgvW = hcrt ? (CommandLineToArgvWptr)GetProcAddress(hcrt, "CommandLineToArgvW") : NULL;
    wchar_t **argv = crt_CommandLineToArgvW ? crt_CommandLineToArgvW(GetCommandLineW(), &argc) : NULL;
    if (argc > 0 && argv != NULL)
    {
        if (argc == 1)
        {
            wchar_t buf[NAMES_LEN];
            buf[NAMES_LEN] = L'\0';
            if (GetEnvironmentVariableW(L"_started_from_console", buf, NAMES_LEN - 1) && (buf[0] == L'y' && buf[1] == L'e' && buf[2] == L's'))
            {
            #ifdef _LOGDEBUG
                logmsg("[libumpv] started from console and no argments\n");
            #endif
                LocalFree(argv);
                return false;
            }
        }
        for (int i = 1; i < argc; ++i)
        {
            if (mp_argument_cmp(argv[i], L"v") == 0 ||
                mp_argument_cmp(argv[i], L"version") == 0 ||
                mp_argument_cmp(argv[i], L"h") == 0 ||
                mp_argument_cmp(argv[i], L"help") == 0 ||
                mp_argument_cmp(argv[i], L"list-options") == 0 ||
                mp_argument_cmp(argv[i], L"register") == 0 ||
                mp_argument_cmp(argv[i], L"unregister") == 0
               )
            {
            #ifdef _LOGDEBUG
                logmsg("[libumpv] No file to play\n");
            #endif
                LocalFree(argv);
                return false;
            }
        }
        LocalFree(argv);
        return true;
    }
    return false;
}
/* uninstall hook and clean up */
void WINAPI undo_it(void)
{
    undo_bosskey();
    uninit_crthook();
    uninit_exeception();
    MH_Uninitialize();
#ifdef _LOGDEBUG
    logmsg("[libumpv] all clean\n");
#endif
    return;
}

void WINAPI do_it(void)
{
    if (!lib_init_once)
    {
    #ifdef _LOGDEBUG   /* 初始化日志记录文件 */
        init_logs();
        logmsg("[libumpv] init log:\n");
    #endif
        if (!init_parser(ini_path, MAX_PATH))
        {
            return;
        }
    }
    if (lib_pid <= 0x4u && (lib_pid = GetCurrentProcessId()) < 0x5u)
    {
        return;
    }
    if (!judg_args())
    {
    #ifdef _LOGDEBUG
        logmsg("[libumpv] no need to load\n");
    #endif
        return;
    }
    if (MH_Initialize() != MH_OK)
    {
        return;
    }
    if (!_InterlockedCompareExchange(&lib_init_once, 1L, 0L))
    {
        init_bosskey();
    }
    if (!(init_crthook() && init_exeception()))
    {
    #ifdef _LOGDEBUG
        logmsg("[libumpv] init_ctw failed...\n");
    #endif
    }
}

#if defined(__GNUC__) && defined(__LTO__)
#pragma GCC pop_options
#endif

/* This is standard DllMain function. */
#ifdef __cplusplus
extern "C" {
#endif

#if !defined(LIBUMPV_STATIC)
int CALLBACK _DllMainCRTStartup(HINSTANCE module, DWORD reason, LPVOID reserved)
{
    (void *)reserved;
    switch(reason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(module);
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
#endif  /* Not LIBUMPV_STATIC */

#ifdef __cplusplus
}
#endif
