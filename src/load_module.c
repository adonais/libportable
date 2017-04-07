#include "load_module.h"
#include "inipara.h"
#include <tchar.h>

#ifndef IMAGE_SIZEOF_BASE_RELOCATION
/* 某些版本没有定义IMAGE_SIZEOF_BASE_RELOCATION */
#define IMAGE_SIZEOF_BASE_RELOCATION (sizeof(IMAGE_BASE_RELOCATION))
#endif

#ifdef _WIN64
#define HOST_MACHINE IMAGE_FILE_MACHINE_AMD64
#else
#define HOST_MACHINE IMAGE_FILE_MACHINE_I386
#endif

typedef int (WINAPI *DllEntryPtr)(HINSTANCE hinstDLL, uint32_t fdwReason, LPVOID lpReserved);
typedef int (WINAPI *ExeEntryPtr)(void);

typedef struct 
{
    PIMAGE_NT_HEADERS headers;
    uint8_t *codeBase;
    HCUSTOMMODULE *modules;
    int numModules;
    bool initialized;
    bool isDLL;
    bool isRelocated;
    CustomAllocFunc alloc;
    CustomFreeFunc free;
    CustomLoadLibraryFunc loadLibrary;
    CustomGetProcAddressFunc getProcAddress;
    CustomFreeLibraryFunc freeLibrary;
    void *userdata;
    ExeEntryPtr exeEntry;
    uint32_t pageSize;
} MEMORYMODULE, *PMEMORYMODULE;

typedef struct 
{
    LPVOID address;
    LPVOID alignedAddress;
    SIZE_T size;
    uint32_t characteristics;
    bool last;
} SECTIONFINALIZEDATA, *PSECTIONFINALIZEDATA;

#define GET_HEADER_DICTIONARY(module, idx)  (&(module)->headers->OptionalHeader.DataDirectory[idx])

static LIB_INLINE uintptr_t
AlignValueDown(uintptr_t value, uintptr_t alignment) 
{
    return value & ~(alignment - 1);
}

static LIB_INLINE LPVOID
AlignAddressDown(LPVOID address, uintptr_t alignment) 
{
    return (LPVOID) AlignValueDown((uintptr_t) address, alignment);
}

static LIB_INLINE size_t
AlignValueUp(size_t value, size_t alignment) 
{
    return (value + alignment - 1) & ~(alignment - 1);
}

static LIB_INLINE void*
OffsetPointer(void* data, ptrdiff_t offset) 
{
    return (void*) ((uintptr_t) data + offset);
}

static LIB_INLINE bool
CheckSize(size_t size, size_t expected) 
{
    return (size >= expected);
}

static bool
CopySections(const uint8_t *data, size_t size, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module)
{
    int i, section_size;
    uint8_t *codeBase = module->codeBase;
    uint8_t *dest;
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
    for (i=0; i<module->headers->FileHeader.NumberOfSections; i++, section++) 
    {
        if (section->SizeOfRawData == 0) 
        {
            /* 区段中的数据是未初始化的 */
            section_size = old_headers->OptionalHeader.SectionAlignment;
            if (section_size > 0)
            {
                dest = (uint8_t *)module->alloc(codeBase + section->VirtualAddress,
                       section_size,
                       MEM_COMMIT,
                       PAGE_READWRITE,
                       module->userdata);
                if (dest == NULL) 
                {
                    return false;
                }

                /* 设置区段所在打开文件中的物理位置 */
                dest = codeBase + section->VirtualAddress;
                section->Misc.PhysicalAddress = (uint32_t) ((uintptr_t)  dest & 0xffffffff);
                fzero(dest, section_size);
            }

            /* 空区段 */
            continue;
        }

        if (!CheckSize(size, section->PointerToRawData + section->SizeOfRawData)) 
        {
            return false;
        }

        /* 申请内存,从dll拷贝数据 */
        dest = (uint8_t *)module->alloc(codeBase + section->VirtualAddress,
               section->SizeOfRawData,
               MEM_COMMIT,
               PAGE_READWRITE,
               module->userdata);
        if (dest == NULL) 
        {
            return false;
        }
        dest = codeBase + section->VirtualAddress;
        memcpy(dest, data + section->PointerToRawData, section->SizeOfRawData);
        section->Misc.PhysicalAddress = (uint32_t) ((uintptr_t) dest & 0xffffffff);
    }
    return true;
}

