#include "general.h"
#include "ini_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <shlwapi.h>
#include <tlhelp32.h>
#include <shlobj.h>

#define PROCESS_NUM 10
static  void* g_handle[PROCESS_NUM];

static HANDLE
search_process(LPCWSTR lpstr, DWORD m_parent)
{
    bool   b_more;
    PROCESSENTRY32W pe32;
    HANDLE hSnapshot = INVALID_HANDLE_VALUE;
    DWORD  chi_pid[PROCESS_NUM] = {0};
    HANDLE m_handle = NULL;
    volatile int    i = 1;
    static   int    h_num = 1;
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapshot == INVALID_HANDLE_VALUE)
    {
    #ifdef _LOGDEBUG
        logmsg("CreateToolhelp32Snapshot (of processes) error %lu\n", GetLastError());
    #endif
        return m_handle;
    }
    chi_pid[0] = m_parent;
    pe32.dwSize = sizeof(pe32);
    b_more = Process32FirstW(hSnapshot, &pe32);
    while (b_more)
    {
        if (m_parent == pe32.th32ParentProcessID)
        {
            chi_pid[i++] = pe32.th32ProcessID;
            if (i >= PROCESS_NUM)
            {
                break;
            }
        }
        if (lpstr && pe32.th32ParentProcessID > 4 && StrStrIW((LPWSTR)lpstr, (LPCWSTR)pe32.szExeFile))
        {
            m_handle = (HANDLE)(uintptr_t)pe32.th32ProcessID;
            break;
        }
        b_more = Process32NextW(hSnapshot, &pe32);
    }
    CloseHandle(hSnapshot);
    if (!m_handle && chi_pid[0])
    {
        for (i = 1 ; i < PROCESS_NUM && h_num < PROCESS_NUM; ++i)
        {
            HANDLE tmp = OpenProcess(PROCESS_TERMINATE, false, chi_pid[i]);
            if (NULL != tmp)
            {
                g_handle[h_num++] = tmp;
                search_process(NULL, chi_pid[i]);
            }
        }
    }
    return m_handle;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  分析ExPath里的进程路径和参数以及启动方式,支持相对路径与环境变量
 *  如果末尾使用了,0 将尽可能的隐藏进程运行
 *  如果程序路径使用了双引号,设置工作目录为浏览器安装目录
 *  这意味着如果运行程序有路径方面的参数,
 *  那么路径可设置为相对于浏览器安装目录的相对路径
 */
static int
get_parameters(LPWSTR cmd, LPWSTR param, LPWSTR wdir, int len)
{
    char *path = NULL;
    char *lp = NULL;
    int  pathlen = 0;
    int  ret = -1;
    char temp[MAX_PATH+1] = {0};
    if (!ini_read_string("attach","ExPath",&path,ini_portable_path, true))
    {
        return -1;
    }
    pathlen = (int) strlen(path);
    if ((lp =  strchr(path, ',')) == NULL  || !isdigit(path[pathlen - 1]))
    {
        free(path);
        return -1;
    }
    if (true)
    {
        ret = path[pathlen-1] - '0';
        path[lp-path] = '\0';
    }
    if (*path == '"')
    {
        if ((lp = strchr(&path[1], '\"')) == NULL)
        {
            free(path);
            return -1;
        }
        _snprintf(temp, (lp - path), "%s", &path[1]);
        if ((lp =  strchr(lp, ' ')) != NULL)
        {
            MultiByteToWideChar(CP_UTF8, 0, lp + 1, -1, param, MAX_PATH);
        }
        MultiByteToWideChar(CP_UTF8, 0, temp, -1, cmd, MAX_PATH);
        if (!wget_process_directory(wdir,len))
        {
            wdir[0] = L'\0';
        }
    }
    else
    {
        if ((lp =  strchr(path, ' ')) != NULL)
        {
            path[lp-path] = L'\0';
            MultiByteToWideChar(CP_UTF8, 0, lp + 1, -1, param, MAX_PATH);
        }
        MultiByteToWideChar(CP_UTF8, 0, path, -1, cmd, MAX_PATH);
    }
    if (cmd[0] == L'.' || cmd[0] == L'%')
    {
        path_to_absolute(cmd, MAX_PATH);
    }
    if (*wdir == L'\0')
    {
        _snwprintf(wdir, len, L"%s", cmd);
        if (!PathRemoveFileSpecW(wdir))
        {
            wdir[0] = L'\0';
        }
    }
    free(path);
    return ret;
}

