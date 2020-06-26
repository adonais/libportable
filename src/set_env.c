#include <stdio.h>
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windows.h>
#include "set_env.h"
#include "general.h"
#include "ini_parser.h"

#define MAX_ENV_SIZE 8192
typedef int(__cdecl *setenv_ptr)(const wchar_t *env);
static setenv_ptr crt_setenv;

static int __cdecl
sys_setenv(const wchar_t *env)
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

/* * * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 * 在PE 输入表查找crt文件 
 */ 
static bool
find_mscrt(void *hmod, WCHAR *crt_buf, int len)
{
    bool ret = false;
    IMAGE_DOS_HEADER *pdos;
    IMAGE_OPTIONAL_HEADER *pheader;
    IMAGE_IMPORT_DESCRIPTOR *pimport;
    if (!hmod)
    {
        return ret;
    }
    pdos = (IMAGE_DOS_HEADER *) hmod;
    pheader = (IMAGE_OPTIONAL_HEADER *) ((BYTE *) hmod + pdos->e_lfanew + SIZE_OF_NT_SIGNATURE + sizeof(IMAGE_FILE_HEADER));
    pimport = (IMAGE_IMPORT_DESCRIPTOR *) ((BYTE *) hmod + pheader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    while (true)
    {
        char *dll_name = NULL;
        char name[CRT_LEN + 1] = { 0 };
        IMAGE_THUNK_DATA *pThunk = (PIMAGE_THUNK_DATA)(uintptr_t) pimport->Characteristics;
        IMAGE_THUNK_DATA *pThunkIAT = (PIMAGE_THUNK_DATA)(uintptr_t) pimport->FirstThunk;
        if (pThunk == 0 && pThunkIAT == 0)
            break;
        dll_name = (char *) ((BYTE *) hmod + pimport->Name);
        if (PathMatchSpecA(dll_name, "vcruntime*.dll") || PathMatchSpecA(dll_name, "msvcr*.dll"))
        {
            *crt_buf = L'\0';
            strncpy(name, dll_name, CRT_LEN);
            ret = MultiByteToWideChar(CP_UTF8, 0, CharLowerA(name), -1, crt_buf, len) > 0;
        #ifdef _LOGDEBUG
            logmsg("crt_buf[%ls]\n", crt_buf);
        #endif
            break;
        }
        pimport++;
    }
    return ret;
}

/* * * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 * 在本地进程目录查找crt文件 
 */
static bool
find_ucrt(WCHAR *crt_path, int len)
{
    bool ret = false;
    int  i = 0;
    const WCHAR *modulenames[] = {
          L"ucrtbase.dll",             /* Visual Studio 2015 and later */
          L"msvcr120.dll",             /* Visual Studio 2013 */
          L"msvcr110.dll",             /* Visual Studio 2012 */
          L"msvcr100.dll",             /* Visual Studio 2010 */
          L"msvcr90.dll",              /* Visual Studio 2008 */
          L"msvcr80.dll",              /* Visual Studio 2005 */
          NULL
    };
    *crt_path = L'\0';
    if (!wget_process_directory(crt_path, len))
    {
        return ret;
    }
    for (i = 0; modulenames[i]; i++)
    {
        WCHAR lpPath[MAX_PATH] = { 0 };
        wnsprintfW(lpPath, MAX_PATH, L"%s\\%s", crt_path, modulenames[i]);
        if (PathFileExistsW(lpPath))
        {
            ret = PathAppendW(crt_path, modulenames[i]);
            break;
        }
    }
    return ret;
}

/* * * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 * mscrt是否已经初始化完成,否则调用系统api
 */
static bool
init_process_envp(HMODULE *pmod)                
{
    WCHAR crt_names[CRT_LEN + 1] = {0}; 
    if (find_mscrt(dll_module, crt_names, CRT_LEN) && *crt_names == 'v')
    {
        crt_setenv = _wputenv;
    }      
    else if (find_mscrt(GetModuleHandleW(NULL), crt_names, CRT_LEN) || find_ucrt(crt_names, CRT_LEN)) 
    {
        if (*crt_names == L'v' && wget_process_directory(crt_names, CRT_LEN))
        {              
            PathAppendW(crt_names, L"ucrtbase.dll");        
        }
        if (!PathFileExistsW(crt_names) && (*pmod = LoadLibraryW(L"ucrtbase.dll")) == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("LoadLibraryW crt return false!\n");
        #endif            
            return false;       	
        }
        else if ((*pmod = LoadLibraryW(crt_names)) == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("LoadLibraryW(%ls) return false!\n", crt_names);
        #endif            
            return false;
        }
        if ((crt_setenv = (setenv_ptr)GetProcAddress(*pmod, "_wputenv")) == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("crt not initialized!!!\n");
        #endif   
            FreeLibrary(*pmod);
            *pmod = NULL;          
            crt_setenv = sys_setenv;
        }
    }
    return true;
}
 
void WINAPI
setenv_tt(void)
{
    HMODULE hmod = NULL;
    char *val_str = NULL;
    wchar_t env_buf[EXCLUDE_NUM][VALUE_LEN + 1];
    ini_cache plist = NULL;
    if (!init_process_envp(&hmod))
    {
        return;
    }
    else
    {
        crt_setenv(L"LIBPORTABLE_SETENV_DEFINED=1");
    }
    if ((plist = iniparser_create_cache(ini_portable_path, false)) != NULL)
    {
        if (inicache_read_string("Env", "NpluginPath", &val_str, &plist))
        {
            WCHAR plugin_str[VALUE_LEN + 1] = {L'M',L'O',L'Z',L'_',L'P',L'L',L'U',L'G',L'I',
                                               L'N',L'_',L'P',L'A',L'T',L'H',L'=',0};
            int envlen = (int)wcslen(plugin_str);
            MultiByteToWideChar(CP_UTF8, 0, val_str, -1, plugin_str+envlen, VALUE_LEN-envlen);
            path_to_absolute(plugin_str+envlen, VALUE_LEN-envlen);
            crt_setenv(plugin_str);       
            free(val_str);        
        }    
        if (inicache_read_int("General", "Portable", &plist) > 0)
        {      
            crt_setenv(L"SNAP_NAME=1");
        }    
    #if defined(_WIN64)
        if (inicache_read_int("Env", "MOZ_DISABLE_NPAPI_SANDBOX", &plist) <= 0)
        {
            crt_setenv(L"MOZ_DISABLE_NPAPI_SANDBOX=1");
        }
    #endif   
        if (inicache_foreach_wkey("Env", env_buf, EXCLUDE_NUM, &plist))
        {
            for (int i = 0; i < EXCLUDE_NUM && *env_buf[i] != L'\0'; ++i)
            {
                if (_wcsnicmp(env_buf[i], L"NpluginPath", wcslen(L"NpluginPath")) &&
                    _wcsnicmp(env_buf[i], L"TmpDataPath", wcslen(L"TmpDataPath")))
                {             
                    crt_setenv(env_buf[i]);
                }                 
            }
        }
        iniparser_destroy_cache(&plist);
    }
    if (hmod)
    {
        FreeLibrary(hmod);
    }     
}
