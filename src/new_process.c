#include "inipara.h"
#include <shlwapi.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define PROCESS_NUM 10
static  void* g_handle[PROCESS_NUM];

HANDLE search_process(LPCWSTR lpstr, DWORD m_parent)
{
    bool   b_more;
    PROCESSENTRY32W pe32;
    HANDLE hSnapshot = INVALID_HANDLE_VALUE;
    DWORD  chi_pid[PROCESS_NUM] = {0};
    HANDLE m_handle = NULL;
    volatile int    i = 1;
    static   int    h_num = 1;
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if( hSnapshot == INVALID_HANDLE_VALUE )
    {
    #ifdef _LOGDEBUG
        logmsg("CreateToolhelp32Snapshot (of processes) error %lu\n",GetLastError() );
    #endif
        return m_handle;
    }
    chi_pid[0] = m_parent;
    pe32.dwSize=sizeof(pe32);
    b_more = Process32FirstW(hSnapshot,&pe32);
    while (b_more)
    {
        if ( m_parent == pe32.th32ParentProcessID )
        {
            chi_pid[i++] = pe32.th32ProcessID;
            if (i>=PROCESS_NUM)
            {
                break;
            }
        }
        if ( lpstr && pe32.th32ParentProcessID>4 && StrStrIW((LPWSTR)lpstr,(LPCWSTR)pe32.szExeFile) )
        {
            m_handle = (HANDLE)(uintptr_t)pe32.th32ProcessID;
            break;
        }
        b_more = Process32NextW(hSnapshot,&pe32);
    }
    CloseHandle(hSnapshot);
    if ( !m_handle && chi_pid[0] )
    {
        for ( i=1 ; i<PROCESS_NUM&&h_num<PROCESS_NUM; ++i )
        {
            HANDLE tmp = OpenProcess(PROCESS_TERMINATE, false, chi_pid[i]);
            if ( NULL != tmp )
            {
                g_handle[h_num++] = tmp;
                search_process(NULL, chi_pid[i]);
            }
        }
    }
    return m_handle;
}

int get_parameters(LPWSTR wdir, LPWSTR lpstrCmd, DWORD len)
{
    int    ret = -1;
    LPWSTR lp = NULL;
    WCHAR  temp[VALUE_LEN+1]   = {0};
    WCHAR  m_para[VALUE_LEN+1] = {0};
    if ( read_appkeyW(L"attach",L"ExPath",temp,VALUE_LEN,NULL) )
    {
        wdir[0] = L'\0';
        lp =  StrChrW(temp,L',');
        if ( (NULL != lp) && isdigit(temp[wcslen(temp)-1]) )
        {
            ret = temp[wcslen(temp)-1] - L'0';
            temp[lp-temp] = L'\0';
            lp =  StrChrW(temp,L' ');
            /* 如果第三方进程存在参数,工作目录设为浏览器主进程所在目录 */
            if ( lp )
            {
                temp[lp-temp] = L'\0';
                wnsprintfW(m_para,VALUE_LEN,L" "L"%ls",lp+1);
                if ( !getw_cwd(wdir,len) )
                {
                    wdir[0] = L'\0';
                }
            }
            wnsprintfW(lpstrCmd,len,L"%ls",temp);
            if ( lpstrCmd[0] == L'.' || lpstrCmd[0] == L'%' )
            {
                path_to_absolute(lpstrCmd,VALUE_LEN);
            }
            wcsncat(lpstrCmd,m_para,len);
            if ( wcslen(wdir) == 0 )
            {
                wnsprintfW(wdir,len,L"%ls",lpstrCmd);
                if ( !PathRemoveFileSpecW(wdir) )
                {
                    wdir[0] = L'\0';
                }
            }
        }
    }
    return ret;
}

bool WINAPI no_gui_boot(void)
{
    LPWSTR  *args = NULL;
    int     m_arg = 0;
    bool    ret = false;
    args = CommandLineToArgvW(GetCommandLineW(), &m_arg);
    if ( NULL != args )
    {
        int i;
        for (i = 1; i < m_arg; ++i)
        {
            if ( StrStrIW(args[i],L"preferences") || StrStrIW(args[i],L"silent") ||
                 StrStrIW(args[i],L"headless") || StrStrIW(args[i],L"screenshot") )
            {
                ret = true;
                break;
            }
        }
        LocalFree(args);
    }
    return ret;
}

void WINAPI refresh_tray(void)
{
    HWND hwnd ;          /* tray hwnd */
    RECT m_trayToolBar;
    int  x;
    hwnd = FindWindowW(L"Shell_TrayWnd", NULL);
    hwnd = FindWindowExW(hwnd, 0, L"TrayNotifyWnd", NULL);
    hwnd = FindWindowExW(hwnd, 0, L"SysPager", NULL);
    hwnd = FindWindowExW(hwnd, 0, L"ToolbarWindow32", NULL);
    GetClientRect(hwnd, &m_trayToolBar);
    for(x = 1; x < m_trayToolBar.right - 1; x++)
    {
        int y = m_trayToolBar.bottom / 2;
        PostMessage(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
    }
}

void WINAPI kill_trees(void)
{
    if (g_handle[0]>0)
    {
        int i;
        for ( i =0 ; i<PROCESS_NUM && g_handle[i]>0 ; ++i )
        {
            TerminateProcess(g_handle[i], (DWORD)-1);
            CloseHandle(g_handle[i]);
        }
        refresh_tray();
    }
    return;
}

HANDLE WINAPI 
create_new(LPCWSTR wcmd, const LPCWSTR pcd, int flags, DWORD *opid)
{
    PROCESS_INFORMATION pi;
    STARTUPINFOW si;
    DWORD dwCreat = 0;
    if (true)
    {
        fzero(&si,sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        if (flags > 1)
        {
            si.wShowWindow = SW_SHOWNOACTIVATE;
        }
        else if (flags == 1)
        {
            si.wShowWindow = SW_MINIMIZE;
        }
        else if (!flags)
        {
            si.wShowWindow = SW_HIDE;
            dwCreat |= CREATE_NEW_PROCESS_GROUP;
        }
        if(!CreateProcessW(NULL,
                          (LPWSTR)wcmd,
                          NULL,
                          NULL,
                          FALSE,
                          dwCreat,
                          NULL,
                          pcd,   
                          &si,&pi))
        {
        #ifdef _LOGDEBUG
            logmsg("CreateProcessW error %lu\n",GetLastError());
        #endif
            return NULL;
        }
        if (NULL != opid)
        {
            *opid = pi.dwProcessId;
        }
    }
    return pi.hProcess;
}

unsigned WINAPI run_process(void * pParam)
{
    WCHAR wcmd[VALUE_LEN+1] = {0};
    WCHAR pcd[VALUE_LEN+1] = {0};
    int flags = get_parameters(pcd, wcmd, VALUE_LEN);
    if (flags<0)
    {
        return (0);
    }
    /* 如果是无界面启动,直接返回 */
    if ( no_gui_boot() )
    {
        return (0);
    }
    Sleep(1000);  /* 重启外部进程需要延迟一下 */
    if ( wcslen(wcmd)>0 && !search_process(wcmd,0) )
    {
        DWORD pid = 0;      
        g_handle[0] = create_new(wcmd, pcd, flags, &pid);
        if ( g_handle[0] != NULL && (SleepEx(3000,false) == 0) )
        {
            /* 延迟3s,不然无法结束进程树 */
            search_process(NULL, pid);
        }
    }
    return (1);
}
