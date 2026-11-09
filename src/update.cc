#include <windows.h>
#include <process.h>
#include <shlwapi.h>
#include <string>
#include "config.h"
#include "utils.h"
#include "update.h"

static HANDLE
create_new(LPCWSTR wcmd, LPCWSTR param, LPCWSTR pcd, int flags, DWORD *opid)
{
    STARTUPINFOW si;
    DWORD dwCreat = 0;
    LPCWSTR lp_dir = NULL;
    PROCESS_INFORMATION pi = {0};
    const size_t len = (wcmd ? wcslen(wcmd) : 1) + (param ? wcslen(param) : 1) + 64;
    WCHAR *process = (WCHAR *)calloc(sizeof(WCHAR), len + 1);
    if (!process)
    {
        return NULL;
    }
    if (pcd != NULL && wcslen(pcd) > 1)
    {
        lp_dir = pcd;
    }
    if (param != NULL && wcslen(param ) > 1)
    {
        _snwprintf(process, len, L"%s %s", wcmd, param);
    }
    else
    {
        _snwprintf(process, len, L"%s", wcmd);
    }
    if (true)
    {
        memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        if (flags > 1)
        {
            si.wShowWindow = SW_NORMAL;
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
                          &si,
                          &pi))
        {
            DebugLog(L"CreateProcessW error, cause: %lu", GetLastError());
            free(process);
            return NULL;
        }
        if (NULL != opid)
        {
            *opid = pi.dwProcessId;
        }
        CloseHandle(pi.hThread);
        free(process);
    }
    return pi.hProcess;
}

static bool
search_upcheck_exists(LPCWSTR parent)
{
    bool ret = false;
    HANDLE hfile = NULL;
    WIN32_FIND_DATAW fd = {0};
    WCHAR pathname[MAX_PATH] = {0};
    WCHAR sub[MAX_PATH] = {0};
    if(parent[wcslen(parent) -1] != '\\')
    {
        _snwprintf(pathname, MAX_PATH, L"%s\\*.*", parent);
    }
    else
    {
        _snwprintf(pathname, MAX_PATH, L"%s*.*", parent);
    }
    if ((hfile = FindFirstFileW(pathname, &fd)) != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(wcscmp(fd.cFileName, L".") && wcscmp(fd.cFileName, L".."))
            {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if(parent[wcslen(parent) -1] != '\\')
                    {
                        _snwprintf(sub, MAX_PATH, L"%s\\%s", parent, fd.cFileName);
                    }
                    else
                    {
                        _snwprintf(sub, MAX_PATH, L"%s%s", parent, fd.cFileName);
                    }
                    if (search_upcheck_exists(sub))
                    {
                        ret = true;
                        break;
                    }
                }
                else if (_wcsicmp(fd.cFileName, L"upcheck.exe") == 0)
                {
                    ret = true;
                    break;
                }
            }
        } while (FindNextFileW(hfile, &fd) != 0);
        FindClose(hfile);
    }
    return ret;
}

static unsigned WINAPI
update_thread(void *lparam) {
    std::wstring pfile = GetAppDir() + L"\\upcheck.exe";
    const std::wstring& ini = GetIniPath();
    const bool installed = ((intptr_t)lparam != 0);
    const bool fnchrome = !(ini.ends_with(L"msedge++.ini") || ini.ends_with(L"msedge++(example).ini"));
    WCHAR wcmd[MAX_BUFFER+1] = {0};
    std::wstring tmp = cache_path;
    tmp += L"\\Updates";
    if (installed) {
        if (search_upcheck_exists(tmp.c_str())) {
            WCHAR path[MAX_PATH+1] = {0};
            DWORD m = GetModuleFileNameW(NULL, path, MAX_PATH);
            if (m > 0 && m <= MAX_PATH) {
                _snwprintf(wcmd, MAX_BUFFER, L"\"%s\" %s -k %lu -e \"%s\" -s \"%s\" -u", pfile.c_str(), fnchrome ? L"-chrome" : L"-edge", GetCurrentProcessId(), tmp.c_str(), path);
            }
        } else {
            // 如果更新包不带upcheck, 移除be_ready项
            _snwprintf(wcmd, MAX_BUFFER, L"\"%s\" -ini \"%s\" update be_ready \"\"", pfile.c_str(), ini.c_str());
        }
    } else {
        Sleep(8000);
        _snwprintf(wcmd, MAX_BUFFER, L"\"%s\" %s -i auto -k %lu -e \"%s\"", pfile.c_str(), fnchrome ? L"-chrome" : L"-edge", GetCurrentProcessId(), tmp.c_str());
    }
    if (wcmd[0]) {
        CloseHandle(create_new(wcmd, NULL, NULL, 0, NULL));
        DebugLog(L"update[{}]", wcmd);
    }
    return 0;
}

void StartUpCheck() {
    std::wstring pfile = GetAppDir() + L"\\upcheck.exe";
    if (config.IsUpdates() && (*cache_path != 0) && PathFileExistsW(pfile.c_str())) {
        if (config.IsReadyUpdates()) {
            update_thread((void *)(intptr_t)0x1);
        } else if (config.IsCheckUp()) {
            CloseHandle((HANDLE)_beginthreadex(NULL, 0, &update_thread, (void *)(intptr_t)0x0, 0, NULL));
        }
    }
}
