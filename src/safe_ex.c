#define SAFE_EXTERN

#include "safe_ex.h"
#include "inipara.h"
#include "header.h"
#include "inject.h"
#include "MinHook.h"
#include <process.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <stdio.h>

static _NtCreateUserProcess         OrgiNtCreateUserProcess, TrueNtCreateUserProcess	    = NULL;
static _NtWriteVirtualMemory		OrgiNtWriteVirtualMemory, TrueNtWriteVirtualMemory		= NULL;
static _CreateProcessInternalW 		OrgiCreateProcessInternalW, TrueCreateProcessInternalW	= NULL;
static _NtProtectVirtualMemory		TrueNtProtectVirtualMemory			= NULL;
static _NtQueryInformationProcess	TrueNtQueryInformationProcess		= NULL;
static _RtlNtStatusToDosError		TrueRtlNtStatusToDosError			= NULL;
static _NtSuspendThread				TrueNtSuspendThread					= NULL;
static _NtResumeThread				TrueNtResumeThread					= NULL;
static _NtLoadLibraryExW			TrueLoadLibraryExW					= NULL;

BOOL in_whitelist(LPCWSTR lpfile)
{
    WCHAR *moz_processes[] = {L"", L"plugin-container.exe", L"plugin-hang-ui.exe", L"webapprt-stub.exe",
                              L"webapp-uninstaller.exe",L"WSEnable.exe",L"uninstall\\helper.exe",
                              L"crashreporter.exe",L"CommandExecuteHandler.exe",L"maintenanceservice.exe",
                              L"maintenanceservice_installer.exe",L"updater.exe"
                             };
    static  WCHAR white_list[EXCLUDE_NUM][VALUE_LEN+1];
    int		i = sizeof(moz_processes)/sizeof(moz_processes[0]);
    LPCWSTR pname = lpfile;
    BOOL    ret = FALSE;
    if (lpfile[0] == L'"')
    {
        pname = &lpfile[1];
    }
    /* 遍历白名单一次,只需遍历一次 */
    ret = stristrW(white_list[1],L"plugin-container.exe") != NULL;
    if ( !ret )
    {
        /* firefox目录下进程的路径 */
        int num;
        WCHAR temp[VALUE_LEN+1];
        GetModuleFileNameW(NULL,temp,VALUE_LEN);
        wcsncpy(white_list[0],(LPCWSTR)temp,VALUE_LEN);
        PathRemoveFileSpecW(temp);
        for(num=1; num<i; ++num)
        {
            _snwprintf(white_list[num],VALUE_LEN,L"%ls\\%ls", temp, moz_processes[num]);
        }
        ret = foreach_section(L"whitelist", &white_list[num], EXCLUDE_NUM-num);
    }
    if ( (ret = !ret) == FALSE )
    {
        /* 核对白名单 */
        for ( i=0; i<EXCLUDE_NUM ; i++ )
        {
            if (wcslen(white_list[i]) == 0)
            {
                continue;
            }
            if ( StrChrW(white_list[i],L'*') || StrChrW(white_list[i],L'?') )
            {
                if ( PathMatchSpecW(pname,white_list[i]) )
                {
                    ret = TRUE;
                    break;
                }
            }
            else if (white_list[i][1] != L':')
            {
                PathToCombineW(white_list[i],VALUE_LEN);
            }
            if (_wcsnicmp(white_list[i],pname,wcslen(white_list[i]))==0)
            {
                ret = TRUE;
                break;
            }
        }
    }
    return ret;
}

BOOL ProcessIsCUI(LPCWSTR lpfile)
{
    WCHAR lpname[VALUE_LEN+1] = {0};
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
    return TRUE;
}

NTSTATUS WINAPI HookNtWriteVirtualMemory(IN HANDLE ProcessHandle,
        IN PVOID BaseAddress,
        IN PVOID Buffer,
        IN SIZE_T NumberOfBytesToWrite,
        OUT PSIZE_T NumberOfBytesWritten)
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


