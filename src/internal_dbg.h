/*
  DbgHelp API stuff.
  Unfortunately, dbghelp.h isn't included in MinGW. So all that's needed goes here.
 */

#ifndef __INTERNAL_DEBGHELPAPI_H__
#define __INTERNAL_DEBGHELPAPI_H__

#include <windows.h>

#if defined(__MINGW32__)
#include <stdint.h>
typedef uint64_t ULONG64;
#endif

typedef DWORD RVA;
typedef ULONG64 RVA64;

typedef enum _ADDRESS_MODE
{
  AddrMode1616,
  AddrMode1632,
  AddrModeReal,
  AddrModeFlat
} ADDRESS_MODE;

#if defined(_IMAGEHLP_SOURCE_) || !defined(_IMAGEHLP64)
typedef struct _tagADDRESS {
    DWORD         Offset;
    WORD          Segment;
    ADDRESS_MODE  Mode;
} ADDRESS, *LPADDRESS;
#else
typedef struct _tagADDRESS64 {
  DWORD64      Offset;
  WORD         Segment;
  ADDRESS_MODE Mode;
} ADDRESS64, *LPADDRESS64;
#define ADDRESS ADDRESS64
#define LPADDRESS LPADDRESS64
#endif

#if defined(_IMAGEHLP_SOURCE_) || !defined(_IMAGEHLP64)
typedef struct _KDHELP {
    DWORD   Thread;
    DWORD   ThCallbackStack;
    DWORD   NextCallback;
    DWORD   FramePointer;
    DWORD   KiCallUserMode;
    DWORD   KeUserCallbackDispatcher;
    DWORD   SystemRangeStart;
    DWORD   ThCallbackBStore;
    DWORD   KiUserExceptionDispatcher;
    DWORD   StackBase;
    DWORD   StackLimit;
    DWORD   Reserved[5];
} KDHELP, *PKDHELP;
#else
typedef struct _KDHELP64 {
  DWORD64 Thread;
  DWORD   ThCallbackStack;
  DWORD   ThCallbackBStore;
  DWORD   NextCallback;
  DWORD   FramePointer;
  DWORD64 KiCallUserMode;
  DWORD64 KeUserCallbackDispatcher;
  DWORD64 SystemRangeStart;
  DWORD64 KiUserExceptionDispatcher;
  DWORD64 StackBase;
  DWORD64 StackLimit;
  DWORD64 Reserved[5];
} KDHELP64, *PKDHELP64;
#define KDHELP KDHELP64
#define PKDHELP PKDHELP64
#endif

#if !defined(_IMAGEHLP_SOURCE_) || !defined(_IMAGEHLP64)
typedef struct _tagSTACKFRAME {
    ADDRESS     AddrPC;
    ADDRESS     AddrReturn;
    ADDRESS     AddrFrame;
    ADDRESS     AddrStack;
    PVOID       FuncTableEntry;
    DWORD       Params[4];
    BOOL        Far;
    BOOL        Virtual; 
    DWORD       Reserved[3];
    KDHELP      KdHelp;
    ADDRESS     AddrBStore; 
} STACKFRAME, *LPSTACKFRAME;
#else
typedef struct _tagSTACKFRAME64 {
  ADDRESS64 AddrPC;
  ADDRESS64 AddrReturn;
  ADDRESS64 AddrFrame;
  ADDRESS64 AddrStack;
  ADDRESS64 AddrBStore;
  PVOID     FuncTableEntry;
  DWORD64   Params[4];
  BOOL      Far;
  BOOL      Virtual;
  DWORD64   Reserved[3];
  KDHELP64  KdHelp;
} STACKFRAME64, *LPSTACKFRAME64;
#define STACKFRAME STACKFRAME64
#define LPSTACKFRAME LPSTACKFRAME64
#endif

typedef struct _SYMBOL_INFO {
  ULONG   SizeOfStruct;
  ULONG   TypeIndex;
  ULONG64 Reserved[2];
  ULONG   Index;
  ULONG   Size;
  ULONG64 ModBase;
  ULONG   Flags;
  ULONG64 Value;
  ULONG64 Address;
  ULONG   Register;
  ULONG   Scope;
  ULONG   Tag;
  ULONG   NameLen;
  ULONG   MaxNameLen;
  TCHAR   Name[1];
} SYMBOL_INFO, *PSYMBOL_INFO;

