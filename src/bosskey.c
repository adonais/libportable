#include "inipara.h"
#include "bosskey.h"
#include "internal_crt.h"
#include <shlwapi.h>
#include <commctrl.h>

static WNDINFO mph;
static WNDPROC boss_wnd;
static volatile long boss_t;
extern volatile DWORD lib_pid;
extern volatile long lib_init_once;
extern volatile INT_PTR mpv_window_hwnd;

static bool
is_mozclass(HWND hwnd)
{
    WCHAR m_temp[VALUE_LEN+1];
    GetClassNameW(hwnd, m_temp, VALUE_LEN);
    return (api_wcsnicmp(m_temp, L"mpv", VALUE_LEN) == 0);
}

static int CALLBACK
find_chwnd(HWND hwnd, LPARAM lParam)
{
    LPWNDINFO pinfo = (LPWNDINFO)lParam;
    DWORD dw_pid = 0;
    GetWindowThreadProcessId(hwnd, &dw_pid);
    if ((dw_pid == pinfo->pid) && dw_pid && IsWindowVisible(hwnd))
    {
        if (is_mozclass(hwnd))
        {
            ShowWindow(hwnd,SW_MINIMIZE);
            ShowWindow(hwnd,SW_HIDE);
        }
    }
    else if ((dw_pid == pinfo->pid) && dw_pid && !IsWindowVisible(hwnd))
    {
        if (is_mozclass(hwnd))
        {
            SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, 3);
            ShowWindow(hwnd,SW_SHOW);
            if (IsIconic(hwnd))
            {
                ShowWindow(hwnd,SW_RESTORE);
            }
        }
    }
    return true;
}

static inline bool
is_modkey(int n)
{
    return (n == MOD_ALT ||
            n == MOD_CONTROL ||
            n == MOD_SHIFT ||
            n == MOD_WIN) ;
}

static void
set_hotkey(LPWNDINFO pinfo)
{
    const WCHAR delim[] = L"+";
    WCHAR tmp_stor[3][16];
    pinfo->key_mod = 0x06;          /* CONTROL+SHIFT 键 */
    pinfo->key_vk = 0xc0;           /* ~键  */
    api_memset(tmp_stor, 0, sizeof(tmp_stor));
    if (true)
    {
        int    i = 0;
        LPWSTR p = pinfo->key_str;
        int    tmp[3] = {0};
        int    num;
        LPWSTR strtmp = (LPWSTR)api_wcsstr(pinfo->key_str, delim);
        while(strtmp != NULL && i < 3)
        {
            strtmp[0] = L'\0';
            api_wcsncpy(tmp_stor[i++],p,15);
            p = strtmp + api_wcslen(delim);
            strtmp = (LPWSTR)api_wcsstr( p, delim);
            if (!strtmp)
            {
                api_wcsncpy(tmp_stor[i],p,15);
            }
        }
        for (num = 0 ; num <= i ; num++)
        {
            tmp[num] = StrToIntW(tmp_stor[num]);
        }
        if (is_modkey(tmp[0]))
        {
            if ((i==2) && is_modkey(tmp[1]) && !is_modkey(tmp[2]))
            {
                pinfo->key_mod = tmp[0]|tmp[1];
                pinfo->key_vk = tmp[2];
            }
            else if (i==1 && tmp[1]>0x2f)
            {
                pinfo->key_mod = tmp[0];
                pinfo->key_vk = tmp[1];
            }
        }
    }
}

static bool
get_moz_hwnd(LPWNDINFO pinfo)
{
    int i = 10;
    while (!pinfo->h && i--)    /* 等待主窗口并获取句柄 */
    {
        HWND  hAll = GetDesktopWindow();
        HWND  hcur = GetNextWindow(hAll, GW_CHILD);
        while (hcur != NULL)
        {
            DWORD pid = 0;
            WCHAR szclass[VALUE_LEN+1];
            if (GetClassNameW(hcur, szclass, VALUE_LEN))
            {
                GetWindowThreadProcessId(hcur, &pid);
            }
            if (pid > 0 && pid == pinfo->pid && !api_wcscmp(szclass, L"mpv"))
            {
                pinfo->h = hcur;
                mpv_window_hwnd = (INT_PTR)pinfo->h;
            #ifdef _LOGDEBUG
                logmsg("[bosskey_thread] mpv_window_hwnd = %08x\n", mpv_window_hwnd);
            #endif
                return true;
            }
            hcur = GetNextWindow(hcur, GW_HWNDNEXT);
        }
        SleepEx(800,false);
    }
    return false;
}

static bool
regster_hotkey(LPWNDINFO pinfo)
{
    WCHAR atom[8+1];
    api_snwprintf(atom, 8, L"%u", pinfo->pid);
    pinfo->atom_str = GlobalAddAtomW(atom)-0xC000;
    set_hotkey(pinfo);
    if (!RegisterHotKey(NULL, pinfo->atom_str, pinfo->key_mod, pinfo->key_vk))
    {
    #ifdef _LOGDEBUG
        logmsg("[bosskey_thread] RegisterHotKey error: %u\n", GetLastError());
    #endif
        return false;
    }
    return true;
}

static LRESULT CALLBACK
bosskey_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_DESTROY:
        {
            PostThreadMessage(boss_t, WM_QUIT, 0, 0);
            break;
        }
        default:
            break;
    }
    return CallWindowProc(boss_wnd, hwnd, message, wParam, lParam);
}

static DWORD WINAPI
bosskey_thread(void * lparam)
{
    if (get_moz_hwnd(&mph) && read_appkey(L"libumpv", L"#HotKey", mph.key_str, sizeof(mph.key_str), NULL) && regster_hotkey(&mph))
    {   /* 重定向主窗口循环, 主要是让bosskey_thread正常退出 */
        if ((boss_wnd = (WNDPROC) SetWindowLongPtrW(mph.h, GWLP_WNDPROC, (LONG_PTR) bosskey_proc)))
        {
            MSG msg;
            while (GetMessageW(&msg, NULL, 0, 0) > 0 && msg.message != WM_QUIT)
            {
                if (msg.message == WM_HOTKEY)
                {
                    EnumWindows(find_chwnd, (LPARAM)&mph);
                }
            }
        #ifdef _LOGDEBUG
            logmsg("[bosskey_thread] exit\n");
        #endif
        }
    }
    return (0);
}

void WINAPI
undo_bosskey(void)
{
    if (boss_t)
    {
        UnregisterHotKey(NULL, mph.atom_str);
        GlobalDeleteAtom(mph.atom_str);
        _InterlockedExchange(&boss_t, 0);
        boss_wnd = NULL;
    }
    if (lib_pid == mph.pid)
    {
        _InterlockedExchange(&lib_init_once, 0);
    }
}

void WINAPI
init_bosskey(void)
{
    api_memset(&mph, 0, sizeof(WNDINFO));
    mph.pid = lib_pid;
    if (!boss_t)
    {
        CloseHandle((HANDLE)CreateThread(NULL, 0, &bosskey_thread, NULL, 0, (DWORD *)&boss_t));
    }
}