bool WINAPI
no_gui_boot(void)
{
    LPWSTR  *args = NULL;
    int     m_arg = 0;
    bool    ret = false;
    args = CommandLineToArgvW(GetCommandLineW(), &m_arg);
    if (NULL != args)
    {
        int i;
        for (i = 1; i < m_arg; ++i)
        {
            if (StrStrIW(args[i],L"preferences") || StrStrIW(args[i],L"silent") ||
                StrStrIW(args[i],L"headless") || StrStrIW(args[i],L"screenshot"))
            {
                ret = true;
                break;
            }
        }
        LocalFree(args);
    }
    return ret;
}

static void
refresh_tray(void)
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

void WINAPI
kill_trees(void)
{
    if (g_handle[0] > 0)
    {
        int i;
        for (i = 0 ; i < PROCESS_NUM && g_handle[i] > 0 ; ++i)
        {
            TerminateProcess(g_handle[i], (DWORD) - 1);
            CloseHandle(g_handle[i]);
        }
        refresh_tray();
    }
    return;
}

HANDLE WINAPI
create_new(LPCWSTR wcmd, LPCWSTR param, LPCWSTR pcd, int flags, DWORD *opid)
{
    PROCESS_INFORMATION pi;
    STARTUPINFOW si;
    DWORD dwCreat = 0;
    LPCWSTR lp_dir = NULL;
    WCHAR process[MAX_PATH+1] = {0};
    if (pcd != NULL && wcslen(pcd) > 1)
    {
        lp_dir = pcd;
    }
    if (param != NULL && wcslen(param ) > 1)
    {
        _snwprintf(process, MAX_PATH, L"%s %s", wcmd, param);
    }
    else
    {
        _snwprintf(process, MAX_PATH, L"%s", wcmd);
    }
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
                          process,
                          NULL,
                          NULL,
                          FALSE,
                          dwCreat,
                          NULL,
                          lp_dir,
                          &si,&pi))
        {
        #ifdef _LOGDEBUG
            logmsg("CreateProcessW %ls error, cause: %lu\n", wcmd, GetLastError());
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  此函数必须以线程方式调用,因为会阻塞主进程
 */
unsigned WINAPI
run_process(void * lparam)
{
    if (e_browser > MOZ_UNKOWN)
    {
        WCHAR wcmd[MAX_PATH+1] = {0};
        WCHAR pcd[MAX_PATH+1] = {0};
        WCHAR param[MAX_PATH+1] = {0};
        int flags = get_parameters(wcmd, param, pcd, MAX_PATH);
        if (flags < 0)
        {
            return (0);
        }
        /* 如果是无界面启动,直接返回 */
        if (no_gui_boot())
        {
            return (0);
        }
    #ifdef _LOGDEBUG
        logmsg("wcmd[%ls], param[%ls], pcd[%ls]\n", wcmd, param, pcd);
    #endif
        /* 重启外部进程需要延迟一下 */
        Sleep(500);
        if (wcslen(wcmd) > 0 && !search_process(wcmd, 0))
        {
            DWORD pid = 0;
            g_handle[0] = create_new(wcmd, param, pcd, flags, &pid);
            if (g_handle[0] != NULL && (SleepEx(3000, false) == 0))
            {
                /* 延迟,因为有可能进程还没创建,无法结束进程树 */
                search_process(NULL, pid);
            }
        }
    }
    return 0;
}