/* 内存页面属性 (Executable, Readable, Writeable) */
static int ProtectionFlags[2][2][2] = 
{
    {
        /* 不可执行 */
        {PAGE_NOACCESS, PAGE_WRITECOPY},
        {PAGE_READONLY, PAGE_READWRITE},
    }, 
    {
        /* 可执行 */
        {PAGE_EXECUTE, PAGE_EXECUTE_WRITECOPY},
        {PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE},
    },
};

static SIZE_T
GetRealSectionSize(PMEMORYMODULE module, PIMAGE_SECTION_HEADER section)
{
    uint32_t size = section->SizeOfRawData;
    if (size == 0) 
    {
        if (section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) 
        {
            size = module->headers->OptionalHeader.SizeOfInitializedData;
        } else if (section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) 
        {
            size = module->headers->OptionalHeader.SizeOfUninitializedData;
        }
    }
    return (SIZE_T) size;
}

static bool
FinalizeSection(PMEMORYMODULE module, PSECTIONFINALIZEDATA sectionData) 
{
    uint32_t protect, oldProtect;
    bool executable;
    bool readable;
    bool writeable;

    if (sectionData->size == 0) 
    {
        return true;
    }

    if (sectionData->characteristics & IMAGE_SCN_MEM_DISCARDABLE) 
    {
        /* 此区段已经不再需要,可以安全的释放 */
        if (sectionData->address == sectionData->alignedAddress &&
            (sectionData->last || module->headers->OptionalHeader.SectionAlignment == module->pageSize ||
            (sectionData->size % module->pageSize) == 0)
           ) 
        {
            /* 释放虚拟内存页面 */
            module->free(sectionData->address, sectionData->size, MEM_DECOMMIT, module->userdata);
        }
        return true;
    }

    /* 根据characteristics确定权限标志 */
    executable = (sectionData->characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
    readable =   (sectionData->characteristics & IMAGE_SCN_MEM_READ) != 0;
    writeable =  (sectionData->characteristics & IMAGE_SCN_MEM_WRITE) != 0;
    protect = ProtectionFlags[executable][readable][writeable];
    if (sectionData->characteristics & IMAGE_SCN_MEM_NOT_CACHED) 
    {
        protect |= PAGE_NOCACHE;
    }

    /* 更改内存权限标志 */
    if (VirtualProtect(sectionData->address, sectionData->size, protect, (PDWORD)&oldProtect) == 0) 
    {
    #ifdef _LOGDEBUG
        logmsg("Error protecting memory page\n");
    #endif
        return false;
    }

    return true;
}

static bool
FinalizeSections(PMEMORYMODULE module)
{
    int i;
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
#ifdef _WIN64
    uintptr_t imageOffset = ((uintptr_t)module->headers->OptionalHeader.ImageBase & 0xffffffff00000000);
#else
    static const uintptr_t imageOffset = 0;
#endif
    SECTIONFINALIZEDATA sectionData;
    sectionData.address = (LPVOID)((uintptr_t)section->Misc.PhysicalAddress | imageOffset);
    sectionData.alignedAddress = AlignAddressDown(sectionData.address, module->pageSize);
    sectionData.size = GetRealSectionSize(module, section);
    sectionData.characteristics = section->Characteristics;
    sectionData.last = false;
    section++;

    /* 遍历所有区段更改访问权限标记 */
    for (i=1; i<module->headers->FileHeader.NumberOfSections; i++, section++) 
    {
        LPVOID sectionAddress = (LPVOID)((uintptr_t)section->Misc.PhysicalAddress | imageOffset);
        LPVOID alignedAddress = AlignAddressDown(sectionAddress, module->pageSize);
        SIZE_T sectionSize = GetRealSectionSize(module, section);
        /* 合并共享页所有区段的的访问标志 */
        if (sectionData.alignedAddress == alignedAddress || (uintptr_t) sectionData.address + sectionData.size > (uintptr_t) alignedAddress) 
        {
            if ((section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) == 0 || (sectionData.characteristics & IMAGE_SCN_MEM_DISCARDABLE) == 0) 
            {
                sectionData.characteristics = (sectionData.characteristics | section->Characteristics) & ~IMAGE_SCN_MEM_DISCARDABLE;
            } else 
            {
                sectionData.characteristics |= section->Characteristics;
            }
            sectionData.size = (((uintptr_t)sectionAddress) + ((uintptr_t) sectionSize)) - (uintptr_t) sectionData.address;
            continue;
        }

        if (!FinalizeSection(module, &sectionData)) 
        {
            return false;
        }
        sectionData.address = sectionAddress;
        sectionData.alignedAddress = alignedAddress;
        sectionData.size = sectionSize;
        sectionData.characteristics = section->Characteristics;
    }
    sectionData.last = true;
    if (!FinalizeSection(module, &sectionData)) 
    {
        return false;
    }
    return true;
}

static bool
ExecuteTLS(PMEMORYMODULE module)
{
    uint8_t *codeBase = module->codeBase;
    PIMAGE_TLS_DIRECTORY tls;
    PIMAGE_TLS_CALLBACK* callback;

    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_TLS);
    if (directory->VirtualAddress == 0) 
    {
        return true;
    }

    tls = (PIMAGE_TLS_DIRECTORY) (codeBase + directory->VirtualAddress);
    callback = (PIMAGE_TLS_CALLBACK *) tls->AddressOfCallBacks;
    if (callback) 
    {
        while (*callback) 
        {
            (*callback)((LPVOID) codeBase, DLL_PROCESS_ATTACH, NULL);
            callback++;
        }
    }
    return true;
}

