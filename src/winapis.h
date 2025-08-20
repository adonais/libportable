#ifndef _WIN_APIS_H_
#  define _WIN_APIS_H_

#include <windows.h>
#include <shlwapi.h>
#include <psapi.h>

#if defined(__GNUC__) || defined(__GNUG__)
#  define __MINGW_EXTENSION __extension__
#else
#  define __C89_NAMELESS
#endif

#ifndef __C89_NAMELESS
#  define __C89_NAMELESS __MINGW_EXTENSION
#endif

#if !defined (_NTDEF_) && !defined (_NTSTATUS_PSDK)
#  define _NTSTATUS_PSDK
typedef LONG NTSTATUS, *PNTSTATUS;
#endif

#define CREATE_PROCESS_BREAKAWAY_FROM_JOB   0x0001
#define CREATE_PROCESS_INHERIT_HANDLES      0x0004
#define CREATE_PROCESS_PROTECTED            0x0040

#define CREATE_THREAD_SUSPENDED             0x0001
#define STATUS_ERROR                        ((NTSTATUS)0x80070000L)
#define NT_SUCCESS(Status)                  (((NTSTATUS)(Status)) >= 0)
#define STATUS_SUCCESS                      ((NTSTATUS)0x00000000)
#define STATUS_OBJECT_PATH_NOT_FOUND        ((NTSTATUS)0xC000003A)
#define NtCurrentProcess()                  ((HANDLE)(LONG_PTR) -1)
#define STATUS_END_OF_FILE                  ((NTSTATUS)0xC0000011)
#ifndef STATUS_PENDING
#define STATUS_PENDING                      ((NTSTATUS)0x00000103)
#endif

#define FILE_SUPERSEDE                      0x00000000
#define FILE_OPEN                           0x00000001
#define FILE_CREATE                         0x00000002
#define FILE_OPEN_IF                        0x00000003
#define FILE_OVERWRITE                      0x00000004
#define FILE_OVERWRITE_IF                   0x00000005
#define FILE_MAXIMUM_DISPOSITION            0x00000005
#define FILE_SYNCHRONOUS_IO_NONALERT        0x00000020
#define FILE_NON_DIRECTORY_FILE             0x00000040

#ifndef ProcThreadAttributeValue
#define ProcThreadAttributeValue( p1, p2, p3, p4 ) \
        (((p1) & PROC_THREAD_ATTRIBUTE_NUMBER) | \
        ((p2 != FALSE) ? PROC_THREAD_ATTRIBUTE_THREAD : 0) | \
        ((p3 != FALSE) ? PROC_THREAD_ATTRIBUTE_INPUT : 0) | \
        ((p4 != FALSE) ? PROC_THREAD_ATTRIBUTE_ADDITIVE : 0))
#endif

#if defined(ProcThreadAttributeValue) && !defined(PROC_THREAD_ATTRIBUTE_PCLIENTID)
#define PROC_THREAD_ATTRIBUTE_PCLIENTID ProcThreadAttributeValue (8, FALSE, FALSE, FALSE)
#endif

#ifndef STATUS_INFO_LENGTH_MISMATCH
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004)
#endif

#ifndef __UNICODE_STRING_DEFINED
#define __UNICODE_STRING_DEFINED
  typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
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

/* Values for the Attributes member */
#define OBJ_INHERIT             0x00000002
#define OBJ_PERMANENT           0x00000010
#define OBJ_EXCLUSIVE           0x00000020
#define OBJ_CASE_INSENSITIVE    0x00000040
#define OBJ_OPENIF              0x00000080
#define OBJ_OPENLINK            0x00000100
#define OBJ_KERNEL_HANDLE       0x00000200
#define OBJ_FORCE_ACCESS_CHECK  0x00000400
#define OBJ_VALID_ATTRIBUTES    0x000007F2

/* Helper Macro */
#define InitializeObjectAttributes(p,n,a,r,s) { \
    (p)->Length = sizeof(OBJECT_ATTRIBUTES); \
    (p)->RootDirectory = (r); \
    (p)->Attributes = (a); \
    (p)->ObjectName = (n); \
    (p)->SecurityDescriptor = (s); \
    (p)->SecurityQualityOfService = NULL; \
}

typedef struct _IO_STATUS_BLOCK {
  __C89_NAMELESS union {
    NTSTATUS Status;
    PVOID Pointer;
  };
  ULONG_PTR Information;
} IO_STATUS_BLOCK,*PIO_STATUS_BLOCK;

typedef VOID (NTAPI *PIO_APC_ROUTINE)(PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,ULONG Reserved);

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

