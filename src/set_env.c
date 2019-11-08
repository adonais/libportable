#include <stdio.h>
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windows.h>
#include "set_env.h"
#include "inipara.h"

#define MAX_ENV_SIZE 8192
typedef int(__cdecl *pSetEnv)(const wchar_t *env);
static pSetEnv envPtrW = NULL;

static bool
get_cwd(LPSTR lpstrName, DWORD len)
{
    int   i = 0;
    WCHAR wPath[MAX_PATH+1] = {0};
    if (getw_cwd(wPath,MAX_PATH))
    {
	#ifdef USE_UTF8
        i = WideCharToMultiByte(CP_UTF8, 0, wPath, -1, lpstrName, (int)len, NULL, NULL);
	#else
        i = WideCharToMultiByte(CP_ACP, 0, wPath, -1, lpstrName, (int)len, NULL, NULL);
	#endif
    }
    return (i>0 && i<(int)len);
}

static int __cdecl
set_env_win(const wchar_t *env)
{
    int ret = -1;
    wchar_t *key = NULL;
    const wchar_t *p = NULL;
    do
    {
        if (env == NULL || *env == L';')
        {
            break;
        }
        p = wcschr(env, L'=');
        if (!p)
        {
            break;
        }  
        key = _wcsdup(env);
        if (!key)
        {
            break;
        }        
        key[p-env] = L'\0';
        if (SetEnvironmentVariableW((LPCWSTR)key, p+1))
        {
            ret = 0;
        }
        free(key);
    }while(0);
    return ret;
}

static bool                                    /* 在PE 输入表查找crt文件 */ 
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

static bool                           /* 在当前目录查找crt文件 */
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
    if (!get_cwd(crt_path, len))
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

static void         /* 导入env字段下的环境变量 */
foreach_env(void)
{
    LPWSTR m_key;
    WCHAR  ini[MAX_PATH+1] = {0};
    WCHAR env_buf[MAX_ENV_SIZE + 1];
#if defined(_WIN64)    
    if (read_appint(L"Env",L"MOZ_DISABLE_NPAPI_SANDBOX") <= 0)
    {
        envPtrW(L"MOZ_DISABLE_NPAPI_SANDBOX=1");
    }
#endif
    if (!get_ini_path(ini, MAX_PATH))
    {
        return;
    }
    if (get_file_version() >= 670 && read_appint(L"General",L"DisDedicate") > 0)
    {      
        envPtrW(L"SNAP_NAME=1");
    }    
    if (GetPrivateProfileSectionW(L"Env", env_buf, MAX_ENV_SIZE, ini) < 4)
    {
        return;
    }
    m_key = env_buf;
    while (*m_key != L'\0')
    {
        if (_wcsnicmp(m_key, L"NpluginPath", wcslen(L"NpluginPath")) &&
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
        path_to_absolute(val_str, VALUE_LEN);
        if (wnsprintfW(env_str, VALUE_LEN, L"%ls%ls", L"MOZ_PLUGIN_PATH=", val_str) > 0)
        {
            envPtrW(env_str);
        }
    }
}

static LIB_INLINE
void setenv_tt(void)
{
    foreach_env();
    set_plugins();
}

unsigned WINAPI
set_envp(void *p)
{
    char crt_names[CRT_LEN + 1] = { 0 };
    if (find_mscrt(dll_module, crt_names, CRT_LEN) && *crt_names == 'v')
    {
        envPtrW = _wputenv;
        setenv_tt();
    }    
    else if (find_mscrt(GetModuleHandleW(NULL), crt_names, CRT_LEN) || find_ucrt(crt_names, CRT_LEN)) 
    {
        HMODULE hMod = NULL;
        bool initialize = false;
        if (*crt_names == 'v' && get_cwd(crt_names, CRT_LEN))
        {
            PathAppendA(crt_names, "ucrtbase.dll");
        }
        if (!PathFileExistsA(crt_names) && (hMod = LoadLibraryA("ucrtbase.dll")) == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("LoadLibraryA crt return false!\n");
        #endif            
            return (0);        	
        }
        else if ((hMod = LoadLibraryA(crt_names)) == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("LoadLibraryA(%ls) return false!\n", crt_names);
        #endif            
            return (0);
        }
        if ((initialize = crt_initialized(hMod)) == false)
        {
        #ifdef _LOGDEBUG
            logmsg("crt not initialized!!!\n");
        #endif
            envPtrW = set_env_win;
        }
        else if ((envPtrW = (pSetEnv)GetProcAddress(hMod, "_wputenv")) == NULL)
        {
            return FreeLibrary(hMod);
        }
        setenv_tt();
        FreeLibrary(hMod);
    }
    return SetEnvironmentVariableW(L"LIBPORTABLE_SETENV_DEFINED", L"1");
}
