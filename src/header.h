#ifndef _HEAD_ER_H_
#  define _HEAD_ER_H_

#include <windows.h>
#include <shlwapi.h>
#include <psapi.h>

#if defined(_MSC_VER)
#define __C89_NAMELESS
#endif

#define CREATE_PROCESS_BREAKAWAY_FROM_JOB	0x0001
#define CREATE_PROCESS_INHERIT_HANDLES		0x0004
#define CREATE_PROCESS_PROTECTED			0x0040

#define CREATE_THREAD_SUSPENDED				0x0001
#define STATUS_ERROR						((NTSTATUS)0x80070000L)
#define NT_SUCCESS(Status)					(((NTSTATUS)(Status)) >= 0)
#define STATUS_SUCCESS						((NTSTATUS)0x00000000)
#define STATUS_OBJECT_PATH_NOT_FOUND		((NTSTATUS)0xC000003A)
#define NtCurrentProcess()					((HANDLE)(LONG_PTR) -1)

#define ProcThreadAttributeValue( p1, p2, p3, p4 ) \
        (((p1) & PROC_THREAD_ATTRIBUTE_NUMBER) | \
        ((p2 != FALSE) ? PROC_THREAD_ATTRIBUTE_THREAD : 0) | \
        ((p3 != FALSE) ? PROC_THREAD_ATTRIBUTE_INPUT : 0) | \
        ((p4 != FALSE) ? PROC_THREAD_ATTRIBUTE_ADDITIVE : 0))

#define PROC_THREAD_ATTRIBUTE_PCLIENTID ProcThreadAttributeValue (8, FALSE, FALSE, FALSE)

#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004)

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
#if defined(_M_X64)
    ULONG pad1;
#endif
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
#if defined(_M_X64)
    ULONG pad2;
#endif
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
  } OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;
#endif

#ifndef __NT_PROC_THREAD_ATTRIBUTE_ENTRY
#define __NT_PROC_THREAD_ATTRIBUTE_ENTRY
typedef struct _NT_PROC_THREAD_ATTRIBUTE_ENTRY {
    ULONG_PTR Attribute;
    ULONG_PTR Size;
    ULONG_PTR* pValue;
    ULONG_PTR Unknown;
} PROC_THREAD_ATTRIBUTE_ENTRY, *PPROC_THREAD_ATTRIBUTE_ENTRY;
#endif

#ifndef __NT_PROC_THREAD_ATTRIBUTE_LIST
#define __NT_PROC_THREAD_ATTRIBUTE_LIST
typedef struct _NT_PROC_THREAD_ATTRIBUTE_LIST {
    ULONG_PTR Length;
    PROC_THREAD_ATTRIBUTE_ENTRY Entry[2];
} NT_PROC_THREAD_ATTRIBUTE_LIST;
typedef NT_PROC_THREAD_ATTRIBUTE_LIST *PNT_PROC_THREAD_ATTRIBUTE_LIST;
#endif

#ifndef __STRING
#define __STRING
typedef struct _STRING
{
     WORD Length;
     WORD MaximumLength;
     CHAR * Buffer;
} STRING, *PSTRING;
#endif