/* windows-internals-book:"Chapter 5" */
#ifndef __PS_CREATE_STATE_DEFINED
#define __PS_CREATE_STATE_DEFINED
    typedef enum _PS_CREATE_STATE
    {
        PsCreateInitialState,
        PsCreateFailOnFileOpen,
        PsCreateFailOnSectionCreate,
        PsCreateFailExeFormat,
        PsCreateFailMachineMismatch,
        PsCreateFailExeName, // Debugger specified
        PsCreateSuccess,
        PsCreateMaximumStates
    } PS_CREATE_STATE;
#endif

#ifndef __PS_CREATE_INFO_DEFINED
#define __PS_CREATE_INFO_DEFINED
    typedef struct _PS_CREATE_INFO
    {
        SIZE_T Size;
        PS_CREATE_STATE State;
        union
        {
            // PsCreateInitialState
            struct
            {
                union
                {
                    ULONG InitFlags;
                    struct
                    {
                        UCHAR WriteOutputOnExit : 1;
                        UCHAR DetectManifest : 1;
                        UCHAR IFEOSkipDebugger : 1;
                        UCHAR IFEODoNotPropagateKeyState : 1;
                        UCHAR SpareBits1 : 4;
                        UCHAR SpareBits2 : 8;
                        USHORT ProhibitedImageCharacteristics : 16;
                    } s1;
                } u1;
                ACCESS_MASK AdditionalFileAccess;
            } InitState;

            // PsCreateFailOnSectionCreate
            struct
            {
                HANDLE FileHandle;
            } FailSection;

            // PsCreateFailExeFormat
            struct
            {
                USHORT DllCharacteristics;
            } ExeFormat;

            // PsCreateFailExeName
            struct
            {
                HANDLE IFEOKey;
            } ExeName;

            // PsCreateSuccess
            struct
            {
                union
                {
                    ULONG OutputFlags;
                    struct
                    {
                        UCHAR ProtectedProcess : 1;
                        UCHAR AddressSpaceOverride : 1;
                        UCHAR DevOverrideEnabled : 1; // From Image File Execution Options
                        UCHAR ManifestDetected : 1;
                        UCHAR ProtectedProcessLight : 1;
                        UCHAR SpareBits1 : 3;
                        UCHAR SpareBits2 : 8;
                        USHORT SpareBits3 : 16;
                    } s2;
                } u2;
                HANDLE FileHandle;
                HANDLE SectionHandle;
                ULONGLONG UserProcessParametersNative;
                ULONG UserProcessParametersWow64;
                ULONG CurrentParameterFlags;
                ULONGLONG PebAddressNative;
                ULONG PebAddressWow64;
                ULONGLONG ManifestAddress;
                ULONG ManifestSize;
            } SuccessState;
        };
    } PS_CREATE_INFO, * PPS_CREATE_INFO;
#endif

#ifdef __cplusplus
    typedef enum _PRIORITY_CLASS : UCHAR
    {
        Undefined,
        Idle,
        Normal,
        High,
        Realtime,
        BelowNormal,
        AboveNormal
    } PRIORITY_CLASS;
#else
    typedef UCHAR PRIORITY_CLASS;
#endif

    typedef struct _PROCESS_PRIORITY_CLASS
    {
        BOOLEAN Foreground;
        PRIORITY_CLASS PriorityClass;
    } PROCESS_PRIORITY_CLASS, * PPROCESS_PRIORITY_CLASS;

#ifndef __PS_ATTRIBUTE_DEFINED
#define __PS_ATTRIBUTE_DEFINED
    typedef struct _PS_ATTRIBUTE
    {
        ULONG_PTR Attribute;                // PROC_THREAD_ATTRIBUTE_XXX | PROC_THREAD_ATTRIBUTE_XXX modifiers, see ProcThreadAttributeValue macro and Windows Internals 6 (372)
        SIZE_T Size;                        // Size of Value or *ValuePtr
        union
        {
            ULONG_PTR Value;                // Reserve 8 bytes for data (such as a Handle or a data pointer)
            PVOID ValuePtr;                 // data pointer
        };
        PSIZE_T ReturnLength;               // Either 0 or specifies size of data returned to caller via "ValuePtr"
    } PS_ATTRIBUTE, * PPS_ATTRIBUTE;

#endif

#ifndef __PS_ATTRIBUTE_LIST_DEFINED
#define __PS_ATTRIBUTE_LIST_DEFINED
    typedef struct _PS_ATTRIBUTE_LIST
    {
        SIZE_T TotalLength;                 // sizeof(PS_ATTRIBUTE_LIST)
        PS_ATTRIBUTE Attributes[2];         // Depends on how many attribute entries should be supplied to NtCreateUserProcess
    } PS_ATTRIBUTE_LIST, * PPS_ATTRIBUTE_LIST;
#endif

