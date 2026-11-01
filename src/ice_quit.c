#ifndef LIBPORTABLE_STATIC
#define TETE_BUILD
#endif

#include "general.h"
#include "bosskey.h"
#include "on_tabs.h"
#include "ini_parser.h"
#include "json_paser.h"
#include "new_process.h"
#include "portable.h"
#include "ice_quit.h"
#include <process.h>

static HHOOK proc_hook = NULL;
static HWND  proc_hwnd = NULL;
static volatile long proc_once = 0;

#ifdef DLL_INJECT
typedef VOID (WINAPI *ExitProcessPtr)(UINT uExitCode);
static ExitProcessPtr pExitProcess, sExitProcess;

extern void window_hooks(void);
extern void wait_observer(void);
#endif

static unsigned WINAPI
proc_thread(void *lparam)
{
    proc_hwnd = get_moz_hwnd();
    return 0;
}

static void
proc_tab_envent(const int ids, const char *key)
{
    if (!ids)
    {
        ini_write_string("tabs", key, "0", ini_portable_path);
    }
    else if (ids > 0)
    {
        ini_write_string("tabs", key, !strcmp(key, "mouse_time") ? "200" : "1", ini_portable_path);
    }
    on_tabs_reload();
}

static void
proc_message(int msg, intptr_t vaule)
{
    switch (msg)
    {
        case PORTABLE_UP:
        {
            if (!vaule)
            {
                ini_write_string("General", "Update", "0", ini_portable_path);
            }
            else if (vaule > 0)
            {
                ini_write_string("General", "Update", "1", ini_portable_path);
            }
            break;
        }
        case PORTABLE_BOS_0:
        {
            uint32_t bossid = 0;
            if ((bossid = get_bosskey_id()) > 0)
            {
                PostThreadMessage(bossid, WM_QUIT, 0, 0);
                uninstall_bosskey();
            }
            if (!vaule)
            {
                ini_write_string("General", "Bosskey", "0", ini_portable_path);
            }
            else if (vaule > 0)
            {
                ini_write_string("General", "Bosskey", "1", ini_portable_path);
                CloseHandle((HANDLE)_beginthreadex(NULL, 0, &bosskey_thread, NULL, 0, NULL));
            }
            break;
        }
        case PORTABLE_TAB_0:
        {
            un_uia();
            if (!vaule)
            {
                ini_write_string("General", "OnTabs", "0", ini_portable_path);
            }
            else if (vaule > 0)
            {
                ini_write_string("General", "OnTabs", "1", ini_portable_path);
                threads_on_tabs();
            }
            break;
        }
        case PORTABLE_TAB_1:
        {
            proc_tab_envent((const int)vaule, "mouse_time");
            break;
        }
        case PORTABLE_TAB_2:
        {
            proc_tab_envent((const int)vaule, "double_click_close");
            break;
        }
        case PORTABLE_TAB_3:
        {
            proc_tab_envent((const int)vaule, "double_click_new");
            break;
        }
        case PORTABLE_TAB_4:
        {
            proc_tab_envent((const int)vaule, "mouse_hover_close");
            break;
        }
        case PORTABLE_TAB_5:
        {
            proc_tab_envent((const int)vaule, "mouse_hover_new");
            break;
        }
        case PORTABLE_TAB_6:
        {
            proc_tab_envent((const int)vaule, "right_click_close");
            break;
        }
        case PORTABLE_TAB_7:
        {
            proc_tab_envent((const int)vaule, "right_click_recover");
            break;
        }
        case PORTABLE_UBO:
        {
            char *pfast = NULL;
            uintptr_t param = 0;
            ini_read_string("update", "faster", &pfast, ini_portable_path, true);
            if (vaule > 0)
            {
                if (pfast)
                {
                    param |= 0x3;
                }
                else
                {
                    param |= 0x1; 
                }
                ini_write_string("General", "EnableUBO", "1", ini_portable_path);
            }
            else
            {
                if (pfast)
                {
                    param |= 0x2;
                }
                ini_write_string("General", "EnableUBO", "0", ini_portable_path);
            }
            CloseHandle((HANDLE) _beginthreadex(NULL, 0, &fn_ubo, (void *)param, 0, NULL));
            if (pfast)
            {
                free(pfast);
            }
            break;
        }
        case PORTABLE_CHR:
        {
            if (!vaule)
            {
                ini_write_string("update", "faster", NULL, ini_portable_path);
            }
            else if (vaule > 0)
            {
                ini_write_string("update", "faster", "https://gh-proxy.org/sourceforge", ini_portable_path);
            }
            break;
        }
    #if defined(DLL_INJECT)
        case PORTABLE_HOOK:
        {
            proc_hwnd = (HWND)vaule;
            window_hooks();
        #ifdef _LOGDEBUG
            logmsg("we recv PORTABLE_HOOK[%p] message\n", proc_hwnd);
        #endif
            break;
        }
    #endif
        default:
        {
            break;
        }
    }
}