static bool
PerformBaseRelocation(PMEMORYMODULE module, ptrdiff_t delta)
{
    uint8_t *codeBase = module->codeBase;
    PIMAGE_BASE_RELOCATION relocation;

    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_BASERELOC);
    if (directory->Size == 0) 
    {
        return (delta == 0);
    }

    relocation = (PIMAGE_BASE_RELOCATION) (codeBase + directory->VirtualAddress);
    for (; relocation->VirtualAddress > 0; ) 
    {
        uint32_t i;
        uint8_t *dest = codeBase + relocation->VirtualAddress;
        uint16_t *relInfo = (uint16_t *)OffsetPointer(relocation, IMAGE_SIZEOF_BASE_RELOCATION);
        for (i=0; i<((relocation->SizeOfBlock-IMAGE_SIZEOF_BASE_RELOCATION) / 2); i++, relInfo++) 
        {
            /* 高4位定义了偏移类型 */
            int type = *relInfo >> 12;
            /* 低12位定义了需要重定位的地址 */
            int offset = *relInfo & 0xfff;

            switch (type)
            {
            case IMAGE_REL_BASED_ABSOLUTE:
                /* 跳过基址重定位 */
                break;

            case IMAGE_REL_BASED_HIGHLOW:
                /* VirtualAddress+offset,应用于此地址上全部32位 */
                {
                    uint32_t *patchAddrHL = (uint32_t *) (dest + offset);
                    *patchAddrHL += (uint32_t) delta;
                }
                break;

        #ifdef _WIN64
            case IMAGE_REL_BASED_DIR64:
                {
                    ULONGLONG *patchAddr64 = (ULONGLONG *) (dest + offset);
                    *patchAddr64 += (ULONGLONG) delta;
                }
                break;
        #endif

            default:
                break;
            }
        }
        FlushInstructionCache(GetCurrentProcess(), (LPCVOID) dest, module->pageSize);
        relocation = (PIMAGE_BASE_RELOCATION) OffsetPointer(relocation, relocation->SizeOfBlock);
    }
    return true;
}

