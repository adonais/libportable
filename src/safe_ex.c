#ifdef DISABLE_SAFE
#error This file should not be compiled!
#endif

#include "general.h"
#include "winapis.h"
#include "inject.h"
#include "ini_parser.h"
#include "new_process.h"
#include "safe_ex.h"
#include <process.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <stdio.h>

#define LEN_PARAM 1024

HANDLE g_mutex = NULL;
LoadLibraryExPtr pLoadLibraryEx = NULL;

static _NtCreateUserProcess    sNtCreateUserProcess;
static _NtCreateUserProcess    pNtCreateUserProcess;
static _NtWriteVirtualMemory   sNtWriteVirtualMemory;
static _NtWriteVirtualMemory   pNtWriteVirtualMemory;
static _RtlNtStatusToDosError  pRtlNtStatusToDosError;
static _CreateProcessInternalW pCreateProcessInternalW;
static _CreateProcessInternalW sCreateProcessInternalW;

static volatile long upgrade_ok = 0;

static bool in_whitelist(LPCWSTR lpfile)
{
    WCHAR *moz_processes[] = {L"", L"%windir%\\system32\\WerFault.exe",
                              L"plugin-container.exe",
                              L"plugin-hang-ui.exe",
                              L"firefox-webcontent.exe",
                              L"Iceweasel-webcontent.exe",
                              L"webapprt-stub.exe",
                              L"webapp-uninstaller.exe",
                              L"WSEnable.exe",
                              L"uninstall\\helper.exe",
                              L"crashreporter.exe",
                              L"crashhelper.exe",
                              L"CommandExecuteHandler.exe",
                              L"maintenanceservice.exe",
                              L"maintenanceservice_installer.exe",
                              L"minidump-analyzer.exe",
                              L"upcheck.exe",
                              L"updater.exe",
                              L"wow_helper.exe",
                              L"pingsender.exe",
                              L"default-browser-agent.exe",
                              L"private_browsing.exe"
                             };
    static  WCHAR white_list[EXCLUDE_NUM][VALUE_LEN];
    int     i = sizeof(moz_processes)/sizeof(moz_processes[0]);
    LPCWSTR pname = lpfile;
    bool    ret = false;
    if (lpfile[0] == L'"')
    {
        pname = &lpfile[1];
    }
    /* 遍历白名单一次,只需遍历一次 */
    if ( NULL == StrStrIW(white_list[1], L"WerFault.exe") )
    {
        /* firefox目录下进程的路径 */
        int   num;
        WCHAR temp[VALUE_LEN] = {0};
        GetModuleFileNameW(NULL,temp,VALUE_LEN);
        wcsncpy(white_list[0],(LPCWSTR)temp,VALUE_LEN);
        wcsncpy(white_list[1], (LPCWSTR)moz_processes[1], VALUE_LEN);
        PathRemoveFileSpecW(temp);
        for(num=2; num<i; ++num)
        {
            _snwprintf(white_list[num], VALUE_LEN, L"%s\\%s", temp, moz_processes[num]);
        }
        ret = ini_foreach_wstring("whitelist", &white_list[num], EXCLUDE_NUM-num, ini_portable_path, true);
    }
    if ( true )
    {
        /* 核对白名单 */
        for ( i=0; i<EXCLUDE_NUM && white_list[i][0] != L'\0' ; i++ )
        {
            if ( !(white_list[i][0] == L'*' || white_list[i][0] == L'?') && white_list[i][1] != L':' )
            {
                path_to_absolute(white_list[i],VALUE_LEN);
            }
            if ( StrChrW(white_list[i],L'*') || StrChrW(white_list[i],L'?') )
            {
                if ( PathMatchSpecW(pname,white_list[i]) )
                {
                    ret = true;
                    break;
                }
            }
            else if (_wcsnicmp(white_list[i],pname,wcslen(white_list[i]))==0)
            {
                ret = true;
                break;
            }
        }
    }
    return ret;
}

