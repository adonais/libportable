#include "ice_error.h"
#include "inipara.h"
#include <shlwapi.h>
#include <dbghelp.h>

typedef LPTOP_LEVEL_EXCEPTION_FILTER (WINAPI *SetUnhandledExceptionFilterPtr)(
                                      LPTOP_LEVEL_EXCEPTION_FILTER 
                                      lpTopLevelExceptionFilter);
typedef BOOL (WINAPI *MiniDumpWriteDumpPtr)(HANDLE,DWORD,HANDLE,MINIDUMP_TYPE,
              PMINIDUMP_EXCEPTION_INFORMATION,
              PMINIDUMP_USER_STREAM_INFORMATION,
              PMINIDUMP_CALLBACK_INFORMATION);

static  SetUnhandledExceptionFilterPtr sSetUnhandledExceptionFilterStub;
static  SetUnhandledExceptionFilterPtr pSetUnhandledExceptionFilter;
static  MiniDumpWriteDumpPtr sMiniDumpWriteDumpStub;
static  HMODULE m_dbg;

static LONG WINAPI 
ProcessException_ice(PEXCEPTION_POINTERS pExceptionInfo)
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
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    ExInfo.ThreadId = GetCurrentThreadId();
    ExInfo.ExceptionPointers = pExceptionInfo;
    ExInfo.ClientPointers = true;

    /* MiniDumpWriteDump输出dump */
    sMiniDumpWriteDumpStub(GetCurrentProcess(),
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
    return sSetUnhandledExceptionFilterStub(ProcessException_ice);
}

unsigned WINAPI init_exeception(void * pParam)
{
    HMODULE  m_kernel;
    m_dbg = sLoadLibraryExWStub?sLoadLibraryExWStub(L"dbghelp.dll",NULL,0):LoadLibraryExW(L"dbghelp.dll",NULL,0);
    m_kernel =  GetModuleHandleW(L"kernel32.dll");
    if ( m_dbg == NULL || m_kernel == NULL ||  
        (sMiniDumpWriteDumpStub = (MiniDumpWriteDumpPtr)GetProcAddress(m_dbg, "MiniDumpWriteDump")) == NULL)
    {
        return (0);
    }
    pSetUnhandledExceptionFilter = (SetUnhandledExceptionFilterPtr)GetProcAddress(m_kernel, "SetUnhandledExceptionFilter");
    return creator_hook(pSetUnhandledExceptionFilter, HookSetUnhandledExceptionFilter, (LPVOID*)&sSetUnhandledExceptionFilterStub);
}

void WINAPI jmp_end(void)
{
    if ( m_dbg )
    {
        FreeLibrary(m_dbg);
    }
    return;
}
