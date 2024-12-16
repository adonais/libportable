#include <shlwapi.h>
#include "inipara.h"
#include "internal_exp.h"
#include "MinHook.h"
#include "internal_dbg.h"

typedef LPTOP_LEVEL_EXCEPTION_FILTER (WINAPI *SetUnhandledExceptionFilterPtr)(\
                                      LPTOP_LEVEL_EXCEPTION_FILTER 
                                      lpTopLevelExceptionFilter);
static  SetUnhandledExceptionFilterPtr SetUnhandledExceptionFilterStub;
static  SetUnhandledExceptionFilterPtr pSetUnhandledExceptionFilterPtr;

static LONG WINAPI 
ProcessException_ice(PEXCEPTION_POINTERS pExceptionInfo)
{

    return EXCEPTION_EXECUTE_HANDLER;
}

static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI
HookSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
    return SetUnhandledExceptionFilterStub(ProcessException_ice);
}

int WINAPI init_exeception(void *param)
{
    int     ret = 0;
    HMODULE m_kernel =  GetModuleHandleW(L"kernel32.dll");
    if ( !m_kernel )
    {
        return ret;
    }
    pSetUnhandledExceptionFilterPtr = (SetUnhandledExceptionFilterPtr)GetProcAddress(\
                                      m_kernel, "SetUnhandledExceptionFilter");
    if ( !pSetUnhandledExceptionFilterPtr )
    {
    #ifdef _LOGDEBUG
        logmsg("GetProcAddress(SetUnhandledExceptionFilter) return false\n", __FUNCTION__);
    #endif
        return ret;
    }
    if (MH_CreateHook(pSetUnhandledExceptionFilterPtr, HookSetUnhandledExceptionFilter, 
       (LPVOID*)&SetUnhandledExceptionFilterStub) == MH_OK )
    {
        if ( MH_EnableHook(pSetUnhandledExceptionFilterPtr) == MH_OK )
        {
            ret = 1;
        }
    }
    return ret;
}

void WINAPI uninit_exeception(void)
{
    if (SetUnhandledExceptionFilterStub)
    {
        MH_DisableHook(pSetUnhandledExceptionFilterPtr);
    }
    return;
}