static bool
process_cui(LPCWSTR lpfile)
{
    WCHAR   lpname[VALUE_LEN+1] = {0};
    LPCWSTR sZfile = lpfile;
    int     n;
    if (lpfile == NULL || *lpfile == L'\0' || wcslen(lpfile) >= VALUE_LEN)
    {
        return true;
    }
    if (lpfile[0] == L'"')
    {
        sZfile = &lpfile[1];
        for (n = 0; *sZfile != L'"'; ++n)
        {
            lpname[n] = *sZfile;
            sZfile++;
        }
    }
    else
    {
        wcsncpy(lpname,sZfile,VALUE_LEN);
    }
    if (wcslen(lpname)>3)
    {
        return (!is_gui(lpname));
    }
    return true;
}

static bool
process_plugin(LPCWSTR lpfile)
{
    if ( StrStrIW(lpfile, L"SumatraPDF.exe")                          ||
    #ifndef _WIN64
        PathMatchSpecW(lpfile, L"*\\plugins\\FlashPlayerPlugin*.exe") ||
    #endif
        StrStrIW(lpfile, L"java.exe")                                 ||
        StrStrIW(lpfile, L"jp2launcher.exe") )
    {
        return true;
    }
    return false;
}

static NTSTATUS WINAPI
HookNtWriteVirtualMemory(HANDLE ProcessHandle,
                         PVOID BaseAddress,
                         PVOID Buffer,
                         SIZE_T NumberOfBytesToWrite,
                         PSIZE_T NumberOfBytesWritten)
{
    if ( GetCurrentProcessId() == GetProcessId(ProcessHandle) )
    {
    #ifdef _LOGDEBUG
        logmsg("HookNtWriteVirtualMemory() blocked\n");
    #endif
        return STATUS_ERROR;
    }
    return sNtWriteVirtualMemory(ProcessHandle,
                                 BaseAddress,
                                 Buffer,
                                 NumberOfBytesToWrite,
                                 NumberOfBytesWritten);
}

static bool
skip_double_quote(wchar_t *p, int len)
{
    int i = (int)wcslen(p);
    wchar_t *tmp = NULL;
    if (p[0] == L'\"')
    {
        int k = 0;
        tmp = (wchar_t *)calloc(sizeof(wchar_t), len + 1);
        if (!tmp)
        {
            return false;
        }
        for(int j = 1; j < i; ++j)
        {

            if (p[j]  != L'\"')
            {
                tmp[k++] = p[j];
            }
        }
        if (tmp[wcslen(tmp) - 1] == L' ')
        {
            tmp[wcslen(tmp) - 1] = L'\0';
        }
        wcsncpy(p, tmp, len);
        free(tmp);
    }
    return true;
}

static void
trace_command(LPCWSTR image_path, LPCWSTR cmd_path)
{
    bool    var = false;
    WCHAR   m_line[LEN_PARAM + 1] = {0};
    LPCWSTR lpfile = cmd_path?cmd_path:image_path;
    wcsncpy(m_line, GetCommandLineW(), LEN_PARAM);
    if (!skip_double_quote(m_line, LEN_PARAM))
    {
    #ifdef _LOGDEBUG
        logmsg("skip_double_quote failed\n");
    #endif
    }
    var = wcscmp(m_line, lpfile) == 0;
    if (!var)
    {
        if (NULL != cmd_path)
        {
            var = wcscmp(cmd_path, image_path) == 0;
        }
        else
        {
            var = true;
        }
    }
    if (var)
    {
        // 为启动器进程做标记, dll退出时销毁句柄
        g_mutex = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READONLY, 0, sizeof(bool), LIBTBL_LOCK);
        if (g_mutex)
        {
        #ifdef _LOGDEBUG
            logmsg("we set LIBPORTABLE_LAUNCHER_PROCESS=1, g_mutex  = 0x%p\n", g_mutex);
        #endif
        }
    }
}

static void
dll_update(void)
{
    WCHAR upcheck[MAX_PATH+1] = {0};
    if (upgrade_ok && wget_process_directory(upcheck, MAX_PATH))
    {
        wcsncat(upcheck, L"\\upcheck.exe", MAX_PATH);
        if (wexist_file(upcheck))
        {
            wcsncat(upcheck, L" -dll2", MAX_PATH);
            CloseHandle(create_new(upcheck, NULL, NULL, 0, NULL));
        }
    }
}

