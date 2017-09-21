#include "inject.h"
#include "inipara.h"
#include "winapis.h"

#ifdef _MSC_VER
#define MOZ_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define MOZ_NOINLINE __attribute__((noinline))
#else
#define MOZ_NOINLINE
#endif

typedef struct _REMOTE_PARAMETER
{
    WCHAR     strDll[VALUE_LEN+1];
    uintptr_t dwLoadLibraryAddr;
    uintptr_t dwRtlInitUnicodeString;
} RemotePara;

static _NtFreeVirtualMemory pNtFreeVirtualMemory = NULL;
static _NtSuspendThread     pNtSuspendThread     = NULL;
static _NtResumeThread      pNtResumeThread      = NULL;

#ifdef _MSC_VER
#pragma check_stack (off)
#endif

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
    ".globl _remote32_asm\n"
    "_remote32_as:\n"
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
        PULONG Flags,
        PUNICODE_STRING ModuleFileName,
        PHANDLE ModuleHandle
    );
    typedef VOID (WINAPI *_NtRtlInitUnicodeString)
    (
        PUNICODE_STRING DestinationString,
        PCWSTR SourceString
    );
    UNICODE_STRING                      usDllName;
    HANDLE                              DllHandle;
    _NtLdrLoadDll                       pfnLdrLoadDll = (_NtLdrLoadDll)pRemotePara->dwLoadLibraryAddr;
    _NtRtlInitUnicodeString pfnRtlInitUnicodeString   = (_NtRtlInitUnicodeString)pRemotePara->dwRtlInitUnicodeString;
    pfnRtlInitUnicodeString(&usDllName, pRemotePara->strDll);
    pfnLdrLoadDll(NULL, NULL, &usDllName, &DllHandle);
    return;
}

static void MOZ_NOINLINE AfterThreadProc (void)
{
    ;
}

#ifdef __MSVC__
#pragma check_stack
#endif

