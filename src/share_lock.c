#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
#include <shlwapi.h>
#include "inipara.h"

static HANDLE     mapped_file_ = NULL;
static LPCWSTR    libtbl_lock_ = L"ice_share_lock_";

bool WINAPI
share_create(bool read_only, uint32_t size) 
{
    mapped_file_ = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL,
                   read_only? PAGE_READONLY : PAGE_READWRITE, 0, (DWORD)size, libtbl_lock_);
    if (!mapped_file_)
    {
        return false;
    }
    if (mapped_file_ == NULL || GetLastError() == ERROR_ALREADY_EXISTS) 
    {
        CloseHandle(mapped_file_);
        return false;
    }
    return true;
}

HANDLE WINAPI
share_open(bool read_only) 
{
    return OpenFileMappingW(read_only? FILE_MAP_READ : FILE_MAP_ALL_ACCESS, false, libtbl_lock_);
}

LPVOID WINAPI
share_map(size_t bytes, bool read_only) 
{
    if (mapped_file_ == NULL)
    {
        return false;
    }
    return MapViewOfFile(mapped_file_, read_only ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS, 
                        0, 0, bytes);
}

bool WINAPI
share_unmap(LPVOID memory) 
{
    if (memory == NULL)
        return false;

    UnmapViewOfFile(memory);
    memory = NULL;
    return true;
}

void WINAPI
share_close(void) 
{
    CloseHandle(mapped_file_);
    mapped_file_ = NULL;
}

void* WINAPI
get_share_handle()
{
  return mapped_file_;
}

void WINAPI
set_share_handle(HANDLE handle) 
{
    mapped_file_ = handle;
}

bool WINAPI
get_ini_path(WCHAR *ini, int len)
{
    if (*sdata.ini != L'\0')
    {
        wnsprintfW(ini, len, L"%ls", sdata.ini);
        return true;
    }
    s_data *memory = share_map(sizeof(s_data), true);
    if (memory)
    {
        wnsprintfW(ini, len, L"%ls", memory->ini);
        share_unmap(memory);
    }
    return true;
}

bool WINAPI
get_appdt_path(WCHAR *path, int len)
{
    if (*sdata.appdt != L'\0')
    {
        wnsprintfW(path, len, L"%ls", sdata.appdt);
        return true;
    }
    s_data *memory = share_map(sizeof(s_data), true);
    if (memory)
    {
        wnsprintfW(path, len, L"%ls", memory->appdt);
        share_unmap(memory);
    }
    return true;
}

bool WINAPI
get_localdt_path(WCHAR *path, int len)
{
    if (*sdata.localdt != L'\0')
    {
        wnsprintfW(path, len, L"%ls", sdata.localdt);
        return true;
    }
    s_data *memory = share_map(sizeof(s_data), true);
    if (memory)
    {
        wnsprintfW(path, len, L"%ls", memory->localdt);
        share_unmap(memory);
    }
    return true;
}

bool WINAPI
get_process_path(WCHAR *path, int len)
{
    if (*sdata.process != L'\0')
    {
        wnsprintfW(path, len, L"%ls", sdata.process);
        return true;
    }
    s_data *memory = share_map(sizeof(s_data), true);
    if (memory)
    {
        wnsprintfW(path, len, L"%ls", memory->process);
        share_unmap(memory);
    }
    return true;
}

bool WINAPI
get_process_flags(void)
{
    bool flags = false;
    if (*sdata.process != L'\0')
    {
        return sdata.restart;
    }
    s_data *memory = share_map(sizeof(s_data), true);
    if (memory)
    {
        flags = memory->restart;
        share_unmap(memory);
    }
    return flags;
}

DWORD WINAPI
get_process_pid(void)
{
    DWORD pid = 0;
    if (*sdata.process != L'\0')
    {
        return sdata.main;
    }
    s_data *memory = share_map(sizeof(s_data), true);
    if (memory)
    {
        pid = (DWORD)memory->main;
        share_unmap(memory);
    }
    return pid;
}

bool WINAPI
get_process_profile(void)
{
    bool flags = false;
    s_data *memory = share_map(sizeof(s_data), true);
    if (memory)
    {
        flags = memory->noprofile;
        share_unmap(memory);
    }
    else
    {
    #ifdef _LOGDEBUG
        logmsg("memory is null in %s!\n", __FUNCTION__);
    #endif           
    }    
    return flags;
}

bool WINAPI
get_process_remote(void)
{
    bool flags = false;
    if (*sdata.process != L'\0')
    {
        return sdata.noremote;
    }
    s_data *memory = share_map(sizeof(s_data), true);
    if (memory)
    {
        flags = memory->noremote;
        share_unmap(memory);
    }
    return flags;
}

void WINAPI
set_process_profile(bool flags)
{
    s_data *memory = share_map(sizeof(s_data), false);
    if (memory)
    {
        memory->noprofile = flags;
        share_unmap(memory);
    }
    else
    {
    #ifdef _LOGDEBUG
        logmsg("memory is null in %s!\n", __FUNCTION__);
    #endif 
    }    
}

void WINAPI
set_process_remote(bool flags)
{
    if (*sdata.process != L'\0')
    {
        sdata.noremote = flags;
    }
    s_data *memory = share_map(sizeof(s_data), false);
    if (memory)
    {
        memory->noremote = flags;
        share_unmap(memory);
    }
}

void WINAPI
set_process_flags(bool flags)
{
    if (*sdata.process != L'\0')
    {
        sdata.restart = flags;
    }
    s_data *memory = share_map(sizeof(s_data), false);
    if (memory)
    {
        memory->restart = flags;
        share_unmap(memory);
    }
}

void WINAPI
set_process_pid(uint32_t pid)
{
    if (*sdata.process != L'\0')
    {
        sdata.main = pid;
        sdata.restart = true;
    }
    s_data *memory = share_map(sizeof(s_data), false);
    if (memory)
    {
        memory->main = pid;
        memory->restart = true;
        share_unmap(memory);
    }
}
