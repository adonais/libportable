#define BAL_EXTERN

#include "balance.h"
#include "inipara.h"
#include <string.h>
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


static int                         /* cpu利用率 = (sys-idl)/sys */    
get_cpu_usage(void) 
{
    FILETIME idle, prev_idle;
    FILETIME kernel, prev_kernel;
    FILETIME user, prev_user;
    uint64_t sys;
    int      cpu;
    GetSystemTimes(&prev_idle, &prev_kernel, &prev_user);
    SleepEx(1000, false);
    GetSystemTimes(&idle, &kernel, &user);
    sys = (ft2ull(&user) - ft2ull(&prev_user)) + \
          (ft2ull(&kernel) - ft2ull(&prev_kernel));
    cpu = (int)((sys - ft2ull(&idle) + ft2ull(&prev_idle)) * 100 / sys);
    return cpu;
}

static __inline bool 
is_foreground_window(HWND hwnd)
{
    return ( GetWindowThreadProcessId(hwnd, NULL) == \
             GetWindowThreadProcessId(GetForegroundWindow(), NULL) 
           );
}

static bool 
set_cpu_priority(HWND hwnd, int val)
{
    bool  ret    = false;
    DWORD m_pri  = GetPriorityClass( GetCurrentProcess() );
    int   m_rate = get_cpu_usage();
    if ( hwnd != NULL ? \
         is_foreground_window(hwnd) != true && m_rate > val : \
         m_rate > val              /* cpu usage above val */    
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
SetCpuAffinity_tt(void * lparam)
{
    LARGE_INTEGER m_duetime;
    HANDLE        m_timer = NULL;
    LPWNDINFO     m_info  = (LPWNDINFO)lparam;
    HWND          m_hwnd  = NULL;
    int           value   = read_appint(L"attach ", L"CpuUse");
    if ( is_specialapp(L"plugin-container.exe") )
    {
    #ifdef _LOGDEBUG
        logmsg("plugin-container[%ld]!\n", m_info->hPid);
    #endif 
    }
    else if ( (m_hwnd = get_moz_hwnd(m_info)) == NULL )
    {
        return (0);
    }
    m_timer = CreateWaitableTimerW(NULL, true, L"cpu_pri_timer");
    if ( !m_timer )
    {
        return (0);
    }
    m_duetime.QuadPart = -10000000;  /* 1 seconds pass */
    if ( !SetWaitableTimer(m_timer, &m_duetime, 0, NULL, NULL, false) )
    {
        CloseHandle(m_timer);
        return (0);
    }
    if ( value < 0 || value > 99 )
    {
        value = 25;   /* default cpu usage 25% */
    }
    while ( true )
    {
        if ( WaitForSingleObject(m_timer, INFINITE) == WAIT_OBJECT_0 )
        {
            SetWaitableTimer(m_timer, &m_duetime, 0, NULL, NULL, false);
            set_cpu_priority(m_hwnd, value);
        }
    }
    CloseHandle(m_timer);
    return (1);
}
