//Copyright (c) 2007-2008, Marton Anka
//
//Permission is hereby granted, free of charge, to any person obtaining a 
//copy of this software and associated documentation files (the "Software"), 
//to deal in the Software without restriction, including without limitation 
//the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//and/or sell copies of the Software, and to permit persons to whom the 
//Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included 
//in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
//OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
//THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
//FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
//IN THE SOFTWARE.

#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#ifndef _MSC_VER
#include <winternl.h>
#include <ntstatus.h>
#endif
#include <tchar.h>
#include <stdio.h>
#ifndef _MSC_VER
#include <inttypes.h>
#endif
#include "mhook.h"

#if defined(_MSC_VER)
#define __C89_NAMELESS
#define STATUS_OPEN_FAILED 2

#ifndef _NTSTATUS_PSDK
#define _NTSTATUS_PSDK
  typedef LONG NTSTATUS;
#endif

#ifndef __UNICODE_STRING_DEFINED
#define __UNICODE_STRING_DEFINED
  typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
  } UNICODE_STRING;
  typedef UNICODE_STRING *PUNICODE_STRING;
#endif

#ifndef __OBJECT_ATTRIBUTES_DEFINED
#define __OBJECT_ATTRIBUTES_DEFINED
  typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
#ifdef _WIN64
    ULONG pad1;
#endif
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
#ifdef _WIN64
    ULONG pad2;
#endif
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
  } OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;
#endif

typedef struct _CLIENT_ID {
  HANDLE UniqueProcess;
  HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

#endif

//=========================================================================
// Define _NtOpenProcess so we can dynamically bind to the function
//

typedef NTSTATUS (NTAPI *_NtOpenProcess) (PHANDLE ProcessHandle,
		ACCESS_MASK DesiredAccess,	
		POBJECT_ATTRIBUTES ObjectAttributes,
		PCLIENT_ID ClientId); 

_NtOpenProcess TrueNtOpenProcess ;
//=========================================================================
// Get the current (original) address to the functions to be hooked
//

//=========================================================================
// This is the function that will replace NtOpenProcess once the hook 
// is in place
//
NTSTATUS WINAPI HookNtOpenProcess(OUT PHANDLE ProcessHandle, 
							     IN ACCESS_MASK AccessMask, 
							     IN POBJECT_ATTRIBUTES ObjectAttributes, 
							     IN PCLIENT_ID ClientId)
{
	UINT_PTR t_pid = 2624;
	printf("***** Call to open process %lu\n", (UINT_PTR)(ClientId->UniqueProcess) );
	if (  t_pid == (UINT_PTR)ClientId->UniqueProcess )
	{
		//MessageBoxW(NULL,L"你想干什么   ",L"警告: ",MB_OK);
		printf("is hooked,return 0\n");
		ProcessHandle = NULL;
		return (STATUS_OPEN_FAILED);
	}
	
	return TrueNtOpenProcess(ProcessHandle, AccessMask, 
		ObjectAttributes, ClientId);
}

//=========================================================================
// This is where the work gets done.
//
int _tmain(int argc, WCHAR* argv[])
{
	HANDLE hProc = NULL;
	TrueNtOpenProcess = (_NtOpenProcess)GetProcAddress(GetModuleHandleW(L"ntdll"), "NtOpenProcess");
	// Set the hook
	if (Mhook_SetHook((PVOID *)&TrueNtOpenProcess, HookNtOpenProcess)) {
		// Now call OpenProcess and observe NtOpenProcess being redirected
		// under the hood.
		hProc = OpenProcess(PROCESS_ALL_ACCESS, 
			FALSE, GetCurrentProcessId());
		if (hProc) {
			printf("Successfully opened self: %p\n", hProc);
			CloseHandle(hProc);
		} else {
			printf("Could not open self: %lu\n", GetLastError());
		}
	}
	else
	{
		printf("Mhook_SetHook false\n");
	}
	// Call OpenProces again - this time there won't be a redirection as
	// the hook has bee removed.
	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 2624);
	if (hProc) {
		printf("Successfully opened self: %p\n", hProc);
		CloseHandle(hProc);
	} else {
		printf("Could not open self: %lu\n", GetLastError());
	}
	getchar();
	// Remove the hook
	Mhook_Unhook((PVOID*)&TrueNtOpenProcess);
	return 0;
}