static NTSTATUS WINAPI
HookNtCreateUserProcess(PHANDLE ProcessHandle,PHANDLE ThreadHandle,
                        ACCESS_MASK ProcessDesiredAccess,
                        ACCESS_MASK ThreadDesiredAccess,
                        POBJECT_ATTRIBUTES ProcessObjectAttributes,
                        POBJECT_ATTRIBUTES ThreadObjectAttributes,
                        ULONG CreateProcessFlags,
                        ULONG CreateThreadFlags,
                        PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
                        PPS_CREATE_INFO CreateInfo,
                        PPS_ATTRIBUTE_LIST AttributeList)
{
    NTSTATUS  status;
    bool      tohook = false;
    RTL_USER_PROCESS_PARAMETERS myProcessParameters;
    trace_command(ProcessParameters->ImagePathName.Buffer, ProcessParameters->CommandLine.Buffer);
    if (is_specialapp(L"updater.exe") &&
       ((ProcessParameters->ImagePathName.Buffer && wcsstr(ProcessParameters->ImagePathName.Buffer, L"/PostUpdate")) ||
       (ProcessParameters->CommandLine.Buffer &&  wcsstr(ProcessParameters->CommandLine.Buffer, L"/PostUpdate"))) &&
       !_InterlockedCompareExchange(&upgrade_ok, 1, 0))
    {
        disable_hook((void **)&pNtCreateUserProcess);
        dll_update();
    #ifdef _LOGDEBUG
        logmsg("ExitProcess ...\n");
    #endif
        ExitProcess(255);
    }
    if (!ini_read_int("General", "SafeEx", ini_portable_path, true))
    {
        return sNtCreateUserProcess(ProcessHandle, ThreadHandle,
                                    ProcessDesiredAccess, ThreadDesiredAccess,
                                    ProcessObjectAttributes, ThreadObjectAttributes,
                                    CreateProcessFlags, CreateThreadFlags, ProcessParameters,
                                    CreateInfo, AttributeList);
    }
    fzero(&myProcessParameters,sizeof(RTL_USER_PROCESS_PARAMETERS));
    if (process_plugin(ProcessParameters->ImagePathName.Buffer))
    {
        tohook = true;
    }
    else if (ini_read_int("General", "EnableWhiteList", ini_portable_path, true) > 0)
    {
        if (ProcessParameters->ImagePathName.Length > 0 &&
            in_whitelist((LPCWSTR)ProcessParameters->ImagePathName.Buffer))
        {
        #ifdef _LOGDEBUG
            logmsg("the process %ls in whitelist\n",ProcessParameters->ImagePathName.Buffer);
        #endif
        }
        else
        {
        #ifdef _LOGDEBUG
            logmsg("the process %ls disabled-runes\n",ProcessParameters->ImagePathName.Buffer);
        #endif
            ProcessParameters = &myProcessParameters;
        }
    }
    else if (!(in_whitelist((LPCWSTR)ProcessParameters->ImagePathName.Buffer) || is_gui((LPCWSTR)ProcessParameters->ImagePathName.Buffer)))
    {
        ProcessParameters = &myProcessParameters;
    }
    status = sNtCreateUserProcess(ProcessHandle, ThreadHandle,
                                  ProcessDesiredAccess, ThreadDesiredAccess,
                                  ProcessObjectAttributes, ThreadObjectAttributes,
                                  CreateProcessFlags, CreateThreadFlags, ProcessParameters,
                                  CreateInfo, AttributeList);
    /* 静态编译时,不能启用远程注入 */
#if !defined(LIBPORTABLE_STATIC)
    if (tohook && NT_SUCCESS(status))
    {
        PROCESS_INFORMATION ProcessInformation;
    #ifdef _LOGDEBUG
        logmsg("hooked this process[%ls]\n",ProcessParameters->ImagePathName.Buffer);
    #endif
        fzero(&ProcessInformation,sizeof(PROCESS_INFORMATION));
        ProcessInformation.hProcess = *ProcessHandle;
        ProcessInformation.hThread = *ThreadHandle;
        InjectDll(&ProcessInformation);
    }
#endif
    return status;
}