static inline void
proc_unhook(void)
{
    if (proc_hook)
    {
        UnhookWindowsHookEx(proc_hook);
        proc_hook = NULL;
    #ifdef _LOGDEBUG
        logmsg("proc_unhook runing!\n");
    #endif
    }
}

static LRESULT WINAPI
proc_function(int code, WPARAM wparam, LPARAM lparam)
{
    PCWPSTRUCT pcs = (PCWPSTRUCT)lparam;
    if (pcs)
    {
        if (!proc_hwnd && !_InterlockedCompareExchange(&proc_once, 1, 0) && _wgetenv(L"LIBPORTABLE_UI_PROCESS"))
        {
            CloseHandle((HANDLE)_beginthreadex(NULL, 0, &proc_thread, NULL, 0, NULL));
        #ifdef _LOGDEBUG
            logmsg("find main window hwnd\n");
        #endif
        }
        if (proc_hwnd && pcs->hwnd == proc_hwnd && pcs->message == WM_DESTROY)
        {
            uint32_t bossid;
        #if defined(DLL_INJECT) || defined(ESR115)
            if (_wgetenv(L"LIBPORTABLE_FIRST_RUN"))
            {
            #ifdef _LOGDEBUG
                logmsg("we ignore WM_DESTROY message\n");
            #endif
                return CallNextHookEx(proc_hook, code, wparam, lparam);
            }
        #endif
        #ifdef _LOGDEBUG
            logmsg("we recv WM_DESTROY message\n");
        #endif
            if ((bossid = get_bosskey_id()) > 0)
            {
                PostThreadMessage(bossid, WM_QUIT, 0, 0);
            }
            if (ini_read_int("aria2", "close", ini_portable_path, true) > 0)
            {
                wchar_t wcmd[MAX_PATH+1] = {0};
                if (wget_process_directory(wcmd, MAX_PATH))
                {
                    wp_wcsncat(wcmd, L"\\upcheck.exe -a2quit", MAX_PATH);
                    CloseHandle(create_new(wcmd, NULL, NULL, 0, NULL));
                }
            }
            proc_unhook();
            undo_it();
        }
        else
        {
            proc_message(pcs->message, (int)pcs->wParam);
        }
    }
    return CallNextHookEx(proc_hook, code, wparam, lparam);
}

int ctype_message_caller(int msg, int vaule)
{
    proc_message(msg, vaule);
    return 0;
}

