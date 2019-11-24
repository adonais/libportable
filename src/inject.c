#ifdef DISABLE_SAFE
#error This file should not be compiled!
#endif

#include "inipara.h"
#include "winapis.h"

#ifdef _MSC_VER
#define MOZ_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define MOZ_NOINLINE __attribute__((noinline))
#else
#define MOZ_NOINLINE
#endif

typedef struct _thread_data
{
    WCHAR     strDll[VALUE_LEN+1];
    uintptr_t dwFuncAddr;
    uintptr_t dwRtlInitStr;
} thread_data;

typedef struct _thread_info
{
    LPVOID data_buff;
    LPVOID func_buff;
    LPVOID code_buff;
    size_t data_size;
    size_t func_size;
    size_t code_size;
} thread_info;

#if defined(_MSC_VER) && !defined(__clang__)
#pragma check_stack (off)
#endif

#if defined(_WIN64)
unsigned char shell_code[] =
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
#elif defined(_M_IX86) || (defined __i386__)
unsigned char shell_code[] =
{
    0x68, 0xef, 0xbe, 0xad, 0xde,           // push 0xDEADBEEF
    0x9c,                                   // pushfd
    0x60,                                   // pushad
    0x68, 0xef, 0xbe, 0xad, 0xde,           // push 0xDEADBEEF
    0xb8, 0xef, 0xbe, 0xad, 0xde,           // mov eax, 0xDEADBEEF
    0xff, 0xd0,                             // call eax
    0x61,                                   // popad
    0x9d,                                   // popfd
    0xc3                                    // ret
};
#else
#error Unsupported compiler.
#endif
  
static void WINAPI 
ThreadProc(thread_data* pt)
{
    typedef NTSTATUS (NTAPI *LdrLoadDllPtr)
    (
        PWCHAR PathToFile,
        PULONG Flags,
        PUNICODE_STRING ModuleFileName,
        PHANDLE ModuleHandle
    );
    typedef VOID (WINAPI *RtlInitStrPtr)
    (
        PUNICODE_STRING DestinationString,
        PCWSTR SourceString
    );
    UNICODE_STRING usDllName;
    HANDLE         DllHandle;
    LdrLoadDllPtr  pfnLdrLoadDll = (LdrLoadDllPtr)pt->dwFuncAddr;
    RtlInitStrPtr  pfnRtlInitStr = (RtlInitStrPtr)pt->dwRtlInitStr;
    pfnRtlInitStr(&usDllName, pt->strDll);
    pfnLdrLoadDll(NULL, NULL, &usDllName, &DllHandle);
}

MOZ_NOINLINE 
static void WINAPI
AfterThreadProc (void)
{
    __nop();
    __nop();
    __nop();
    __nop();
}

#if defined(_MSC_VER) && !defined(__clang__)
#pragma check_stack
#endif

static bool 
init_data(thread_data *pt)
{
    bool    res = false;
    WCHAR   dll_name[VALUE_LEN+1];
    HMODULE nt_handle = GetModuleHandleW(L"ntdll.dll");
    if (NULL == nt_handle)
    {
        return res;
    }    
    fzero(pt, sizeof(thread_data));
    pt->dwFuncAddr   = (uintptr_t)GetProcAddress(nt_handle, "LdrLoadDll");
    pt->dwRtlInitStr = (uintptr_t)GetProcAddress(nt_handle, "RtlInitUnicodeString");
    if (pt->dwFuncAddr && pt->dwRtlInitStr && GetModuleFileNameW(dll_module,dll_name,VALUE_LEN) >0)
    {
        wcsncpy(pt->strDll,dll_name,VALUE_LEN);
        res = true;
    }
    return res;
}

static bool
write_memory(HANDLE handle, LPVOID base, LPCVOID buffer, size_t size)
{
    DWORD flags = 0;
    do
    {
        if (!WriteProcessMemory(handle, base, buffer, size, NULL))
        {
        #ifdef _LOGDEBUG
            logmsg("%s_WriteProcessMemory error:[%lu]\n", __FUNCTION__, GetLastError());
        #endif
            break;
        }
        if (!VirtualProtectEx(handle, base, size, PAGE_EXECUTE_READ, &flags))
        {
        #ifdef _LOGDEBUG
            logmsg("%s_VirtualProtectExy error:[%lu]\n", __FUNCTION__, GetLastError());
        #endif
            break;
        }
    }while (0);
    return (flags != 0);
}

