#include <stdio.h>
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windows.h>
#include "set_env.h"
#include "general.h"
#include "ini_parser.h"

#define MAX_ENV_SIZE 8192
typedef int(__cdecl *setenv_ptr)(const char *env);
static setenv_ptr crt_setenv;

/* * * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 * 字符串大写字母转为小写
 */ 
static 
char *strlowr(char *str)
{
    char *orign=str;
    for (; *str!='\0'; str++)
    {
        *str = tolower(*str);
    }
    return orign;
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
    pdos = (IMAGE_DOS_HEADER *) hmod;
    if (!pdos)
    {
        return false;
    }
    pheader = (IMAGE_OPTIONAL_HEADER *) ((BYTE *) hmod + pdos->e_lfanew + SIZE_OF_NT_SIGNATURE + sizeof(IMAGE_FILE_HEADER));
    pimport = (IMAGE_IMPORT_DESCRIPTOR *) ((BYTE *) hmod + pheader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    while (true)
    {
        char *dll_name = NULL;
        char name[CRT_LEN + 1] = {0};
        IMAGE_THUNK_DATA *pThunk = (PIMAGE_THUNK_DATA)(uintptr_t) pimport->Characteristics;
        IMAGE_THUNK_DATA *pThunkIAT = (PIMAGE_THUNK_DATA)(uintptr_t) pimport->FirstThunk;
        if (pThunk == 0 && pThunkIAT == 0)
            break;
        dll_name = (char *) ((BYTE *) hmod + pimport->Name);
        if (PathMatchSpecA(dll_name, "vcruntime*.dll") || PathMatchSpecA(dll_name, "msvcr*.dll"))
        {
            *crt_buf = L'\0';
            strncpy(name, dll_name, CRT_LEN);
            ret = MultiByteToWideChar(CP_UTF8, 0, strlowr(name), -1, crt_buf, len) > 0;
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
 * 仅兼容链接msvcrt.dll的版本, 因为存在跨越crt边界问题
 * 在这里调用nss3.dll的环境变量函数.
 */
static bool
init_process_envp(HMODULE *pmod)                
{
    WCHAR crt_names[CRT_LEN + 1] = {0}; 
    if (find_mscrt(dll_module, crt_names, CRT_LEN) && *crt_names == L'v')
    {
    #ifdef _LOGDEBUG
        logmsg("crt_setenv = _putenv\n");
    #endif          
        crt_setenv = _putenv;
    }
    else if (find_mscrt(GetModuleHandleW(NULL), crt_names, CRT_LEN))
    {
        WCHAR crt_path[MAX_PATH+1] = {0};
        const char *funcptr = "_putenv";
        if (!wget_process_directory(crt_path, MAX_PATH))
        {
            return false;
        }
        if (*crt_names == L'v')
        {
            PathAppendW(crt_path, L"nss3.dll");
            if ((*pmod = LoadLibraryW(crt_path)) == NULL)
            {
                return false;
            }
            funcptr = "PR_SetEnv";
        }
        else
        {
            PathAppendW(crt_path, crt_names);
            if ((*pmod = LoadLibraryW(crt_names)) == NULL)
            {
            #ifdef _LOGDEBUG
                logmsg("LoadLibraryW(%ls) return false!\n", crt_names);
            #endif                
            }
            return false;
        }
        if ((crt_setenv = (setenv_ptr)GetProcAddress(*pmod, funcptr)) == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("crt not initialized!!!\n");
        #endif   
            FreeLibrary(*pmod);
            *pmod = NULL;
            return false;
        }
        else
        {
        #ifdef _LOGDEBUG
            logmsg("GetProcAddress(%s) ok\n", funcptr);
        #endif               
        }
    }
    else
    {
        return false;
    }    
    return true;
}

/* * * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 * 导入Env区段下的环境变量,忽略NpluginPath与TmpDataPath,因为前面已经读入变量了.
 * 因为crt版本的差异,以及链接方式的不同,先查询dll与目标加载crt的方式.
 */ 
void WINAPI
setenv_tt(void)
{
    HMODULE hmod = NULL;
    char *val_str = NULL;
    char env_buf[EXCLUDE_NUM][VALUE_LEN + 1];
    ini_cache plist = NULL;
    if (!init_process_envp(&hmod))
    {
        return;
    }
    else
    {
        crt_setenv("LIBPORTABLE_SETENV_DEFINED=1");
    }
    if ((plist = iniparser_create_cache(ini_portable_path, false)) != NULL)
    {
        if (inicache_read_string("Env", "NpluginPath", &val_str, &plist))
        {
            char *plugin_env = NULL;
            WCHAR plugin_str[VALUE_LEN + 1] = {L'M',L'O',L'Z',L'_',L'P',L'L',L'U',L'G',L'I',
                                               L'N',L'_',L'P',L'A',L'T',L'H',L'=',0};
            int envlen = (int)wcslen(plugin_str);
            MultiByteToWideChar(CP_UTF8, 0, val_str, -1, plugin_str+envlen, VALUE_LEN-envlen);
            path_to_absolute(plugin_str+envlen, VALUE_LEN-envlen);
            // 对于包含路径的环境变量,转换为mbcs多字节,因为mscrt的io函数不支持utf8
            if ((plugin_env = utf16_to_mbcs(plugin_str)) != NULL)
            {
                crt_setenv(plugin_env);
                free(plugin_env);
            }     
            free(val_str);        
        }        
        if (inicache_read_int("General", "Portable", &plist) > 0)
        {          
            crt_setenv("SNAP_NAME=1");
        }    
    #if defined(_WIN64)
        if (inicache_read_int("Env", "MOZ_DISABLE_NPAPI_SANDBOX", &plist) <= 0)
        {
            crt_setenv("MOZ_DISABLE_NPAPI_SANDBOX=1");
        }
    #endif
        if (inicache_foreach_key("Env", env_buf, EXCLUDE_NUM, &plist))
        {
            for (int i = 0; i < EXCLUDE_NUM && *env_buf[i] != '\0'; ++i)
            {
                if (_strnicmp(env_buf[i], "NpluginPath", strlen("NpluginPath")) &&
                    _strnicmp(env_buf[i], "TmpDataPath", strlen("TmpDataPath")))
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