static bool
BuildImportTable(PMEMORYMODULE module)
{
    uint8_t *codeBase = module->codeBase;
    PIMAGE_IMPORT_DESCRIPTOR importDesc;
    bool result = true;

    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_IMPORT);
    if (directory->Size == 0) 
    {
        return true;
    }

    importDesc = (PIMAGE_IMPORT_DESCRIPTOR) (codeBase + directory->VirtualAddress);
    for (; !IsBadReadPtr(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) && importDesc->Name; importDesc++) 
    {
        uintptr_t *thunkRef;
        FARPROC *funcRef;
        HCUSTOMMODULE *tmp;
        HCUSTOMMODULE handle = module->loadLibrary((LPCSTR) (codeBase + importDesc->Name), module->userdata);
        if (handle == NULL) 
        {
            SetLastError(ERROR_MOD_NOT_FOUND);
            result = false;
            break;
        }

        tmp = (HCUSTOMMODULE *) realloc(module->modules, (module->numModules+1)*(sizeof(HCUSTOMMODULE)));
        if (tmp == NULL) 
        {
            module->freeLibrary(handle, module->userdata);
            SetLastError(ERROR_OUTOFMEMORY);
            result = false;
            break;
        }
        module->modules = tmp;

        module->modules[module->numModules++] = handle;
        if (importDesc->OriginalFirstThunk) 
        {
            thunkRef = (uintptr_t *) (codeBase + importDesc->OriginalFirstThunk);
            funcRef = (FARPROC *) (codeBase + importDesc->FirstThunk);
        } else 
        {
            /* 没有hint表项 */
            thunkRef = (uintptr_t *) (codeBase + importDesc->FirstThunk);
            funcRef = (FARPROC *) (codeBase + importDesc->FirstThunk);
        }
        for (; *thunkRef; thunkRef++, funcRef++) 
        {
            if (IMAGE_SNAP_BY_ORDINAL(*thunkRef)) 
            {
                *funcRef = module->getProcAddress(handle, (LPCSTR)IMAGE_ORDINAL(*thunkRef), module->userdata);
            } else 
            {
                PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME) (codeBase + (*thunkRef));
                *funcRef = module->getProcAddress(handle, (LPCSTR)&thunkData->Name, module->userdata);
            }
            if (*funcRef == 0) 
            {
                result = false;
                break;
            }
        }

        if (!result) 
        {
            module->freeLibrary(handle, module->userdata);
            SetLastError(ERROR_PROC_NOT_FOUND);
            break;
        }
    }

    return result;
}

LPVOID memDefaultAlloc(LPVOID address, SIZE_T size, uint32_t allocationType, uint32_t protect, void* userdata)
{
	UNREFERENCED_PARAMETER(userdata);
	return VirtualAlloc(address, size, allocationType, protect);
}

int memDefaultFree(LPVOID lpAddress, SIZE_T dwSize, uint32_t dwFreeType, void* userdata)
{
	UNREFERENCED_PARAMETER(userdata);
	return VirtualFree(lpAddress, dwSize, dwFreeType);
}

HCUSTOMMODULE memDefaultLoadLibrary(LPCSTR filename, void *userdata)
{
    HMODULE result;
    UNREFERENCED_PARAMETER(userdata);
    result = LoadLibraryA(filename);
    if (result == NULL) 
    {
        return NULL;
    }

    return (HCUSTOMMODULE) result;
}

FARPROC memDefaultGetProcAddress(HCUSTOMMODULE module, LPCSTR name, void *userdata)
{
    UNREFERENCED_PARAMETER(userdata);
    return (FARPROC) GetProcAddress((HMODULE) module, name);
}

void memDefaultFreeLibrary(HCUSTOMMODULE module, void *userdata)
{
    UNREFERENCED_PARAMETER(userdata);
    FreeLibrary((HMODULE) module);
}

HMEMORYMODULE memLoadLibrary(const void *data, size_t size)
{
    return memLoadLibraryEx(data, size, memDefaultAlloc, memDefaultFree, memDefaultLoadLibrary, memDefaultGetProcAddress, memDefaultFreeLibrary, NULL);
}

