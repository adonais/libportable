#include "inipara.h"
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>

static LIB_INLINE uint64_t             /* 时间转换 */
ft2ull(const FILETIME* ftime)
{
    ULARGE_INTEGER li;
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

/*********************************************************/
/* 使用apc调用,消除SleepEx函数可能对界面的影响      ******/
/* 通过apc的连续调用,计算出cpu系统使用率.           ******/
/* cpu利用率 = (sys-idl)/sys                        ******/
/*********************************************************/
static void CALLBACK                          
get_cpu_usage(LPVOID lpArg,                /* 用户apc回调参数 */
              DWORD  dwTimerLowValue,      /* 定时器低位值    */
              DWORD  dwTimerHighValue)     /* 定时器高位值    */
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
#ifdef _LOGDEBUG
    if (!first)
    {
       logmsg("CpuUse: %d%%\n", *cpu);
    }
#endif
}

static LIB_INLINE bool 
is_foreground_window(void)
{
    return (GetCurrentProcessId() == GetWindowThreadProcessId(GetForegroundWindow(), NULL));
}

static void
set_cpu_priority(int val, int m_rate)
{
    DWORD m_pri  = GetPriorityClass(GetCurrentProcess());
    if (is_foreground_window() || val > m_rate)
    {
        if (m_pri != NORMAL_PRIORITY_CLASS)
        {
            SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
        }
    }
    else if (m_pri != BELOW_NORMAL_PRIORITY_CLASS)
    {
        SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
    }   
}

unsigned WINAPI
set_cpu_balance(void *lparam)
{
    LARGE_INTEGER m_duetime;
    HANDLE        m_timer = NULL;
    LPCWSTR       m_pref  = L"cpu_pri_timer";
    WCHAR         m_name[32] = {0};
    int           m_cpu   = 0;
    int           value   = read_appint(L"attach", L"CpuUse");
    wnsprintfW(m_name, 32, L"%ls_%lu",m_pref, GetCurrentProcessId());
    m_timer = CreateWaitableTimerW(NULL, false, m_name);
    if ( !m_timer )
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
    while (true)
    {
        if (WaitForSingleObjectEx(m_timer, INFINITE, true) == WAIT_OBJECT_0)
        {
            set_cpu_priority(value, m_cpu);
        }
    }
    CloseHandle(m_timer);
    return (1);
}
