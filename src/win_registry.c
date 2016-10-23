#include "inipara.h"
#include "win_registry.h"
#include "MinHook.h"
#include <windows.h>

typedef	LONG (WINAPI *RegOpenKeyExPtr)(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
static  RegOpenKeyExPtr pRegOpenKeyExW, sRegOpenKeyExWStub;

static LONG WINAPI 
HookRegOpenKeyExW(HKEY    hKey,
                  LPCWSTR lpSubKey,
                  DWORD   ulOptions,
                  REGSAM  samDesired,
                  PHKEY   phkResult)
{
    if ( lpSubKey &&
         (stristrW(lpSubKey, L"\\Extensions") || 
         stristrW(lpSubKey, L"\\MozillaPlugins") ||
         stristrW(lpSubKey, L"\\QuickTimePlayer.exe") ||
         stristrW(lpSubKey, L"\\wmplayer.exe") ||
         stristrW(lpSubKey, L"\\Adobe\\")) )
    {
    #ifdef _LOGDEBUG
        logmsg("lpSubKey[%ls]\n", lpSubKey);
    #endif
        return ERROR_REGISTRY_IO_FAILED;
    }
    return sRegOpenKeyExWStub(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

unsigned WINAPI init_winreg(void * pParam)
{
    HMODULE m_adv = GetModuleHandleW(L"advapi32.dll");
    if ( !m_adv )
    {
        return (0);
    }
    if ( (pRegOpenKeyExW = (RegOpenKeyExPtr)GetProcAddress(m_adv, "RegOpenKeyExW")) != NULL && 
         MH_CreateHook(pRegOpenKeyExW, HookRegOpenKeyExW, (LPVOID*)&sRegOpenKeyExWStub) == MH_OK )
    {
        if ( MH_EnableHook(pRegOpenKeyExW) != MH_OK )
        {
        #ifdef _LOGDEBUG
            logmsg("RegOpenKeyExW hook failed!\n");
        #endif
        }
    }
    return (1);
}

void WINAPI winreg_end(void)
{
    if (sRegOpenKeyExWStub)
    {
        MH_DisableHook(pRegOpenKeyExW);
    }
    return;
}