typedef struct _FILE_NAME_INFORMATION {
  ULONG FileNameLength;
  WCHAR FileName[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

typedef enum _PROCESSINFOCLASS {
  ProcessBasicInformation,
  ProcessQuotaLimits,
  ProcessIoCounters,
  ProcessVmCounters,
  ProcessTimes,
  ProcessBasePriority,
  ProcessRaisePriority,
  ProcessDebugPort,
  ProcessExceptionPort,
  ProcessAccessToken,
  ProcessLdtInformation,
  ProcessLdtSize,
  ProcessDefaultHardErrorMode,
  ProcessIoPortHandlers,
  ProcessPooledUsageAndLimits,
  ProcessWorkingSetWatch,
  ProcessUserModeIOPL,
  ProcessEnableAlignmentFaultFixup,
  ProcessPriorityClass,
  ProcessWx86Information,
  ProcessHandleCount,
  ProcessAffinityMask,
  ProcessPriorityBoost,
  ProcessDeviceMap,
  ProcessSessionInformation,
  ProcessForegroundInformation,
  ProcessWow64Information,
  ProcessImageFileName,
  ProcessLUIDDeviceMapsEnabled,
  ProcessBreakOnTermination,
  ProcessDebugObjectHandle,
  ProcessDebugFlags,
  ProcessHandleTracing,
  ProcessIoPriority,
  ProcessExecuteFlags,
  ProcessTlsInformation,
  ProcessCookie,
  ProcessImageInformation,
  ProcessCycleTime,
  ProcessPagePriority,
  ProcessInstrumentationCallback,
  ProcessThreadStackAllocation,
  ProcessWorkingSetWatchEx,
  ProcessImageFileNameWin32,
  ProcessImageFileMapping,
  ProcessAffinityUpdateMode,
  ProcessMemoryAllocationMode,
  ProcessGroupInformation,
  ProcessTokenVirtualizationEnabled,
  ProcessConsoleHostProcess,
  ProcessWindowInformation,
  MaxProcessInfoClass
} PROCESSINFOCLASS;

typedef struct _CLIENT_ID {
  HANDLE UniqueProcess;
  HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

#ifndef __RTL_DRIVE_LETTER_CURDIR
#define __RTL_DRIVE_LETTER_CURDIR
typedef struct _RTL_DRIVE_LETTER_CURDIR {
	USHORT                  Flags;
	USHORT                  Length;
	ULONG                   TimeStamp;
	UNICODE_STRING          DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;
#endif

#ifndef __RTL_USER_PROCESS_PARAMETERS
#define __RTL_USER_PROCESS_PARAMETERS
typedef struct _RTL_USER_PROCESS_PARAMETERS {
	ULONG                   MaximumLength;
	ULONG                   Length;
	ULONG                   Flags;
	ULONG                   DebugFlags;
	PVOID                   ConsoleHandle;
	ULONG                   ConsoleFlags;
	HANDLE                  StdInputHandle;
	HANDLE                  StdOutputHandle;
	HANDLE                  StdErrorHandle;
	UNICODE_STRING          CurrentDirectoryPath;
	HANDLE                  CurrentDirectoryHandle;
	UNICODE_STRING          DllPath;
	UNICODE_STRING          ImagePathName;
	UNICODE_STRING          CommandLine;
	PVOID                   Environment;
	ULONG                   StartingPositionLeft;
	ULONG                   StartingPositionTop;
	ULONG                   Width;
	ULONG                   Height;
	ULONG                   CharWidth;
	ULONG                   CharHeight;
	ULONG                   ConsoleTextAttributes;
	ULONG                   WindowFlags;
	ULONG                   ShowWindowFlags;
	UNICODE_STRING          WindowTitle;
	UNICODE_STRING          DesktopName;
	UNICODE_STRING          ShellInfo;
	UNICODE_STRING          RuntimeData;
	RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;
#endif

typedef struct PEB_FREE_BLOCK *_PEB_FREE_BLOCK;
typedef struct _PEB_FREE_BLOCK {
     _PEB_FREE_BLOCK *Next;
     ULONG Size;
} PEB_FREE_BLOCK;

typedef struct _PEB_LDR_DATA {
	ULONG                   Length;
	BOOLEAN                 Initialized;
	PVOID                   SsHandle;
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct tagRTL_BITMAP {
    ULONG  SizeOfBitMap; /* Number of bits in the bitmap */
    PULONG Buffer; /* Bitmap data, assumed sized to a DWORD boundary */
} RTL_BITMAP, *PRTL_BITMAP;

typedef struct _PEB
{
    BOOLEAN                      InheritedAddressSpace;             /*  00 */
    BOOLEAN                      ReadImageFileExecOptions;          /*  01 */
    BOOLEAN                      BeingDebugged;                     /*  02 */
    BOOLEAN                      SpareBool;                         /*  03 */
    HANDLE                       Mutant;                            /*  04 */
    HMODULE                      ImageBaseAddress;                  /*  08 */
    PPEB_LDR_DATA                LdrData;                           /*  0c */
    RTL_USER_PROCESS_PARAMETERS *ProcessParameters;                 /*  10 */
    PVOID                        SubSystemData;                     /*  14 */
    HANDLE                       ProcessHeap;                       /*  18 */
    PRTL_CRITICAL_SECTION        FastPebLock;                       /*  1c */
    PVOID /*PPEBLOCKROUTINE*/    FastPebLockRoutine;                /*  20 */
    PVOID /*PPEBLOCKROUTINE*/    FastPebUnlockRoutine;              /*  24 */
    ULONG                        EnvironmentUpdateCount;            /*  28 */
    PVOID                        KernelCallbackTable;               /*  2c */
    PVOID                        EventLogSection;                   /*  30 */
    PVOID                        EventLog;                          /*  34 */
    PVOID /*PPEB_FREE_BLOCK*/    FreeList;                          /*  38 */
    ULONG                        TlsExpansionCounter;               /*  3c */
    PRTL_BITMAP                  TlsBitmap;                         /*  40 */
    ULONG                        TlsBitmapBits[2];                  /*  44 */
    PVOID                        ReadOnlySharedMemoryBase;          /*  4c */
    PVOID                        ReadOnlySharedMemoryHeap;          /*  50 */
    PVOID                       *ReadOnlyStaticServerData;          /*  54 */
    PVOID                        AnsiCodePageData;                  /*  58 */
    PVOID                        OemCodePageData;                   /*  5c */
    PVOID                        UnicodeCaseTableData;              /*  60 */
    ULONG                        NumberOfProcessors;                /*  64 */
    ULONG                        NtGlobalFlag;                      /*  68 */
    BYTE                         Spare2[4];                         /*  6c */
    LARGE_INTEGER                CriticalSectionTimeout;            /*  70 */
    ULONG                        HeapSegmentReserve;                /*  78 */
    ULONG                        HeapSegmentCommit;                 /*  7c */
    ULONG                        HeapDeCommitTotalFreeThreshold;    /*  80 */
    ULONG                        HeapDeCommitFreeBlockThreshold;    /*  84 */
    ULONG                        NumberOfHeaps;                     /*  88 */
    ULONG                        MaximumNumberOfHeaps;              /*  8c */
    PVOID                       *ProcessHeaps;                      /*  90 */
    PVOID                        GdiSharedHandleTable;              /*  94 */
    PVOID                        ProcessStarterHelper;              /*  98 */
    PVOID                        GdiDCAttributeList;                /*  9c */
    PVOID                        LoaderLock;                        /*  a0 */
    ULONG                        OSMajorVersion;                    /*  a4 */
    ULONG                        OSMinorVersion;                    /*  a8 */
    ULONG                        OSBuildNumber;                     /*  ac */
    ULONG                        OSPlatformId;                      /*  b0 */
    ULONG                        ImageSubSystem;                    /*  b4 */
    ULONG                        ImageSubSystemMajorVersion;        /*  b8 */
    ULONG                        ImageSubSystemMinorVersion;        /*  bc */
    ULONG                        ImageProcessAffinityMask;          /*  c0 */
    ULONG                        GdiHandleBuffer[34];               /*  c4 */
    ULONG                        PostProcessInitRoutine;            /* 14c */
    PRTL_BITMAP                  TlsExpansionBitmap;                /* 150 */
    ULONG                        TlsExpansionBitmapBits[32];        /* 154 */
    ULONG                        SessionId;                         /* 1d4 */
} PEB, *PPEB;

typedef struct _PROCESS_BASIC_INFORMATION
{
    LONG ExitStatus;
    LPVOID PebBaseAddress;
    ULONG_PTR AffinityMask;
    LONG BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

typedef struct _PROCESS_EXTENDED_BASIC_INFORMATION
{
    SIZE_T Size; /* set to sizeof structure on input  */
    PROCESS_BASIC_INFORMATION BasicInfo;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG IsProtectedProcess : 1;
            ULONG IsWow64Process : 1;
            ULONG IsProcessDeleting : 1;
            ULONG IsCrossSessionCreate : 1;
            ULONG SpareBits : 28;
        };
    };
} PROCESS_EXTENDED_BASIC_INFORMATION, *PPROCESS_EXTENDED_BASIC_INFORMATION;

typedef struct _LDR_DATA_TABLE_ENTRY {
    PVOID Reserved1[2];
    LIST_ENTRY InMemoryOrderLinks;
    PVOID Reserved2[2];
    PVOID DllBase;
    PVOID EntryPoint;
    PVOID Reserved3;
    UNICODE_STRING FullDllName;
    BYTE Reserved4[8];
    PVOID Reserved5[3];
    union {
        ULONG CheckSum;
        PVOID Reserved6;
    };
    ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _LDR_MODULE {
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
	PVOID                   BaseAddress;
	PVOID                   EntryPoint;
	ULONG                   SizeOfImage;
	UNICODE_STRING          FullDllName;
	UNICODE_STRING          BaseDllName;
	ULONG                   Flags;
	SHORT                   LoadCount;
	SHORT                   TlsIndex;
	LIST_ENTRY              HashTableEntry;
	ULONG                   TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;

typedef enum _SYSTEM_INFORMATION_CLASS
{
        SystemBasicInformation, /* q: SYSTEM_BASIC_INFORMATION  */
        SystemProcessorInformation, /* q: SYSTEM_PROCESSOR_INFORMATION  */
        SystemPerformanceInformation, /* q: SYSTEM_PERFORMANCE_INFORMATION  */
        SystemTimeOfDayInformation, /* q: SYSTEM_TIMEOFDAY_INFORMATION  */
        SystemPathInformation, /* not implemented  */
        SystemProcessInformation, /* q: SYSTEM_PROCESS_INFORMATION  */
        SystemCallCountInformation, /* q: SYSTEM_CALL_COUNT_INFORMATION  */
        SystemDeviceInformation, /* q: SYSTEM_DEVICE_INFORMATION  */
        SystemProcessorPerformanceInformation, /* q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION  */
        SystemFlagsInformation, /* q: SYSTEM_FLAGS_INFORMATION  */
        SystemCallTimeInformation, /* 10, not implemented  */
        SystemModuleInformation, /* q: RTL_PROCESS_MODULES  */
        SystemLocksInformation,
        SystemStackTraceInformation,
        SystemPagedPoolInformation, /* not implemented  */
        SystemNonPagedPoolInformation, /* not implemented  */
        SystemHandleInformation, /* q: SYSTEM_HANDLE_INFORMATION  */
        SystemObjectInformation, /* q: SYSTEM_OBJECTTYPE_INFORMATION mixed with SYSTEM_OBJECT_INFORMATION  */
        SystemPageFileInformation, /* q: SYSTEM_PAGEFILE_INFORMATION  */
        SystemVdmInstemulInformation, /* q  */
        SystemVdmBopInformation, /* 20, not implemented  */
        SystemFileCacheInformation, /* q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemCache)  */
        SystemPoolTagInformation, /* q: SYSTEM_POOLTAG_INFORMATION  */
        SystemInterruptInformation, /* q: SYSTEM_INTERRUPT_INFORMATION  */
        SystemDpcBehaviorInformation, /* q: SYSTEM_DPC_BEHAVIOR_INFORMATION; s: SYSTEM_DPC_BEHAVIOR_INFORMATION (requires SeLoadDriverPrivilege)  */
        SystemFullMemoryInformation, /* not implemented  */
        SystemLoadGdiDriverInformation, /* s (kernel-mode only)  */
        SystemUnloadGdiDriverInformation, /* s (kernel-mode only)  */
        SystemTimeAdjustmentInformation, /* q: SYSTEM_QUERY_TIME_ADJUST_INFORMATION; s: SYSTEM_SET_TIME_ADJUST_INFORMATION (requires SeSystemtimePrivilege)  */
        SystemSummaryMemoryInformation, /* not implemented  */
        SystemMirrorMemoryInformation, /* 30, s (requires license value "Kernel-MemoryMirroringSupported") (requires SeShutdownPrivilege)  */
        SystemPerformanceTraceInformation, /* s  */
        SystemObsolete0, /* not implemented  */
        SystemExceptionInformation, /* q: SYSTEM_EXCEPTION_INFORMATION  */
        SystemCrashDumpStateInformation, /* s (requires SeDebugPrivilege)  */
        SystemKernelDebuggerInformation, /* q: SYSTEM_KERNEL_DEBUGGER_INFORMATION  */
        SystemContextSwitchInformation, /* q: SYSTEM_CONTEXT_SWITCH_INFORMATION  */
        SystemRegistryQuotaInformation, /* q: SYSTEM_REGISTRY_QUOTA_INFORMATION; s (requires SeIncreaseQuotaPrivilege)  */
        SystemExtendServiceTableInformation, /* s (requires SeLoadDriverPrivilege) loads win32k only  */
        SystemPrioritySeperation, /* s (requires SeTcbPrivilege)  */
        SystemVerifierAddDriverInformation, /* 40, s (requires SeDebugPrivilege)  */
        SystemVerifierRemoveDriverInformation, /* s (requires SeDebugPrivilege)  */
        SystemProcessorIdleInformation, /* q: SYSTEM_PROCESSOR_IDLE_INFORMATION  */
        SystemLegacyDriverInformation, /* q: SYSTEM_LEGACY_DRIVER_INFORMATION  */
        SystemCurrentTimeZoneInformation, /* q  */
        SystemLookasideInformation, /* q: SYSTEM_LOOKASIDE_INFORMATION  */
        SystemTimeSlipNotification, /* s (requires SeSystemtimePrivilege)  */
        SystemSessionCreate, /* not implemented  */
        SystemSessionDetach, /* not implemented  */
        SystemSessionInformation, /* not implemented  */
        SystemRangeStartInformation, /* 50, q  */
        SystemVerifierInformation, /* q: SYSTEM_VERIFIER_INFORMATION; s (requires SeDebugPrivilege)  */
        SystemVerifierThunkExtend, /* s (kernel-mode only)  */
        SystemSessionProcessInformation, /* q: SYSTEM_SESSION_PROCESS_INFORMATION  */
        SystemLoadGdiDriverInSystemSpace, /* s (kernel-mode only) (same as SystemLoadGdiDriverInformation)  */
        SystemNumaProcessorMap, /* q  */
        SystemPrefetcherInformation, /* q: PREFETCHER_INFORMATION; s: PREFETCHER_INFORMATION  PfSnQueryPrefetcherInformation  */
        SystemExtendedProcessInformation, /* q: SYSTEM_PROCESS_INFORMATION  */
        SystemRecommendedSharedDataAlignment, /* q  */
        SystemComPlusPackage, /* q; s  */
        SystemNumaAvailableMemory, /* 60  */
        SystemProcessorPowerInformation, /* q: SYSTEM_PROCESSOR_POWER_INFORMATION  */
        SystemEmulationBasicInformation, /* q  */
        SystemEmulationProcessorInformation,
        SystemExtendedHandleInformation, /* q: SYSTEM_HANDLE_INFORMATION_EX  */
        SystemLostDelayedWriteInformation, /* q: ULONG  */
        SystemBigPoolInformation, /* q: SYSTEM_BIGPOOL_INFORMATION  */
        SystemSessionPoolTagInformation, /* q: SYSTEM_SESSION_POOLTAG_INFORMATION  */
        SystemSessionMappedViewInformation, /* q: SYSTEM_SESSION_MAPPED_VIEW_INFORMATION  */
        SystemHotpatchInformation, /* q; s  */
        SystemObjectSecurityMode, /* 70, q  */
        SystemWatchdogTimerHandler, /* s (kernel-mode only)  */
        SystemWatchdogTimerInformation, /* q (kernel-mode only); s (kernel-mode only)  */
        SystemLogicalProcessorInformation, /* q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION  */
        SystemWow64SharedInformationObsolete, /* not implemented  */
        SystemRegisterFirmwareTableInformationHandler, /* s (kernel-mode only)  */
        SystemFirmwareTableInformation, /* not implemented  */
        SystemModuleInformationEx, /* q: RTL_PROCESS_MODULE_INFORMATION_EX  */
        SystemVerifierTriageInformation, /* not implemented  */
        SystemSuperfetchInformation, /* q: SUPERFETCH_INFORMATION; s: SUPERFETCH_INFORMATION  PfQuerySuperfetchInformation  */
        SystemMemoryListInformation, /* 80, q: SYSTEM_MEMORY_LIST_INFORMATION; s: SYSTEM_MEMORY_LIST_COMMAND (requires SeProfileSingleProcessPrivilege)  */
        SystemFileCacheInformationEx, /* q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (same as SystemFileCacheInformation)  */
        SystemThreadPriorityClientIdInformation, /* s: SYSTEM_THREAD_CID_PRIORITY_INFORMATION (requires SeIncreaseBasePriorityPrivilege)  */
        SystemProcessorIdleCycleTimeInformation, /* q: SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION[]  */
        SystemVerifierCancellationInformation, /* not implemented  name:wow64:whNT32QuerySystemVerifierCancellationInformation  */
        SystemProcessorPowerInformationEx, /* not implemented  */
        SystemRefTraceInformation, /* q; s  ObQueryRefTraceInformation  */
        SystemSpecialPoolInformation, /* q; s (requires SeDebugPrivilege)  MmSpecialPoolTag, then MmSpecialPoolCatchOverruns != 0  */
        SystemProcessIdInformation, /* q: SYSTEM_PROCESS_ID_INFORMATION  */
        SystemErrorPortInformation, /* s (requires SeTcbPrivilege)  */
        SystemBootEnvironmentInformation, /* 90, q: SYSTEM_BOOT_ENVIRONMENT_INFORMATION  */
        SystemHypervisorInformation, /* q; s (kernel-mode only)  */
        SystemVerifierInformationEx, /* q; s  */
        SystemTimeZoneInformation, /* s (requires SeTimeZonePrivilege)  */
        SystemImageFileExecutionOptionsInformation, /* s: SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION (requires SeTcbPrivilege)  */
        SystemCoverageInformation, /* q; s  name:wow64:whNT32QuerySystemCoverageInformation; ExpCovQueryInformation  */
        SystemPrefetchPatchInformation, /* not implemented  */
        SystemVerifierFaultsInformation, /* s (requires SeDebugPrivilege)  */
        SystemSystemPartitionInformation, /* q: SYSTEM_SYSTEM_PARTITION_INFORMATION  */
        SystemSystemDiskInformation, /* q: SYSTEM_SYSTEM_DISK_INFORMATION  */
        SystemProcessorPerformanceDistribution, /* 100, q: SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION  */
        SystemNumaProximityNodeInformation, /* q  */
        SystemDynamicTimeZoneInformation, /* q; s (requires SeTimeZonePrivilege)  */
        SystemCodeIntegrityInformation, /* q  SeCodeIntegrityQueryInformation  */
        SystemProcessorMicrocodeUpdateInformation, /* s  */
        SystemProcessorBrandString, /* q  HaliQuerySystemInformation -> HalpGetProcessorBrandString, info class 23  */
        SystemVirtualAddressInformation, /* q: SYSTEM_VA_LIST_INFORMATION[]; s: SYSTEM_VA_LIST_INFORMATION[] (requires SeIncreaseQuotaPrivilege)  MmQuerySystemVaInformation  */
        SystemLogicalProcessorAndGroupInformation, /* q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX,since WIN7 ,KeQueryLogicalProcessorRelationship  */
        SystemProcessorCycleTimeInformation, /* q: SYSTEM_PROCESSOR_CYCLE_TIME_INFORMATION[]  */
        SystemStoreInformation, /* q; s SmQueryStoreInformation  */
        SystemRegistryAppendString, /* 110, s: SYSTEM_REGISTRY_APPEND_STRING_PARAMETERS  */
        SystemAitSamplingValue, /* s: ULONG (requires SeProfileSingleProcessPrivilege)  */
        SystemVhdBootInformation, /* q: SYSTEM_VHD_BOOT_INFORMATION  */
        SystemCpuQuotaInformation, /* q; s PsQueryCpuQuotaInformation  */
        SystemNativeBasicInformation, /* not implemented  */
        SystemSpare1, /* not implemented  */
        SystemLowPriorityIoInformation, /* q: SYSTEM_LOW_PRIORITY_IO_INFORMATION  */
        SystemTpmBootEntropyInformation, /* q: TPM_BOOT_ENTROPY_NT_RESULT  ExQueryTpmBootEntropyInformation  */
        SystemVerifierCountersInformation, /* q: SYSTEM_VERIFIER_COUNTERS_INFORMATION  */
        SystemPagedPoolInformationEx, /* q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypePagedPool)  */
        SystemSystemPtesInformationEx, /* 120, q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemPtes)  */
        SystemNodeDistanceInformation, /* q  */
        SystemAcpiAuditInformation, /* q: SYSTEM_ACPI_AUDIT_INFORMATION  HaliQuerySystemInformation -> HalpAuditQueryResults, info class 26  */
        SystemBasicPerformanceInformation, /* q: SYSTEM_BASIC_PERFORMANCE_INFORMATION  name:wow64:whNtQuerySystemInformation_SystemBasicPerformanceInformation  */
        SystemQueryPerformanceCounterInformation, /* q: SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION, since WIN7 SP1  */
        MaxSystemInfoClass
} SYSTEM_INFORMATION_CLASS;

typedef LONG KPRIORITY;

typedef enum _KWAIT_REASON
{
    Executive,
    FreePage,
    PageIn,
    PoolAllocation,
    DelayExecution,
    Suspended,
    UserRequest,
    WrExecutive,
    WrFreePage,
    WrPageIn,
    WrPoolAllocation,
    WrDelayExecution,
    WrSuspended,
    WrUserRequest,
    WrEventPair,
    WrQueue,
    WrLpcReceive,
    WrLpcReply,
    WrVirtualMemory,
    WrPageOut,
    WrRendezvous,
    WrKeyedEvent,
    WrTerminated,
    WrProcessInSwap,
    WrCpuRateControl,
    WrCalloutStack,
    WrKernel,
    WrResource,
    WrPushLock,
    WrMutex,
    WrQuantumEnd,
    WrDispatchInt,
    WrPreempted,
    WrYieldExecution,
    WrFastMutex,
    WrGuardedMutex,
    WrRundown,
    MaximumWaitReason
} KWAIT_REASON, *PKWAIT_REASON;

typedef struct _SYSTEM_THREAD_INFORMATION
{
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    KPRIORITY Priority;
    LONG BasePriority;
    ULONG ContextSwitches;
    ULONG ThreadState;
    KWAIT_REASON WaitReason;
} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER WorkingSetPrivateSize; /* since VISTA */
    ULONG HardFaultCount; /* since WIN7 */
    ULONG NumberOfThreadsHighWatermark; /* since WIN7 */
    ULONGLONG CycleTime; /* since WIN7 */
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR UniqueProcessKey; /* since VISTA (requires SystemExtendedProcessInformation) */
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER ReadOperationCount;
    LARGE_INTEGER WriteOperationCount;
    LARGE_INTEGER OtherOperationCount;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
    SYSTEM_THREAD_INFORMATION Threads[1];
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef NTSTATUS (NTAPI *_NtQueryObject)(HANDLE ObjectHandle,
										ULONG  ObjectInformationClass,
										PVOID  ObjectInformation,
										ULONG  ObjectInformationLength,
										PULONG ReturnLength);
typedef NTSTATUS (NTAPI *_NtQuerySection) (HANDLE SectionHandle, 
										ULONG SectionInformationClass,
										PVOID SectionInformation,
										ULONG SectionInformationLength,
										PULONG ResultLength);
typedef  NTSTATUS (NTAPI *_NtCreateSection)(OUT PHANDLE SectionHandle,
										IN ACCESS_MASK DesiredAccess,
										IN POBJECT_ATTRIBUTES ObjectAttributes,
										IN PLARGE_INTEGER SectionSize OPTIONAL,
										IN ULONG Protect,
										IN ULONG Attributes,
										IN HANDLE FileHandle);
typedef  NTSTATUS (NTAPI *_NtCreateUserProcess)(OUT PHANDLE ProcessHandle,
										OUT PHANDLE ThreadHandle,
										IN ACCESS_MASK ProcessDesiredAccess,
										IN ACCESS_MASK ThreadDesiredAccess,
										IN POBJECT_ATTRIBUTES ProcessObjectAttributes OPTIONAL,
										IN POBJECT_ATTRIBUTES ThreadObjectAttributes OPTIONAL,
										IN ULONG CreateProcessFlags,
										IN ULONG CreateThreadFlags,
										IN PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
										IN PVOID Parameter9,
										IN NT_PROC_THREAD_ATTRIBUTE_LIST *AttributeList);
typedef NTSTATUS (NTAPI *_NtTerminateProcess)(HANDLE hProcess,
									    NTSTATUS ExitStatus);
typedef NTSTATUS (NTAPI *_NtUnmapViewOfSection)( HANDLE ProcessHandle,
										PVOID BaseAddress );
typedef NTSTATUS (NTAPI *_NtCLOSE) ( HANDLE ); 
typedef NTSTATUS (NTAPI *_NtAllocateVirtualMemory)(HANDLE ProcessHandle,
										PVOID *BaseAddress,
										ULONG_PTR ZeroBits,
										PSIZE_T RegionSize,
										ULONG AllocationType,
										ULONG Protect);
typedef NTSTATUS (NTAPI *_NtFreeVirtualMemory)(HANDLE ProcessHandle,
										PVOID *BaseAddress,
										PSIZE_T RegionSize,
										ULONG FreeType);
typedef NTSTATUS (NTAPI *_NtWriteVirtualMemory)(IN HANDLE ProcessHandle,
										IN PVOID BaseAddress,
										IN PVOID Buffer, 
										IN SIZE_T NumberOfBytesToWrite,
										OUT PSIZE_T NumberOfBytesWritten);
typedef NTSTATUS (NTAPI *_NtProtectVirtualMemory) (HANDLE, PVOID, PULONG, ULONG , PULONG);
typedef NTSTATUS (NTAPI *_NtCreateThreadEx)(OUT PHANDLE ThreadHandle,
										IN ACCESS_MASK DesiredAccess,
										IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
										IN HANDLE ProcessHandle,
										IN PTHREAD_START_ROUTINE StartRoutine,
										IN PVOID StartContext,
										IN DWORD CreateThreadFlags,
										IN ULONG ZeroBits OPTIONAL,
										IN ULONG_PTR StackSize OPTIONAL,
										IN ULONG_PTR MaximumStackSize OPTIONAL,
										IN NT_PROC_THREAD_ATTRIBUTE_LIST* AttributeList);
typedef NTSTATUS (NTAPI *_NtCreateProcessEx)(OUT PHANDLE ProcessHandle,
										IN ACCESS_MASK DesiredAccess,
										IN POBJECT_ATTRIBUTES ObjectAttributes,
										IN HANDLE ParentProcess,
										IN ULONG Flags,
										IN HANDLE SectionHandle,
										IN HANDLE DebugPort,
										IN HANDLE ExceptionPort,
										IN BOOLEAN InJob);
typedef NTSTATUS (WINAPI *_NtQueryInformationProcess)(HANDLE ProcessHandle,
										PROCESSINFOCLASS ProcessInformationClass,
										PVOID ProcessInformation,
										ULONG ProcessInformationLength,
										PULONG ReturnLength);
typedef	 NTSTATUS (NTAPI *_NtOpenProcess)(PHANDLE ProcessHandle,
										ACCESS_MASK DesiredAccess,	
										POBJECT_ATTRIBUTES ObjectAttributes,
										PCLIENT_ID ClientId);
typedef NTSTATUS (NTAPI *_NtOpenThread)(PHANDLE ProcessHandle,
										ACCESS_MASK DesiredAccess,	
										POBJECT_ATTRIBUTES ObjectAttributes,
										PCLIENT_ID ClientId);
typedef NTSTATUS (NTAPI *_NtQuerySystemInformation)(SYSTEM_INFORMATION_CLASS ,PVOID SystemInformation,ULONG SystemInformationLength,PULONG ReturnLength);
typedef NTSTATUS (NTAPI *_NtReadVirtualMemory)(IN HANDLE ProcessHandle,
										IN  LPCVOID BaseAddress,
										OUT PVOID Buffer,
										IN SIZE_T NumberOfBytesToRead,
										OUT SIZE_T* NumberOfBytesRead);
typedef NTSTATUS (NTAPI *_RtlCreateUserThread)(IN HANDLE ProcessHandle,
										IN PSECURITY_DESCRIPTOR SecurityDescriptor OPTIONAL,
										IN BOOLEAN CreateSuspended,
										IN ULONG StackZeroBits,
										IN SIZE_T StackReserve,
										IN SIZE_T StackCommit,
										IN PVOID StartAddress,
										IN PVOID Parameter,
										OUT PHANDLE ThreadHandle,
										OUT PCLIENT_ID ClientId);
typedef ULONG (WINAPI *_RtlNtStatusToDosError)(NTSTATUS Status);
typedef BOOL  (WINAPI *_CreateProcessInternalW)(HANDLE hToken,
										LPCWSTR lpApplicationName,
										LPWSTR lpCommandLine,
										LPSECURITY_ATTRIBUTES lpProcessAttributes,
										LPSECURITY_ATTRIBUTES lpThreadAttributes,
										BOOL bInheritHandles,
										DWORD dwCreationFlags,
										LPVOID lpEnvironment,
										LPCWSTR lpCurrentDirectory,
										LPSTARTUPINFOW lpStartupInfo,
										LPPROCESS_INFORMATION lpProcessInformation,
										PHANDLE hNewToken);
typedef NTSTATUS (NTAPI *_NtSuspendThread)(IN HANDLE ThreadHandle,
										OUT PULONG PreviousSuspendCount);
typedef NTSTATUS (NTAPI *_NtResumeThread)(IN HANDLE ThreadHandle,
										OUT PULONG SuspendCount);
typedef HMODULE  (WINAPI *_NtLoadLibraryExW)(LPCWSTR lpFileName,
									    HANDLE hFile, 
									    DWORD dwFlags);
typedef NTSTATUS (NTAPI *_NtLdrpProcessImportDirectory)(PLDR_DATA_TABLE_ENTRY Module,
										PLDR_DATA_TABLE_ENTRY ImportedModule,
										PCHAR ImportedName);

#endif  /* _HEAD_ER_H_ */
