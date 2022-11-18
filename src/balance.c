#include "general.h"
#include "ini_parser.h"
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>

/**************************************************************************
 * 时间转换函数.
 */
static
LIB_INLINE uint64_t
ft2ull(const FILETIME* ftime)
{
    ULARGE_INTEGER li;
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

/**************************************************************************
 * 通过apc的连续调用,计算出cpu系统使用率.
 * cpu利用率 = (sys-idl)/sys
 * lpArg, 用户apc回调参数
 * dwTimerLowValue, 定时器低位值
 * dwTimerHighValue, 定时器高位值
 */
static void CALLBACK
get_cpu_usage(LPVOID lpArg, DWORD  dwTimerLowValue, DWORD  dwTimerHighValue)
{
    FILETIME idle, kernel, user;
    static FILETIME prev_idle, prev_kernel, prev_user;
    static int first = 1;
    uint64_t sys;
    int*     cpu = (int *)lpArg;
    if (GetSystemTimes(&idle, &kernel, &user) && first)
    {
        prev_idle   = idle;
        prev_kernel = kernel;
        prev_user   = user;
        first = 0;
        return;
    }
    sys = (ft2ull(&user) - ft2ull(&prev_user)) + \
          (ft2ull(&kernel) - ft2ull(&prev_kernel));
    *cpu= (int)((sys - ft2ull(&idle) + ft2ull(&prev_idle)) * 100 / sys);
    prev_idle   = idle;
    prev_kernel = kernel;
    prev_user   = user;
}

static
LIB_INLINE uint32_t
get_foreground_window(void)
{
    uint32_t pid = 0;
    GetWindowThreadProcessId(GetForegroundWindow(), (LPDWORD)&pid);
    return pid;
}

static void
set_cpu_priority(int val, int m_rate)
{
    uint32_t m_pri  = GetPriorityClass(GetCurrentProcess());
    if (m_rate > val)
    {
        uint32_t m_windows = get_foreground_window();
        if (GetCurrentProcessId() == m_windows)
        {
            if (m_pri != ABOVE_NORMAL_PRIORITY_CLASS)
            {
                SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
            }
        }
        else if (GetCurrentProcessId() != BELOW_NORMAL_PRIORITY_CLASS)
        {
            SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
        }
    }
    else if (m_pri != NORMAL_PRIORITY_CLASS)
    {
        SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
    }
}

/**************************************************************************
 * 提供所谓的进程均衡调度功能.当cpu达到某个阈值,即CpuUse=?
 * 浏览器如果在前台运行,则提高它的运行级别.
 * 浏览器如果在后台运行,则降低它的运行级别.
 */
unsigned WINAPI
set_cpu_balance(void *lparam)
{
    HANDLE  m_timer = NULL;
    LPCWSTR m_pref  = L"cpu_pri_timer";
    WCHAR   m_name[32] = {0};
    WNDINFO m_windows;
    int     m_cpu = 0;
    LARGE_INTEGER m_duetime;
    int value = ini_read_int("attach", "CpuUse", ini_portable_path);
    memset(&m_windows, 0, sizeof(WNDINFO));
    m_windows.hPid = GetCurrentProcessId();
    if (!get_moz_hwnd(&m_windows))
    {
        return (0);
    }
    wnsprintfW(m_name, 32, L"%ls_%lu",m_pref, m_windows.hPid);
    if ((m_timer = CreateWaitableTimerW(NULL, false, m_name)) == NULL)
    {
        return (0);
    }
    m_duetime.QuadPart = -20000000;  /* 2 seconds pass */
    if (!SetWaitableTimer(m_timer, &m_duetime,2000, get_cpu_usage, (LPVOID)&m_cpu, false))
    {
        CloseHandle(m_timer);
        return (0);
    }
    if (value < 0 || value > 99)
    {
        value = 25;                 /* default cpu usage 25% */
    }
    while (SleepEx(INFINITE,true))
    {
    #ifdef _LOGDEBUG
        logmsg("CpuUse: %d%%\n", m_cpu);
    #endif
        set_cpu_priority(value, m_cpu);
    }
    CloseHandle(m_timer);
    return (1);
}