static bool WINAPI
HookCreateProcessInternalW(HANDLE hToken,
                           LPCWSTR lpApplicationName,
                           LPWSTR lpCommandLine,
                           LPSECURITY_ATTRIBUTES lpProcessAttributes,
                           LPSECURITY_ATTRIBUTES lpThreadAttributes,
                           bool bInheritHandles,
                           DWORD dwCreationFlags,
                           LPVOID lpEnvironment,
                           LPCWSTR lpCurrentDirectory,
                           LPSTARTUPINFOW lpStartupInfo,
                           LPPROCESS_INFORMATION lpProcessInformation,
                           PHANDLE hNewToken)
{
    bool    ret= false;
    bool    tohook = false;
    LPCWSTR lpfile = lpCommandLine?lpCommandLine:lpApplicationName;
    trace_command(lpApplicationName, lpCommandLine);
    if (!ini_read_int("General", "SafeEx", ini_portable_path, true))
    {
        return sCreateProcessInternalW(hToken,lpApplicationName,lpCommandLine,lpProcessAttributes,
               lpThreadAttributes,bInheritHandles,dwCreationFlags,
               lpEnvironment,lpCurrentDirectory,
               lpStartupInfo,lpProcessInformation,hNewToken);
    }
    if (lpApplicationName && wcslen(lpApplicationName)>1)
    {
        lpfile = (LPWSTR)lpApplicationName;
    }
    /* 禁止启动16位程序 */
    if (dwCreationFlags&CREATE_SHARED_WOW_VDM || dwCreationFlags&CREATE_SEPARATE_WOW_VDM)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return ret;
    }
    /* 存在不安全插件,注入保护 */
    if ( process_plugin(lpfile) )
    {
    /* 静态编译时,不能启用远程注入 */
    #if !defined(LIBPORTABLE_STATIC)
        tohook = true;
    #endif
    }
    /* 如果启用白名单制度(严格检查) */
    else if (ini_read_int("General", "EnableWhiteList", ini_portable_path, true) > 0)
    {
        if (!in_whitelist((LPCWSTR)lpfile))
        {
        #ifdef _LOGDEBUG
            logmsg("the process %ls disabled-runes by libportable\n",lpfile);
        #endif
            SetLastError(pRtlNtStatusToDosError(STATUS_ERROR));
            return ret;
        }
    }
    /* 如果不存在于白名单,则自动阻止命令行程序启动 */
    else if (!(in_whitelist((LPCWSTR)lpfile) || is_gui(lpfile)))
    {
    #ifdef _LOGDEBUG
        logmsg("%ls process, disabled-runes\n",lpfile);
    #endif
        SetLastError(pRtlNtStatusToDosError(STATUS_ERROR));
        return ret;
    }
    ret = sCreateProcessInternalW(hToken,lpApplicationName,lpCommandLine,lpProcessAttributes,
                                  lpThreadAttributes,bInheritHandles,dwCreationFlags,
                                  lpEnvironment,lpCurrentDirectory,
                                  lpStartupInfo,lpProcessInformation,hNewToken);
    if (tohook && ret)
    {
        InjectDll(lpProcessInformation);
    }
    return ret;
}

static bool
is_authorized(LPCWSTR lpFileName)
{
    bool    ret   = false;
    LPWSTR  filename = NULL;
    WCHAR   *szAuthorizedList[] = { L"comctl32.dll",
                                    L"uxtheme.dll",
                                    L"indicdll.dll",
                                    L"msctf.dll",
                                    L"shell32.dll",
                                    L"imageres.dll",
                                    L"winmm.dll",
                                    L"ole32.dll",
                                    L"oleacc.dll",
                                    L"version.dll",
                                    L"oleaut32.dll",
                                    L"secur32.dll",
                                    L"shlwapi.dll",
                                    L"ImSCTip.DLL",
                                    L"gdi32.dll",
                                    L"dwmapi.dll"
                                  };
    bool     wow64 = is_wow64();
    uint16_t line = sizeof(szAuthorizedList)/sizeof(szAuthorizedList[0]);
    wchar_t  dllpath[MAX_PATH+1] = {0};
    GetModuleFileNameW(dll_module,dllpath,MAX_PATH);
    if (_wcsicmp(lpFileName,dllpath) == 0)
    {
        return true;
    }
    if (lpFileName[1] == L':')
    {
        wchar_t sysdir[VALUE_LEN+1] = {0};
        GetEnvironmentVariableW(L"SystemRoot",sysdir,VALUE_LEN);
        if (wow64)
        {
            PathAppendW(sysdir,L"SysWOW64");
        }
        else
        {
            PathAppendW(sysdir,L"system32");
        }
        if (_wcsnicmp(lpFileName,sysdir,wcslen(sysdir)) == 0)
        {
            filename = PathFindFileNameW(lpFileName);
        }
        else if (wow64 && wcslen(sysdir)>0)   /* compare system32 directory again */
        {
            PathRemoveFileSpecW(sysdir);
            PathAppendW(sysdir,L"system32");
            filename = _wcsnicmp(lpFileName,sysdir,wcslen(sysdir))?NULL:PathFindFileNameW(lpFileName);
        }
    }
    else
    {
        filename = (LPWSTR)lpFileName;
    }
    if (filename)
    {
        uint16_t  i;
        for(i=0; i<line; i++)
        {
            if (_wcsicmp(filename,szAuthorizedList[i]) == 0)
            {
                ret = true;
                break;
            }
        }
    }
    return ret;
}