#if defined(_UNICODE) || defined(UNICODE)
#define SYMBOL_INFOW SYMBOL_INFO
#else
#define SYMBOL_INFOA SYMBOL_INFO
#endif

#define SYMFLAG_PARAMETER               0x00000040

#define SYMOPT_UNDNAME                  0x00000002
#define SYMOPT_DEFERRED_LOADS           0x00000004
#define SYMOPT_LOAD_LINES               0x00000010
#define SYMOPT_FAIL_CRITICAL_ERRORS     0x00000200

typedef enum _SYM_TYPE
{
    SymNone = 0,
    SymCoff,
    SymCv,
    SymPdb,
    SymExport,
    SymDeferred,
    SymSym,
    SymDia,
    SymVirtual,
    NumSymTypes
} SYM_TYPE;

typedef struct _IMAGEHLP_MODULE64 {
  DWORD    SizeOfStruct;
  DWORD64  BaseOfImage;
  DWORD    ImageSize;
  DWORD    TimeDateStamp;
  DWORD    CheckSum;
  DWORD    NumSyms;
  SYM_TYPE SymType;
  TCHAR    ModuleName[32];
  TCHAR    ImageName[256];
  TCHAR    LoadedImageName[256];
  TCHAR    LoadedPdbName[256];
  DWORD    CVSig;
  TCHAR    CVData[MAX_PATH*3];
  DWORD    PdbSig;
  GUID     PdbSig70;
  DWORD    PdbAge;
  BOOL     PdbUnmatched;
  BOOL     DbgUnmatched;
  BOOL     LineNumbers;
  BOOL     GlobalSymbols;
  BOOL     TypeInfo;
  BOOL     SourceIndexed;
  BOOL     Publics;
} IMAGEHLP_MODULE64, *PIMAGEHLP_MODULE64;
#if !defined(_IMAGEHLP_SOURCE_) && defined(_IMAGEHLP64)
#define IMAGEHLP_MODULE IMAGEHLP_MODULE64
#define PIMAGEHLP_MODULE PIMAGEHLP_MODULE64
#define IMAGEHLP_MODULEW IMAGEHLP_MODULEW64
#define PIMAGEHLP_MODULEW PIMAGEHLP_MODULEW64
#else
typedef struct _IMAGEHLP_MODULE {
    DWORD    SizeOfStruct;
    DWORD    BaseOfImage; 
    DWORD    ImageSize; 
    DWORD    TimeDateStamp; 
    DWORD    CheckSum; 
    DWORD    NumSyms; 
    SYM_TYPE SymType; 
    CHAR     ModuleName[32];  
    CHAR     ImageName[256]; 
    CHAR     LoadedImageName[256]; 
} IMAGEHLP_MODULE, *PIMAGEHLP_MODULE;

typedef struct _IMAGEHLP_MODULEW {
    DWORD    SizeOfStruct;  
    DWORD    BaseOfImage; 
    DWORD    ImageSize;  
    DWORD    TimeDateStamp; 
    DWORD    CheckSum; 
    DWORD    NumSyms; 
    SYM_TYPE SymType; 
    WCHAR    ModuleName[32]; 
    WCHAR    ImageName[256]; 
    WCHAR    LoadedImageName[256]; 
} IMAGEHLP_MODULEW, *PIMAGEHLP_MODULEW;
#endif

typedef struct _IMAGEHLP_LINE64 {
  DWORD   SizeOfStruct;
  PVOID   Key;
  DWORD   LineNumber;
  PTSTR   FileName;
  DWORD64 Address;
} IMAGEHLP_LINE64, *PIMAGEHLP_LINE64;

#if !defined(_IMAGEHLP_SOURCE_) && defined(_IMAGEHLP64)
#define IMAGEHLP_LINE IMAGEHLP_LINE64
#define PIMAGEHLP_LINE PIMAGEHLP_LINE64
#else
typedef struct _IMAGEHLP_LINE {
    DWORD    SizeOfStruct; 
    PVOID    Key; 
    DWORD    LineNumber; 
    PCHAR    FileName; 
    DWORD    Address; 
} IMAGEHLP_LINE, *PIMAGEHLP_LINE;

