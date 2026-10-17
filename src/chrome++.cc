#include <windows.h>

#include <psapi.h>
#include <stdio.h>

#include "detours.h"

#include "com_initializer.h"
#include "appid.h"
#include "config.h"
#include "green.h"
#include "hotkey.h"
#include "portable.h"
#include "utils.h"
#include "version.h"
#include "on_tabs.h"

using Startup = int (*)();
static bool should_run_exit_cmd = false;
Startup ExeMain = nullptr;

void ChromePlus() {
  // Shortcut.
  SetAppId();
  
  // Portable hijack patch.
  MakeGreen();
  
  // Process the hotkey.
  GetHotkey();
  
  // ontabs
  threads_on_tabs();
}

void ChromePlusCommand(LPWSTR param) {
  if (!wcsstr(param, L"--portable")) {
    Portable(param);
  } else {
    ChromePlus();
    LaunchCommands(config.GetLaunchOnStartup());
    should_run_exit_cmd = true;
  }
}

int Loader() {
  // Only main interface.
  LPWSTR param = GetCommandLineW();
  // DebugLog(L"param {}", param);
  if (!wcsstr(param, L"-type=")) {
    ChromePlusCommand(param);
  }
  // Return to the main function.
  return ExeMain();
}

void InstallLoader() {
  // Get the address of the original entry point of the main module.
  MODULEINFO mi;
  GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &mi,
                       sizeof(MODULEINFO));
  ExeMain = reinterpret_cast<Startup>(mi.EntryPoint);

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach(reinterpret_cast<LPVOID*>(&ExeMain),
               reinterpret_cast<void*>(Loader));
  auto status = DetourTransactionCommit();
  if (status != NO_ERROR) {
    DebugLog(L"InstallLoader failed: {}", status);
  }
}

__declspec(dllexport) extern "C" void __stdcall portable_edge() {}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID pv) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hModule);
    hInstance = hModule;
    InstallLoader();

  } else if (dwReason == DLL_PROCESS_DETACH) {
    if (::should_run_exit_cmd) {
      LaunchCommands(config.GetLaunchOnExit());
      should_run_exit_cmd = false;
    }
    un_uia();
  }
  return TRUE;
}