static HMODULE WINAPI
HookLoadLibraryExW(LPCWSTR lpFileName,HANDLE hFile,DWORD dwFlags)
{
    uintptr_t dwCaller = (uintptr_t)_ReturnAddress();
    if ( is_authorized(lpFileName) )
    {
        return sLoadLibraryExStub(lpFileName, hFile, dwFlags);
    }
    if ( is_specialdll(dwCaller,L"user32.dll") )
    {
        if ( !in_whitelist(lpFileName) )
        {
        #ifdef _LOGDEBUG
            logmsg("disable loading %ls!\n", lpFileName);
        #endif
            lpFileName = NULL;
        }
    }
    return sLoadLibraryExStub(lpFileName, hFile, dwFlags);
}

unsigned WINAPI init_safed(void)
{
    HMODULE		hNtdll, hKernel;
    DWORD		ver = get_os_version();
    hNtdll   =  GetModuleHandleW(L"ntdll.dll");
    hKernel  =  GetModuleHandleW(L"kernel32.dll");
    if ( hNtdll == NULL || hKernel  == NULL ||
        (pRtlNtStatusToDosError = (_RtlNtStatusToDosError)GetProcAddress(hNtdll, "RtlNtStatusToDosError")) == NULL )
    {
        return 0;
    }
    if (ver > 503)  /* vista - win10 */
    {
        pNtCreateUserProcess = (_NtCreateUserProcess)GetProcAddress(hNtdll, "NtCreateUserProcess");
        if (!creator_hook(pNtCreateUserProcess, HookNtCreateUserProcess, (LPVOID*)&sNtCreateUserProcess))
        {
        #ifdef _LOGDEBUG
            logmsg("NtCreateUserProcess hook failed!\n");
        #endif
        }
    }
    else          /* winxp-2003 */
    {
        pCreateProcessInternalW	= (_CreateProcessInternalW)GetProcAddress(hKernel, "CreateProcessInternalW");
        if (!creator_hook(pCreateProcessInternalW, HookCreateProcessInternalW, (LPVOID*)&sCreateProcessInternalW))
        {
        #ifdef _LOGDEBUG
            logmsg("pCreateProcessInternalW hook failed!\n");
        #endif
        }
    }
#ifndef DISABLE_SAFE
    if (ver < 600 && ini_read_int("General", "SafeEx", ini_portable_path, true) > 0)
    {
        pLoadLibraryEx = (LoadLibraryExPtr)GetProcAddress(hKernel, "LoadLibraryExW");
        if (!creator_hook(pLoadLibraryEx, HookLoadLibraryExW, (LPVOID*)&sLoadLibraryExStub))
        {
        #ifdef _LOGDEBUG
            logmsg("LoadLibraryExW hook failed!\n");
        #endif
        }
        pNtWriteVirtualMemory = (_NtWriteVirtualMemory)GetProcAddress(hNtdll, "NtWriteVirtualMemory");
        if (!creator_hook(pNtWriteVirtualMemory, HookNtWriteVirtualMemory, (LPVOID*)&sNtWriteVirtualMemory))
        {
        #ifdef _LOGDEBUG
            logmsg("NtWriteVirtualMemory hook failed!\n");
        #endif
        }
    }
#endif
    return (1);
}