typedef struct _IMAGEHLP_LINEW {
    DWORD    SizeOfStruct; 
    PVOID    Key; 
    DWORD    LineNumber; 
    PCHAR    FileName; 
    DWORD64  Address; 
} IMAGEHLP_LINEW, *PIMAGEHLP_LINEW;
#endif

typedef struct _IMAGEHLP_STACK_FRAME {
  ULONG64 InstructionOffset;
  ULONG64 ReturnOffset;
  ULONG64 FrameOffset;
  ULONG64 StackOffset;
  ULONG64 BackingStoreOffset;
  ULONG64 FuncTableEntry;
  ULONG64 Params[4];
  ULONG64 Reserved[5];
  BOOL    Virtual;
  ULONG   Reserved2;
} IMAGEHLP_STACK_FRAME, *PIMAGEHLP_STACK_FRAME;

typedef void* PIMAGEHLP_CONTEXT;

typedef struct _MINIDUMP_EXCEPTION_INFORMATION {
  DWORD               ThreadId;
  PEXCEPTION_POINTERS ExceptionPointers;
  BOOL                ClientPointers;
} MINIDUMP_EXCEPTION_INFORMATION, *PMINIDUMP_EXCEPTION_INFORMATION;


typedef enum _MINIDUMP_CALLBACK_TYPE { 
  ModuleCallback,
  ThreadCallback,
  ThreadExCallback,
  IncludeThreadCallback,
  IncludeModuleCallback,
  MemoryCallback,
  CancelCallback,
  WriteKernelMinidumpCallback,
  KernelMinidumpStatusCallback,
  RemoveMemoryCallback,
  IncludeVmRegionCallback,
  IoStartCallback,
  IoWriteAllCallback,
  IoFinishCallback,
  ReadMemoryFailureCallback,
  SecondaryFlagsCallback
} MINIDUMP_CALLBACK_TYPE;

typedef struct MINIDUMP_THREAD_CALLBACK 
{  
  ULONG ThreadId;  
  HANDLE ThreadHandle;  
  CONTEXT Context;  
  ULONG SizeOfContext;  
  ULONG64 StackBase;  
  ULONG64 StackEnd;
} MINIDUMP_THREAD_CALLBACK, *PMINIDUMP_THREAD_CALLBACK;

typedef struct _MINIDUMP_THREAD_EX_CALLBACK {
  ULONG   ThreadId;
  HANDLE  ThreadHandle;
  CONTEXT Context;
  ULONG   SizeOfContext;
  ULONG64 StackBase;
  ULONG64 StackEnd;
  ULONG64 BackingStoreBase;
  ULONG64 BackingStoreEnd;
} MINIDUMP_THREAD_EX_CALLBACK, *PMINIDUMP_THREAD_EX_CALLBACK;

typedef struct MINIDUMP_MODULE_CALLBACK 
{  
  PWCHAR FullPath;  
  ULONG64 BaseOfImage;  
  ULONG SizeOfImage;  
  ULONG CheckSum;  
  ULONG TimeDateStamp;  
  VS_FIXEDFILEINFO VersionInfo;  
  PVOID CvRecord;  
  ULONG SizeOfCvRecord;  
  PVOID MiscRecord;  
  ULONG SizeOfMiscRecord;
} MINIDUMP_MODULE_CALLBACK, *PMINIDUMP_MODULE_CALLBACK;

typedef struct MINIDUMP_INCLUDE_THREAD_CALLBACK 
{  
  ULONG ThreadId;
} MINIDUMP_INCLUDE_THREAD_CALLBACK, *PMINIDUMP_INCLUDE_THREAD_CALLBACK;

typedef struct MINIDUMP_INCLUDE_MODULE_CALLBACK 
{  
  ULONG64 BaseOfImage;
} MINIDUMP_INCLUDE_MODULE_CALLBACK, *PMINIDUMP_INCLUDE_MODULE_CALLBACK;


