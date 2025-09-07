#include "ice_error.h"
#include "general.h"
#include <dbghelp.h>

typedef LPTOP_LEVEL_EXCEPTION_FILTER (WINAPI *SetUnhandledExceptionFilterPtr)(
                                      LPTOP_LEVEL_EXCEPTION_FILTER
                                      lpTopLevelExceptionFilter);

static  SetUnhandledExceptionFilterPtr sSetUnhandledExceptionFilterStub;
static  SetUnhandledExceptionFilterPtr pSetUnhandledExceptionFilter;

static LONG WINAPI
ProcessException_ice(PEXCEPTION_POINTERS pExceptionInfo)
{
    return EXCEPTION_EXECUTE_HANDLER;
}

static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI
HookSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
    return sSetUnhandledExceptionFilterStub(ProcessException_ice);
}

unsigned WINAPI init_exeception(void * pParam)
{
    if (e_browser == MOZ_ICEWEASEL)
    {
        HMODULE m_kernel =  GetModuleHandleW(L"kernel32.dll");
        pSetUnhandledExceptionFilter = m_kernel ? (SetUnhandledExceptionFilterPtr)GetProcAddress(m_kernel, "SetUnhandledExceptionFilter") : NULL;
        if (pSetUnhandledExceptionFilter != NULL)
        {
            creator_hook(pSetUnhandledExceptionFilter, HookSetUnhandledExceptionFilter, (LPVOID*)&sSetUnhandledExceptionFilterStub);
        }
    }
    return 0;
}
