/*
 *  MinHook - The Minimalistic API Hooking Library for x64/x86
 *  Copyright (C) 2009-2017 Tsuda Kageyu.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "MinHook.h"
#include "buffer.h"
#include "trampoline.h"
#include <stdbool.h>
#include <limits.h>
#include <intrin.h>
#include <tlhelp32.h>
#include <processsnapshot.h>

// Initial capacity of the HOOK_ENTRY buffer.
#define INITIAL_HOOK_CAPACITY   32

// Initial capacity of the thread IDs buffer.
#define INITIAL_THREAD_CAPACITY 128

// Special hook position values.
#define INVALID_HOOK_POS UINT_MAX
#define ALL_HOOKS_POS    UINT_MAX

// Freeze() action argument defines.
#define ACTION_DISABLE      0
#define ACTION_ENABLE       1
#define ACTION_APPLY_QUEUED 2

// Hook information.
typedef struct _HOOK_ENTRY
{
    LPVOID pTarget;             // Address of the target function.
    LPVOID pDetour;             // Address of the detour or relay function.
    LPVOID pTrampoline;         // Address of the trampoline function.
    UINT8  backup[8];           // Original prologue of the target function.

    UINT8   patchAbove  : 1;     // Uses the hot patch area.
    UINT8   isEnabled   : 1;     // Enabled.
    UINT8   queueEnable : 1;     // Queued for enabling/disabling when != isEnabled.

    UINT   nIP : 4;             // Count of the instruction boundaries.
    UINT8  oldIPs[8];           // Instruction boundaries of the target function.
    UINT8  newIPs[8];           // Instruction boundaries of the trampoline function.
} HOOK_ENTRY, *PHOOK_ENTRY;

// Hook entries.
typedef struct _global_hooks
{
    PHOOK_ENTRY pItems;     // Data heap
    UINT        capacity;   // Size of allocated data heap, items
    UINT        size;       // Actual number of data items
} global_hooks;

//-------------------------------------------------------------------------
// Windows 8.x functions:
//-------------------------------------------------------------------------

typedef DWORD (*ptr_PssCaptureSnapshot)(HANDLE ProcessHandle, PSS_CAPTURE_FLAGS CaptureFlags, DWORD ThreadContextFlags, HPSS *SnapshotHandle);
typedef DWORD (*ptr_PssWalkMarkerCreate)(PSS_ALLOCATOR const *Allocator, HPSSWALK *WalkMarkerHandle);
typedef DWORD (*ptr_PssWalkSnapshot)(HPSS SnapshotHandle, PSS_WALK_INFORMATION_CLASS InformationClass, HPSSWALK WalkMarkerHandle, void *Buffer, DWORD BufferLength);
typedef DWORD (*ptr_PssWalkMarkerFree)(HPSSWALK WalkMarkerHandle);
typedef DWORD (*ptr_PssFreeSnapshot)(HANDLE ProcessHandle, HPSS SnapshotHandle);

static ptr_PssCaptureSnapshot fn_PssCaptureSnapshot;
static ptr_PssWalkMarkerCreate fn_PssWalkMarkerCreate;
static ptr_PssWalkSnapshot fn_PssWalkSnapshot;
static ptr_PssWalkMarkerFree fn_PssWalkMarkerFree;
static ptr_PssFreeSnapshot fn_PssFreeSnapshot;

//-------------------------------------------------------------------------
// Global Variables:
//-------------------------------------------------------------------------
// Spin lock flag for EnterSpinLock()/LeaveSpinLock().
static volatile LONG g_isLocked = 0;

// Private heap handle. If not NULL, this library is initialized.
static volatile HANDLE g_hHeap = NULL;

static global_hooks g_hooks;

//-------------------------------------------------------------------------
// Returns INVALID_HOOK_POS if not found.
static UINT FindHookEntry(LPVOID pTarget)
{
    UINT i;
    for (i = 0; i < g_hooks.size; ++i)
    {
        if ((ULONG_PTR)pTarget == (ULONG_PTR)g_hooks.pItems[i].pTarget)
            return i;
    }
    return INVALID_HOOK_POS;
}

//-------------------------------------------------------------------------
static PHOOK_ENTRY AddHookEntry()
{
    if (g_hooks.pItems == NULL)
    {
        g_hooks.capacity = INITIAL_HOOK_CAPACITY;
        g_hooks.pItems = (PHOOK_ENTRY)HeapAlloc(
            g_hHeap, 0, g_hooks.capacity * sizeof(HOOK_ENTRY));
        if (g_hooks.pItems == NULL)
            return NULL;
    }
    else if (g_hooks.size >= g_hooks.capacity)
    {
        PHOOK_ENTRY p = (PHOOK_ENTRY)HeapReAlloc(
            g_hHeap, 0, g_hooks.pItems, (g_hooks.capacity * 2) * sizeof(HOOK_ENTRY));
        if (p == NULL)
            return NULL;

        g_hooks.capacity *= 2;
        g_hooks.pItems = p;
    }
    return &g_hooks.pItems[g_hooks.size++];
}

//-------------------------------------------------------------------------
static VOID DeleteHookEntry(UINT pos)
{
    if (pos < g_hooks.size - 1)
        g_hooks.pItems[pos] = g_hooks.pItems[g_hooks.size - 1];

    g_hooks.size--;

    if (g_hooks.capacity / 2 >= INITIAL_HOOK_CAPACITY && g_hooks.capacity / 2 >= g_hooks.size)
    {
        PHOOK_ENTRY p = (PHOOK_ENTRY)HeapReAlloc(
            g_hHeap, 0, g_hooks.pItems, (g_hooks.capacity / 2) * sizeof(HOOK_ENTRY));
        if (p == NULL)
            return;

        g_hooks.capacity /= 2;
        g_hooks.pItems = p;
    }
}

//-------------------------------------------------------------------------
static DWORD_PTR FindOldIP(PHOOK_ENTRY pHook, DWORD_PTR ip)
{
    UINT i;

    if (pHook->patchAbove && ip == ((DWORD_PTR)pHook->pTarget - sizeof(JMP_REL)))
        return (DWORD_PTR)pHook->pTarget;

    for (i = 0; i < pHook->nIP; ++i)
    {
        if (ip == ((DWORD_PTR)pHook->pTrampoline + pHook->newIPs[i]))
            return (DWORD_PTR)pHook->pTarget + pHook->oldIPs[i];
    }

#if defined(_M_X64) || defined(__x86_64__)
    // Check relay function.
    if (ip == (DWORD_PTR)pHook->pDetour)
        return (DWORD_PTR)pHook->pTarget;
#endif
    return 0;
}

//-------------------------------------------------------------------------
static DWORD_PTR FindNewIP(PHOOK_ENTRY pHook, DWORD_PTR ip)
{
    UINT i;
    for (i = 0; i < pHook->nIP; ++i)
    {
        if (ip == ((DWORD_PTR)pHook->pTarget + pHook->oldIPs[i]))
            return (DWORD_PTR)pHook->pTrampoline + pHook->newIPs[i];
    }
    return 0;
}

//-------------------------------------------------------------------------
static VOID ProcessThreadIPs(HANDLE hThread, UINT pos, UINT action)
{
    // If the thread suspended in the overwritten area,
    // move IP to the proper address.

    CONTEXT c;
#if defined(_M_X64) || defined(__x86_64__)
    DWORD64 *pIP = &c.Rip;
#else
    DWORD   *pIP = &c.Eip;
#endif
    UINT count;

    c.ContextFlags = CONTEXT_CONTROL;
    if (!GetThreadContext(hThread, &c))
        return;

    if (pos == ALL_HOOKS_POS)
    {
        pos = 0;
        count = g_hooks.size;
    }
    else
    {
        count = pos + 1;
    }

    for (; pos < count; ++pos)
    {
        PHOOK_ENTRY pHook = &g_hooks.pItems[pos];
        bool        enable = true;
        DWORD_PTR   ip;

        switch (action)
        {
        case ACTION_DISABLE:
            enable = false;
            break;

        case ACTION_ENABLE:
            enable = true;
            break;

        default: // ACTION_APPLY_QUEUED
            enable = pHook->queueEnable;
            break;
        }
        if (pHook->isEnabled == enable)
            continue;

        if (enable)
            ip = FindNewIP(pHook, *pIP);
        else
            ip = FindOldIP(pHook, *pIP);

        if (ip != 0)
        {
            *pIP = ip;
            SetThreadContext(hThread, &c);
        }
    }
}

//-------------------------------------------------------------------------
static bool EnumerateThreads(PFROZEN_THREADS pThreads)
{
    bool succeeded = false;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te;
        te.dwSize = sizeof(THREADENTRY32);
        if (Thread32First(hSnapshot, &te))
        {
            succeeded = true;
            do
            {
                if (te.dwSize >= (FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(DWORD))
                    && te.th32OwnerProcessID == GetCurrentProcessId()
                    && te.th32ThreadID != GetCurrentThreadId())
                {
                    if (pThreads->pItems == NULL)
                    {
                        pThreads->capacity = INITIAL_THREAD_CAPACITY;
                        pThreads->pItems
                            = (LPDWORD)HeapAlloc(g_hHeap, 0, pThreads->capacity * sizeof(DWORD));
                        if (pThreads->pItems == NULL)
                        {
                            succeeded = false;
                            break;
                        }
                    }
                    else if (pThreads->size >= pThreads->capacity)
                    {
                        LPDWORD p;
                        pThreads->capacity *= 2;
                        p = (LPDWORD)HeapReAlloc(
                            g_hHeap, 0, pThreads->pItems, pThreads->capacity * sizeof(DWORD));
                        if (p == NULL)
                        {
                            succeeded = false;
                            break;
                        }
                        pThreads->pItems = p;
                    }
                    pThreads->pItems[pThreads->size++] = te.th32ThreadID;
                }
                te.dwSize = sizeof(THREADENTRY32);
            } while (Thread32Next(hSnapshot, &te));

            if (succeeded && GetLastError() != ERROR_NO_MORE_FILES)
            {
                succeeded = false;
            }
            if (!succeeded && pThreads->pItems != NULL)
            {
                HeapFree(g_hHeap, 0, pThreads->pItems);
                pThreads->pItems = NULL;
            }
        }
        CloseHandle(hSnapshot);
    }
    return succeeded;
}

static bool SnapThreads(PFROZEN_THREADS pThreads)
{
    bool succeeded = false;
    HPSS hsnapshot = NULL;
    HANDLE hprocess = NULL;
#if (defined _M_X64) || (defined __x86_64__)
    if (!fn_PssCaptureSnapshot)
    {
        HMODULE hssapi = GetModuleHandleW(L"kernel32.dll");
        fn_PssCaptureSnapshot = hssapi ? (ptr_PssCaptureSnapshot)GetProcAddress(hssapi, "PssCaptureSnapshot") : NULL;
        if (fn_PssCaptureSnapshot)
        {
            fn_PssWalkMarkerCreate = (ptr_PssWalkMarkerCreate)GetProcAddress(hssapi, "PssWalkMarkerCreate");
            fn_PssWalkSnapshot = (ptr_PssWalkSnapshot)GetProcAddress(hssapi, "PssWalkSnapshot");
            fn_PssWalkMarkerFree = (ptr_PssWalkMarkerFree)GetProcAddress(hssapi, "PssWalkMarkerFree");
            fn_PssFreeSnapshot = (ptr_PssFreeSnapshot)GetProcAddress(hssapi, "PssFreeSnapshot");
        }
    }
#endif
    if (!fn_PssCaptureSnapshot || !fn_PssWalkMarkerCreate || !fn_PssWalkSnapshot || !fn_PssWalkMarkerFree || !fn_PssFreeSnapshot)
    {
        return EnumerateThreads(pThreads);
    }
    if ((hprocess = GetCurrentProcess()) != NULL && fn_PssCaptureSnapshot(hprocess, PSS_CAPTURE_THREADS, 0, &hsnapshot) == ERROR_SUCCESS)
    {
        HPSSWALK hwalk = NULL;
        if (ERROR_SUCCESS == fn_PssWalkMarkerCreate(NULL, &hwalk))
        {
            PSS_THREAD_ENTRY te;
            DWORD res = ERROR_SUCCESS;
            succeeded = true;
            while ((res = fn_PssWalkSnapshot(hsnapshot, PSS_WALK_THREADS, hwalk, &te, sizeof(te))) == ERROR_SUCCESS)
            {
                if (te.ProcessId == GetCurrentProcessId() && te.ThreadId != GetCurrentThreadId())
                {
                    if (pThreads->pItems == NULL)
                    {
                        pThreads->capacity = INITIAL_THREAD_CAPACITY;
                        pThreads->pItems = (LPDWORD)HeapAlloc(g_hHeap, 0, pThreads->capacity * sizeof(DWORD));
                        if (pThreads->pItems == NULL)
                        {
                            succeeded = false;
                            break;
                        }
                    }
                    else if (pThreads->size >= pThreads->capacity)
                    {
                        LPDWORD p;
                        pThreads->capacity *= 2;
                        p = (LPDWORD)HeapReAlloc(g_hHeap, 0, pThreads->pItems, pThreads->capacity * sizeof(DWORD));
                        if (p == NULL)
                        {
                            succeeded = false;
                            break;
                        }
                        pThreads->pItems = p;
                    }
                    pThreads->pItems[pThreads->size++] = te.ThreadId;
                }
            }
            if (succeeded && res != ERROR_NO_MORE_ITEMS)
            {
                succeeded = false;
            }
            if (!succeeded && pThreads->pItems != NULL)
            {
                HeapFree(g_hHeap, 0, pThreads->pItems);
                pThreads->pItems = NULL;
            }
            fn_PssWalkMarkerFree(hwalk);
        }
        fn_PssFreeSnapshot(hprocess, hsnapshot);
    }
    return succeeded;
}

//-------------------------------------------------------------------------
static MH_STATUS Freeze(PFROZEN_THREADS pThreads, UINT pos, UINT action)
{
    MH_STATUS status = MH_OK;
    pThreads->pItems   = NULL;
    pThreads->capacity = 0;
    pThreads->size     = 0;
    if (!SnapThreads(pThreads))
    {
        status = MH_ERROR_MEMORY_ALLOC;
    }
    else if (pThreads->pItems != NULL)
    {
        UINT i;
        for (i = 0; i < pThreads->size; ++i)
        {
            HANDLE hThread = OpenThread(THREAD_ACCESS, FALSE, pThreads->pItems[i]);
            BOOL suspended = FALSE;
            if (hThread != NULL)
            {
                DWORD result = SuspendThread(hThread);
                if (result != 0xFFFFFFFF)
                {
                    suspended = TRUE;
                    ProcessThreadIPs(hThread, pos, action);
                }
                CloseHandle(hThread);
            }
            if (!suspended)
            {
                // Mark thread as not suspended, so it's not resumed later on.
                pThreads->pItems[i] = 0;
            }
        }
    }
    return status;
}

//-------------------------------------------------------------------------
static VOID Unfreeze(PFROZEN_THREADS pThreads)
{
    if (pThreads->pItems != NULL)
    {
        UINT i;
        for (i = 0; i < pThreads->size; ++i)
        {
            DWORD threadId = pThreads->pItems[i];
            if (threadId != 0)
            {
                HANDLE hThread = OpenThread(THREAD_ACCESS, FALSE, threadId);
                if (hThread != NULL)
                {
                    ResumeThread(hThread);
                    CloseHandle(hThread);
                }
            }
        }
        HeapFree(g_hHeap, 0, pThreads->pItems);
    }
}

//-------------------------------------------------------------------------
static MH_STATUS EnableHookLL(UINT pos, bool enable)
{
    PHOOK_ENTRY pHook = &g_hooks.pItems[pos];
    DWORD  oldProtect;
    SIZE_T patchSize    = sizeof(JMP_REL);
    LPBYTE pPatchTarget = (LPBYTE)pHook->pTarget;

    if (pHook->patchAbove)
    {
        pPatchTarget -= sizeof(JMP_REL);
        patchSize    += sizeof(JMP_REL_SHORT);
    }

    if (!VirtualProtect(pPatchTarget, patchSize, PAGE_EXECUTE_READWRITE, &oldProtect))
        return MH_ERROR_MEMORY_PROTECT;

    if (enable)
    {
        PJMP_REL pJmp = (PJMP_REL)pPatchTarget;
        pJmp->opcode = 0xE9;
        pJmp->operand = (UINT32)((LPBYTE)pHook->pDetour - (pPatchTarget + sizeof(JMP_REL)));

        if (pHook->patchAbove)
        {
            PJMP_REL_SHORT pShortJmp = (PJMP_REL_SHORT)pHook->pTarget;
            pShortJmp->opcode = 0xEB;
            pShortJmp->operand = (UINT8)(LONG_PTR)(0 - (sizeof(JMP_REL_SHORT) + sizeof(JMP_REL)));
        }
    }
    else
    {
        if (pHook->patchAbove)
            CopyMemory(pPatchTarget, pHook->backup, sizeof(JMP_REL) + sizeof(JMP_REL_SHORT));
        else
            CopyMemory(pPatchTarget, pHook->backup, sizeof(JMP_REL));
    }
    VirtualProtect(pPatchTarget, patchSize, oldProtect, &oldProtect);

    // Just-in-case measure.
    FlushInstructionCache(GetCurrentProcess(), pPatchTarget, patchSize);

    pHook->isEnabled   = enable;
    pHook->queueEnable = enable;
    return MH_OK;
}

//-------------------------------------------------------------------------
static MH_STATUS EnableAllHooksLL(bool enable)
{
    MH_STATUS status = MH_OK;
    UINT i, first = INVALID_HOOK_POS;
    for (i = 0; i < g_hooks.size; ++i)
    {
        if (g_hooks.pItems[i].isEnabled != enable)
        {
            first = i;
            break;
        }
    }
    if (first != INVALID_HOOK_POS)
    {
        FROZEN_THREADS threads;
        status = Freeze(&threads, ALL_HOOKS_POS, enable ? ACTION_ENABLE : ACTION_DISABLE);
        if (status == MH_OK)
        {
            for (i = first; i < g_hooks.size; ++i)
            {
                if (g_hooks.pItems[i].isEnabled != enable)
                {
                    status = EnableHookLL(i, enable);
                    if (status != MH_OK)
                        break;
                }
            }
            Unfreeze(&threads);
        }
    }
    return status;
}


//-------------------------------------------------------------------------
static VOID EnterSpinLock(VOID)
{
    SIZE_T spinCount = 0;
    // Wait until the flag is 0.
    while (_InterlockedCompareExchange(&g_isLocked, 1, 0) != 0)
    {
        // No need to generate a memory barrier here, since InterlockedCompareExchange()
        // generates a full memory barrier itself.
        // Prevent the loop from being too busy.
        if (spinCount < 32)
        {
            Sleep(0);
        }
        else
        {
            Sleep(1);
        }
        spinCount++;
    }
}

//-------------------------------------------------------------------------
static __inline VOID LeaveSpinLock(VOID)
{
    // No need to generate a memory barrier here, since InterlockedExchange()
    // generates a full memory barrier itself.
    _InterlockedExchange(&g_isLocked, 0);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_Initialize(VOID)
{
    MH_STATUS status = MH_OK;

    EnterSpinLock();

    if (g_hHeap == NULL)
    {
        g_hHeap = HeapCreate(0, 0, 0);
        if (g_hHeap != NULL)
        {
            // Initialize the internal function buffer.
            InitializeBuffer();
        }
        else
        {
            status = MH_ERROR_MEMORY_ALLOC;
        }
    }
    else
    {
        status = MH_ERROR_ALREADY_INITIALIZED;
    }

    LeaveSpinLock();

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_Uninitialize(VOID)
{
    MH_STATUS status = MH_OK;

    EnterSpinLock();

    if (g_hHeap != NULL)
    {
        status = EnableAllHooksLL(FALSE);
        if (status == MH_OK)
        {
            // Free the internal function buffer.

            // HeapFree is actually not required, but some tools detect a false
            // memory leak without HeapFree.

            UninitializeBuffer();

            HeapFree(g_hHeap, 0, g_hooks.pItems);
            HeapDestroy(g_hHeap);

            g_hHeap = NULL;

            g_hooks.pItems   = NULL;
            g_hooks.capacity = 0;
            g_hooks.size     = 0;
        }
    }
    else
    {
        status = MH_ERROR_NOT_INITIALIZED;
    }

    LeaveSpinLock();

    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_CreateHook(LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal)
{
    MH_STATUS status = MH_OK;
    EnterSpinLock();
    do
    {
        UINT        pos;
        LPVOID      pBuffer;
        TRAMPOLINE  ct;
        PHOOK_ENTRY pHook;

        if (g_hHeap == NULL)
        {
            status = MH_ERROR_NOT_INITIALIZED;
            break;
        }

        if (!(IsExecutableAddress(pTarget) && IsExecutableAddress(pDetour)))
        {
            status = MH_ERROR_NOT_EXECUTABLE;
            break;
        }

        pos = FindHookEntry(pTarget);
        if (pos != INVALID_HOOK_POS)
        {
            status = MH_ERROR_ALREADY_CREATED;
            break;
        }

        pBuffer = AllocateBuffer(pTarget);
        if (pBuffer == NULL)
        {
            status = MH_ERROR_MEMORY_ALLOC;
            break;
        }

        ct.pTarget     = pTarget;
        ct.pDetour     = pDetour;
        ct.pTrampoline = pBuffer;
        if (!CreateTrampolineFunction(&ct))
        {
            FreeBuffer(pBuffer);
            status = MH_ERROR_UNSUPPORTED_FUNCTION;
            break;
        }

        pHook = AddHookEntry();
        if (pHook == NULL)
        {
            FreeBuffer(pBuffer);
            status = MH_ERROR_MEMORY_ALLOC;
            break;
        }

        pHook->pTarget     = ct.pTarget;
#if defined(_M_X64) || defined(__x86_64__)
        pHook->pDetour     = ct.pRelay;
#else
        pHook->pDetour     = ct.pDetour;
#endif
        pHook->pTrampoline = ct.pTrampoline;
        pHook->patchAbove  = (UINT8)ct.patchAbove;
        pHook->isEnabled   = false;
        pHook->queueEnable = false;
        pHook->nIP         = ct.nIP;
        CopyMemory(pHook->oldIPs, ct.oldIPs, ARRAYSIZE(ct.oldIPs));
        CopyMemory(pHook->newIPs, ct.newIPs, ARRAYSIZE(ct.newIPs));

        // Back up the target function.
        if (ct.patchAbove)
        {
            CopyMemory(
                pHook->backup,
                (LPBYTE)pTarget - sizeof(JMP_REL),
                sizeof(JMP_REL) + sizeof(JMP_REL_SHORT));
        }
        else
        {
            CopyMemory(pHook->backup, pTarget, sizeof(JMP_REL));
        }
        if (ppOriginal != NULL)
            *ppOriginal = pHook->pTrampoline;
    } while(0);
    LeaveSpinLock();
    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_RemoveHook(LPVOID pTarget)
{
    MH_STATUS status = MH_OK;
    EnterSpinLock();
    do
    {
        UINT pos;
        if (g_hHeap == NULL)
        {
            status = MH_ERROR_NOT_INITIALIZED;
            break;
        }
        pos = FindHookEntry(pTarget);
        if (pos == INVALID_HOOK_POS)
        {
            status = MH_ERROR_NOT_CREATED;
            break;
        }
        if (g_hooks.pItems[pos].isEnabled)
        {
            FROZEN_THREADS threads;
            status = Freeze(&threads, pos, ACTION_DISABLE);
            if (status == MH_OK)
            {
                status = EnableHookLL(pos, false);
                Unfreeze(&threads);
            }
        }
        if (status == MH_OK)
        {
            FreeBuffer(g_hooks.pItems[pos].pTrampoline);
            DeleteHookEntry(pos);
        }
    } while (0);
    LeaveSpinLock();
    return status;
}

//-------------------------------------------------------------------------
static MH_STATUS EnableHook(LPVOID pTarget, bool enable)
{
    MH_STATUS status = MH_OK;
    EnterSpinLock();
    do
    {
        if (g_hHeap == NULL)
        {
            status = MH_ERROR_NOT_INITIALIZED;
            break;
        }
        if (pTarget == MH_ALL_HOOKS)
        {
            status = EnableAllHooksLL(enable);
            break;
        }
        else
        {
            FROZEN_THREADS threads;
            UINT pos = FindHookEntry(pTarget);
            if (pos == INVALID_HOOK_POS)
            {
                status = MH_ERROR_NOT_CREATED;
                break;
            }
            if (g_hooks.pItems[pos].isEnabled == enable)
            {
                status = enable ? MH_ERROR_ENABLED : MH_ERROR_DISABLED;
                break;
            }
            status = Freeze(&threads, pos, ACTION_ENABLE);
            if (status == MH_OK)
            {
                status = EnableHookLL(pos, enable);
                Unfreeze(&threads);
            }
        }
    } while(0);
    LeaveSpinLock();
    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_EnableHook(LPVOID pTarget)
{
    return EnableHook(pTarget, true);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_DisableHook(LPVOID pTarget)
{
    return EnableHook(pTarget, false);
}

//-------------------------------------------------------------------------
static MH_STATUS QueueHook(LPVOID pTarget, bool queueEnable)
{
    MH_STATUS status = MH_OK;
    EnterSpinLock();
    do
    {
        if (g_hHeap == NULL)
        {
            status = MH_ERROR_NOT_INITIALIZED;
            break;
        }

        if (pTarget == MH_ALL_HOOKS)
        {
            UINT i;
            for (i = 0; i < g_hooks.size; ++i)
                g_hooks.pItems[i].queueEnable = queueEnable;
        }
        else
        {
            UINT pos = FindHookEntry(pTarget);
            if (pos == INVALID_HOOK_POS)
            {
                status = MH_ERROR_NOT_CREATED;
            }
            else
            {
                g_hooks.pItems[pos].queueEnable = queueEnable;
            }
        }
    } while(0);
    LeaveSpinLock();
    return status;
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_QueueEnableHook(LPVOID pTarget)
{
    return QueueHook(pTarget, true);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_QueueDisableHook(LPVOID pTarget)
{
    return QueueHook(pTarget, false);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_ApplyQueued(VOID)
{
    MH_STATUS status = MH_OK;
    UINT i, first = INVALID_HOOK_POS;

    EnterSpinLock();

    if (g_hHeap != NULL)
    {
        for (i = 0; i < g_hooks.size; ++i)
        {
            if (g_hooks.pItems[i].isEnabled != g_hooks.pItems[i].queueEnable)
            {
                first = i;
                break;
            }
        }

        if (first != INVALID_HOOK_POS)
        {
            FROZEN_THREADS threads;
            status = Freeze(&threads, ALL_HOOKS_POS, ACTION_APPLY_QUEUED);
            if (status == MH_OK)
            {
                for (i = first; i < g_hooks.size; ++i)
                {
                    PHOOK_ENTRY pHook = &g_hooks.pItems[i];
                    if (pHook->isEnabled != pHook->queueEnable)
                    {
                        status = EnableHookLL(i, pHook->queueEnable);
                        if (status != MH_OK)
                            break;
                    }
                }
                Unfreeze(&threads);
            }
        }
    }
    else
    {
        status = MH_ERROR_NOT_INITIALIZED;
    }

    LeaveSpinLock();

    return status;
}
