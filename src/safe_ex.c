#include "safe_ex.h"
#include "inipara.h"
#include "winapis.h"
#include "inject.h"
#include <process.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <stdio.h>

static _NtQueryInformationProcess  pNtQueryInformationProcess;
static _RtlNtStatusToDosError      pRtlNtStatusToDosError;
static _NtCreateUserProcess        sNtCreateUserProcess, 
                                   pNtCreateUserProcess;
static _NtWriteVirtualMemory       sNtWriteVirtualMemory, 
                                   pNtWriteVirtualMemory;
static _CreateProcessInternalW     sCreateProcessInternalW, 
                                   pCreateProcessInternalW;
static LoadLibraryExWPtr           pLoadLibraryExW;

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
                              L"CommandExecuteHandler.exe",
                              L"maintenanceservice.exe",
                              L"maintenanceservice_installer.exe",
                              L"updater.exe",
                              L"wow_helper.exe",
                              L"pingsender.exe"
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
            wnsprintfW(white_list[num],VALUE_LEN,L"%ls\\%ls", temp, moz_processes[num]);
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
    return sNtWriteVirtualMemory(ProcessHandle,
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
     #ifndef _M_X64
         PathMatchSpecW(ProcessParameters->ImagePathName.Buffer,               \
         L"*\\plugins\\FlashPlayerPlugin_*.exe")                              ||
     #endif
         stristrW(ProcessParameters->ImagePathName.Buffer, L"java.exe")       ||
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
    status = sNtCreateUserProcess(ProcessHandle, ThreadHandle,
                                  ProcessDesiredAccess, ThreadDesiredAccess,
                                  ProcessObjectAttributes, ThreadObjectAttributes,
                                  CreateProcessFlags, CreateThreadFlags, ProcessParameters,
                                  CreateInfo, AttributeList);
    /* 静态编译或者启用GCC lto时,不能启用远程注入 */
#if !( defined(LIBPORTABLE_STATIC) || defined(__LTO__) )
    if ( NT_SUCCESS(status) && tohook )
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
    if ( stristrW(lpfile, L"SumatraPDF.exe")                            ||
     #ifndef _M_X64
         PathMatchSpecW(lpfile,L"*\\plugins\\FlashPlayerPlugin_*.exe")  ||
     #endif
         stristrW(lpfile, L"java.exe")                                  ||
         stristrW(lpfile, L"jp2launcher.exe"))
    {
        /* 静态编译或者启用GCC lto时,不能启用远程注入 */
    #if !( defined(LIBPORTABLE_STATIC) || defined(__LTO__) )
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
            SetLastError( pRtlNtStatusToDosError(STATUS_ERROR) );
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
            SetLastError( pRtlNtStatusToDosError(STATUS_ERROR) );
            return ret;
        }
    }
    ret = sCreateProcessInternalW(hToken,lpApplicationName,lpCommandLine,lpProcessAttributes,
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
        return sLoadLibraryExWStub(lpFileName, hFile, dwFlags);
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
    return sLoadLibraryExWStub(lpFileName, hFile, dwFlags);
}

unsigned WINAPI init_safed(void * pParam)
{
    HMODULE		hNtdll, hKernel;
    DWORD		ver = GetOsVersion();
    hNtdll   =  GetModuleHandleW(L"ntdll.dll");
    hKernel  =  GetModuleHandleW(L"kernel32.dll");

    if ( hNtdll == NULL || hKernel  == NULL ||
        (pRtlNtStatusToDosError = (_RtlNtStatusToDosError)GetProcAddress(hNtdll, "RtlNtStatusToDosError")) == NULL ||
        (pNtWriteVirtualMemory = (_NtWriteVirtualMemory)GetProcAddress(hNtdll, "NtWriteVirtualMemory")) == NULL )
    {
        return 0;
    }
    if ( ver>601 )  /* win8 */
    {
        pNtCreateUserProcess = (_NtCreateUserProcess)GetProcAddress(hNtdll, "NtCreateUserProcess");
        if (!creator_hook(pNtCreateUserProcess, HookNtCreateUserProcess, (LPVOID*)&sNtCreateUserProcess))
        {
        
        #ifdef _LOGDEBUG
            logmsg("NtCreateUserProcess hook failed!\n");
        #endif
        }
    }
    else
    {
        pCreateProcessInternalW	= (_CreateProcessInternalW)GetProcAddress(hKernel, "CreateProcessInternalW");
        if (!creator_hook(pCreateProcessInternalW, HookCreateProcessInternalW, (LPVOID*)&sCreateProcessInternalW))
        {
        #ifdef _LOGDEBUG
            logmsg("pCreateProcessInternalW hook failed!\n");
        #endif
        }
    }
    if ( ver<503 )  /* winxp-2003 */
    {
        pLoadLibraryExW	= (LoadLibraryExWPtr)GetProcAddress(hKernel, "LoadLibraryExW");
        if (!creator_hook(pLoadLibraryExW, HookLoadLibraryExW, (LPVOID*)&sLoadLibraryExWStub))
        {
        #ifdef _LOGDEBUG
            logmsg("LoadLibraryExW hook failed!\n");
        #endif
        }
    }
    return creator_hook(pNtWriteVirtualMemory, HookNtWriteVirtualMemory, (LPVOID*)&sNtWriteVirtualMemory);
}
