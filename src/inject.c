#define JECT_EXTERN

#include "inject.h"
#include "inipara.h"
#include "header.h"

typedef struct _REMOTE_PARAMETER
{
    WCHAR		strDll[VALUE_LEN+1];
    DWORD_PTR	dwLoadLibraryAddr;
    DWORD_PTR	dwRtlInitUnicodeString;
} RemotePara;

typedef NTSTATUS (NTAPI *_NtLdrLoadDll)
(
    IN PWCHAR PathToFile,
    IN ULONG Flags,
    IN PUNICODE_STRING ModuleFileName,
    OUT PHANDLE ModuleHandle
);

typedef VOID (WINAPI *_NtRtlInitUnicodeString)(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
);

static void WINAPI ThreadProc(RemotePara* pRemotePara)
{
    typedef NTSTATUS (NTAPI *_NtLdrLoadDll)
    (
        PWCHAR PathToFile,
        ULONG Flags,
        PUNICODE_STRING ModuleFileName,
        PHANDLE ModuleHandle
    );

    typedef VOID (WINAPI *_NtRtlInitUnicodeString)
    (
        PUNICODE_STRING DestinationString,
        PCWSTR SourceString
    );

    UNICODE_STRING			usDllName;
    HANDLE					DllHandle;
    _NtLdrLoadDll			pfnLdrLoadDll = (_NtLdrLoadDll)pRemotePara->dwLoadLibraryAddr;
    _NtRtlInitUnicodeString pfnRtlInitUnicodeString = (_NtRtlInitUnicodeString)pRemotePara->dwRtlInitUnicodeString;
    pfnRtlInitUnicodeString(&usDllName, pRemotePara->strDll);
    pfnLdrLoadDll(NULL, 0, &usDllName, &DllHandle);
    return;
}

static void AfterThreadProc (void) { }

extern  DWORD_PTR __cdecl GetLoaderPicSize(void);
extern  VOID __cdecl GetLoaderPic(PVOID picBuffer, PVOID LoadLibraryA, PCHAR dllName, DWORD_PTR dllNameLen);

/* PIC inject for win8/8.1 */
BOOL pic_inject(void *mpara, LPCWSTR dll_name)
{
    BOOL		exitCode = FALSE;
    CONTEXT		context;
    DWORD_PTR	*returnPointer;
    DWORD_PTR	i;
    PVOID		picBuf;
    LPVOID		funcBuff;
    SIZE_T		cbSize;
    char		dllName[VALUE_LEN+1];
    HMODULE		hNtdll;
    PROCESS_INFORMATION pi = *(LPPROCESS_INFORMATION)mpara;
    _NtAllocateVirtualMemory TrueNtAllocateVirtualMemory = NULL;
    _NtWriteVirtualMemory	 TrueNtWriteVirtualMemory	 = NULL;
    _NtFreeVirtualMemory	 TrueNtFreeVirtualMemory	 = NULL;
    _NtResumeThread			 TrueNtResumeThread			 = NULL;
    hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll)
    {
        return exitCode;
    }
    TrueNtAllocateVirtualMemory = (_NtAllocateVirtualMemory)GetProcAddress(hNtdll, "NtAllocateVirtualMemory");
    TrueNtWriteVirtualMemory = (_NtWriteVirtualMemory)GetProcAddress(hNtdll, "NtWriteVirtualMemory");
    TrueNtFreeVirtualMemory = (_NtFreeVirtualMemory)GetProcAddress(hNtdll, "NtFreeVirtualMemory");
    TrueNtResumeThread = (_NtResumeThread)GetProcAddress(hNtdll, "NtResumeThread");
    if ( !(TrueNtAllocateVirtualMemory &&
           TrueNtWriteVirtualMemory    &&
           TrueNtFreeVirtualMemory     &&
           TrueNtResumeThread)
       )
    {
        return exitCode;
    }
    if ( !WideCharToMultiByte(CP_ACP, 0,dll_name,(int)((wcslen(dll_name)+1)*sizeof(WCHAR)), \
                              dllName, VALUE_LEN,"" , NULL)
       )
    {
        return exitCode;
    }
    /* Get its context, so we know where to return to after redirecting logic flow. */
    context.ContextFlags = CONTEXT_FULL;
    GetThreadContext(pi.hThread, &context);
#ifdef _WIN64
    returnPointer = &context.Rip;
#define PLACEHOLDER 0xDEADBEEFDEADBEEF
#else
    returnPointer = &context.Eip;
#define PLACEHOLDER 0xDEADBEEF
#endif
    cbSize = GetLoaderPicSize();
    /* Make a buffer for the PIC */
    picBuf = SYS_MALLOC(cbSize);
    if ( !picBuf )
    {
        return exitCode;
    }