#ifndef __STRING
#define __STRING
typedef struct _STRING
{
    WORD Length;
    WORD MaximumLength;
    CHAR *Buffer;
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

typedef struct _CURDIR
{
    UNICODE_STRING DosPath;
    PVOID Handle;
} CURDIR, *PCURDIR;

#ifndef __RTL_DRIVE_LETTER_CURDIR
#define __RTL_DRIVE_LETTER_CURDIR
typedef struct _RTL_DRIVE_LETTER_CURDIR {
    USHORT          Flags;
    USHORT          Length;
    ULONG           TimeStamp;
    UNICODE_STRING  DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;
#endif

#define RTL_MAX_DRIVE_LETTERS 32
#define RTL_DRIVE_LETTER_VALID (USHORT)0x0001

#ifndef __RTL_USER_PROCESS_PARAMETERS
#define __RTL_USER_PROCESS_PARAMETERS
typedef struct _RTL_USER_PROCESS_PARAMETERS
{
    ULONG MaximumLength;
    ULONG Length;

    ULONG Flags;
    ULONG DebugFlags;

    HANDLE ConsoleHandle;
    ULONG ConsoleFlags;
    HANDLE StandardInput;
    HANDLE StandardOutput;
    HANDLE StandardError;

    CURDIR CurrentDirectory;
    UNICODE_STRING DllPath;
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
    PWCHAR Environment;

    ULONG StartingX;
    ULONG StartingY;
    ULONG CountX;
    ULONG CountY;
    ULONG CountCharsX;
    ULONG CountCharsY;
    ULONG FillAttribute;

    ULONG WindowFlags;
    ULONG ShowWindowFlags;
    UNICODE_STRING WindowTitle;
    UNICODE_STRING DesktopInfo;
    UNICODE_STRING ShellInfo;
    UNICODE_STRING RuntimeData;
    RTL_DRIVE_LETTER_CURDIR CurrentDirectories[RTL_MAX_DRIVE_LETTERS];

    ULONG_PTR EnvironmentSize;
    ULONG_PTR EnvironmentVersion;
    PVOID PackageDependencyData;
    ULONG ProcessGroupId;
    ULONG LoaderThreads;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

#endif

#define RTL_USER_PROCESS_PARAMETERS_NORMALIZED              0x01
#define RTL_USER_PROCESS_PARAMETERS_PROFILE_USER            0x02
#define RTL_USER_PROCESS_PARAMETERS_PROFILE_KERNEL          0x04
#define RTL_USER_PROCESS_PARAMETERS_PROFILE_SERVER          0x08
#define RTL_USER_PROCESS_PARAMETERS_RESERVE_1MB             0x20
#define RTL_USER_PROCESS_PARAMETERS_RESERVE_16MB            0x40
#define RTL_USER_PROCESS_PARAMETERS_CASE_SENSITIVE          0x80
#define RTL_USER_PROCESS_PARAMETERS_DISABLE_HEAP_DECOMMIT   0x100
#define RTL_USER_PROCESS_PARAMETERS_DLL_REDIRECTION_LOCAL   0x1000
#define RTL_USER_PROCESS_PARAMETERS_APP_MANIFEST_PRESENT    0x2000
#define RTL_USER_PROCESS_PARAMETERS_IMAGE_KEY_MISSING       0x4000
#define RTL_USER_PROCESS_PARAMETERS_NX_OPTIN                0x20000

typedef struct PEB_FREE_BLOCK *_PEB_FREE_BLOCK;
typedef struct _PEB_FREE_BLOCK {
    _PEB_FREE_BLOCK *Next;
    ULONG Size;
} PEB_FREE_BLOCK;

typedef struct _PEB_LDR_DATA
{
    ULONG               Length;
    BOOLEAN             Initialized;
    PVOID               SsHandle;
    LIST_ENTRY          InLoadOrderModuleList;
    LIST_ENTRY          InMemoryOrderModuleList;
    LIST_ENTRY          InInitializationOrderModuleList;
    PVOID               EntryInProgress;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct tagRTL_BITMAP {
    ULONG  SizeOfBitMap; /* Number of bits in the bitmap */
    PULONG Buffer; /* Bitmap data, assumed sized to a DWORD boundary */
} RTL_BITMAP, *PRTL_BITMAP;

/* value for Flags field (FIXME: not the correct name) */
#define PROCESS_PARAMS_FLAG_NORMALIZED 1

typedef struct _GDI_TEB_BATCH
{
    ULONG  Offset;
    HANDLE HDC;
    ULONG  Buffer[0x136];
} GDI_TEB_BATCH;

typedef struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME
{
    struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME *Previous;
    struct _ACTIVATION_CONTEXT                 *ActivationContext;
    ULONG                                       Flags;
} RTL_ACTIVATION_CONTEXT_STACK_FRAME, *PRTL_ACTIVATION_CONTEXT_STACK_FRAME;

typedef struct _ACTIVATION_CONTEXT_STACK
{
    ULONG                               Flags;
    ULONG                               NextCookieSequenceNumber;
    RTL_ACTIVATION_CONTEXT_STACK_FRAME *ActiveFrame;
    LIST_ENTRY                          FrameListCache;
} ACTIVATION_CONTEXT_STACK, *PACTIVATION_CONTEXT_STACK;

typedef struct _PEB
{                                                                 /* win32/win64 */
    BOOLEAN                      InheritedAddressSpace;             /* 000/000 */
    BOOLEAN                      ReadImageFileExecOptions;          /* 001/001 */
    BOOLEAN                      BeingDebugged;                     /* 002/002 */
    BOOLEAN                      SpareBool;                         /* 003/003 */
    HANDLE                       Mutant;                            /* 004/008 */
    HMODULE                      ImageBaseAddress;                  /* 008/010 */
    PPEB_LDR_DATA                LdrData;                           /* 00c/018 */
    RTL_USER_PROCESS_PARAMETERS *ProcessParameters;                 /* 010/020 */
    PVOID                        SubSystemData;                     /* 014/028 */
    HANDLE                       ProcessHeap;                       /* 018/030 */
    PRTL_CRITICAL_SECTION        FastPebLock;                       /* 01c/038 */
    PVOID /*PPEBLOCKROUTINE*/    FastPebLockRoutine;                /* 020/040 */
    PVOID /*PPEBLOCKROUTINE*/    FastPebUnlockRoutine;              /* 024/048 */
    ULONG                        EnvironmentUpdateCount;            /* 028/050 */
    PVOID                        KernelCallbackTable;               /* 02c/058 */
    ULONG                        Reserved[2];                       /* 030/060 */
    PVOID /*PPEB_FREE_BLOCK*/    FreeList;                          /* 038/068 */
    ULONG                        TlsExpansionCounter;               /* 03c/070 */
    PRTL_BITMAP                  TlsBitmap;                         /* 040/078 */
    ULONG                        TlsBitmapBits[2];                  /* 044/080 */
    PVOID                        ReadOnlySharedMemoryBase;          /* 04c/088 */
    PVOID                        ReadOnlySharedMemoryHeap;          /* 050/090 */
    PVOID                       *ReadOnlyStaticServerData;          /* 054/098 */
    PVOID                        AnsiCodePageData;                  /* 058/0a0 */
    PVOID                        OemCodePageData;                   /* 05c/0a8 */
    PVOID                        UnicodeCaseTableData;              /* 060/0b0 */
    ULONG                        NumberOfProcessors;                /* 064/0b8 */
    ULONG                        NtGlobalFlag;                      /* 068/0bc */
    LARGE_INTEGER                CriticalSectionTimeout;            /* 070/0c0 */
    SIZE_T                       HeapSegmentReserve;                /* 078/0c8 */
    SIZE_T                       HeapSegmentCommit;                 /* 07c/0d0 */
    SIZE_T                       HeapDeCommitTotalFreeThreshold;    /* 080/0d8 */
    SIZE_T                       HeapDeCommitFreeBlockThreshold;    /* 084/0e0 */
    ULONG                        NumberOfHeaps;                     /* 088/0e8 */
    ULONG                        MaximumNumberOfHeaps;              /* 08c/0ec */
    PVOID                       *ProcessHeaps;                      /* 090/0f0 */
    PVOID                        GdiSharedHandleTable;              /* 094/0f8 */
    PVOID                        ProcessStarterHelper;              /* 098/100 */
    PVOID                        GdiDCAttributeList;                /* 09c/108 */
    PVOID                        LoaderLock;                        /* 0a0/110 */
    ULONG                        OSMajorVersion;                    /* 0a4/118 */
    ULONG                        OSMinorVersion;                    /* 0a8/11c */
    ULONG                        OSBuildNumber;                     /* 0ac/120 */
    ULONG                        OSPlatformId;                      /* 0b0/124 */
    ULONG                        ImageSubSystem;                    /* 0b4/128 */
    ULONG                        ImageSubSystemMajorVersion;        /* 0b8/12c */
    ULONG                        ImageSubSystemMinorVersion;        /* 0bc/130 */
    ULONG                        ImageProcessAffinityMask;          /* 0c0/134 */
    HANDLE                       GdiHandleBuffer[28];               /* 0c4/138 */
    ULONG                        unknown[6];                        /* 134/218 */
    PVOID                        PostProcessInitRoutine;            /* 14c/230 */
    PRTL_BITMAP                  TlsExpansionBitmap;                /* 150/238 */
    ULONG                        TlsExpansionBitmapBits[32];        /* 154/240 */
    ULONG                        SessionId;                         /* 1d4/2c0 */
    ULARGE_INTEGER               AppCompatFlags;                    /* 1d8/2c8 */
    ULARGE_INTEGER               AppCompatFlagsUser;                /* 1e0/2d0 */
    PVOID                        ShimData;                          /* 1e8/2d8 */
    PVOID                        AppCompatInfo;                     /* 1ec/2e0 */
    UNICODE_STRING               CSDVersion;                        /* 1f0/2e8 */
    PVOID                        ActivationContextData;             /* 1f8/2f8 */
    PVOID                        ProcessAssemblyStorageMap;         /* 1fc/300 */
    PVOID                        SystemDefaultActivationData;       /* 200/308 */
    PVOID                        SystemAssemblyStorageMap;          /* 204/310 */
    SIZE_T                       MinimumStackCommit;                /* 208/318 */
    PVOID                       *FlsCallback;                       /* 20c/320 */
    LIST_ENTRY                   FlsListHead;                       /* 210/328 */
    PRTL_BITMAP                  FlsBitmap;                         /* 218/338 */
    ULONG                        FlsBitmapBits[4];                  /* 21c/340 */
} PEB, *PPEB;

/***********************************************************************
 * TEB data structure
 */
typedef struct _TEB
{                                                                 /* win32/win64 */
    NT_TIB                       Tib;                               /* 000/0000 */
    PVOID                        EnvironmentPointer;                /* 01c/0038 */
    CLIENT_ID                    ClientId;                          /* 020/0040 */
    PVOID                        ActiveRpcHandle;                   /* 028/0050 */
    PVOID                        ThreadLocalStoragePointer;         /* 02c/0058 */
    PPEB                         Peb;                               /* 030/0060 */
    ULONG                        LastErrorValue;                    /* 034/0068 */
    ULONG                        CountOfOwnedCriticalSections;      /* 038/006c */
    PVOID                        CsrClientThread;                   /* 03c/0070 */
    PVOID                        Win32ThreadInfo;                   /* 040/0078 */
    ULONG                        Win32ClientInfo[31];               /* 044/0080 used for user32 private data in Wine */
    PVOID                        WOW32Reserved;                     /* 0c0/0100 */
    ULONG                        CurrentLocale;                     /* 0c4/0108 */
    ULONG                        FpSoftwareStatusRegister;          /* 0c8/010c */
    PVOID                        SystemReserved1[54];               /* 0cc/0110 used for kernel32 private data in Wine */
    LONG                         ExceptionCode;                     /* 1a4/02c0 */
    ACTIVATION_CONTEXT_STACK     ActivationContextStack;            /* 1a8/02c8 */
    BYTE                         SpareBytes1[24];                   /* 1bc/02e8 used for ntdll private data in Wine */
    PVOID                        SystemReserved2[10];               /* 1d4/0300 used for ntdll private data in Wine */
    GDI_TEB_BATCH                GdiTebBatch;                       /* 1fc/0350 used for vm86 private data in Wine */
    HANDLE                       gdiRgn;                            /* 6dc/0838 */
    HANDLE                       gdiPen;                            /* 6e0/0840 */
    HANDLE                       gdiBrush;                          /* 6e4/0848 */
    CLIENT_ID                    RealClientId;                      /* 6e8/0850 */
    HANDLE                       GdiCachedProcessHandle;            /* 6f0/0860 */
    ULONG                        GdiClientPID;                      /* 6f4/0868 */
    ULONG                        GdiClientTID;                      /* 6f8/086c */
    PVOID                        GdiThreadLocaleInfo;               /* 6fc/0870 */
    ULONG                        UserReserved[5];                   /* 700/0878 */
    PVOID                        glDispachTable[280];               /* 714/0890 */
    PVOID                        glReserved1[26];                   /* b74/1150 */
    PVOID                        glReserved2;                       /* bdc/1220 */
    PVOID                        glSectionInfo;                     /* be0/1228 */
    PVOID                        glSection;                         /* be4/1230 */
    PVOID                        glTable;                           /* be8/1238 */
    PVOID                        glCurrentRC;                       /* bec/1240 */
    PVOID                        glContext;                         /* bf0/1248 */
    ULONG                        LastStatusValue;                   /* bf4/1250 */
    UNICODE_STRING               StaticUnicodeString;               /* bf8/1258 used by advapi32 */
    WCHAR                        StaticUnicodeBuffer[261];          /* c00/1268 used by advapi32 */
    PVOID                        DeallocationStack;                 /* e0c/1478 */
    PVOID                        TlsSlots[64];                      /* e10/1480 */
    LIST_ENTRY                   TlsLinks;                          /* f10/1680 */
    PVOID                        Vdm;                               /* f18/1690 */
    PVOID                        ReservedForNtRpc;                  /* f1c/1698 */
    PVOID                        DbgSsReserved[2];                  /* f20/16a0 */
    ULONG                        HardErrorDisabled;                 /* f28/16b0 */
    PVOID                        Instrumentation[16];               /* f2c/16b8 */
    PVOID                        WinSockData;                       /* f6c/1738 */
    ULONG                        GdiBatchCount;                     /* f70/1740 */
    ULONG                        Spare2;                            /* f74/1744 */
    PVOID                        Spare3;                            /* f78/1748 */
    PVOID                        Spare4;                            /* f7c/1750 */
    PVOID                        ReservedForOle;                    /* f80/1758 */
    ULONG                        WaitingOnLoaderLock;               /* f84/1760 */
    PVOID                        Reserved5[3];                      /* f88/1768 used for wineserver shared memory */
    PVOID                       *TlsExpansionSlots;                 /* f94/1780 */
    ULONG                        ImpersonationLocale;               /* f98/1788 */
    ULONG                        IsImpersonating;                   /* f9c/178c */
    PVOID                        NlsCache;                          /* fa0/1790 */
    PVOID                        ShimData;                          /* fa4/1798 */
    ULONG                        HeapVirtualAffinity;               /* fa8/17a0 */
    PVOID                        CurrentTransactionHandle;          /* fac/17a8 */
    PVOID                        ActiveFrame;                       /* fb0/17b0 */
#ifdef _WIN64
    PVOID                        unknown[2];                        /*     17b8 */
#endif
    PVOID                       *FlsSlots;                          /* fb4/17c8 */
} TEB, *PTEB;

typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID Reserved1;
    PPEB PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;

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
    LIST_ENTRY      InLoadOrderModuleList;
    LIST_ENTRY      InMemoryOrderModuleList;
    LIST_ENTRY      InInitializationOrderModuleList;
    PVOID           BaseAddress;
    PVOID           EntryPoint;
    ULONG           SizeOfImage;
    UNICODE_STRING  FullDllName;
    UNICODE_STRING  BaseDllName;
    ULONG           Flags;
    SHORT           LoadCount;
    SHORT           TlsIndex;
    LIST_ENTRY      HashTableEntry;
    ULONG           TimeDateStamp;
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

/* This is used by NtQuerySystemInformation */
typedef struct _SYSTEM_THREAD_INFORMATION
{                                    /* win32/win64 */
    LARGE_INTEGER KernelTime;          /* 00/00 */
    LARGE_INTEGER UserTime;            /* 08/08 */
    LARGE_INTEGER CreateTime;          /* 10/10 */
    DWORD         dwTickCount;         /* 18/18 */
    LPVOID        StartAddress;        /* 1c/20 */
    CLIENT_ID     ClientId;            /* 20/28 */
    DWORD         dwCurrentPriority;   /* 28/38 */
    DWORD         dwBasePriority;      /* 2c/3c */
    DWORD         dwContextSwitches;   /* 30/40 */
    DWORD         dwThreadState;       /* 34/44 */
    DWORD         dwWaitReason;        /* 38/48 */
    DWORD         dwUnknown;           /* 3c/4c */
} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;
    BYTE Reserved1[52];
    PVOID Reserved2[3];
    HANDLE UniqueProcessId;
    PVOID Reserved3;
    ULONG HandleCount;
    BYTE Reserved4[4];
    PVOID Reserved5[11];
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER Reserved6[6];
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

// private
typedef enum _PS_ATTRIBUTE_NUM
{
    PsAttributeParentProcess, // in HANDLE
    PsAttributeDebugObject, // in HANDLE
    PsAttributeToken, // in HANDLE
    PsAttributeClientId, // out PCLIENT_ID
    PsAttributeTebAddress, // out PTEB *
    PsAttributeImageName, // in PWSTR
    PsAttributeImageInfo, // out PSECTION_IMAGE_INFORMATION
    PsAttributeMemoryReserve, // in PPS_MEMORY_RESERVE
    PsAttributePriorityClass, // in UCHAR
    PsAttributeErrorMode, // in ULONG
    PsAttributeStdHandleInfo, // in PPS_STD_HANDLE_INFO // 10
    PsAttributeHandleList, // in HANDLE[]
    PsAttributeGroupAffinity, // in PGROUP_AFFINITY
    PsAttributePreferredNode, // in PUSHORT
    PsAttributeIdealProcessor, // in PPROCESSOR_NUMBER
    PsAttributeUmsThread, // in PUMS_CREATE_THREAD_ATTRIBUTES
    PsAttributeMitigationOptions, // in PPS_MITIGATION_OPTIONS_MAP (PROCESS_CREATION_MITIGATION_POLICY_*) // since WIN8
    PsAttributeProtectionLevel, // in PS_PROTECTION // since WINBLUE
    PsAttributeSecureProcess, // in PPS_TRUSTLET_CREATE_ATTRIBUTES, since THRESHOLD
    PsAttributeJobList, // in HANDLE[]
    PsAttributeChildProcessPolicy, // in PULONG (PROCESS_CREATION_CHILD_PROCESS_*) // since THRESHOLD2 // 20
    PsAttributeAllApplicationPackagesPolicy, // in PULONG (PROCESS_CREATION_ALL_APPLICATION_PACKAGES_*) // since REDSTONE
    PsAttributeWin32kFilter, // in PWIN32K_SYSCALL_FILTER
    PsAttributeSafeOpenPromptOriginClaim, // in SE_SAFE_OPEN_PROMPT_RESULTS
    PsAttributeBnoIsolation, // in PPS_BNO_ISOLATION_PARAMETERS // since REDSTONE2
    PsAttributeDesktopAppPolicy, // in PULONG (PROCESS_CREATION_DESKTOP_APP_*)
    PsAttributeChpe, // in BOOLEAN // since REDSTONE3
    PsAttributeMitigationAuditOptions, // in PPS_MITIGATION_AUDIT_OPTIONS_MAP (PROCESS_CREATION_MITIGATION_AUDIT_POLICY_*) // since 21H1
    PsAttributeMachineType, // in USHORT // since 21H2
    PsAttributeComponentFilter, // in COMPONENT_FILTER
    PsAttributeEnableOptionalXStateFeatures, // in ULONG64 // since WIN11 // 30
    PsAttributeSupportedMachines, // in ULONG // since 24H2
    PsAttributeSveVectorLength, // PPS_PROCESS_CREATION_SVE_VECTOR_LENGTH
    PsAttributeMax
} PS_ATTRIBUTE_NUM;

// private
#define PS_ATTRIBUTE_NUMBER_MASK 0x0000ffff
#define PS_ATTRIBUTE_THREAD 0x00010000 // may be used with thread creation
#define PS_ATTRIBUTE_INPUT 0x00020000 // input only
#define PS_ATTRIBUTE_ADDITIVE 0x00040000 // "accumulated" e.g. bitmasks, counters, etc.

#define PsAttributeValue(Number, Thread, Input, Additive) \
    (((Number) & PS_ATTRIBUTE_NUMBER_MASK) | \
    ((Thread) ? PS_ATTRIBUTE_THREAD : 0) | \
    ((Input) ? PS_ATTRIBUTE_INPUT : 0) | \
    ((Additive) ? PS_ATTRIBUTE_ADDITIVE : 0))

#define PS_ATTRIBUTE_IMAGE_NAME \
    PsAttributeValue(PsAttributeImageName, FALSE, TRUE, FALSE)

#undef NtCurrentPeb
#define NtCurrentPeb() (NtCurrentTeb()->Peb)
#define RtlProcessHeap() (NtCurrentPeb()->ProcessHeap)

typedef NTSTATUS (NTAPI *PRTL_HEAP_COMMIT_ROUTINE) (PVOID Base, PVOID *CommitAddress, PSIZE_T CommitSize);

typedef struct _RTL_HEAP_PARAMETERS {
  ULONG Length;
  SIZE_T SegmentReserve;
  SIZE_T SegmentCommit;
  SIZE_T DeCommitFreeBlockThreshold;
  SIZE_T DeCommitTotalFreeThreshold;
  SIZE_T MaximumAllocationSize;
  SIZE_T VirtualMemoryThreshold;
  SIZE_T InitialCommit;
  SIZE_T InitialReserve;
  PRTL_HEAP_COMMIT_ROUTINE CommitRoutine;
  SIZE_T Reserved[ 2 ];
} RTL_HEAP_PARAMETERS, *PRTL_HEAP_PARAMETERS;

typedef BOOLEAN (NTAPI *_RtlFreeHeap)(PVOID HeapHandle, ULONG Flags, PVOID HeapBase);
typedef PVOID (NTAPI *_RtlAllocateHeap)(PVOID HeapHandle, ULONG Flags, SIZE_T Size);
typedef PVOID (NTAPI *_RtlCreateHeap)(ULONG Flags, PVOID HeapBase, SIZE_T ReserveSize, SIZE_T CommitSize, PVOID Lock, PRTL_HEAP_PARAMETERS Parameters);
typedef PVOID (NTAPI *_RtlDestroyHeap)(PVOID HeapHandle);

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
                                        IN PPS_CREATE_INFO CreateInfo,
                                        IN PPS_ATTRIBUTE_LIST AttributeList);
typedef NTSTATUS (NTAPI *_NtTerminateProcess)(HANDLE hProcess, NTSTATUS ExitStatus);
typedef NTSTATUS (NTAPI *_NtUnmapViewOfSection)( HANDLE ProcessHandle, PVOID BaseAddress );
typedef NTSTATUS (NTAPI *_NtClose) ( HANDLE );
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
typedef DWORD    (WINAPI *PTHREAD_START_ROUTINE) (LPVOID lpThreadParameter);
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
typedef NTSTATUS (NTAPI *_NtOpenProcess)(PHANDLE ProcessHandle,
                                        ACCESS_MASK DesiredAccess,
                                        POBJECT_ATTRIBUTES ObjectAttributes,
                                        PCLIENT_ID ClientId);
typedef NTSTATUS (NTAPI *_NtOpenThread)(PHANDLE ProcessHandle,
                                        ACCESS_MASK DesiredAccess,
                                        POBJECT_ATTRIBUTES ObjectAttributes,
                                        PCLIENT_ID ClientId);
typedef NTSTATUS (NTAPI *_NtQuerySystemInformation)(SYSTEM_INFORMATION_CLASS ,
                                        PVOID SystemInformation,
                                        ULONG SystemInformationLength,
                                        PULONG ReturnLength);
typedef NTSTATUS (NTAPI *_NtReadVirtualMemory)(IN  HANDLE ProcessHandle,
                                        IN  LPCVOID BaseAddress,
                                        OUT PVOID Buffer,
                                        IN  SIZE_T NumberOfBytesToRead,
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
typedef NTSTATUS (NTAPI *_NtSuspendThread)(IN HANDLE ThreadHandle,OUT PULONG PreviousSuspendCount);
typedef NTSTATUS (NTAPI *_NtResumeThread)(IN HANDLE ThreadHandle,OUT PULONG SuspendCount);

typedef NTSTATUS (NTAPI *_NtLdrpProcessImportDirectory)(PLDR_DATA_TABLE_ENTRY Module,
                                        PLDR_DATA_TABLE_ENTRY ImportedModule,
                                        PCHAR ImportedName);
typedef NTSTATUS (NTAPI *_NtLdrLoadDll) (PWCHAR PathToFile,
                                        PULONG flags,
                                        PUNICODE_STRING ModuleFileName,
                                        PHANDLE ModuleHandle);
typedef VOID (NTAPI *_RtlInitUnicodeString)(PUNICODE_STRING DestinationString,
                                        PCWSTR SourceString);
typedef BOOL (NTAPI *_RtlDosPathNameToNtPathName_U)(PCWSTR DosPathName,
                                        PUNICODE_STRING NtPathName,
                                        PCWSTR *NtFileNamePart,
                                        VOID *DirectoryInfo);
typedef NTSTATUS (NTAPI *_RtlCreateProcessParametersEx)(PRTL_USER_PROCESS_PARAMETERS * pProcessParameters,
                                        PUNICODE_STRING ImagePathName,
                                        PUNICODE_STRING DllPath,
                                        PUNICODE_STRING CurrentDirectory,
                                        PUNICODE_STRING CommandLine,
                                        PVOID Environment,
                                        PUNICODE_STRING WindowTitle,
                                        PUNICODE_STRING DesktopInfo,
                                        PUNICODE_STRING ShellInfo,
                                        PUNICODE_STRING RuntimeData,
                                        ULONG Flags);
typedef NTSTATUS (NTAPI *_RtlDestroyProcessParameters)(PRTL_USER_PROCESS_PARAMETERS ProcessParameters);
typedef VOID (NTAPI *_RtlFreeUnicodeString)(PUNICODE_STRING DestinationString);
typedef NTSTATUS (NTAPI *_NtCreateFile)(PHANDLE FileHandle,
                                        ACCESS_MASK DesiredAccess,
                                        POBJECT_ATTRIBUTES ObjectAttributes,
                                        PIO_STATUS_BLOCK IoStatusBlock,
                                        PLARGE_INTEGER AllocationSize,
                                        ULONG FileAttributes,
                                        ULONG ShareAccess,
                                        ULONG CreateDisposition,
                                        ULONG CreateOptions,
                                        PVOID EaBuffer,
                                        ULONG EaLength
                                        );
typedef NTSTATUS (NTAPI *_NtReadFile)(HANDLE FileHandle,
                                      HANDLE Event,
                                      PIO_APC_ROUTINE ApcRoutine,
                                      PVOID ApcContext,
                                      PIO_STATUS_BLOCK IoStatusBlock,
                                      PVOID Buffer,
                                      ULONG Length,
                                      PLARGE_INTEGER ByteOffset,
                                      PULONG Key
                                      );
typedef NTSTATUS (NTAPI *_NtWriteFile)(HANDLE FileHandle,
                                       HANDLE Event,
                                       PIO_APC_ROUTINE ApcRoutine,
                                       PVOID ApcContext,
                                       PIO_STATUS_BLOCK IoStatusBlock,
                                       PVOID Buffer,
                                       ULONG Length,
                                       PLARGE_INTEGER ByteOffset,
                                       PULONG Key
                                       );
typedef NTSTATUS (NTAPI *_RtlCreateEnvironment)(BOOLEAN Inherit, PVOID *Environment );
typedef VOID (NTAPI *_RtlSetCurrentEnvironment)(PVOID NewEnvironment, PVOID *OldEnvironment);
typedef VOID (NTAPI *_RtlDestroyEnvironment)(PVOID Environment);

#endif  /* _WIN_APIS_H_ */
