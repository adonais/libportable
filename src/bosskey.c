#include "general.h"
#include "ini_parser.h"
#include <string.h>
#include <shlwapi.h>

static int g_atom = 0;

bool
init_bosskey(LPWNDINFO pinfo)
{
    WCHAR atom[8 + 1] = {0};
    if (!get_moz_hwnd(pinfo))
    {
        return false;
    }
    _ui64tow(pinfo->hPid, atom, 10);
    pinfo->atom_str = GlobalAddAtomW(atom) - 0xC000;
    return RegisterHotKey(NULL, pinfo->atom_str, pinfo->key_mod, pinfo->key_vk);
}

bool
is_mozclass(HWND hwnd)
{
    WCHAR m_temp[VALUE_LEN + 1] = {0};
    GetClassNameW(hwnd, m_temp, VALUE_LEN);
    return (_wcsnicmp(m_temp, L"MozillaWindowClass", VALUE_LEN) == 0 ||
            _wcsnicmp(m_temp, L"MozillaDialogClass", VALUE_LEN) == 0) ;
}

int CALLBACK
find_chwnd(HWND hwnd, LPARAM lParam)
{
    LPWNDINFO pinfo = (LPWNDINFO)lParam;
    DWORD dwPid = 0;
    GetWindowThreadProcessId(hwnd, &dwPid);
    if ((dwPid == pinfo->hPid) && dwPid && IsWindowVisible(hwnd))
    {
        if (is_mozclass(hwnd))
        {
            ShowWindow(hwnd, SW_HIDE);
        }
    }
    else if ((dwPid == pinfo->hPid) && dwPid && !IsWindowVisible(hwnd))
    {
        if (is_mozclass(hwnd))
        {
            SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, 3);
            ShowWindow(hwnd, SW_SHOW);
            if (IsIconic(hwnd))
            {
                ShowWindow(hwnd, SW_RESTORE);
            }
        }
    }
    return true;
}

bool
is_modkey(int n)
{
    return (n == MOD_ALT ||
            n == MOD_CONTROL ||
            n == MOD_SHIFT ||
            n == MOD_WIN) ;
}

void
set_hotkey(LPWNDINFO pinfo)
{
    char *lpstr = NULL;
    const char *delim = "+";
    char  tmp_stor[3][16] = { {0,0} };
    pinfo->key_mod = 0x06;          /* CONTROL+SHIFT 键 */
    pinfo->key_vk = 0xc0;           /* ~键  */
    if (ini_read_string("attach", "Hotkey", &lpstr, ini_portable_path, true))
    {
        int	i = 0;
        char *p = lpstr;
        int	tmp[3] = {0};
        int	num;
        char *strtmp = strstr(lpstr, delim);
        while(strtmp != NULL && i < 3)
        {
            strtmp[0]='\0';
            strncpy(tmp_stor[i++], p, 15);
            p = strtmp + strlen(delim);
            strtmp = strstr(p, delim);
            if (!strtmp)
            {
                strncpy(tmp_stor[i], p, 15);
            }
        }
        for (num = 0 ; num <= i ; num++)
        {
            tmp[num] = atoi(tmp_stor[num]);
        }
        if (is_modkey(tmp[0]))
        {
            if ((i == 2) && is_modkey(tmp[1]) && !is_modkey(tmp[2]))
            {
                pinfo->key_mod = tmp[0]|tmp[1];
                pinfo->key_vk = tmp[2];
            }
            else if (i == 1 && tmp[1] > 0x2f )
            {
                pinfo->key_mod = tmp[0];
                pinfo->key_vk = tmp[1];
            }
        }
    }
}

void WINAPI
uninstall_bosskey(void)
{
    if (g_atom)
    {
        UnregisterHotKey(NULL, g_atom);
        GlobalDeleteAtom((ATOM)(g_atom + 0xC000));
        g_atom = 0;
    }
}

unsigned WINAPI
bosskey_thread(void *lparam)
{
    if (is_specialapp(L"Iceweasel.exe") || is_specialapp(L"firefox.exe"))
    {
        WNDINFO  ff_info = {0};
        ff_info.hPid = GetCurrentProcessId();
        set_hotkey(&ff_info);
        if (init_bosskey(&ff_info))
        {
            MSG msg;
            g_atom = ff_info.atom_str;
            while (GetMessageW(&msg, NULL, 0, 0) > 0)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                EnumWindows(find_chwnd, (LPARAM)&ff_info);
            }
        }
    }
    return 0;
}