HMEMORYMODULE memLoadLibraryEx(const void *data, size_t size,
    CustomAllocFunc allocMemory,
    CustomFreeFunc freeMemory,
    CustomLoadLibraryFunc loadLibrary,
    CustomGetProcAddressFunc getProcAddress,
    CustomFreeLibraryFunc freeLibrary,
    void *userdata)
{
    PMEMORYMODULE result = NULL;
    PIMAGE_DOS_HEADER dos_header;
    PIMAGE_NT_HEADERS old_header;
    uint8_t *code, *headers;
    ptrdiff_t locationDelta;
    SYSTEM_INFO sysInfo;
    PIMAGE_SECTION_HEADER section;
    uint32_t i;
    size_t optionalSectionSize;
    size_t lastSectionEnd = 0;
    size_t alignedImageSize;

    if (!CheckSize(size, sizeof(IMAGE_DOS_HEADER))) 
    {
        return NULL;
    }
    dos_header = (PIMAGE_DOS_HEADER)data;
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) 
    {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return NULL;
    }

    if (!CheckSize(size, dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS))) 
    {
        return NULL;
    }
    old_header = (PIMAGE_NT_HEADERS)&((const uint8_t *)(data))[dos_header->e_lfanew];
    if (old_header->Signature != IMAGE_NT_SIGNATURE) 
    {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return NULL;
    }

    if (old_header->FileHeader.Machine != HOST_MACHINE) 
    {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return NULL;
    }

    if (old_header->OptionalHeader.SectionAlignment & 1) 
    {
        /* 只支持对齐的区段,2的整数倍 */
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return NULL;
    }

    section = IMAGE_FIRST_SECTION(old_header);
    optionalSectionSize = old_header->OptionalHeader.SectionAlignment;
    for (i=0; i<old_header->FileHeader.NumberOfSections; i++, section++) 
    {
        size_t endOfSection;
        if (section->SizeOfRawData == 0) 
        {
            /* 区段内没有数据 */
            endOfSection = section->VirtualAddress + optionalSectionSize;
        } else 
        {
            endOfSection = section->VirtualAddress + section->SizeOfRawData;
        }

        if (endOfSection > lastSectionEnd) 
        {
            lastSectionEnd = endOfSection;
        }
    }

    GetNativeSystemInfo(&sysInfo);
    alignedImageSize = AlignValueUp(old_header->OptionalHeader.SizeOfImage, sysInfo.dwPageSize);
    if (alignedImageSize != AlignValueUp(lastSectionEnd, sysInfo.dwPageSize)) 
    {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return NULL;
    }

    /* 以dll的ImageBase映像地址分配虚拟内存空间,如果不这样做,运行dll入口函数时将发生异常 */
    code = (uint8_t *)allocMemory((LPVOID)(old_header->OptionalHeader.ImageBase),
           alignedImageSize,
           MEM_RESERVE | MEM_COMMIT,
           PAGE_READWRITE,
           userdata);

    if (code == NULL) 
    {
        /* 尝试在任意位置分配虚拟内存 */
        code = (uint8_t *)allocMemory(NULL,
               alignedImageSize,
               MEM_RESERVE | MEM_COMMIT,
               PAGE_READWRITE,
               userdata);
        if (code == NULL) 
        {
            SetLastError(ERROR_OUTOFMEMORY);
            return NULL;
        }
    }

    result = (PMEMORYMODULE)SYS_MALLOC(sizeof(MEMORYMODULE));
    if (result == NULL) 
    {
        freeMemory(code, 0, MEM_RELEASE, userdata);
        SetLastError(ERROR_OUTOFMEMORY);
        return NULL;
    }

    result->codeBase = code;
    result->isDLL = (old_header->FileHeader.Characteristics & IMAGE_FILE_DLL) != 0;
    result->alloc = allocMemory;
    result->free = freeMemory;
    result->loadLibrary = loadLibrary;
    result->getProcAddress = getProcAddress;
    result->freeLibrary = freeLibrary;
    result->userdata = userdata;
    result->pageSize = sysInfo.dwPageSize;

    if (!CheckSize(size, old_header->OptionalHeader.SizeOfHeaders)) 
    {
        goto error;
    }

    /* 为NT映像头分配保留的内存页空间 */
    headers = (uint8_t *)allocMemory(code,
              old_header->OptionalHeader.SizeOfHeaders,
              MEM_COMMIT,
              PAGE_READWRITE,
              userdata);

    /* 从dos_header开始,拷贝PE头到code地址 */
    memcpy(headers, dos_header, old_header->OptionalHeader.SizeOfHeaders);
    result->headers = (PIMAGE_NT_HEADERS)&((const uint8_t *)(headers))[dos_header->e_lfanew];

    /* 更新DLL在进程内部的映像地址 */
    result->headers->OptionalHeader.ImageBase = (uintptr_t)code;

    /* 拷贝各区段到新的内存位置 */
    if (!CopySections((const uint8_t *) data, size, old_header, result)) 
    {
        goto error;
    }

    /* 调整导入表数据的映像基址 */
    locationDelta = (ptrdiff_t)(result->headers->OptionalHeader.ImageBase - old_header->OptionalHeader.ImageBase);
    if (locationDelta != 0) 
    {
        result->isRelocated = PerformBaseRelocation(result, locationDelta);
    } else 
    {
        result->isRelocated = true;
    }

    /* 加载依赖的dll与重建输入表 */
    if (!BuildImportTable(result)) 
    {
        goto error;
    }

    /* 根据区段头标记内存页面,释放的区段标记为"废弃" */
    if (!FinalizeSections(result)) 
    {
        goto error;
    }

    /* 在dllmain运行之前,初始化tls */
    if (!ExecuteTLS(result)) 
    {
        goto error;
    }

    /* 获取动态库的入口指针 */
    if (result->headers->OptionalHeader.AddressOfEntryPoint != 0) 
    {
        if (result->isDLL) 
        {
            DllEntryPtr DllEntry = (DllEntryPtr)(LPVOID)(code + result->headers->OptionalHeader.AddressOfEntryPoint);
            /* 当DLL加载到进程时运行 */
            bool successfull = (*DllEntry)((HINSTANCE)code, DLL_PROCESS_ATTACH, 0);
            if (!successfull) 
            {
                SetLastError(ERROR_DLL_INIT_FAILED);
                goto error;
            }
            result->initialized = true;
        } else 
        {
            result->exeEntry = (ExeEntryPtr)(LPVOID)(code + result->headers->OptionalHeader.AddressOfEntryPoint);
        }
    } else 
    {
        result->exeEntry = NULL;
    }

    return (HMEMORYMODULE)result;