#ifdef _M_IX86
bool inject32(void *mpara,RemotePara* func_param)
{
    CONTEXT  ctx;
    LPVOID   picBuf = NULL;
    LPVOID   funcBuff = NULL;
    DWORD    old_protect = 0;
    SIZE_T   cbSize,tsize;
    bool     exitCode = false;
    PROCESS_INFORMATION pi = *(LPPROCESS_INFORMATION)mpara;
    pNtSuspendThread(pi.hThread,&old_protect);
    ctx.ContextFlags = CONTEXT_CONTROL;
    if ( !GetThreadContext(pi.hThread, &ctx) )
    {
    #ifdef _LOGDEBUG
        logmsg("GetThreadContext() in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto clear;
    }
    cbSize = ((LPBYTE)&AfterThreadProc - (LPBYTE)&remote32_asm + 0x0F) & ~0x0F;    /* 函数代码尺寸按16bits对齐 */
    if ( cbSize < 1 || cbSize > 16384) /* 16kb,超过范围说明函数代码被编译器优化掉了 */
    {
    #ifdef _LOGDEBUG
        logmsg("size error , cbSize = %lu\n",cbSize);
    #endif
        goto clear;
    }
    tsize  = cbSize+sizeof(RemotePara);
    /* 在目标进程内分配空间 */
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
    if ( !WriteProcessMemory(pi.hProcess, funcBuff, (LPCVOID)picBuf, cbSize, NULL) )
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
    ctx.Eip = (uintptr_t)funcBuff;
    ctx.ContextFlags = CONTEXT_CONTROL;
    exitCode = SetThreadContext(pi.hThread,&ctx);
    if (!exitCode)
    {
    #ifdef _LOGDEBUG
        logmsg("SetThreadContext(ctx) in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
    }    
clear:
    pNtResumeThread(pi.hThread, NULL);
    Sleep(60);
    if ( picBuf )
    {
        SYS_FREE(picBuf);
    }
    if ( funcBuff )
    {
        tsize = 0;
        pNtFreeVirtualMemory(pi.hProcess,funcBuff,&tsize,MEM_RELEASE);
    }
    return exitCode;
}
#endif

#if defined(_M_X64)
bool inject64(void *mpara,RemotePara* func_param)
{
    LPVOID      m_code = NULL;
    SIZE_T      param_size,func_size,code_size,t_size;
    bool        exitCode = false;
    DWORD       old_protect = 0;
    CONTEXT     ctx;
    uintptr_t   dwOldIP;
    SIZE_T      nNumBytesWritten = 0;

    PROCESS_INFORMATION  pi = *(LPPROCESS_INFORMATION)mpara;
    /* 参数占用大小 */
    param_size = sizeof(RemotePara);
    /* 被调用函数代码尺寸 */
    func_size = ((LPBYTE)&AfterThreadProc - (LPBYTE)&ThreadProc + 0x0F) & ~0x0F;
    /* shellcode占用空间 */
    code_size = sizeof(codeToInject);
    /* 需要在进程内部申请的总空间尺寸 */
    t_size = param_size+func_size+code_size;
    pNtSuspendThread(pi.hThread,&old_protect);
    if ( func_size < 1 || func_size > 16384)
    {
    #ifdef _LOGDEBUG
        logmsg("size error , func_size = %lu\n",func_size);
    #endif
        goto cleanup;
    }
    ctx.ContextFlags = CONTEXT_FULL;
    if(!GetThreadContext(pi.hThread, &ctx))
    {
    #ifdef _LOGDEBUG
        logmsg("GetThreadContext(ctx) in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto cleanup;
    }
    
    /* 在进程内部分配内存 */   
    if ( (m_code = VirtualAllocEx(pi.hProcess, NULL,  t_size, MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE)) == NULL )
    {
    #ifdef _LOGDEBUG
        logmsg("VirtualAllocEx() in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto cleanup;
    }
    /* 写入函数参数 */
    if ( !WriteProcessMemory(pi.hProcess, (BYTE *)m_code+code_size, (PVOID)func_param, param_size, &nNumBytesWritten) )
    {
    #ifdef _LOGDEBUG
        logmsg("WriteProcessMemory(func_param) in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto cleanup;
    }
    /* 写入被调用函数 */
    if ( !WriteProcessMemory(pi.hProcess, (BYTE *)m_code+code_size+param_size, (PVOID)&ThreadProc, func_size, &nNumBytesWritten) )
    {
    #ifdef _LOGDEBUG
        logmsg("WriteProcessMemory(ThreadProc) in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto cleanup;
    }

    /* 替换shellcode跳转地址 */
    dwOldIP = ctx.Rip;
    memcpy(codeToInject + 5, &dwOldIP, sizeof(dwOldIP));
    *(uintptr_t*)((uintptr_t)codeToInject + 45)  = (uintptr_t)m_code+code_size;
    *(uintptr_t*)((uintptr_t)codeToInject + 55)  = (uintptr_t)m_code+code_size+param_size;
    /* 写入shellcode */
    if ( !WriteProcessMemory(pi.hProcess, m_code, (PVOID)codeToInject, code_size, &nNumBytesWritten) )
    {
    #ifdef _LOGDEBUG
        logmsg("WriteProcessMemory(codeToInject) in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto cleanup;
    }
    if (!VirtualProtectEx(pi.hProcess, m_code, code_size, PAGE_EXECUTE_READ, &old_protect))
    {
    #ifdef _LOGDEBUG
        logmsg("VirtualProtectEx() in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
    #endif
        goto cleanup;
    }

    /* 栈指针前移,因为怕覆盖某些代码 */
    ctx.Rsp -= 128;
    /* 确保栈16位右对齐 用于LdrLoadDll调用 */
    ctx.Rsp = ctx.Rsp & ~15;
    ctx.Rsp -= 8;
    ctx.Rip = (uintptr_t)m_code;

    if ( !FlushInstructionCache(pi.hProcess,  m_code, code_size) )
    {
    #ifdef _LOGDEBUG
        logmsg("FlushInstructionCache() in %s return false,error:[%lu]\n", __FUNCTION__, GetLastError());
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
    /* 主线程继续并且清理分配的数据 */
    pNtResumeThread(pi.hThread, NULL);
    Sleep(60);
    if(m_code)
    {
        code_size = 0;
        pNtFreeVirtualMemory(pi.hProcess,m_code,&code_size,MEM_RELEASE);
    }
    return exitCode;
}
#endif

unsigned WINAPI InjectDll(void *mpara)
{
    RemotePara myPara;
    WCHAR      dll_name[VALUE_LEN+1];
    HMODULE    hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return (0);

    /* 初始化远程函数参数 */
    fzero(&myPara, sizeof(RemotePara));
    pNtFreeVirtualMemory = (_NtFreeVirtualMemory)GetProcAddress(hNtdll, "NtFreeVirtualMemory");
    pNtSuspendThread = (_NtResumeThread)GetProcAddress(hNtdll, "NtSuspendThread");
    pNtResumeThread = (_NtResumeThread)GetProcAddress(hNtdll, "NtResumeThread");
    myPara.dwLoadLibraryAddr = (uintptr_t)GetProcAddress(hNtdll, "LdrLoadDll");
    myPara.dwRtlInitUnicodeString = (uintptr_t)GetProcAddress(hNtdll, "RtlInitUnicodeString");
    if ( pNtFreeVirtualMemory && pNtSuspendThread && pNtResumeThread && \
         myPara.dwRtlInitUnicodeString && myPara.dwLoadLibraryAddr && \
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
    return (0);
}
