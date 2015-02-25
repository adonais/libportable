/*
 *  MinHook - The Minimalistic API Hooking Library for x64/x86
 *  Copyright (C) 2009-2014 Tsuda Kageyu.
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
#include <tlhelp32.h>
#include <limits.h>
#include "intrin_c.h"

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

    bool   patchAbove  : 1;     // Uses the hot patch area.
    bool   isEnabled   : 1;     // Enabled.
    bool   queueEnable : 1;     // Queued for enabling/disabling when != isEnabled.

    UINT   nIP : 3;             // Count of the instruction boundaries.
    UINT8  oldIPs[8];           // Instruction boundaries of the target function.
    UINT8  newIPs[8];           // Instruction boundaries of the trampoline function.
} HOOK_ENTRY, *PHOOK_ENTRY;

//-------------------------------------------------------------------------
// Global Variables:
//-------------------------------------------------------------------------

// Spin lock flag for EnterSpinLock()/LeaveSpinLock().
volatile LONG g_isLocked = false;

// Private heap handle. If not NULL, this library is initialized.
HANDLE g_hHeap = NULL;

// Hook entries.
struct
{
    PHOOK_ENTRY pItems;     // Data heap
    UINT        capacity;   // Size of allocated data heap, items
    UINT        size;       // Actual number of data items
} g_hooks;

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
static PHOOK_ENTRY NewHookEntry()
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
static void DelHookEntry(UINT pos)
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

#ifdef _M_X64
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
static void ProcessThreadIPs(HANDLE hThread, UINT pos, UINT action)
{
    // If the thread suspended in the overwritten area,
    // move IP to the proper address.

    CONTEXT c;
#ifdef _M_X64
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

        case ACTION_APPLY_QUEUED:
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
static VOID EnumerateThreads(PFROZEN_THREADS pThreads)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te;
        te.dwSize = sizeof(THREADENTRY32);
        if (Thread32First(hSnapshot, &te))
        {
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
                            break;
                    }
                    else if (pThreads->size >= pThreads->capacity)
                    {
                        LPDWORD p = (LPDWORD)HeapReAlloc(
                            g_hHeap, 0, pThreads->pItems, (pThreads->capacity * 2) * sizeof(DWORD));
                        if (p == NULL)
                            break;

                        pThreads->capacity *= 2;
                        pThreads->pItems = p;
                    }
                    pThreads->pItems[pThreads->size++] = te.th32ThreadID;
                }

                te.dwSize = sizeof(THREADENTRY32);
            } while (Thread32Next(hSnapshot, &te));
        }
        CloseHandle(hSnapshot);
    }
}

//-------------------------------------------------------------------------
VOID WINAPI Freezex(PFROZEN_THREADS pThreads)
{
    pThreads->pItems   = NULL;
    pThreads->capacity = 0;
    pThreads->size     = 0;
    EnumerateThreads(pThreads);

    if (pThreads->pItems != NULL)
    {
        UINT i;
        for (i = 0; i < pThreads->size; ++i)
        {
            HANDLE hThread = OpenThread(THREAD_ACCESS, false, pThreads->pItems[i]);
            if (hThread != NULL)
            {
                SuspendThread(hThread);
                CloseHandle(hThread);
            }
        }
    }
}

//-------------------------------------------------------------------------
static VOID Freeze(PFROZEN_THREADS pThreads, UINT pos, UINT action)
{
    pThreads->pItems   = NULL;
    pThreads->capacity = 0;
    pThreads->size     = 0;
    EnumerateThreads(pThreads);

    if (pThreads->pItems != NULL)
    {
        UINT i;
        for (i = 0; i < pThreads->size; ++i)
        {
            HANDLE hThread = OpenThread(THREAD_ACCESS, false, pThreads->pItems[i]);
            if (hThread != NULL)
            {
                SuspendThread(hThread);
                ProcessThreadIPs(hThread, pos, action);
                CloseHandle(hThread);
            }
        }
    }
}

//-------------------------------------------------------------------------
VOID WINAPI Unfreeze(PFROZEN_THREADS pThreads)
{
    if (pThreads->pItems != NULL)
    {
        UINT i;
        for (i = 0; i < pThreads->size; ++i)
        {
            HANDLE hThread = OpenThread(THREAD_ACCESS, false, pThreads->pItems[i]);
            if (hThread != NULL)
            {
                ResumeThread(hThread);
                CloseHandle(hThread);
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
            pShortJmp->operand = (UINT8)(0 - (sizeof(JMP_REL_SHORT) + sizeof(JMP_REL)));
        }
    }
    else
    {
        if (pHook->patchAbove)
            memcpy(pPatchTarget, pHook->backup, sizeof(JMP_REL) + sizeof(JMP_REL_SHORT));
        else
            memcpy(pPatchTarget, pHook->backup, sizeof(JMP_REL));
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
    UINT i;
    MH_STATUS status = MH_OK;
    for (i = 0; i < g_hooks.size; ++i)
    {
        if (g_hooks.pItems[i].isEnabled != enable)
        {
            FROZEN_THREADS threads;
            Freeze(&threads, ALL_HOOKS_POS, enable ? ACTION_ENABLE : ACTION_DISABLE);
            for (; i < g_hooks.size; ++i)
            {
                if (g_hooks.pItems[i].isEnabled != enable)
                {
                    status = EnableHookLL(i, enable);
                    if (status != MH_OK) break;
                }
            }
            
            Unfreeze(&threads);
            if (status != MH_OK) break;
        }
    }

    return status;
}


//-------------------------------------------------------------------------
VOID WINAPI EnterSpinLock(VOID)
{
    // Wait until the flag is false.
    while (_InterlockedCompareExchange(&g_isLocked, true, false) != false)
    {
        // Prevent the loop from being too busy.
        Sleep(1);
    }
}

//-------------------------------------------------------------------------
VOID WINAPI LeaveSpinLock(VOID)
{
    _InterlockedExchange(&g_isLocked, false);
}

//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_Initialize(VOID)
{
    MH_STATUS status = MH_OK;
    EnterSpinLock();
    do
    {
        if (g_hHeap != NULL)
        {
            status = MH_ERROR_ALREADY_INITIALIZED;
            break;
        }
        g_hHeap = HeapCreate(0, 0, 0); 
        if (g_hHeap == NULL)
            status = MH_ERROR_MEMORY_ALLOC;
        if (status == MH_OK)
        {
            InitializeBuffer();
        }
    } while(0);
    LeaveSpinLock();
    return status;
}


//-------------------------------------------------------------------------
MH_STATUS WINAPI MH_Uninitialize(VOID)
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
        status = EnableAllHooksLL(false);
        if (status != MH_OK) break;
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
    } while(0);
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

        if (!IsExecutableAddress(pTarget) || !IsExecutableAddress(pDetour))
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

        pHook = NewHookEntry();
        if (pHook == NULL)
        {
            FreeBuffer(pBuffer);
            status = MH_ERROR_MEMORY_ALLOC;
            break;
        }

        pHook->pTarget     = ct.pTarget;
#ifdef _M_X64
        pHook->pDetour     = ct.pRelay;
#else
        pHook->pDetour     = ct.pDetour;
#endif
        pHook->pTrampoline = ct.pTrampoline;
        pHook->patchAbove  = ct.patchAbove;
        pHook->isEnabled   = false;
        pHook->queueEnable = false;
        pHook->nIP         = ct.nIP;
        memcpy(pHook->oldIPs, ct.oldIPs, ARRAYSIZE(ct.oldIPs));
        memcpy(pHook->newIPs, ct.newIPs, ARRAYSIZE(ct.newIPs));

        // Back up the target function.
        if (ct.patchAbove)
        {
            memcpy(
                pHook->backup,
                (LPBYTE)pTarget - sizeof(JMP_REL),
                sizeof(JMP_REL) + sizeof(JMP_REL_SHORT));
        }
        else
        {
            memcpy(pHook->backup, pTarget, sizeof(JMP_REL));
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
            Freeze(&threads, pos, ACTION_DISABLE);
            status = EnableHookLL(pos, false);
            Unfreeze(&threads);
        }

        FreeBuffer(g_hooks.pItems[pos].pTrampoline);
        DelHookEntry(pos);
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

            Freeze(&threads, pos, ACTION_ENABLE);
            status = EnableHookLL(pos, enable);
            Unfreeze(&threads);
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
    UINT i;
    MH_STATUS status = MH_OK;
    if (g_hHeap == NULL)
    {
        return MH_ERROR_NOT_INITIALIZED;
    }
    EnterSpinLock();
    for (i = 0; i < g_hooks.size; ++i)
    {
        if (g_hooks.pItems[i].isEnabled != g_hooks.pItems[i].queueEnable)
        {
            FROZEN_THREADS threads;
            Freeze(&threads, ALL_HOOKS_POS, ACTION_APPLY_QUEUED);
            for (; i < g_hooks.size; ++i)
            {
                PHOOK_ENTRY pHook = &g_hooks.pItems[i];
                if (pHook->isEnabled != pHook->queueEnable)
                {
                    status = EnableHookLL(i, pHook->queueEnable);
                    if (status != MH_OK) break;
                }
            }
            Unfreeze(&threads);
            if (status != MH_OK) break;
        }
    }
    LeaveSpinLock();
    return status;
}