int ctype_download_caller(int id, const char *url, const char *name, const char *save, const char *refer, const char *cookie, const char *buf)
{
    if (id >= 0  && url)
    {
        wchar_t *purl = ini_utf8_utf16(url, NULL);
        wchar_t *pname = name ? ini_utf8_utf16(name, NULL) : _wcsdup(L"");
        wchar_t *psave = save ? ini_utf8_utf16(save, NULL) : _wcsdup(L"");
        wchar_t *prefer = refer ? ini_utf8_utf16(refer, NULL) : _wcsdup(L"");
        wchar_t *pck = cookie ? ini_utf8_utf16(cookie, NULL) : _wcsdup(L"");
        wchar_t *pbuf = buf ? ini_utf8_utf16(buf, NULL) : _wcsdup(L"");
        const size_t len = (pname ? wcslen(pname) : 1) +
                           (psave ? wcslen(psave) : 1) +
                           (prefer ? wcslen(prefer) : 1) +
                           (pck ? wcslen(pck) : 1) +
                           (pbuf ? wcslen(pbuf) : 1) +
                           (purl ? wcslen(purl) : 1) +
                           MAX_BUFF;
        wchar_t *wcmd = (wchar_t *)calloc(sizeof(wchar_t), len + 1);
        if (id >= 0 && purl && pname && psave && prefer && pck && pbuf && wcmd)
        {
            wchar_t *p = NULL;
            wchar_t  dirs[MAX_PATH+1] = {0};
            if ((GetModuleFileNameW(NULL, dirs, MAX_PATH) > 0) && (p = wcsrchr(dirs, L'\\')) != NULL)
            {
                p[1] = 0;
            }
            if (*dirs)
            {
                _snwprintf(wcmd, len, L"%s"_UPDATE L"-m %d -i \"%s\" -ref \"%s\" -b \"%s\" -cok \"%s\"", dirs, id, purl, prefer, pck, pbuf);
                *dirs = L'\0';
                if (wcslen(psave) > 0 && wcslen(pname) > 0)
                {
                    _snwprintf(dirs, MAX_PATH, L"\"%s\\%s\"", psave, pname);
                }
                else if (wcslen(pname) > 0)
                {
                    _snwprintf(dirs, MAX_PATH, L"\"%s\"", pname);
                }
                if (wcslen(dirs) > 1)
                {
                    wp_wcsncat(wcmd, L" -o ", len);
                    wp_wcsncat(wcmd, dirs, len);
                }
            }
            if (*wcmd)
            {
                CloseHandle(create_new(wcmd, NULL, NULL, 0, NULL));
            }
        }
        ini_safe_free(purl);
        ini_safe_free(pname);
        ini_safe_free(psave);
        ini_safe_free(prefer);
        ini_safe_free(pck);
        ini_safe_free(pbuf);
    }
    return 0;
}

#ifdef DLL_INJECT
static WINAPI VOID
HookExitProcess(UINT uExitCode)
{
    wait_observer();
#ifdef _LOGDEBUG
    logmsg("[%lu]exitcode = %u!\n", GetCurrentProcessId(), uExitCode);
#endif
    return sExitProcess(uExitCode);
}
#endif

void WINAPI
init_exemsg(uint32_t tid)
{
    if (e_browser > MOZ_UNKOWN && is_browser())
    {
        proc_hook = SetWindowsHookExW(WH_CALLWNDPROC, proc_function, dll_module, tid);
        if (proc_hook == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("SetWindowsHookEx WH_CALLWNDPROC failed, error = %lu!\n", GetLastError());
        #endif
        }
    }
}

#ifdef DLL_INJECT
void WINAPI
init_exequit(void)
{
    if (e_browser > MOZ_UNKOWN && is_browser())
    {
        HMODULE hkernel;
        if (!(hkernel = GetModuleHandleW(L"kernel32.dll")))
        {
        #ifdef _LOGDEBUG
            logmsg("GetModuleHandleW(kernel32.dll) failed!\n");
        #endif
        }
        pExitProcess = (ExitProcessPtr)GetProcAddress(hkernel, "ExitProcess");
        if (!creator_hook(pExitProcess, HookExitProcess, (LPVOID*)&sExitProcess))
        {
        #ifdef _LOGDEBUG
            logmsg("pExitProcess hook failed!\n");
        #endif
        }
    }
}
#endif
