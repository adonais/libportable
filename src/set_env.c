#include "set_env.h"
#include "inipara.h"
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <stdio.h>
#include <windows.h>

#define MAX_ENV_SIZE 8192
typedef int(__cdecl *pSetEnv)(const wchar_t *env);
extern WCHAR ini_path[MAX_PATH + 1];
static pSetEnv envPtrW = NULL;

static WCHAR *               /* c风格的unicode字符串替换函数 */
dull_replace(WCHAR *in,      /* 目标字符串 */
             size_t in_size, /* 字符串长度 */
             const WCHAR *pattern,
             const WCHAR *by)
{
    WCHAR *in_ptr = in;
    WCHAR res[MAX_PATH + 1] = { 0 };
    size_t resoffset = 0;
    WCHAR *needle;
    while ((needle = StrStrW(in, pattern)) && resoffset < in_size)
    {
        wcsncpy(res + resoffset, in, needle - in);
        resoffset += needle - in;
        in = needle + (int) wcslen(pattern);
        wcsncpy(res + resoffset, by, wcslen(by));
        resoffset += (int) wcslen(by);
    }
    wcscpy(res + resoffset, in);
    wnsprintfW(in_ptr, (int) in_size, L"%ls", res);
    return in_ptr;
}

int WINAPI /* 在PE 输入表查找crt文件 */ 
find_mscrt(void *hMod, char *crt_buf, int len)
{
    bool ret = false;
    IMAGE_DOS_HEADER *pDos;
    IMAGE_OPTIONAL_HEADER *pOptHeader;
    IMAGE_IMPORT_DESCRIPTOR *pImport;
    if (!hMod)
    {
        return ret;
    }
    pDos = (IMAGE_DOS_HEADER *) hMod;
    pOptHeader = (IMAGE_OPTIONAL_HEADER *) ((BYTE *) hMod + pDos->e_lfanew + SIZE_OF_NT_SIGNATURE + sizeof(IMAGE_FILE_HEADER));
    pImport = (IMAGE_IMPORT_DESCRIPTOR *) ((BYTE *) hMod + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    while (true)
    {
        char *pszDllName = NULL;
        char name[CRT_LEN + 1] = { 0 };
        IMAGE_THUNK_DATA *pThunk = (PIMAGE_THUNK_DATA)(uintptr_t) pImport->Characteristics;
        IMAGE_THUNK_DATA *pThunkIAT = (PIMAGE_THUNK_DATA)(uintptr_t) pImport->FirstThunk;
        if (pThunk == 0 && pThunkIAT == 0)
            break;
        pszDllName = (char *) ((BYTE *) hMod + pImport->Name);
        if (PathMatchSpecA(pszDllName, "vcruntime*.dll") || PathMatchSpecA(pszDllName, "msvcr*.dll"))
        {
            *crt_buf = '\0';
            strncpy(name, pszDllName, CRT_LEN);
            strncpy(crt_buf, CharLowerA(name), len);
            ret = true;
        #ifdef _LOGDEBUG
            logmsg("crt_buf[%s]\n", crt_buf);
        #endif
            break;
        }
        pImport++;
    }
    return ret;
}

int WINAPI /* 在当前目录查找crt文件 */
find_ucrt(char *crt_path, int len)
{
    bool ret = false;
    int  i = 0;
    const char *modulenames[] = {
          "ucrtbase.dll",             /* Visual Studio 2015 and later */
          "msvcr120.dll",             /* Visual Studio 2013 */
          "msvcr110.dll",             /* Visual Studio 2012 */
          "msvcr100.dll",             /* Visual Studio 2010 */
          "msvcr90.dll",              /* Visual Studio 2008 */
          "msvcr80.dll",              /* Visual Studio 2005 */
          NULL
    };
    *crt_path = '\0';
    if (!GetCurrentWorkDirA(crt_path, len))
    {
        return ret;
    }
    for (i = 0; modulenames[i]; i++)
    {
        char lpPath[MAX_PATH] = { 0 };
        wnsprintfA(lpPath, MAX_PATH, "%s\\%s", crt_path, modulenames[i]);
        if (PathFileExistsA(lpPath))
        {
            ret = PathAppendA(crt_path, modulenames[i]);
            break;
        }
    }
    return ret;
}

static LIB_INLINE bool
crt_initialized(void *module)
{
    typedef char ** (__cdecl* __p__acmdlnPtr)();
    __p__acmdlnPtr __p__acmdlnStub = (__p__acmdlnPtr)GetProcAddress(module, "__p__acmdln");
    return __p__acmdlnStub && *__p__acmdlnStub();
}

void  WINAPI /* 导入env字段下的环境变量 */
foreach_env(void)
{
    LPWSTR m_key;
    WCHAR env_buf[MAX_ENV_SIZE + 1];
    if (GetPrivateProfileSectionW(L"Env", env_buf, MAX_ENV_SIZE, ini_path) < 4)
    {
        return;
    }
    m_key = env_buf;
    while (*m_key != L'\0')
    {
        if (_wcsnicmp(m_key, L"NpluginPath", wcslen(L"NpluginPath")) &&
            _wcsnicmp(m_key, L"VimpPentaHome", wcslen(L"VimpPentaHome")) && 
            _wcsnicmp(m_key, L"TmpDataPath", wcslen(L"TmpDataPath")))
        {
            envPtrW(m_key);
        }
        m_key += wcslen(m_key) + 1;
    }
}

static void /* 重定向插件目录 */
set_plugins(void)
{
    WCHAR val_str[VALUE_LEN + 1] = { 0 };
    if (read_appkey(L"Env", L"NpluginPath", val_str, sizeof(val_str), NULL))
    {
        WCHAR env_str[VALUE_LEN + 1] = { 0 };
        PathToCombineW(val_str, VALUE_LEN);
        if (wnsprintfW(env_str, VALUE_LEN, L"%ls%ls", L"MOZ_PLUGIN_PATH=", val_str) > 0)
        {
            envPtrW(env_str);
        }
    }
}

static void /* 重定向Pentadactyl/Vimperator主目录 */
pentadactyl_fixed(void)
{
    WCHAR rc_path[VALUE_LEN + 1] = { 0 };
    WCHAR m_env[VALUE_LEN + 1] = { 0 };
    WCHAR m_value[VALUE_LEN + 1] = { 0 };
    if (!read_appkey(L"Env", L"VimpPentaHome", m_value, sizeof(m_value), NULL))
    {
        return;
    }
    if (!(PathToCombineW(m_value, VALUE_LEN) && create_dir(m_value)))
    {
        return;
    }
    do
    {
        int m = wnsprintfW(m_env, VALUE_LEN, L"%ls%ls", L"HOME=", m_value);
        if (m > 0 && m < VALUE_LEN)
        {
            envPtrW(m_env);
        }
        dull_replace(m_value, VALUE_LEN, L"\\", L"\\\\");
        m = wnsprintfW(m_env, VALUE_LEN, L"%ls%ls", L"PENTADACTYL_RUNTIME=", m_value);
        if (m > 0 && m < VALUE_LEN)
        {
            envPtrW(m_env);
        }

        m = wnsprintfW(rc_path, VALUE_LEN, L"%ls\\\\_pentadactylrc", m_value);
        if (!(m > 0 && m < VALUE_LEN))
        {
            break;
        }
        m = wnsprintfW(m_env, VALUE_LEN, L"%ls%ls", L"PENTADACTYL_INIT=:source ", rc_path);
        if (m > 0 && m < VALUE_LEN)
        {
            envPtrW(m_env);
        }
        if (rc_path[1] == L':' && !exists_dir(rc_path))
        {
            const char *desc = "\" File created by libportable.\r\n"
                               "loadplugins \'\\.(js|penta)$\'\r\n";
            HANDLE h_file = CreateFileW(rc_path, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (goodHandle(h_file))
            {
                DWORD m_bytes;
                WriteFile(h_file, desc, (DWORD) strlen(desc), &m_bytes, NULL);
                CloseHandle(h_file);
            }
        }
    } while (0);
}

unsigned WINAPI
setenv_tt(void *p)
{
    HMODULE hMod = NULL;
    if (NULL != p)
    {
        hMod = LoadLibraryA("ucrtbase.dll");
        if (hMod == NULL || (envPtrW = (pSetEnv)GetProcAddress(hMod, "_wputenv")) == NULL)
        {
            return (0);
        }
    }
    foreach_env();
    set_plugins();
    pentadactyl_fixed();
    if (NULL != hMod)
    {
        FreeLibrary(hMod);
    }
    return (1);
}

unsigned WINAPI
set_envp(void *p)
{
    char crt_names[CRT_LEN + 1] = { 0 };
    if (find_mscrt(dll_module, crt_names, CRT_LEN) && *crt_names == 'v')
    {
        envPtrW = _wputenv;
        return setenv_tt(NULL);
    }
    if (find_mscrt(GetModuleHandleW(NULL), crt_names, CRT_LEN) || find_ucrt(crt_names, CRT_LEN)) 
    {
        HMODULE hMod = NULL;
        HANDLE m_thread = NULL;
        bool initialize = false;
        if ( *crt_names == 'v' )
        {
            hMod = LoadLibraryA("ucrtbase.dll");
        }
        else
        {
            hMod = LoadLibraryA(crt_names);
        }
        if (NULL == hMod)
        {
            return (0);
        }
        initialize = crt_initialized(hMod);
        
        if (!initialize)
        {
        #ifdef _LOGDEBUG
            logmsg("crt not initialized!!!\n");
        #endif
            CloseHandle((HANDLE)_beginthreadex(NULL,0,&setenv_tt,&crt_names,0,NULL));
            return FreeLibrary(hMod);
        }
        if ((envPtrW = (pSetEnv)GetProcAddress(hMod, "_wputenv")) != NULL)
        {
            setenv_tt(NULL);
        }
        FreeLibrary(hMod);
    }
    return (1);
}