NTSTATUS WINAPI HookNtCreateUserProcess(PHANDLE ProcessHandle,PHANDLE ThreadHandle,
                                        ACCESS_MASK ProcessDesiredAccess,ACCESS_MASK ThreadDesiredAccess,
                                        POBJECT_ATTRIBUTES ProcessObjectAttributes,
                                        POBJECT_ATTRIBUTES ThreadObjectAttributes,
                                        ULONG CreateProcessFlags,
                                        ULONG CreateThreadFlags,
                                        PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
                                        PVOID CreateInfo,
                                        PNT_PROC_THREAD_ATTRIBUTE_LIST AttributeList)
{
    RTL_USER_PROCESS_PARAMETERS mY_ProcessParameters;
    NTSTATUS	status;
    BOOL		tohook	= FALSE;
    fzero(&mY_ProcessParameters,sizeof(RTL_USER_PROCESS_PARAMETERS));
    if ( stristrW(ProcessParameters->ImagePathName.Buffer, L"SumatraPDF.exe") ||
         stristrW(ProcessParameters->ImagePathName.Buffer, L"java.exe") ||
         stristrW(ProcessParameters->ImagePathName.Buffer, L"jp2launcher.exe"))
    {
        tohook = TRUE;
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
#if !defined(LIBPORTABLE_STATIC)
    if ( NT_SUCCESS(status)&&tohook )
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

BOOL WINAPI HookCreateProcessInternalW (HANDLE hToken,
                                        LPCWSTR lpApplicationName,
                                        LPWSTR lpCommandLine,
                                        LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                        LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                        BOOL bInheritHandles,
                                        DWORD dwCreationFlags,
                                        LPVOID lpEnvironment,
                                        LPCWSTR lpCurrentDirectory,
                                        LPSTARTUPINFOW lpStartupInfo,
                                        LPPROCESS_INFORMATION lpProcessInformation,
                                        PHANDLE hNewToken)
{
    BOOL	ret		= FALSE;
    LPWSTR	lpfile	= lpCommandLine;
    BOOL    tohook	= FALSE;
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
        /* 静态编译时,不能启用远程注入 */
    #if !defined(LIBPORTABLE_STATIC)
        dwCreationFlags |= CREATE_SUSPENDED;
        tohook = TRUE;
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
                                      lpThreadAttributes,bInheritHandles,dwCreationFlags,lpEnvironment,lpCurrentDirectory,
                                      lpStartupInfo,lpProcessInformation,hNewToken);
    if ( ret && tohook )
    {
        InjectDll(lpProcessInformation);
    }
    return ret;
}

BOOL iSAuthorized(LPCWSTR lpFileName)
{
    BOOL	ret = FALSE;
    BOOL    wow64 = FALSE;
    LPWSTR	filename = NULL;
    wchar_t *szAuthorizedList[] = {L"comctl32.dll", L"uxtheme.dll", L"indicdll.dll",
                                   L"msctf.dll",L"shell32.dll", L"imageres.dll",
                                   L"winmm.dll",L"ole32.dll", L"oleacc.dll",
                                   L"oleaut32.dll",L"secur32.dll",L"shlwapi.dll",
                                   L"ImSCTip.DLL",L"gdi32.dll",L"dwmapi.dll"
                                  };
    WORD line = sizeof(szAuthorizedList)/sizeof(szAuthorizedList[0]);
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
            filename = _wcsnicmp(lpFileName,sysdir,wcslen(sysdir))?NULL:PathFindFileNameW(lpFileName);
        }
    }
    else
    {
        filename = (LPWSTR)lpFileName;
    }
    if (filename)
    {
        WORD  i;
        for(i=0; i<line; i++)
        {
            if ( _wcsicmp(filename,szAuthorizedList[i]) == 0 )
            {
                ret = TRUE;
                break;
            }
        }
    }
    return ret;
}

HMODULE WINAPI HookLoadLibraryExW(LPCWSTR lpFileName,HANDLE hFile,DWORD dwFlags)
{
    do
    {
        UINT_PTR dwCaller;
        if ( iSAuthorized(lpFileName) ) 
        {
            break;
        }
    #ifdef __GNUC__
        dwCaller = (UINT_PTR)__builtin_return_address(0);
    #else
        dwCaller = (UINT_PTR)_ReturnAddress();
    #endif
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
    }while (0);
    return TrueLoadLibraryExW(lpFileName, hFile, dwFlags);
}


unsigned WINAPI init_safed(void * pParam)
{
    HMODULE		hNtdll;
    DWORD		ver = GetOsVersion();
    hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll)
    {
        TrueNtSuspendThread				= (_NtSuspendThread)GetProcAddress
                                          (hNtdll, "NtSuspendThread");
        TrueNtResumeThread				= (_NtResumeThread)GetProcAddress
                                          (hNtdll, "NtResumeThread");
        TrueNtQueryInformationProcess	= (_NtQueryInformationProcess)GetProcAddress(hNtdll,
                                          "NtQueryInformationProcess");
        TrueNtWriteVirtualMemory		= (_NtWriteVirtualMemory)GetProcAddress(hNtdll,
                                          "NtWriteVirtualMemory");
        TrueRtlNtStatusToDosError		= (_RtlNtStatusToDosError)GetProcAddress(hNtdll,
                                          "RtlNtStatusToDosError");
        if (ver>601)  /* win8 */
        {
            TrueNtCreateUserProcess     = (_NtCreateUserProcess)GetProcAddress(hNtdll, "NtCreateUserProcess");
            if (TrueNtCreateUserProcess && \
                MH_CreateHook(TrueNtCreateUserProcess, HookNtCreateUserProcess, (LPVOID*)&OrgiNtCreateUserProcess) == MH_OK )
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
            TrueCreateProcessInternalW	= (_CreateProcessInternalW)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "CreateProcessInternalW");
            if (TrueCreateProcessInternalW && \
                MH_CreateHook(TrueCreateProcessInternalW, HookCreateProcessInternalW, (LPVOID*)&OrgiCreateProcessInternalW) == MH_OK )
            {
                if ( MH_EnableHook(TrueCreateProcessInternalW) != MH_OK )
                {
                #ifdef _LOGDEBUG
                    logmsg("TrueCreateProcessInternalW hook failed!\n");
                #endif
                }
            }
        }
    }
    if ( MH_CreateHook(&LoadLibraryExW, &HookLoadLibraryExW, (LPVOID*)&TrueLoadLibraryExW) == MH_OK )
    {
        if ( MH_EnableHook(&LoadLibraryExW) != MH_OK )
        {
        #ifdef _LOGDEBUG
            logmsg("LoadLibraryExW hook failed!\n");
        #endif
        }
    }
    if (TrueNtWriteVirtualMemory && \
        MH_CreateHook(TrueNtWriteVirtualMemory, HookNtWriteVirtualMemory, (LPVOID*)&OrgiNtWriteVirtualMemory) == MH_OK )
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
    if (TrueLoadLibraryExW)
    {
        MH_DisableHook(&LoadLibraryExW);
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
