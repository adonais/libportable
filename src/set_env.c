#include <stdio.h>
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windows.h>
#include "set_env.h"
#include "general.h"
#include "ini_parser.h"

#define MAX_ENV_SIZE 8192
typedef int (__cdecl *setenv_ptr)(const wchar_t *env);
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
 * 在 PE 输入表查找 crt 文件, 把 crt 文件名写入crt_buf缓冲区
 */
static bool
find_mscrt(void *hmod, WCHAR *crt_buf, int len)
{
    bool ret = false;
    IMAGE_OPTIONAL_HEADER *pheader;
    IMAGE_IMPORT_DESCRIPTOR *pimport;
    IMAGE_DOS_HEADER *pdos = (IMAGE_DOS_HEADER *) hmod;
    if (!pdos || !crt_buf || !len)
    {
        return false;
    }
    pheader = (IMAGE_OPTIONAL_HEADER *) ((BYTE *) hmod + pdos->e_lfanew + SIZE_OF_NT_SIGNATURE + sizeof(IMAGE_FILE_HEADER));
    pimport = (IMAGE_IMPORT_DESCRIPTOR *) ((BYTE *) hmod + pheader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    *crt_buf = L'\0';
    while (true)
    {
        char name[CRT_LEN + 1] = {0};
        IMAGE_THUNK_DATA *pThunk = (PIMAGE_THUNK_DATA)(uintptr_t) pimport->Characteristics;
        IMAGE_THUNK_DATA *pThunkIAT = (PIMAGE_THUNK_DATA)(uintptr_t) pimport->FirstThunk;
        if (pThunk == 0 && pThunkIAT == 0)
        {
            break;
        }
        const char *dll_name = (const char *) ((BYTE *) hmod + pimport->Name);
        if (PathMatchSpecA(dll_name, "vcruntime*.dll") || PathMatchSpecA(dll_name, "msvcr*.dll"))
        {
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
 * 检测是否链接新版c库, 移除了旧的兼容代码
 */

static bool
init_process_envp(void)
{
    WCHAR crt_names[CRT_LEN + 1] = {0};
    if (find_mscrt(dll_module, crt_names, CRT_LEN) && *crt_names == L'v')
    {
    #ifdef _LOGDEBUG
        logmsg("ok, The /MD build was also used\n");
    #endif
        crt_setenv = _wputenv;
        return true;
    }
    return false;
}

/* * * ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 * 导入Env区段下的环境变量,忽略NpluginPath与TmpDataPath,因为前面已经读入变量了.
 * 因为crt版本的差异,以及链接方式的不同,先查询dll与目标加载crt的方式.
 */
void WINAPI
setenv_tt(void)
{
    ini_cache plist = NULL;
    if (init_process_envp())
    {
        wchar_t env_buf[EXCLUDE_NUM][VALUE_LEN + 1];
        crt_setenv(L"LIBPORTABLE_SETENV_DEFINED=1");
        if ((plist = iniparser_create_cache(ini_portable_path, false)) != NULL)
        {
            char *val_str = NULL;
            if (inicache_read_string("Env", "NpluginPath", &val_str, &plist))
            {
                WCHAR plugin_str[VALUE_LEN + 1] = {L'M',L'O',L'Z',L'_',L'P',L'L',L'U',L'G',L'I',
                                                   L'N',L'_',L'P',L'A',L'T',L'H',L'=',0};
                int envlen = (int)wcslen(plugin_str);
                MultiByteToWideChar(CP_UTF8, 0, val_str, -1, plugin_str+envlen, VALUE_LEN-envlen);
                path_to_absolute(plugin_str+envlen, VALUE_LEN-envlen);
                crt_setenv(plugin_str);
            #ifdef _LOGDEBUG
                logmsg("NpluginPath[%ls]\n", plugin_str+envlen);
            #endif
                free(val_str);
            }
            if (inicache_read_int("General", "Portable", &plist) > 0)
            {
                char *app = NULL;
                char *local = NULL;
                if (get_file_version() > 131 && _wgetenv(L"XRE_PROFILE_PATH") == NULL && !cmd_has_profile())
                {
                    WCHAR appdt[MAX_PATH + 1] =  {0};
                    WCHAR localdt[MAX_PATH + 1] =  {0};
                    WCHAR env_appdt[MAX_BUFF] =  {0};
                    WCHAR env_localdt[MAX_BUFF] =  {0};
                    if (!inicache_read_string("General", "PortableDataPath", &app, &plist))
                    {
                        wnsprintfW(appdt, MAX_PATH, L"%ls", L"../Profiles");
                    }
                    else
                    {
                        MultiByteToWideChar(CP_UTF8, 0, app, -1, appdt, MAX_PATH);
                    }
                    if (!inicache_read_string("Env", "TmpDataPath", &local, &plist))
                    {
                        wnsprintfW(localdt, MAX_PATH, L"%ls", appdt);
                    }
                    else
                    {
                        MultiByteToWideChar(CP_UTF8, 0, local, -1, localdt, MAX_PATH);
                    }
                    if (*appdt)
                    {
                        path_to_absolute(appdt, MAX_PATH);
                        wnsprintfW(env_appdt, MAX_BUFF, L"XRE_PROFILE_PATH=%ls", appdt);
                    }
                    if (*localdt)
                    {
                        path_to_absolute(localdt, MAX_PATH);
                        wcsncat(localdt, L"\\LocalAppData\\Temp\\Fx", MAX_PATH);
                        wnsprintfW(env_localdt, MAX_BUFF, L"XRE_PROFILE_LOCAL_PATH=%ls", localdt);
                    }
                    if (env_appdt[0] && env_localdt[0])
                    {
                    #ifdef _LOGDEBUG
                        logmsg("Fx 132.x used portable profiles\n");
                    #endif
                        crt_setenv(env_appdt);
                        crt_setenv(env_localdt);
                    }
                }
                crt_setenv(L"MOZ_LEGACY_PROFILES=0");
                crt_setenv(L"SNAP_NAME=firefox");
            #ifdef _LOGDEBUG
                logmsg("disable multipath configuration\n");
            #endif
                if (app)
                {
                    free(app);
                }
                if (local)
                {
                    free(local);
                } 
            }
            if (inicache_foreach_wkey("Env", env_buf, EXCLUDE_NUM, &plist))
            {
                for (int i = 0; i < EXCLUDE_NUM && *env_buf[i] != L'\0'; ++i)
                {
                    if (_wcsnicmp(env_buf[i], L"NpluginPath", 11) &&
                        _wcsnicmp(env_buf[i], L"TmpDataPath", 11))
                    {
                        crt_setenv(env_buf[i]);
                    }
                }
            }
            iniparser_destroy_cache(&plist);
        }
    }
}
