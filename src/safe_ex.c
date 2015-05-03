#include "safe_ex.h"
#include "inipara.h"
#include "winapis.h"
#include "inject.h"
#include "MinHook.h"
#include <process.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <stdio.h>

static _NtProtectVirtualMemory     TrueNtProtectVirtualMemory;
static _NtQueryInformationProcess  TrueNtQueryInformationProcess;
static _RtlNtStatusToDosError      TrueRtlNtStatusToDosError;
static _NtSuspendThread            TrueNtSuspendThread;
static _NtResumeThread             TrueNtResumeThread;
static _NtCreateUserProcess        OrgiNtCreateUserProcess, 
                                   TrueNtCreateUserProcess;
static _NtWriteVirtualMemory       OrgiNtWriteVirtualMemory, 
                                   TrueNtWriteVirtualMemory;
static _CreateProcessInternalW     OrgiCreateProcessInternalW, 
                                   TrueCreateProcessInternalW;
static _NtLoadLibraryExW           TrueLoadLibraryExW;

static bool in_whitelist(LPCWSTR lpfile)
{
    WCHAR *moz_processes[] = {L"", L"%windir%\\system32\\WerFault.exe",
                              L"plugin-container.exe", 
                              L"plugin-hang-ui.exe", 
                              L"webapprt-stub.exe",
                              L"webapp-uninstaller.exe",
                              L"WSEnable.exe",
                              L"uninstall\\helper.exe",
                              L"crashreporter.exe",
                              L"CommandExecuteHandler.exe",
                              L"maintenanceservice.exe",
                              L"maintenanceservice_installer.exe",
                              L"updater.exe",
                              L"wow_helper.exe"
                             };
    static  WCHAR white_list[EXCLUDE_NUM][VALUE_LEN+1];
    int     i = sizeof(moz_processes)/sizeof(moz_processes[0]);
    LPCWSTR pname = lpfile;
    bool    ret = false;
    if (lpfile[0] == L'"')
    {
        pname = &lpfile[1];
    }
    /* 遍历白名单一次,只需遍历一次 */
    ret = stristrW(white_list[1],L"WerFault.exe") != NULL;
    if ( !ret )
    {
        /* firefox目录下进程的路径 */
        int   num;
        WCHAR temp[VALUE_LEN+1];
        GetModuleFileNameW(NULL,temp,VALUE_LEN);
        wcsncpy(white_list[0],(LPCWSTR)temp,VALUE_LEN);
        PathRemoveFileSpecW(temp);
        for(num=2; num<i; ++num)
        {
            _snwprintf(white_list[num],VALUE_LEN,L"%ls\\%ls", temp, moz_processes[num]);
        }
        ret = foreach_section(L"whitelist", &white_list[num], EXCLUDE_NUM-num);
    }
    if ( (ret = !ret) == false )
    {
        /* 核对白名单 */
        for ( i=0; i<EXCLUDE_NUM ; i++ )
        {
            if (wcslen(white_list[i]) == 0)
            {
                continue;
            }
            if ( !(white_list[i][0] == L'*' || white_list[i][0] == L'?') && \
                 white_list[i][1] != L':' )
            {
                PathToCombineW(white_list[i],VALUE_LEN);
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

/* 获得父线程PID */
uintptr_t WINAPI getid_parental(uint32_t dwProcessId)
{
    NTSTATUS  status;
    uintptr_t dwParentPID = 0;
    HANDLE    hProcess = NULL;
    PROCESS_BASIC_INFORMATION pbi;
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    do
    {
        if( !hNtdll )
        {
            break;
        }
        TrueNtQueryInformationProcess = (_NtQueryInformationProcess)GetProcAddress(\
                                        hNtdll, "NtQueryInformationProcess");
        if ( !TrueNtQueryInformationProcess )
        {
            break;
        }
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
                               false, dwProcessId);
        if (!hProcess)
        {
            break;
        }
        status = TrueNtQueryInformationProcess(hProcess,
                 ProcessBasicInformation,
                 (PVOID)&pbi,
                 sizeof(PROCESS_BASIC_INFORMATION),
                 NULL
                 );
        if ( NT_SUCCESS(status) )
        {
            dwParentPID = (uintptr_t)pbi.InheritedFromUniqueProcessId;
        }
    }while(0);
    if ( hProcess )
    {
        CloseHandle(hProcess);
    }
    return dwParentPID;
}  


static bool 
ProcessIsCUI(LPCWSTR lpfile)
{
    WCHAR   lpname[VALUE_LEN+1] = {0};
    LPCWSTR sZfile = lpfile;
    int     n;
    if ( lpfile[0] == L'"' )
    {
        sZfile = &lpfile[1];
        for ( n = 0; *sZfile != L'"'; ++n )
        {
            lpname[n] = *sZfile;
            sZfile++;
        }
    }
    else
    {
        wcsncpy(lpname,sZfile,VALUE_LEN);
    }
    if ( wcslen(lpname)>3 )
    {
        return ( !IsGUI(lpname) );
    }
    return true;
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
    return OrgiNtWriteVirtualMemory(ProcessHandle,
                                    BaseAddress,
                                    Buffer,
                                    NumberOfBytesToWrite,
                                    NumberOfBytesWritten);
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
    RTL_USER_PROCESS_PARAMETERS mY_ProcessParameters;
    NTSTATUS    status;
    bool        tohook	= false;
    fzero(&mY_ProcessParameters,sizeof(RTL_USER_PROCESS_PARAMETERS));
    if ( stristrW(ProcessParameters->ImagePathName.Buffer, L"SumatraPDF.exe") ||
         stristrW(ProcessParameters->ImagePathName.Buffer, L"java.exe") ||
         stristrW(ProcessParameters->ImagePathName.Buffer, L"jp2launcher.exe"))
    {
        tohook = true;
    }
    else if ( read_appint(L"General",L"EnableWhiteList") > 0 )
    {
        if ( ProcessParameters->ImagePathName.Length > 0 &&
             in_whitelist((LPCWSTR)ProcessParameters->ImagePathName.Buffer) )
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
            ProcessParameters = &mY_ProcessParameters;
        }
    }
    else if ( in_whitelist((LPCWSTR)ProcessParameters->ImagePathName.Buffer) )
    {
        ;
    }
    else
    {
        if ( !IsGUI((LPCWSTR)ProcessParameters->ImagePathName.Buffer) )
            ProcessParameters = &mY_ProcessParameters;
    }
    status = OrgiNtCreateUserProcess(ProcessHandle, ThreadHandle,
                                     ProcessDesiredAccess, ThreadDesiredAccess,
                                     ProcessObjectAttributes, ThreadObjectAttributes,
                                     CreateProcessFlags, CreateThreadFlags, ProcessParameters,
                                     CreateInfo, AttributeList);
    /* 静态编译或者启用GCC lto时,不能启用远程注入 */
#if !( defined(LIBPORTABLE_STATIC) || defined(__LTO__) )
    if ( NT_SUCCESS(status) && tohook )
    {
        PROCESS_INFORMATION ProcessInformation;
        ULONG Suspend = 0;
        fzero(&ProcessInformation,sizeof(PROCESS_INFORMATION));
        ProcessInformation.hProcess = *ProcessHandle;
        ProcessInformation.hThread = *ThreadHandle;
        if ( NT_SUCCESS(TrueNtSuspendThread(ProcessInformation.hThread,&Suspend)) )
        {
            InjectDll(&ProcessInformation);
        }
    }
#endif
    return status;
}

static bool WINAPI 
HookCreateProcessInternalW (HANDLE hToken,
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
    bool	ret		= false;
    LPWSTR	lpfile	= lpCommandLine;
    bool    tohook	= false;
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
    if ( stristrW(lpfile, L"SumatraPDF.exe") ||
         stristrW(lpfile, L"java.exe")       ||
         stristrW(lpfile, L"jp2launcher.exe"))
    {
        /* 静态编译或者启用GCC lto时,不能启用远程注入 */
    #if !( defined(LIBPORTABLE_STATIC) || defined(__LTO__) )
        dwCreationFlags |= CREATE_SUSPENDED;
        tohook = true;
    #endif
    }
    /* 如果启用白名单制度(严格检查) */
    else if ( read_appint(L"General",L"EnableWhiteList") > 0 )
    {
        if ( !in_whitelist((LPCWSTR)lpfile) )
        {
        #ifdef _LOGDEBUG
            logmsg("the process %ls disabled-runes\n",lpfile);
        #endif
            SetLastError( TrueRtlNtStatusToDosError(STATUS_ERROR) );
            return ret;
        }
    }
    else if ( in_whitelist((LPCWSTR)lpfile) )
    {
        ;
    }
    /* 如果不存在于白名单,则自动阻止命令行程序启动 */
    else
    {
        if ( ProcessIsCUI(lpfile) )
        {
        #ifdef _LOGDEBUG
            logmsg("%ls process, disabled-runes\n",lpfile);
        #endif
            SetLastError( TrueRtlNtStatusToDosError(STATUS_ERROR) );
            return ret;
        }
    }
    ret =  OrgiCreateProcessInternalW(hToken,lpApplicationName,lpCommandLine,lpProcessAttributes,
                                      lpThreadAttributes,bInheritHandles,dwCreationFlags,
                                      lpEnvironment,lpCurrentDirectory,
                                      lpStartupInfo,lpProcessInformation,hNewToken);
    if ( ret && tohook )
    {
        InjectDll(lpProcessInformation);
    }
    return ret;
}

static bool 
is_authorized(LPCWSTR lpFileName)
{
    bool    ret   = false;
    int     wow64 = 0;
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
    uint16_t line = sizeof(szAuthorizedList)/sizeof(szAuthorizedList[0]);
    IsWow64Process(NtCurrentProcess(),&wow64);
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
        if ( _wcsnicmp(lpFileName,sysdir,wcslen(sysdir)) == 0 )
        {
            filename = PathFindFileNameW(lpFileName);
        }
        else if ( wow64 && wcslen(sysdir)>0 )   /* compare system32 directory again */
        {
            PathRemoveFileSpecW(sysdir);
            PathAppendW(sysdir,L"system32");
            filename = _wcsnicmp(lpFileName,sysdir,
                       wcslen(sysdir))?NULL:PathFindFileNameW(lpFileName);
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
            if ( _wcsicmp(filename,szAuthorizedList[i]) == 0 )
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
        return OrgiLoadLibraryExW(lpFileName, hFile, dwFlags);
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
    return OrgiLoadLibraryExW(lpFileName, hFile, dwFlags);
}

unsigned WINAPI init_safed(void * pParam)
{
    HMODULE		hNtdll, hKernel;
    DWORD		ver = GetOsVersion();
    hNtdll   =  GetModuleHandleW(L"ntdll.dll");
    hKernel  =  GetModuleHandleW(L"kernel32.dll");
    if ( !(hNtdll&&hKernel) ) return (0);
    TrueNtSuspendThread           = (_NtSuspendThread)GetProcAddress(\
                                    hNtdll, "NtSuspendThread");
    TrueNtResumeThread            = (_NtResumeThread)GetProcAddress(\
                                    hNtdll, "NtResumeThread");
    TrueNtWriteVirtualMemory      = (_NtWriteVirtualMemory)GetProcAddress(\
                                    hNtdll, "NtWriteVirtualMemory");
    TrueRtlNtStatusToDosError     = (_RtlNtStatusToDosError)GetProcAddress(\
                                    hNtdll, "RtlNtStatusToDosError");
    if ( ver>601 )  /* win8 */
    {
        TrueNtCreateUserProcess   = (_NtCreateUserProcess)GetProcAddress(\
                                    hNtdll, "NtCreateUserProcess");
        if (TrueNtCreateUserProcess && MH_CreateHook \
           (TrueNtCreateUserProcess, HookNtCreateUserProcess, \
           (LPVOID*)&OrgiNtCreateUserProcess) == MH_OK )
        {
            if ( MH_EnableHook(TrueNtCreateUserProcess) != MH_OK )
            {
            #ifdef _LOGDEBUG
                logmsg("TrueNtCreateUserProcess hook failed!\n");
            #endif
            }
        }
    }
    else
    {
        TrueCreateProcessInternalW	= (_CreateProcessInternalW)GetProcAddress(\
                                      hKernel, "CreateProcessInternalW");
        if (TrueCreateProcessInternalW && MH_CreateHook \
           (TrueCreateProcessInternalW, HookCreateProcessInternalW, \
           (LPVOID*)&OrgiCreateProcessInternalW) == MH_OK )
        {
            if ( MH_EnableHook(TrueCreateProcessInternalW) != MH_OK )
            {
            #ifdef _LOGDEBUG
                logmsg("TrueCreateProcessInternalW hook failed!\n");
            #endif
            }
        }
    }
    if ( ver<503 )  /* winxp-2003 */
    {
        TrueLoadLibraryExW	= (_NtLoadLibraryExW)GetProcAddress(\
                              hKernel, "LoadLibraryExW");
    }
    if (TrueLoadLibraryExW && MH_CreateHook \
       (TrueLoadLibraryExW, HookLoadLibraryExW, \
       (LPVOID*)&OrgiLoadLibraryExW) == MH_OK )
    {
        if ( MH_EnableHook(TrueLoadLibraryExW) != MH_OK )
        {
        #ifdef _LOGDEBUG
            logmsg("LoadLibraryExW hook failed!\n");
        #endif
        }
    }
    if (TrueNtWriteVirtualMemory && MH_CreateHook \
       (TrueNtWriteVirtualMemory, HookNtWriteVirtualMemory, \
       (LPVOID*)&OrgiNtWriteVirtualMemory) == MH_OK )
    {
        if ( MH_EnableHook(TrueNtWriteVirtualMemory) != MH_OK )
        {
        #ifdef _LOGDEBUG
            logmsg("TrueNtWriteVirtualMemory hook failed!\n");
        #endif
        }
    }
    return (1);
}

void WINAPI safe_end(void)
{
    if (OrgiLoadLibraryExW)
    {
        MH_DisableHook(TrueLoadLibraryExW);
    }
    if (OrgiCreateProcessInternalW)
    {
        MH_DisableHook(TrueCreateProcessInternalW);
    }
    if (OrgiNtCreateUserProcess)
    {
        MH_DisableHook(TrueNtCreateUserProcess);
    }
    if (OrgiNtWriteVirtualMemory)
    {
        MH_DisableHook(TrueNtWriteVirtualMemory);
    }
    return;
}