error:
    memFreeLibrary(result);
    return NULL;
}

FARPROC memGetProcAddress(HMEMORYMODULE module, LPCSTR name)
{
    uint8_t *codeBase = ((PMEMORYMODULE)module)->codeBase;
    uint32_t idx = 0;
    PIMAGE_EXPORT_DIRECTORY exports;
    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY((PMEMORYMODULE)module, IMAGE_DIRECTORY_ENTRY_EXPORT);
    if (directory->Size == 0) 
    {
        /* 没有输入表项 */
        SetLastError(ERROR_PROC_NOT_FOUND);
        return NULL;
    }

    exports = (PIMAGE_EXPORT_DIRECTORY) (codeBase + directory->VirtualAddress);
    if (exports->NumberOfNames == 0 || exports->NumberOfFunctions == 0) 
    {
        /* 输入表没有任何条目 */
        SetLastError(ERROR_PROC_NOT_FOUND);
        return NULL;
    }

    if (HIWORD(name) == 0) 
    {
        /* 通过序号查找函数 */
        if (LOWORD(name) < exports->Base) 
        {
            SetLastError(ERROR_PROC_NOT_FOUND);
            return NULL;
        }

        idx = LOWORD(name) - exports->Base;
    } else 
    {
        /* 通过函数名称查找函数 */
        uint32_t i;
        uint32_t *nameRef = (uint32_t *) (codeBase + exports->AddressOfNames);
        WORD *ordinal = (WORD *) (codeBase + exports->AddressOfNameOrdinals);
        bool found = false;
        for (i=0; i<exports->NumberOfNames; i++, nameRef++, ordinal++) 
        {
            if (_stricmp(name, (const char *) (codeBase + (*nameRef))) == 0) 
            {
                idx = *ordinal;
                found = true;
            #ifdef _LOGDEBUG
                logmsg("we have found \"%s\".", name);
            #endif
                break;
            }
        }

        if (!found) 
        {
            /* 函数名称没找到 */
            SetLastError(ERROR_PROC_NOT_FOUND);
            return NULL;
        }
    }

    if (idx > exports->NumberOfFunctions) 
    {
        /* 序号值也没找到 */
        SetLastError(ERROR_PROC_NOT_FOUND);
        return NULL;
    }

    /* 函数所在地址rva转va */
    return (FARPROC)(LPVOID)(codeBase + (*(uint32_t *) (codeBase + exports->AddressOfFunctions + (idx*4))));
}

void memFreeLibrary(HMEMORYMODULE mod)
{
    PMEMORYMODULE module = (PMEMORYMODULE)mod;

    if (module == NULL) 
    {
        return;
    }
    if (module->initialized) 
    {
        /* 通知库从进程分离 */
        DllEntryPtr DllEntry = (DllEntryPtr)(LPVOID)(module->codeBase + module->headers->OptionalHeader.AddressOfEntryPoint);
        (*DllEntry)((HINSTANCE)module->codeBase, DLL_PROCESS_DETACH, 0);
    }

    if (module->modules != NULL) 
    {
        /* 卸载预先加载的库与释放内存地址 */
        int i;
        for (i=0; i<module->numModules; i++) 
        {
            if (module->modules[i] != NULL) 
            {
                module->freeLibrary(module->modules[i], module->userdata);
            }
        }

        free(module->modules);
    }

    if (module->codeBase != NULL) 
    {
        /* 释放库在进程内的虚拟地址 */
        module->free(module->codeBase, 0, MEM_RELEASE, module->userdata);
    }

    SYS_FREE(module);
}
