#define BOSS_EXTERN

#include "bosskey.h"
#include "inipara.h"
#include <string.h>
#include <shlwapi.h>

BOOL init_bosskey(LPWNDINFO pInfo)
{
    HWND    hwnd = NULL;
    while ( !pInfo->hFF )                 /* 等待主窗口并获取句柄 */
    {
        BOOL  m_loop = FALSE;
        DWORD dwProcessId = 0;
        HWND  hwnd_next = NULL;
        hwnd = FindWindowExW( NULL, hwnd, L"MozillaWindowClass", NULL );
        GetWindowThreadProcessId(hwnd, &dwProcessId);
        m_loop = (dwProcessId>0 &&dwProcessId == pInfo->hPid);
        if ( !m_loop )
        {
            pInfo->hFF = NULL;
        }
        if (NULL != hwnd && m_loop)
        {
            pInfo->hFF = hwnd;
            break;
        }
        Sleep(800);
    }
    if ( 1 )
    {
        WCHAR atom[8+1] = {0};
        _ui64tow(pInfo->hPid,atom,10);
    #ifdef _LOGDEBUG
       logmsg("atom = %ls\n",atom);
    #endif
        pInfo->atom_str = GlobalAddAtomW(atom)-0xC000;
    }
    return RegisterHotKey(NULL, pInfo->atom_str, pInfo->key_mod, pInfo->key_vk);
}

BOOL is_mozclass(HWND hwnd)
{
    WCHAR m_temp[VALUE_LEN+1] = {0};
    GetClassNameW(hwnd,m_temp,VALUE_LEN);
    return ( _wcsnicmp(m_temp,L"MozillaWindowClass",VALUE_LEN) ==0 ||
             _wcsnicmp(m_temp,L"MozillaDialogClass",VALUE_LEN) ==0 ) ;
}

BOOL CALLBACK find_chwnd(HWND hwnd, LPARAM lParam)
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
    return TRUE;
}

BOOL is_modkey(int n)
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
    if ( read_appkey(L"attach",L"Hotkey",lpstr,sizeof(lpstr)) )
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

unsigned WINAPI bosskey_thread(void * lparam)
{
    MSG msg;
    LPWNDINFO lpInfo = (LPWNDINFO)lparam;
    set_hotkey(lpInfo);
    if ( is_browser()  || is_thunderbird() )
    {
        if ( init_bosskey(lpInfo) )
        {
            while( GetMessageW(&msg, NULL, 0, 0) > 0 )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                EnumWindows(find_chwnd, (LPARAM)lpInfo);
            }
        }
    }
    return (1);
}
