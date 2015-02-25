#define ERROR_EXTERN

#include <shlwapi.h>
#include "inipara.h"
#include "ice_error.h"
#include "MinHook.h"
#include "internal_dbg.h"

typedef LPTOP_LEVEL_EXCEPTION_FILTER (WINAPI *_NtSetUnhandledExceptionFilter)(\
                                      LPTOP_LEVEL_EXCEPTION_FILTER 
                                      lpTopLevelExceptionFilter);
static  _NtSetUnhandledExceptionFilter OrgiSetUnhandledExceptionFilter;
static  _NtSetUnhandledExceptionFilter TrueSetUnhandledExceptionFilter;
static  _NtMiniDumpWriteDump TrueMiniDumpWriteDump;
static  HMODULE m_dbg;

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
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    ExInfo.ThreadId = GetCurrentThreadId();
    ExInfo.ExceptionPointers = pExceptionInfo;
    ExInfo.ClientPointers = true;

    /* MiniDumpWriteDump输出dump */
    TrueMiniDumpWriteDump(GetCurrentProcess(),
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
    return OrgiSetUnhandledExceptionFilter(ProcessException_ice);
}

unsigned WINAPI init_exeception(void * pParam)
{
    unsigned ret = 0;
    HMODULE  m_dbg, m_kernel;
    m_dbg = OrgiLoadLibraryExW?OrgiLoadLibraryExW(L"dbghelp.dll",NULL,0):\
            LoadLibraryExW(L"dbghelp.dll",NULL,0);
    m_kernel =  GetModuleHandleW(L"kernel32.dll");
    if ( !(m_dbg&&m_kernel) ) return ret;
    TrueMiniDumpWriteDump           = (_NtMiniDumpWriteDump)GetProcAddress(\
                                      m_dbg, "MiniDumpWriteDump");
    if ( !TrueMiniDumpWriteDump )
    {
    #ifdef _LOGDEBUG
        logmsg("GetProcAddress(MiniDumpWriteDump) return false\n", __FUNCTION__);
    #endif
        return ret;
    }
    TrueSetUnhandledExceptionFilter = (_NtSetUnhandledExceptionFilter)GetProcAddress(\
                                      m_kernel, "SetUnhandledExceptionFilter");
    if ( !TrueSetUnhandledExceptionFilter )
    {
    #ifdef _LOGDEBUG
        logmsg("GetProcAddress(SetUnhandledExceptionFilter) return false\n", __FUNCTION__);
    #endif
        return ret;
    }
    if (MH_CreateHook(TrueSetUnhandledExceptionFilter, HookSetUnhandledExceptionFilter, 
       (LPVOID*)&OrgiSetUnhandledExceptionFilter) == MH_OK )
    {
        if ( MH_EnableHook(TrueSetUnhandledExceptionFilter) == MH_OK )
        {
            ret = 1;
        }
    }
    return ret;
}

void WINAPI jmp_end(void)
{
    if (OrgiSetUnhandledExceptionFilter)
    {
        MH_DisableHook(TrueSetUnhandledExceptionFilter);
    }
    if ( m_dbg )
    {
        FreeLibrary(m_dbg);
    }
    return;
}
