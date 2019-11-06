#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
#include <shlwapi.h>
#include "share_lock.h"

s_data    sdata = {0};

bool WINAPI
get_env_status(LPCWSTR env)
{
    WCHAR   m_value[2] = {0};
    if (GetEnvironmentVariableW(env, m_value, 1) > 0 && *m_value == L'1')
    {
        return true;
    }
    return false;
}

bool WINAPI
get_ini_path(WCHAR *ini, int len)
{
    if (*sdata.ini != L'\0' && *sdata.ini != L' ')
    {
        wnsprintfW(ini, len, L"%ls", sdata.ini);
        return true;
    }
    return false;
}

bool WINAPI
get_appdt_path(WCHAR *path, int len)
{
    if (*sdata.appdt != L'\0' && *sdata.appdt != L' ')
    {
        wnsprintfW(path, len, L"%ls", sdata.appdt);
        return true;
    }
    return false;
}

bool WINAPI
get_localdt_path(WCHAR *path, int len)
{
    if (*sdata.localdt != L'\0' && *sdata.localdt != L' ')
    {
        wnsprintfW(path, len, L"%ls", sdata.localdt);
        return true;
    }
    return false;
}

bool WINAPI
get_process_path(WCHAR *path, int len)
{
    if (*sdata.process != L'\0' && *sdata.process != L' ')
    {
        wnsprintfW(path, len, L"%ls", sdata.process);
        return true;
    }
    return false;
}
