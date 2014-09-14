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

static _NtFreeVirtualMemory  TrueNtFreeVirtualMemory	 = NULL;
static _NtResumeThread		 TrueNtResumeThread			 = NULL;

#if defined(_M_X64)
unsigned char codeToInject[] =
{
    /* Push a dummy value for the return address */
    0x50,                                                                   // push rax
    0x9c,                                                                   // pushfq
    0x50,                                                                   // push rax
    /* rax is saved, now overwrite the return address we pushed earlier */
    0x48, 0xB8, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,	            // mov rax, 0CCCCCCCCCCCCCCCCh
    0x48, 0x89, 0x84, 0x24, 0x10, 0x00, 0x00, 0x00,                         // mov qword ptr [rsp+16],rax  
    0x51,                                                                   // push rcx
    0x52,                                                                   // push rdx
    0x53,                                                                   // push rbx
    0x55,                                                                   // push rbp
    0x56,                                                                   // push rsi
    0x57,                                                                   // push rdi
    0x41, 0x50,                                                             // push r8
    0x41, 0x51,                                                             // push r9
    0x41, 0x52,                                                             // push r10
    0x41, 0x53,                                                             // push r11
    0x41, 0x54,                                                             // push r12
    0x41, 0x55,                                                             // push r13
    0x41, 0x56,                                                             // push r14
    0x41, 0x57,                                                             // push r15
    0x48, 0xB9, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,             // mov rcx, 0CCCCCCCCCCCCCCCCh
    0x48, 0xB8, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,             // mov rax, 0CCCCCCCCCCCCCCCCh
    0xFF, 0xD0,                                                             // call rax
    /* Restore the registers */
    0x41, 0x5F,                                                             // pop r15
    0x41, 0x5E,                                                             // pop r14
    0x41, 0x5D,                                                             // pop r13
    0x41, 0x5C,                                                             // pop r12
    0x41, 0x5B,                                                             // pop r11
    0x41, 0x5A,                                                             // pop r10
    0x41, 0x59,                                                             // pop r9
    0x41, 0x58,                                                             // pop r8
    0x5F,                                                                   // pop rdi
    0x5E,                                                                   // pop rsi
    0x5D,                                                                   // pop rbp
    0x5B,                                                                   // pop rbx
    0x5A,                                                                   // pop rdx
    0x59,                                                                   // pop rcx
    0x58,                                                                   // pop rax
    /* Restore the flags */
    0x9D,                                                                   // popfq
    0xC3                                                                    // ret
};
#elif defined(_M_IX86)
#define PLACEHOLDER 0xDEADBEEF
#if defined(_MSC_VER)
__declspec(naked) int remote32_asm(void)
{
_asm{
    push 0xDEADBEEF
    pushfd
    pushad
    push 0xDEADBEEF
    mov eax, 0xDEADBEEF
    call eax
    popad
    popfd
    ret
    }
}
#elif defined(__GNUC__)
static void remote32_asm(void)
{
    __asm(
      ".globl _remote_stub\n"
      "_remote_stub:\n"
      "pushl $0xDEADBEEF\n"         // return address, [1]
      "pushfl\n"
      "pushal\n"
      "pushl $0xDEADBEEF\n"         // function parameter, [8]
      "movl $0xDEADBEEF, %eax\n"    // function to call, [13]
      "call *%eax\n"
      "popal\n"
      "popfl\n"
      "ret"
    );
}
#endif
#endif

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

