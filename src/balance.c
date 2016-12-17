#include "balance.h"
#include "inipara.h"
#include <string.h>
#include <stdio.h>
#include <windows.h>

static __inline int
get_cpu_works(void)
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (int)(si.dwNumberOfProcessors);
}

static __inline uint64_t             /* 时间转换 */
ft2ull(const FILETIME* ftime)
{
    ULARGE_INTEGER li;
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

static int                          /* 获取firefox cpu使用率 */
getfx_usage(void)
{
    FILETIME now;
    FILETIME creation_time;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;
    static uint64_t last_time_;      /* 上一次的时间  */
    static uint64_t last_system_time_;
    static int processor_count_= -1;
    uint64_t system_time;
    uint64_t time;
    uint64_t system_time_delta;
    uint64_t time_delta;
    int      cpu;
    if ( processor_count_ == -1)
    {
         processor_count_ = get_cpu_works();
    }
    GetSystemTimeAsFileTime(&now);
    if (!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time,
                         &kernel_time, &user_time)) 
    {
        return 0;
    }
    system_time = (ft2ull(&kernel_time) + ft2ull(&user_time)) /
                   processor_count_;
    time = ft2ull(&now);

    if ((last_system_time_ == 0) || (last_time_ == 0)) 
    {   /* First call, just set the last values. */                                  
        last_system_time_ = system_time;
        last_time_ = time;
        return 0;
    }

    system_time_delta = system_time - last_system_time_;
    time_delta = time - last_time_;
    if (time_delta == 0) return 0;
    /* We add time_delta / 2 so the result is rounded. */
    cpu = (int)((system_time_delta * 100 + time_delta / 2) /
                 time_delta);

    last_system_time_ = system_time;
    last_time_ = time;
    return cpu;
}

/************************************************/
/* 使用apc调用,消除SleepEx函数可能对界面的影响  */ 
/* 通过apc的连续调用,计算出cpu系统使用率.       */ 
/* cpu利用率 = (sys-idl)/sys                    */ 
/************************************************/ 
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
    if ( GetSystemTimes(&idle, &kernel, &user) && first )
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
    if ( !first )
    {
       logmsg("CpuUse: %d%%\n", *cpu);
    }
#endif
}

static __inline bool 
is_foreground_window(HWND hwnd)
{
    return ( GetWindowThreadProcessId(hwnd, NULL) ==
             GetWindowThreadProcessId(GetForegroundWindow(), NULL) );
}

static bool 
set_cpu_priority(HWND hwnd, int val, int m_rate)
{
    bool  ret    = false;
    DWORD m_pri  = GetPriorityClass( GetCurrentProcess() );
    if ( hwnd != NULL ?
         is_foreground_window(hwnd) != true &&   /* browser not brought to foreground  */
         m_rate > val :
         m_rate > val                            /* cpu usage above val */  
       )
    {
        ret = m_pri != BELOW_NORMAL_PRIORITY_CLASS ? \
              SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS) : \
              false;
    }
    else
    {
        ret = m_pri != NORMAL_PRIORITY_CLASS ? \
              SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS) : \
              false;
    }
    return ret;
}

unsigned WINAPI
set_cpu_balance(void *fx_info)
{
    LARGE_INTEGER m_duetime;
    HANDLE        m_timer = NULL;
    LPWNDINFO     m_info  = (LPWNDINFO)fx_info;
    HWND          m_hwnd  = NULL;
    LPCWSTR       m_pref  = L"cpu_pri_timer";
    WCHAR         m_name[32] = {0};
    int           m_cpu   = 0;
    int           value   = read_appint(L"attach ", L"CpuUse");
    _snwprintf(m_name, 32, L"%ls_%lu",m_pref, GetCurrentProcessId());
    m_timer = CreateWaitableTimerW(NULL, false, m_name);
    if ( !m_timer )
    {
        return (0);
    }
    m_duetime.QuadPart = -20000000;  /* 2 seconds pass */
    if ( !SetWaitableTimer(m_timer, &m_duetime,2000, get_cpu_usage, (LPVOID)&m_cpu, false) )
    {
        CloseHandle(m_timer);
        return (0);
    }
    if ( value < 0 || value > 99 )
    {
        value = 25;                 /* default cpu usage 25% */
    }
    while ( true )
    {
        if ( WaitForSingleObjectEx(m_timer, INFINITE, true) == WAIT_OBJECT_0 )
        {
            set_cpu_priority(m_hwnd, value, m_cpu);
        }
    }
    CloseHandle(m_timer);
    return (1);
}
