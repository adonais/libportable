#define SAFE_EXTERN

#include "safe_ex.h"
#include "inipara.h"
#include "header.h"
#include "inject.h"
#include "mhook-lib/mhook.h"
#include <process.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <stdio.h>

static _NtCreateUserProcess             TrueNtCreateUserProcess				= NULL;
static _NtWriteVirtualMemory			TrueNtWriteVirtualMemory			= NULL;
static _NtProtectVirtualMemory			TrueNtProtectVirtualMemory			= NULL;
static _NtQueryInformationProcess		TrueNtQueryInformationProcess		= NULL;
static _RtlNtStatusToDosError			TrueRtlNtStatusToDosError			= NULL;
static _CreateProcessInternalW 			TrueCreateProcessInternalW			= NULL;
static _NtSuspendThread					TrueNtSuspendThread					= NULL;
static _NtResumeThread					TrueNtResumeThread					= NULL;
static _NtLoadLibraryExW				TrueLoadLibraryExW					= NULL;

BOOL WINAPI in_whitelist(LPCWSTR lpfile)
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

ULONG_PTR WINAPI GetParentProcess(HANDLE hProcess)
{
	ULONG_PTR	dwParentPID;
	NTSTATUS	status;
	PROCESS_BASIC_INFORMATION pbi;
	status = TrueNtQueryInformationProcess( hProcess,
											ProcessBasicInformation,
											(PVOID)&pbi,
											sizeof(PROCESS_BASIC_INFORMATION),
											NULL );

	if ( NT_SUCCESS(status) )
		dwParentPID = (ULONG_PTR)pbi.Reserved3;
	else
		dwParentPID = 0;
	return dwParentPID;
}

BOOL WINAPI ProcessIsCUI(LPCWSTR lpfile)
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
	return TrueNtWriteVirtualMemory(ProcessHandle,
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
	PROCESS_INFORMATION ProcessInformation;
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
	status = TrueNtCreateUserProcess(ProcessHandle, ThreadHandle,
								  ProcessDesiredAccess, ThreadDesiredAccess,
								  ProcessObjectAttributes, ThreadObjectAttributes,
								  CreateProcessFlags, CreateThreadFlags, ProcessParameters,
								  CreateInfo, AttributeList);
	if ( NT_SUCCESS(status)&&tohook)
	{
		ULONG Suspend;
		fzero(&ProcessInformation,sizeof(PROCESS_INFORMATION));
		ProcessInformation.hProcess = *ProcessHandle;
		ProcessInformation.hThread = *ThreadHandle;
	/* when tcmalloc enabled or MinGW compile time,InjectDll crash on win8/8.1 */
	#if !defined(ENABLE_TCMALLOC) && !defined(__GNUC__) 
		if ( NT_SUCCESS(TrueNtSuspendThread(ProcessInformation.hThread,&Suspend)) )
		{
		#ifdef _LOGDEBUG
			logmsg("NtInjectDll() run .\n");
		#endif
			InjectDll(&ProcessInformation);
		}
	#endif
	}
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
		 stristrW(lpfile, L"java.exe") ||
		 stristrW(lpfile, L"jp2launcher.exe"))
	{
		dwCreationFlags |= CREATE_SUSPENDED;
		tohook = TRUE;
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
	ret =  TrueCreateProcessInternalW(hToken,lpApplicationName,lpCommandLine,lpProcessAttributes,
		   lpThreadAttributes,bInheritHandles,dwCreationFlags,lpEnvironment,lpCurrentDirectory,
		   lpStartupInfo,lpProcessInformation,hNewToken);
	/* 远程注入进程 */
	if ( ret && tohook )
	{
	#ifdef _LOGDEBUG
		logmsg("InjectDll run .\n");
	#endif
		InjectDll(lpProcessInformation);
	}
	return ret;
}	

BOOL WINAPI iSAuthorized(LPCWSTR lpFileName)
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
		for(i=0;i<line;i++)
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
    UINT_PTR	dwCaller;
	/* 是否信任的dll */
	if ( iSAuthorized(lpFileName) )
	{
		return TrueLoadLibraryExW(lpFileName, hFile, dwFlags);
	}
#ifdef __GNUC__
	dwCaller = (UINT_PTR)__builtin_return_address(0);
#else
	dwCaller = (UINT_PTR)_ReturnAddress();
#endif
    /* 判断是否是从User32.dll调用 */
	if ( is_specialdll(dwCaller,L"user32.dll") )
	{
		if ( PathMatchSpecW(lpFileName, L"*.exe") || in_whitelist(lpFileName) )
		{
			/* javascript api 获取应用程序图标时 */
		#ifdef _LOGDEBUG
			logmsg("%ls in whitelist\n",lpFileName);
		#endif
			return TrueLoadLibraryExW(lpFileName, hFile, dwFlags);  
		}
		else
		{
		#ifdef _LOGDEBUG
			logmsg("the  %ls disable load\n",lpFileName);
		#endif
			return NULL;  
		}
	}
    return TrueLoadLibraryExW(lpFileName, hFile, dwFlags);  
}


unsigned WINAPI init_safed(void * pParam)
{
	HMODULE		hNtdll;
	DWORD		ver = GetOsVersion();
	TrueLoadLibraryExW = (_NtLoadLibraryExW)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"LoadLibraryExW");
	if (!TrueLoadLibraryExW)
	{
	#ifdef _LOGDEBUG
		logmsg("TrueLoadLibraryExW is null %lu\n",GetLastError());
	#endif
	}
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
			if (TrueNtCreateUserProcess)
			{
				Mhook_SetHook((PVOID*)&TrueNtCreateUserProcess, (PVOID)HookNtCreateUserProcess); 
			}
		}
		else
		{
			TrueCreateProcessInternalW	= (_CreateProcessInternalW)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "CreateProcessInternalW");
			if (TrueCreateProcessInternalW)
			{
				Mhook_SetHook((PVOID*)&TrueCreateProcessInternalW, (PVOID)HookCreateProcessInternalW);
			}
		}
	}
	if (TrueLoadLibraryExW)
	{
		Mhook_SetHook((PVOID*)&TrueLoadLibraryExW, (PVOID)HookLoadLibraryExW);
	}
	if (TrueNtWriteVirtualMemory)
	{
		Mhook_SetHook((PVOID*)&TrueNtWriteVirtualMemory, (PVOID)HookNtWriteVirtualMemory);
	}
	return (1);
}

void safe_end(void)
{
	if (TrueLoadLibraryExW)
	{
		Mhook_Unhook((PVOID*)&TrueLoadLibraryExW);
	}
	if (TrueCreateProcessInternalW)
	{
		Mhook_Unhook((PVOID*)&TrueCreateProcessInternalW);
	}
	if (TrueNtCreateUserProcess)
	{
		Mhook_Unhook((PVOID*)&TrueNtCreateUserProcess);
	}
	if (TrueNtWriteVirtualMemory)
	{
		Mhook_Unhook((PVOID*)&TrueNtWriteVirtualMemory);
	}
	return;
}
