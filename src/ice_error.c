#include <shlwapi.h>
#include "inipara.h"
#include "ice_error.h"
#include "MinHook.h"
#include "internal_dbg.h"

typedef LPTOP_LEVEL_EXCEPTION_FILTER (WINAPI *SetUnhandledExceptionFilterPtr)(\
                                      LPTOP_LEVEL_EXCEPTION_FILTER 
                                      lpTopLevelExceptionFilter);
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
    unsigned ret = 0;
    HMODULE  m_kernel;
    m_dbg = sLoadLibraryExWStub?sLoadLibraryExWStub(L"dbghelp.dll",NULL,0):\
            LoadLibraryExW(L"dbghelp.dll",NULL,0);
    m_kernel =  GetModuleHandleW(L"kernel32.dll");
    if ( !(m_dbg && m_kernel) ) return ret;
    sMiniDumpWriteDumpStub           = (MiniDumpWriteDumpPtr)GetProcAddress(\
                                      m_dbg, "MiniDumpWriteDump");
    if ( !sMiniDumpWriteDumpStub )
    {
    #ifdef _LOGDEBUG
        logmsg("GetProcAddress(MiniDumpWriteDump) return false\n", __FUNCTION__);
    #endif
        return ret;
    }
    pSetUnhandledExceptionFilter = (SetUnhandledExceptionFilterPtr)GetProcAddress(\
                                      m_kernel, "SetUnhandledExceptionFilter");
    if ( !pSetUnhandledExceptionFilter )
    {
    #ifdef _LOGDEBUG
        logmsg("GetProcAddress(SetUnhandledExceptionFilter) return false\n", __FUNCTION__);
    #endif
        return ret;
    }
    if (MH_CreateHook(pSetUnhandledExceptionFilter, HookSetUnhandledExceptionFilter, 
       (LPVOID*)&sSetUnhandledExceptionFilterStub) == MH_OK )
    {
        if ( MH_EnableHook(pSetUnhandledExceptionFilter) == MH_OK )
        {
            ret = 1;
        }
    }
    return ret;
}

void WINAPI jmp_end(void)
{
    if (sSetUnhandledExceptionFilterStub)
    {
        MH_DisableHook(pSetUnhandledExceptionFilter);
    }
    if ( m_dbg )
    {
        FreeLibrary(m_dbg);
    }
    return;
}