static void
install_jmp(CONTEXT *pc, thread_info *pinfo)
{
#if defined(_WIN64)
    /* 替换shellcode跳转地址 */
    uintptr_t old_ip = pc->Rip;
    memcpy(shell_code + 5, &old_ip, sizeof(old_ip));
    *(uintptr_t*)((uintptr_t)shell_code + 45)  = (uintptr_t)pinfo->code_buff+pinfo->code_size;
    *(uintptr_t*)((uintptr_t)shell_code + 55)  = (uintptr_t)pinfo->code_buff+pinfo->code_size+pinfo->data_size;
    /* 栈指针下移,因为怕覆盖某些代码,被调用函数恢复栈平衡 */
    pc->Rsp -= 128;
    /* 确保栈16位右对齐 用于LdrLoadDll调用 */
    pc->Rsp = pc->Rsp & ~15;
    pc->Rsp -= 8;
    pc->Rip = (uintptr_t)pinfo->code_buff;
#else
    *(uintptr_t*)((uintptr_t)shell_code + 1)  = pc->Eip;
    *(uintptr_t*)((uintptr_t)shell_code + 8)  = (uintptr_t)pinfo->code_buff+pinfo->code_size;
    *(uintptr_t*)((uintptr_t)shell_code + 13) = (uintptr_t)pinfo->code_buff+pinfo->code_size+pinfo->data_size;
    pc->Eip = (uintptr_t)pinfo->code_buff;
#endif
}

unsigned WINAPI 
InjectDll(void *process)
{
    thread_data dt;
    CONTEXT     ctx;
    thread_info df = {0};
    PROCESS_INFORMATION pi = *(LPPROCESS_INFORMATION)process;
    size_t      t_size = 0;

    df.data_size = sizeof(thread_data);
    df.code_size = sizeof(shell_code);
    df.func_size = ((LPBYTE)&AfterThreadProc - (LPBYTE)&ThreadProc + 0x0F) & ~0x0F;
    /* 有可能会被编译器优化掉 */
    if (df.func_size < 1 || df.func_size > 16384)
    {
    #ifdef _LOGDEBUG
        logmsg("size error , func_size = %lu\n",df.func_size);
    #endif
        /* 我们并不需要获取函数精确的尺寸,不小于函数体就行 */
        df.func_size = VALUE_LEN;
    }    
    t_size = df.data_size + df.code_size + df.func_size;
    do
    {
        if (SuspendThread(pi.hThread) == (uint32_t)-1)
        {
        #ifdef _LOGDEBUG
            logmsg("SuspendThread error:[%lu]\n", GetLastError());
        #endif
            break;
        }
        ctx.ContextFlags = CONTEXT_CONTROL;
        if(!GetThreadContext(pi.hThread, &ctx))
        {
        #ifdef _LOGDEBUG
            logmsg("GetThreadContext(ctx) error:[%lu]\n", GetLastError());
        #endif
            break;
        }
        if (!init_data(&dt))
        {
        #ifdef _LOGDEBUG
            logmsg("init_data false\n");
        #endif  
            break;          
        }
        if ((df.code_buff = VirtualAllocEx(pi.hProcess, 0, t_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)) == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("VirtualAllocEx error:[%lu]\n", GetLastError());
        #endif
            break;
        }
        if (!write_memory(pi.hProcess, (uint8_t *)df.code_buff+df.code_size, &dt, df.data_size))
        {
            break;
        } 
        if (!write_memory(pi.hProcess, (uint8_t *)df.code_buff+df.code_size+df.data_size, ThreadProc, df.func_size))
        {
            break;
        }
        install_jmp(&ctx, &df);
        if (!write_memory(pi.hProcess, df.code_buff, shell_code, df.code_size))
        {
            break;
        }
        if (!SetThreadContext(pi.hThread, &ctx))
        {
        #ifdef _LOGDEBUG
            logmsg("SetThreadContext(ctx) error:[%lu]\n", GetLastError());
        #endif
        }
    }while (0);
    if(df.code_buff && t_size)
    {
        VirtualFreeEx(pi.hProcess, df.code_buff, t_size, MEM_RELEASE);
    }
    return ResumeThread(pi.hThread);
}