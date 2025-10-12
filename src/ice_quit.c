#include "ice_quit.h"
#include "general.h"
#include "bosskey.h"
#include "on_tabs.h"
#include "ini_parser.h"
#include "json_paser.h"
#include <process.h>

#define PORTABLE_UP    (WM_USER + 0x4E20)
#define PORTABLE_BOS_0 (PORTABLE_UP + 0x1)
#define PORTABLE_TAB_0 (PORTABLE_UP + 0x2)
#define PORTABLE_TAB_1 (PORTABLE_UP + 0x3)
#define PORTABLE_TAB_2 (PORTABLE_UP + 0x4)
#define PORTABLE_TAB_3 (PORTABLE_UP + 0x5)
#define PORTABLE_TAB_4 (PORTABLE_UP + 0x6)
#define PORTABLE_TAB_5 (PORTABLE_UP + 0x7)
#define PORTABLE_TAB_6 (PORTABLE_UP + 0x8)
#define PORTABLE_TAB_7 (PORTABLE_UP + 0x9)
#define PORTABLE_UBO   (PORTABLE_UP + 0x10)
#define PORTABLE_CHR   (PORTABLE_UP + 0x11)

static HHOOK proc_hook = NULL;
static HWND  proc_hwnd = NULL;
static volatile long proc_once = 0;

extern void WINAPI undo_it(void);

static unsigned WINAPI
proc_thread(void *lparam)
{
    WNDINFO  ff_info = {0};
    ff_info.hPid = GetCurrentProcessId();
    proc_hwnd = get_moz_hwnd(&ff_info);
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

static LRESULT WINAPI
proc_function(int code, WPARAM wparam, LPARAM lparam)
{
    uint32_t bossid = 0;
    PCWPSTRUCT pcs = (PCWPSTRUCT)lparam;
    if (pcs)
    {
        if (!proc_hwnd && !_InterlockedCompareExchange(&proc_once, 1, 0))
        {
            CloseHandle((HANDLE)_beginthreadex(NULL, 0, &proc_thread, NULL, 0, NULL));
        }
        else if (pcs->hwnd == proc_hwnd)
        {
            switch (pcs->message)
            {
                case WM_DESTROY:
                {
                    if ((bossid = get_bosskey_id()) > 0)
                    {
                        PostThreadMessage(bossid, WM_QUIT, 0, 0);
                    }
                    UnhookWindowsHookEx(proc_hook);
                    proc_hook = NULL;
                    undo_it();
                    return 1;
                }
                case PORTABLE_UP:
                {
                    if (!pcs->wParam)
                    {
                        ini_write_string("General", "Update", "0", ini_portable_path);
                    }
                    else if (pcs->wParam > 0)
                    {
                        ini_write_string("General", "Update", "1", ini_portable_path);
                    }
                    break;
                }
                case PORTABLE_BOS_0:
                {
                    if ((bossid = get_bosskey_id()) > 0)
                    {
                        PostThreadMessage(bossid, WM_QUIT, 0, 0);
                        uninstall_bosskey();
                    }
                    if (!pcs->wParam)
                    {
                        ini_write_string("General", "Bosskey", "0", ini_portable_path);
                    }
                    else if (pcs->wParam > 0)
                    {
                        ini_write_string("General", "Bosskey", "1", ini_portable_path);
                        CloseHandle((HANDLE)_beginthreadex(NULL, 0, &bosskey_thread, NULL, 0, NULL));
                    }
                    break;
                }
                case PORTABLE_TAB_0:
                {
                    un_uia();
                    if (!pcs->wParam)
                    {
                        ini_write_string("General", "OnTabs", "0", ini_portable_path);
                    }
                    else if (pcs->wParam > 0)
                    {
                        ini_write_string("General", "OnTabs", "1", ini_portable_path);
                        threads_on_tabs();
                    }
                    break;
                }
                case PORTABLE_TAB_1:
                {
                    proc_tab_envent((const int)pcs->wParam, "mouse_time");
                    break;
                }
                case PORTABLE_TAB_2:
                {
                    proc_tab_envent((const int)pcs->wParam, "double_click_close");
                    break;
                }
                case PORTABLE_TAB_3:
                {
                    proc_tab_envent((const int)pcs->wParam, "double_click_new");
                    break;
                }
                case PORTABLE_TAB_4:
                {
                    proc_tab_envent((const int)pcs->wParam, "mouse_hover_close");
                    break;
                }
                case PORTABLE_TAB_5:
                {
                    proc_tab_envent((const int)pcs->wParam, "mouse_hover_new");
                    break;
                }
                case PORTABLE_TAB_6:
                {
                    proc_tab_envent((const int)pcs->wParam, "right_click_close");
                    break;
                }
                case PORTABLE_TAB_7:
                {
                    proc_tab_envent((const int)pcs->wParam, "right_click_recover");
                    break;
                }
                case PORTABLE_UBO:
                {
                    if (!pcs->wParam)
                    {
                        ini_write_string("General", "EnableUBO", "0", ini_portable_path);
                        CloseHandle((HANDLE) _beginthreadex(NULL, 0, &fn_ubo, (void *)(uintptr_t)0, 0, NULL));
                    }
                    else if (pcs->wParam > 0)
                    {
                        ini_write_string("General", "EnableUBO", "1", ini_portable_path);
                        CloseHandle((HANDLE) _beginthreadex(NULL, 0, &fn_ubo, (void *)(uintptr_t)1, 0, NULL));
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
    return CallNextHookEx(proc_hook, code, wparam, lparam);
}

void WINAPI
init_exequit(void)
{
    if (e_browser > MOZ_UNKOWN && is_browser())
    {
        proc_hook = SetWindowsHookExW(WH_CALLWNDPROC, proc_function, dll_module, GetCurrentThreadId());
        if (proc_hook == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("SetWindowsHookEx WH_CALLWNDPROC failed, error = %lu!\n", GetLastError());
        #endif
        }
    }
}