#ifdef _LOGDEBUG
    logmsg("cbSize = %lu\n",cbSize);
#endif
    /* Have the pic copied into that buffer. */
    GetLoaderPic(picBuf, GetProcAddress(GetModuleHandleA("Kernel32.dll"),"LoadLibraryA"), \
                 dllName, (DWORD_PTR)(strlen(dllName)+1));
    /* Replace deadbeef (return address) in the pic with a pointer to the thread's current position. */
    for(i=0; i < cbSize - sizeof(PVOID); i++)
    {
        DWORD_PTR *deadbeef = (DWORD_PTR*)((DWORD_PTR)picBuf + i);
        if(*deadbeef == PLACEHOLDER) {
            *deadbeef = *returnPointer;
            break;
        }
    }

    /* Create a code funcBuff in the target process. */
    funcBuff = VirtualAllocEx(pi.hProcess, 0, cbSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(pi.hProcess, funcBuff, picBuf, cbSize, NULL);
    *returnPointer = (DWORD_PTR)funcBuff;
    exitCode = SetThreadContext(pi.hThread,&context);
    TrueNtResumeThread(pi.hThread,NULL);
    SYS_FREE(picBuf);
    cbSize = 0;
    TrueNtFreeVirtualMemory(pi.hProcess,funcBuff,&cbSize,MEM_RELEASE);
    return exitCode;
}

JECT_EXTERN
unsigned WINAPI InjectDll(void *mpara)
{
    BOOL			bRet		= FALSE;
    LPVOID			funcBuff	= NULL;
    HANDLE			hRemote		= NULL;
    LPVOID			pBuff		= NULL;
    SIZE_T			cbSize		= sizeof(RemotePara);
    SIZE_T			cbCodeSize	= (LPBYTE)AfterThreadProc - (LPBYTE)ThreadProc;
    HMODULE			hNtdll;
    WCHAR			dll_name[VALUE_LEN+1];
    RemotePara		myPara;
    DWORD           os	   = GetOsVersion();
    PROCESS_INFORMATION pi = *(LPPROCESS_INFORMATION)mpara;

    if ( GetModuleFileNameW(dll_module,dll_name,VALUE_LEN) <=0 )
    {
        return bRet;
    }
    /* when tcmalloc enabled or MinGW x64 compile time,InjectDll crash on win8/8.1 */
#if !defined(ENABLE_TCMALLOC) && !defined(__MINGW64__)
    if ( os > 601 )
    {
#ifdef _LOGDEBUG
        logmsg("pic inject runing\n");
#endif
        return pic_inject(mpara,dll_name);
    }
#endif
    hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll)
    {
        return bRet;
    }
    fzero(&myPara, sizeof(myPara));
    myPara.dwLoadLibraryAddr = (DWORD_PTR)GetProcAddress(hNtdll, "LdrLoadDll");
    myPara.dwRtlInitUnicodeString = (DWORD_PTR)GetProcAddress(hNtdll, "RtlInitUnicodeString");
    if ( (myPara.dwLoadLibraryAddr)&&(myPara.dwRtlInitUnicodeString) )
    {
        DWORD dwThreadId  = 0;
        wcsncpy(myPara.strDll,dll_name,VALUE_LEN);
        /* 在进程内分配空间 */
        pBuff = VirtualAllocEx(pi.hProcess, 0, cbSize+cbCodeSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        funcBuff = ((BYTE *)pBuff+cbSize);
        /* 写入参数空间 */
        WriteProcessMemory(pi.hProcess, pBuff, (PVOID)&myPara, cbSize, NULL);
        /* 写入代码空间 */
        WriteProcessMemory(pi.hProcess, funcBuff, (PVOID)ThreadProc, cbCodeSize, NULL);
        hRemote = CreateRemoteThread(pi.hProcess,
                                     NULL,
                                     1024*1024,
                                     (LPTHREAD_START_ROUTINE)funcBuff,
                                     pBuff,
                                     CREATE_SUSPENDED,
                                     &dwThreadId);
        if (hRemote)
        {
            ResumeThread(hRemote);
            WaitForSingleObject(hRemote,1500);
            CloseHandle(hRemote);
        }
        if (pBuff)
        {
            VirtualFreeEx(pi.hProcess, pBuff, 0, MEM_RELEASE);
        }
        if (funcBuff)
        {
            VirtualFreeEx(pi.hProcess, funcBuff, 0, MEM_RELEASE);
        }
    }
    ResumeThread(pi.hThread);
    return bRet;
}
