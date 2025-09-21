#include "general.h"
#include "win_registry.h"
#include <windows.h>
#include <shlwapi.h>

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
         (StrStrIW(lpSubKey, L"\\Extensions")         || 
         StrStrIW(lpSubKey, L"\\MozillaPlugins")      ||
         StrStrIW(lpSubKey, L"\\QuickTimePlayer.exe") ||
         StrStrIW(lpSubKey, L"\\wmplayer.exe")        ||
         StrStrIW(lpSubKey, L"\\Adobe\\")) )
    {
    #ifdef _LOGDEBUG
        logmsg("lpSubKey[%ls]\n", lpSubKey);
    #endif
        return ERROR_REGISTRY_IO_FAILED;
    }
    return sRegOpenKeyExWStub(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

unsigned WINAPI
init_winreg(void * pParam)
{
    if (e_browser > MOZ_UNKOWN && is_browser())
    {
        HMODULE m_adv = GetModuleHandleW(L"advapi32.dll");
        if ( m_adv && (pRegOpenKeyExW = (RegOpenKeyExPtr)GetProcAddress(m_adv, "RegOpenKeyExW")) != NULL )
        {
            return creator_hook(pRegOpenKeyExW, HookRegOpenKeyExW, (LPVOID*)&sRegOpenKeyExWStub);
        }
    }
    return 0;
}