#ifdef _M_IX86
BOOL inject32(void *mpara,RemotePara* func_param)
{
    CONTEXT		ctx;
    PVOID		picBuf = NULL;
    LPVOID		funcBuff = NULL;
    DWORD       old_protect;
    SIZE_T		cbSize,tsize;
    BOOL		exitCode = FALSE;
    PROCESS_INFORMATION pi = *(LPPROCESS_INFORMATION)mpara;
    ctx.ContextFlags = CONTEXT_CONTROL;
    if ( !GetThreadContext(pi.hThread, &ctx) )
    {
    #ifdef _LOGDEBUG
        logmsg("GetThreadContext() in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto clear;
    }
    cbSize = ((LPBYTE)&AfterThreadProc - (LPBYTE)&remote32_asm + 0x0F) & ~0x0F;    /* 函数代码尺寸按16bits对齐 */
    tsize  = cbSize+sizeof(RemotePara);
    /* Create a code funcBuff in the target process. */
    if ( (funcBuff = VirtualAllocEx(pi.hProcess, 0, tsize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)) == NULL )
    {
    #ifdef _LOGDEBUG
        logmsg("VirtualAllocEx() in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto clear;
    }
    if ( (picBuf = SYS_MALLOC(cbSize) ) == NULL )
    {
    #ifdef _LOGDEBUG
        logmsg("SYS_MALLOC() return false,cbSize = %lu\n",cbSize);
    #endif
        goto clear;
    }
    memcpy( picBuf, &remote32_asm, cbSize );
	*(DWORD*)((DWORD)picBuf + 1)  = ctx.Eip;
	*(DWORD*)((DWORD)picBuf + 8)  = (DWORD)funcBuff + cbSize;
	*(DWORD*)((DWORD)picBuf + 13) = (DWORD)funcBuff + ((DWORD)ThreadProc - (DWORD)remote32_asm);
    if ( !WriteProcessMemory(pi.hProcess, funcBuff, picBuf, cbSize, NULL) )
    {
    #ifdef _LOGDEBUG
        logmsg("WriteProcessMemory(picBuf) in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto clear;
    }
    if ( !WriteProcessMemory(pi.hProcess, (BYTE *)funcBuff+cbSize, (PVOID)func_param, sizeof(RemotePara), NULL) )
    {
    #ifdef _LOGDEBUG
        logmsg("WriteProcessMemory(func_param) in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto clear;
    }
    if (!VirtualProtectEx(pi.hProcess, funcBuff, tsize, PAGE_EXECUTE_READ, &old_protect))
    {
    #ifdef _LOGDEBUG
        logmsg("VirtualProtectEx() in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto clear;
    }
    if ( !FlushInstructionCache(pi.hProcess, funcBuff, tsize) )
    {
    #ifdef _LOGDEBUG
        logmsg("FlushInstructionCache() in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto clear;
    }
	ctx.Eip = (DWORD_PTR)funcBuff;
	ctx.ContextFlags = CONTEXT_CONTROL;
    exitCode = SetThreadContext(pi.hThread,&ctx);    
clear:
    TrueNtResumeThread(pi.hThread, NULL);
    Sleep(800);
    if ( picBuf )
    {
        SYS_FREE(picBuf);
    }
    if ( funcBuff )
    {
        tsize = 0;
        TrueNtFreeVirtualMemory(pi.hProcess,funcBuff,&tsize,MEM_RELEASE);
    }
#ifdef _LOGDEBUG
    logmsg("%s return(%lu).\n", __FUNCTION__, exitCode);
#endif
    return exitCode;
}
#endif

#if defined(_M_X64)
BOOL inject64(void *mpara,RemotePara* func_param)
{
	LPVOID      pRemoteMemDllName = NULL;
	LPVOID      pRemoteMemFunction = NULL;
	SIZE_T      nDllNameBuffSize = 0;
	DWORD_PTR   nFunctionBuffSize;
    BOOL        exitCode = FALSE;
    CONTEXT     ctx;
    DWORD_PTR   dwOldIP;
    DWORD_PTR   pfnLoadLibrary;
    PROCESS_INFORMATION  pi = *(LPPROCESS_INFORMATION)mpara;

    nFunctionBuffSize = sizeof(codeToInject);
	/* Allocate memory on the target process with current DLL path */
	nDllNameBuffSize = (wcslen(func_param->strDll)+1) * sizeof(WCHAR);
	pRemoteMemDllName = VirtualAllocEx(pi.hProcess, NULL, nDllNameBuffSize, MEM_COMMIT,PAGE_READWRITE);
	if(pRemoteMemDllName)
	{
        SIZE_T nNumBytesWritten = 0;
		exitCode = WriteProcessMemory(pi.hProcess, pRemoteMemDllName, (void*)func_param->strDll,
                                      nDllNameBuffSize, &nNumBytesWritten);
	}
 
	if(!exitCode)
	{
    #ifdef _LOGDEBUG
        logmsg("WriteProcessMemory(DllName) in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
		goto cleanup;
	}
 
	/* Allocate memory for the stub */
	pRemoteMemFunction = VirtualAllocEx(pi.hProcess, NULL, nFunctionBuffSize, MEM_COMMIT,PAGE_EXECUTE_READWRITE);
 
	pfnLoadLibrary = (DWORD_PTR)GetProcAddress(GetModuleHandleW(L"Kernel32"), "LoadLibraryW");
	if(!pfnLoadLibrary)
	{
    #ifdef _LOGDEBUG
        logmsg("GetProcAddress(LoadLibraryW) in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
		goto cleanup;
	}
	/* Set the instruction pointer to point to our function */
	ctx.ContextFlags = CONTEXT_FULL;
	if(!GetThreadContext(pi.hThread, &ctx))
	{
    #ifdef _LOGDEBUG
        logmsg("GetThreadContext(ctx) in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
		goto cleanup;
	}
    dwOldIP = ctx.Rip;
	/* Jump ahead the stack a little bit so we don't accidentally overwrite something */
	ctx.Rsp -= 128;
	/* Make sure the stack will be aligned to 16 bytes right at the LoadLibrary call */
	ctx.Rsp = ctx.Rsp & ~15;
	ctx.Rsp -= 8;
	ctx.Rip = (DWORD_PTR) pRemoteMemFunction;
	ctx.ContextFlags = CONTEXT_FULL;

	/* Replace placeholders */
	memcpy(codeToInject + 5, &dwOldIP, sizeof(dwOldIP));
	memcpy(codeToInject + 45, &pRemoteMemDllName, sizeof(pRemoteMemDllName));
	memcpy(codeToInject + 55, &pfnLoadLibrary, sizeof(pfnLoadLibrary));
	if(!WriteProcessMemory(pi.hProcess, pRemoteMemFunction, codeToInject, nFunctionBuffSize, NULL))
	{
    #ifdef _LOGDEBUG
        logmsg("WriteProcessMemory(codeToInject) in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
		goto cleanup;
	}
	exitCode = SetThreadContext(pi.hThread, &ctx);
    if (!exitCode)
	{
    #ifdef _LOGDEBUG
        logmsg("SetThreadContext(ctx) in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
	}
 
cleanup:
	/* Cleanup allocated data */
	TrueNtResumeThread(pi.hThread, NULL);
    Sleep(800);    
	if(pRemoteMemDllName)
    {
		nDllNameBuffSize = 0;
        TrueNtFreeVirtualMemory(pi.hProcess,pRemoteMemDllName,&nDllNameBuffSize,MEM_RELEASE);
    }
	if(pRemoteMemFunction)
    {
        nFunctionBuffSize = 0 ;
        TrueNtFreeVirtualMemory(pi.hProcess,pRemoteMemFunction,&nFunctionBuffSize,MEM_RELEASE);
    }
#ifdef _LOGDEBUG
    logmsg("%s return(%lu).\n", __FUNCTION__, exitCode);
#endif
	return exitCode;
}
#endif

JECT_EXTERN
unsigned WINAPI InjectDll(void *mpara)
{
    RemotePara	    myPara;
    WCHAR			dll_name[VALUE_LEN+1];
    BOOL			bRet   = FALSE;
    HMODULE		    hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return bRet;

    /* 初始化远程函数参数 */
    fzero(&myPara, sizeof(RemotePara));
    TrueNtFreeVirtualMemory = (_NtFreeVirtualMemory)GetProcAddress(hNtdll, "NtFreeVirtualMemory");
    TrueNtResumeThread = (_NtResumeThread)GetProcAddress(hNtdll, "NtResumeThread");
    myPara.dwLoadLibraryAddr = (DWORD_PTR)GetProcAddress(hNtdll, "LdrLoadDll");
    myPara.dwRtlInitUnicodeString = (DWORD_PTR)GetProcAddress(hNtdll, "RtlInitUnicodeString");
    if ( TrueNtFreeVirtualMemory && TrueNtResumeThread && myPara.dwLoadLibraryAddr && myPara.dwRtlInitUnicodeString && \
         GetModuleFileNameW(dll_module,dll_name,VALUE_LEN) >0 )
    {
        wcsncpy(myPara.strDll,dll_name,VALUE_LEN);
    #if defined(_M_IX86)
        return inject32(mpara,&myPara);
    #elif defined(_M_X64)
        return inject64(mpara,&myPara);
    #else
    #error "unsupported platform"
    #endif
    }
    return bRet;
}
