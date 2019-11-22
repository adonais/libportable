#include "inipara.h"
#include <string.h>
#include <shlwapi.h>

static int g_atom = 0;

bool init_bosskey(LPWNDINFO pInfo)
{
    WCHAR atom[8+1] = {0};
    if ( !get_moz_hwnd(pInfo) )
    {
        return false;
    }
    _ui64tow(pInfo->hPid,atom,10);
    pInfo->atom_str = GlobalAddAtomW(atom)-0xC000;    
    return RegisterHotKey(NULL, pInfo->atom_str, pInfo->key_mod, pInfo->key_vk);
}

bool is_mozclass(HWND hwnd)
{
    WCHAR m_temp[VALUE_LEN+1] = {0};
    GetClassNameW(hwnd,m_temp,VALUE_LEN);
    return ( _wcsnicmp(m_temp,L"MozillaWindowClass",VALUE_LEN) ==0 ||
             _wcsnicmp(m_temp,L"MozillaDialogClass",VALUE_LEN) ==0 ) ;
}

int CALLBACK find_chwnd(HWND hwnd, LPARAM lParam)
{
    LPWNDINFO pInfo = (LPWNDINFO)lParam;
    DWORD dwPid = 0;
    GetWindowThreadProcessId(hwnd, &dwPid);
    if ( (dwPid == pInfo->hPid) && dwPid && IsWindowVisible(hwnd) )
    {
        if ( is_mozclass(hwnd) )
        {
            ShowWindow(hwnd,SW_HIDE);
        }
    }
    else if ( (dwPid == pInfo->hPid) && dwPid && !IsWindowVisible(hwnd) )
    {
        if ( is_mozclass(hwnd) )
        {
            SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, 3);
            ShowWindow(hwnd,SW_SHOW);
            if ( IsIconic(hwnd) )
            {
                ShowWindow(hwnd,SW_RESTORE);
            }
        }
    }
    return true;
}

bool is_modkey(int n)
{
    return ( n == MOD_ALT ||
             n == MOD_CONTROL ||
             n == MOD_SHIFT ||
             n == MOD_WIN ) ;
}

void set_hotkey(LPWNDINFO pInfo)
{
    WCHAR	lpstr[VALUE_LEN+1];
    const	WCHAR delim[] = L"+";
    WCHAR   tmp_stor[3][16] = { {0,0} };
    pInfo->key_mod = 0x06;          /* CONTROL+SHIFT 键 */
    pInfo->key_vk = 0xc0;           /* ~键  */
    if ( read_appkeyW(L"attach",L"Hotkey",lpstr,VALUE_LEN,NULL) )
    {
        int		i = 0;
        LPWSTR	p = lpstr;
        int		tmp[3] = {0};
        int		num;
        LPWSTR strtmp = StrStrW(lpstr, delim);
        while( strtmp != NULL && i < 3 )
        {
            strtmp[0]=L'\0';
            wcsncpy(tmp_stor[i++],p,15);
            p = strtmp + wcslen(delim);
            strtmp = StrStrW( p, delim);
            if (!strtmp)
            {
                wcsncpy(tmp_stor[i],p,15);
            }
        }
        for ( num = 0 ; num <= i ; num++ )
        {
            tmp[num] = StrToIntW(tmp_stor[num]);
        }
        if ( is_modkey(tmp[0]) )
        {
            if ( (i==2) && is_modkey(tmp[1]) && !is_modkey(tmp[2]) )
            {
                pInfo->key_mod = tmp[0]|tmp[1];
                pInfo->key_vk = tmp[2];
            }
            else if ( i==1 && tmp[1]>0x2f )
            {
                pInfo->key_mod = tmp[0];
                pInfo->key_vk = tmp[1];
            }
        }
    }
}

void WINAPI uninstall_bosskey(void)
{
    if (g_atom)
    {
        UnregisterHotKey(NULL, g_atom);
        GlobalDeleteAtom(g_atom);  
        g_atom = 0;
    }     
}

unsigned WINAPI bosskey_thread(void *lparam)
{
    WNDINFO  ff_info = { 0 };
    ff_info.hPid = GetCurrentProcessId();
    set_hotkey(&ff_info);
    if ( init_bosskey(&ff_info) )
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
    return (1);
}
