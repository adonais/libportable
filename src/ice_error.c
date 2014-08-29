#define ERROR_EXTERN

#include "ice_error.h"
#include <shlwapi.h>
#include <dbghelp.h>
#include "mhook-lib/mhook.h"

typedef LPTOP_LEVEL_EXCEPTION_FILTER (WINAPI *_NtSetUnhandledExceptionFilter)
(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);
static _NtSetUnhandledExceptionFilter TrueSetUnhandledExceptionFilter = NULL;

BOOL WINAPI MiniDumpWriteDump_func( HANDLE hProcess,DWORD ProcessId,HANDLE hFile,
                                    MINIDUMP_TYPE DumpType,
                                    PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                    PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                    PMINIDUMP_CALLBACK_INFORMATION CallbackParam )
{
    BOOL    ret = FALSE;
    typedef BOOL (WINAPI *_NtMiniDumpWriteDump)(HANDLE,DWORD,HANDLE,MINIDUMP_TYPE,PMINIDUMP_EXCEPTION_INFORMATION,
            PMINIDUMP_USER_STREAM_INFORMATION,PMINIDUMP_CALLBACK_INFORMATION);
    static _NtMiniDumpWriteDump TrueMiniDumpWriteDump = NULL;
    HMODULE hdbg = LoadLibraryW(L"dbghelp.dll");
    if (hdbg)
    {
        TrueMiniDumpWriteDump  = (_NtMiniDumpWriteDump)GetProcAddress(hdbg, "MiniDumpWriteDump");
        if (TrueMiniDumpWriteDump)
        {
            ret = TrueMiniDumpWriteDump(hProcess,ProcessId,hFile,DumpType,ExceptionParam,
                                        UserStreamParam,CallbackParam);
            FreeLibrary(hdbg);
        }
    }
    return ret;
}

LONG WINAPI ProcessException_ice(PEXCEPTION_POINTERS pExceptionInfo)
{
    /* 异常信息结构体 */
    MINIDUMP_EXCEPTION_INFORMATION  ExInfo;
    /* dump生成目录 */
    wchar_t appdir[MAX_PATH+1] = {0};
    HANDLE hFile = NULL;
    if ( !(GetEnvironmentVariableW(L"APPDATA",appdir,MAX_PATH) > 0) )
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    PathAppendW(appdir,L"Iceweasel.dmp");
    /* 创建文件句柄 */
    hFile = CreateFileW(appdir,
                        GENERIC_WRITE,
                        FILE_SHARE_WRITE,
                        NULL,
                        TRUNCATE_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
    if (INVALID_HANDLE_VALUE == hFile && ERROR_FILE_NOT_FOUND == GetLastError())
    {
        hFile = CreateFileW(appdir,
                            GENERIC_WRITE,
                            FILE_SHARE_WRITE,
                            NULL,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
    }
    if (INVALID_HANDLE_VALUE == hFile)
        return EXCEPTION_CONTINUE_SEARCH;
    ExInfo.ThreadId = GetCurrentThreadId();
    ExInfo.ExceptionPointers = pExceptionInfo;
    ExInfo.ClientPointers = TRUE;

    /* MiniDumpWriteDump输出dump */
    MiniDumpWriteDump_func(GetCurrentProcess(),
                           GetCurrentProcessId(),
                           hFile,
                           MiniDumpNormal,
                           &ExInfo,
                           NULL,
                           NULL );

    CloseHandle(hFile);
    return EXCEPTION_EXECUTE_HANDLER;
}

static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI
HookSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
    return TrueSetUnhandledExceptionFilter(ProcessException_ice);
}

unsigned WINAPI init_exeception(void * pParam)
{
    TrueSetUnhandledExceptionFilter = (_NtSetUnhandledExceptionFilter)GetProcAddress
                                      (GetModuleHandleW(L"kernel32.dll"), "SetUnhandledExceptionFilter");
    if (TrueSetUnhandledExceptionFilter)
    {
        Mhook_SetHook((PVOID*)&TrueSetUnhandledExceptionFilter, (PVOID)HookSetUnhandledExceptionFilter);
    }
    return (1);
}

void jmp_end(void)
{
    if (TrueSetUnhandledExceptionFilter)
    {
        Mhook_Unhook((PVOID*)&TrueSetUnhandledExceptionFilter);
    }
    return;
}