typedef struct MINIDUMP_CALLBACK_INPUT       
{  
  ULONG ProcessId;  
  HANDLE ProcessHandle;  
  ULONG CallbackType;  
  union 
  {    
    MINIDUMP_THREAD_CALLBACK Thread;    
    MINIDUMP_THREAD_EX_CALLBACK ThreadEx;    
    MINIDUMP_MODULE_CALLBACK Module;    
    MINIDUMP_INCLUDE_THREAD_CALLBACK IncludeThread;    
    MINIDUMP_INCLUDE_MODULE_CALLBACK IncludeModule;  
  };
} MINIDUMP_CALLBACK_INPUT, *PMINIDUMP_CALLBACK_INPUT;

typedef enum _MODULE_WRITE_FLAGS { 
  ModuleWriteModule         = 0x0001,
  ModuleWriteDataSeg        = 0x0002,
  ModuleWriteMiscRecord     = 0x0004,
  ModuleWriteCvRecord       = 0x0008,
  ModuleReferencedByMemory  = 0x0010,
  ModuleWriteTlsData        = 0x0020,
  ModuleWriteCodeSegs       = 0x0040
} MODULE_WRITE_FLAGS;

typedef enum _THREAD_WRITE_FLAGS { 
  ThreadWriteThread             = 0x0001,
  ThreadWriteStack              = 0x0002,
  ThreadWriteContext            = 0x0004,
  ThreadWriteBackingStore       = 0x0008,
  ThreadWriteInstructionWindow  = 0x0010,
  ThreadWriteThreadData         = 0x0020,
  ThreadWriteThreadInfo         = 0x0040
} THREAD_WRITE_FLAGS;

typedef struct _MINIDUMP_USER_STREAM {
  ULONG32 Type;
  ULONG   BufferSize;
  PVOID   Buffer;
} MINIDUMP_USER_STREAM, *PMINIDUMP_USER_STREAM;

typedef struct _MINIDUMP_USER_STREAM_INFORMATION {
  ULONG                 UserStreamCount;
  PMINIDUMP_USER_STREAM UserStreamArray;
} MINIDUMP_USER_STREAM_INFORMATION, *PMINIDUMP_USER_STREAM_INFORMATION;

typedef struct _MINIDUMP_CALLBACK_OUTPUT 
{  
  union 
  {    
    ULONG ModuleWriteFlags;    
    ULONG ThreadWriteFlags;  
  };
} MINIDUMP_CALLBACK_OUTPUT, *PMINIDUMP_CALLBACK_OUTPUT;

typedef BOOL (CALLBACK* MINIDUMP_CALLBACK_ROUTINE) (PVOID CallbackParam, 
  const PMINIDUMP_CALLBACK_INPUT CallbackInput, 
  PMINIDUMP_CALLBACK_OUTPUT CallbackOutput);

typedef struct _MINIDUMP_CALLBACK_INFORMATION {
  MINIDUMP_CALLBACK_ROUTINE CallbackRoutine;
  PVOID                     CallbackParam;
} MINIDUMP_CALLBACK_INFORMATION, *PMINIDUMP_CALLBACK_INFORMATION;

typedef enum _MINIDUMP_TYPE { 
  MiniDumpNormal                          = 0x00000000,
  MiniDumpWithDataSegs                    = 0x00000001,
  MiniDumpWithFullMemory                  = 0x00000002,
  MiniDumpWithHandleData                  = 0x00000004,
  MiniDumpFilterMemory                    = 0x00000008,
  MiniDumpScanMemory                      = 0x00000010,
  MiniDumpWithUnloadedModules             = 0x00000020,
  MiniDumpWithIndirectlyReferencedMemory  = 0x00000040,
  MiniDumpFilterModulePaths               = 0x00000080,
  MiniDumpWithProcessThreadData           = 0x00000100,
  MiniDumpWithPrivateReadWriteMemory      = 0x00000200,
  MiniDumpWithoutOptionalData             = 0x00000400,
  MiniDumpWithFullMemoryInfo              = 0x00000800,
  MiniDumpWithThreadInfo                  = 0x00001000,
  MiniDumpWithCodeSegs                    = 0x00002000,
  MiniDumpWithoutAuxiliaryState           = 0x00004000,
  MiniDumpWithFullAuxiliaryState          = 0x00008000,
  MiniDumpWithPrivateWriteCopyMemory      = 0x00010000,
  MiniDumpIgnoreInaccessibleMemory        = 0x00020000,
  MiniDumpWithTokenInformation            = 0x00040000,
  MiniDumpWithModuleHeaders               = 0x00080000,
  MiniDumpFilterTriage                    = 0x00100000,
  MiniDumpValidTypeFlags                  = 0x001fffff
} MINIDUMP_TYPE;

