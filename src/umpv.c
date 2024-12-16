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
#pragma data_seg(".shrd")
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
    if (MH_Initialize() != MH_OK)
    {
        return;
    }
    if (!lib_init_once)
    {
        init_bosskey();
        *(long volatile*)&lib_init_once = 1;
    }
    if (!(init_crthook(NULL) && init_exeception(NULL)))
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
