#ifndef LIBPORTABLE_STATIC
#define TETE_BUILD
#endif

#include "ice_quit.h"
#include "general.h"
#include "bosskey.h"
#include "on_tabs.h"
#include "ini_parser.h"
#include "json_paser.h"
#include "new_process.h"
#include "portable.h"
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
                    if (ini_read_int("aria2", "close", ini_portable_path, true) > 0)
                    {
                        wchar_t *p = NULL;
                        wchar_t wcmd[MAX_PATH+1] = {0};
                        if (GetModuleFileNameW(NULL, wcmd, MAX_PATH) > 0 && (p = wcsrchr(wcmd, L'\\')) != NULL)
                        {
                            p[1] = L'\0';
                            wcsncat(wcmd, L"upcheck.exe -a2quit", MAX_PATH);
                            CloseHandle(create_new(wcmd, NULL, NULL, 0, NULL));
                        }
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
                if (wcslen(psave) > 0)
                {
                    _snwprintf(dirs, MAX_PATH, L"\"%s\\%s\"", psave, pname);
                }
                else
                {
                    _snwprintf(dirs, MAX_PATH, L"\"%s\"", pname);
                }
                if (wcslen(dirs) > 1)
                {
                    wcsncat(wcmd, L" -o ", len);
                    wcsncat(wcmd, dirs, len);
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