typedef enum _MINIDUMP_STREAM_TYPE { 
  UnusedStream               = 0,
  ReservedStream0            = 1,
  ReservedStream1            = 2,
  ThreadListStream           = 3,
  ModuleListStream           = 4,
  MemoryListStream           = 5,
  ExceptionStream            = 6,
  SystemInfoStream           = 7,
  ThreadExListStream         = 8,
  Memory64ListStream         = 9,
  CommentStreamA             = 10,
  CommentStreamW             = 11,
  HandleDataStream           = 12,
  FunctionTableStream        = 13,
  UnloadedModuleListStream   = 14,
  MiscInfoStream             = 15,
  MemoryInfoListStream       = 16,
  ThreadInfoListStream       = 17,
  HandleOperationListStream  = 18,
  LastReservedStream         = 0xffff
} MINIDUMP_STREAM_TYPE;

typedef struct _MINIDUMP_LOCATION_DESCRIPTOR {
  ULONG32 DataSize;
  RVA     Rva;
} MINIDUMP_LOCATION_DESCRIPTOR;
typedef struct _MINIDUMP_LOCATION_DESCRIPTOR64 {
  ULONG64 DataSize;
  RVA64 Rva;
} MINIDUMP_LOCATION_DESCRIPTOR64;

typedef struct _MINIDUMP_MEMORY_DESCRIPTOR {
  ULONG64                      StartOfMemoryRange;
  MINIDUMP_LOCATION_DESCRIPTOR Memory;
} MINIDUMP_MEMORY_DESCRIPTOR, *PMINIDUMP_MEMORY_DESCRIPTOR;
typedef struct _MINIDUMP_MEMORY_DESCRIPTOR64 {
    ULONG64 StartOfMemoryRange;
    ULONG64 DataSize;
} MINIDUMP_MEMORY_DESCRIPTOR64, *PMINIDUMP_MEMORY_DESCRIPTOR64;

typedef struct _MINIDUMP_MEMORY_LIST {
  ULONG32                    NumberOfMemoryRanges;
  MINIDUMP_MEMORY_DESCRIPTOR MemoryRanges[];
} MINIDUMP_MEMORY_LIST, *PMINIDUMP_MEMORY_LIST;
typedef struct _MINIDUMP_MEMORY64_LIST {
    ULONG64 NumberOfMemoryRanges;
    RVA64 BaseRva;
    MINIDUMP_MEMORY_DESCRIPTOR64 MemoryRanges [0];
} MINIDUMP_MEMORY64_LIST, *PMINIDUMP_MEMORY64_LIST;

typedef struct _MINIDUMP_HEADER {
  ULONG32 Signature;
  ULONG32 Version;
  ULONG32 NumberOfStreams;
  RVA     StreamDirectoryRva;
  ULONG32 CheckSum;
  union {
    ULONG32 Reserved;
    ULONG32 TimeDateStamp;
  };
  ULONG64 Flags;
} MINIDUMP_HEADER, *PMINIDUMP_HEADER;

typedef struct _MINIDUMP_DIRECTORY {
  ULONG32                      StreamType;
  MINIDUMP_LOCATION_DESCRIPTOR Location;
} MINIDUMP_DIRECTORY, *PMINIDUMP_DIRECTORY;

typedef BOOL (WINAPI *_NtMiniDumpWriteDump)(HANDLE,DWORD,HANDLE,MINIDUMP_TYPE,
              PMINIDUMP_EXCEPTION_INFORMATION,
              PMINIDUMP_USER_STREAM_INFORMATION,
              PMINIDUMP_CALLBACK_INFORMATION);
#endif /* __INTERNAL_DEBGHELPAPI_H__ */
