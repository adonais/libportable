#include "set_env.h"
#include "inipara.h"
#include "load_module.h"
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <stdio.h>
#include <windows.h>

#define MAX_ENV_SIZE 8192
typedef int(__cdecl *pSetEnv)(const char *env);
extern WCHAR ini_path[MAX_PATH + 1];
static pSetEnv envPtrA = NULL;

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

static LIB_INLINE int
envPtrW(LPCWSTR env)
{
    int len = MAX_PATH;
    char lpkey[MAX_PATH + 1] = { 0 };
    WideCharToMultiByte(CP_ACP, 0, env, -1, lpkey, len, NULL, NULL);
    return envPtrA(lpkey);
}

static bool
find_mscrt(HMODULE hMod, char *crt_buf, int len)
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

static bool /* 在当前目录或系统目录查找 */
find_ucrt(LPCSTR names, char *ucrt_path, int len)
{
    bool ret = false;
    do
    {
        const char *path = "%SystemRoot%";
        char lpPath[MAX_PATH + 1] = { 0 };
        if (!GetCurrentWorkDirA(ucrt_path, len))
        {
            break;
        }
        if (!PathAppendA(ucrt_path, names))
        {
            break;
        }
        if (!!(ret = PathFileExistsA(ucrt_path)))
        {
            break;
        }
        if (!ExpandEnvironmentStringsA(path, lpPath, MAX_PATH))
        {
            break;
        }
        if (ucrt_path)
        {
            ucrt_path[len - 1] = '\0';
        }
        if (is_wow64())
        {
            wnsprintfA(ucrt_path, len, "%s\\%s\\%s", lpPath, "SysWOW64", names);
        }
        else
        {
            wnsprintfA(ucrt_path, len, "%s\\%s\\%s", lpPath, "System32", names);
        }
        ret = ucrt_path[len - 1] == '\0' && PathFileExistsA(ucrt_path);
    } while (0);
    return ret;
}

static HMODULE /* 加载 nss3.dll或msvcr*.dll或ucrtbase.dll */
init_mscrt(int *n)
{
    FILE *fp = NULL;
    uint8_t *data = NULL;
    long size;
    size_t read;
    HMEMORYMODULE handle = NULL;
    UNREFERENCED_PARAMETER(fp);
    UNREFERENCED_PARAMETER(size);
    *n = 0;   /* n值的不同,决定以那种方式释放dll句柄 */
    do
    {
        char crt_names[CRT_LEN + 1] = { 0 };
        /* 当使用动态链接与fireofx相匹配版本crt的时候,可以直接使用_putenv函数 */
        if (find_mscrt(dll_module, crt_names, CRT_LEN) && *crt_names == 'v')
        {
            envPtrA = _putenv;
            break;
        }
        /* 从nss3.dll获取PR_SetEnv函数的入口地址 */
        if ((handle = memDefaultLoadLibrary("nss3.dll", NULL)) != NULL)
        {
            if ((envPtrA = (pSetEnv) memDefaultGetProcAddress(handle, "PR_SetEnv", NULL)) != NULL)
            {
                uint32_t ver = GetOsVersion();
            #ifdef _LOGDEBUG
                logmsg("memDefaultGetProcAddress(PR_SetEnv 0x%x)\n", (uintptr_t) envPtrA);
            #endif
                if (ver > 700)
                {
                    break;
                }
                /* vs2017编译的版本在某些平台上无法从nss3.dll获取正确的入口地址 */
                if (NULL != envPtrA && ver > 503
                #ifndef _M_X64
                    && *(uint32_t *) envPtrA == '\x55\x8B\xEC\x83'          /*   PUSH EBP         */
                    && *(uint16_t *) ((PBYTE) envPtrA + 4) == '\xE4\xF8'    /*   MOV EBP,ESP      */
                    && *(uint8_t *) ((PBYTE) envPtrA + 6) == '\x51'         /*   AND ESP,FFFFFFF8 */
                #else                                                       /*   PUSH ECX         */
                    && *(uint16_t *) envPtrA == '\x40\x53'                  /*   PUSH RBX         */
                #endif                                                      
                    )                                                       
                {                                                           
                #ifdef _LOGDEBUG
                    logmsg("Normal function calls\n");
                #endif
                    break;
                }
                else if ( ver > 503 )
                {
                #ifdef _LOGDEBUG
                    logmsg("Abnormal function calls\n");
                #endif
                    /* 不知道为啥可以??? */
                    envPtrA = _putenv;
                    break; 
                }
            }
            /* 旧版fx的nss3.dll内没有PR_SetEnv函数,则从msvcrxxx.dll中获取_putenv入口地址 */
            if (!find_mscrt(handle, crt_names, CRT_LEN))
            {
                break;
            }
            if (*crt_names == 'm' && (handle = memDefaultLoadLibrary(crt_names, NULL)) != NULL &&
                (envPtrA = (pSetEnv) memDefaultGetProcAddress(handle, "_putenv", NULL)) != NULL)
            {
                break;
            }
        }
        else
        {
        #ifdef _LOGDEBUG
            logmsg("memDefaultLoadLibrary(nss3.dll) fail[%lu]\n", GetLastError());
        #endif
        }
        /* 在某些系统中无法加载nss3.dll或crt库, 尝试从内存加载 */
        if (NULL == envPtrA)
        {
            char lpPath[MAX_PATH + 1] = { 0 };
            if (!find_ucrt("ucrtbase.dll", lpPath, MAX_PATH))
            {
            #ifdef _LOGDEBUG
                logmsg("not found ucrtbase.dll.\n");
            #endif
                break;
            }
            if ((fp = fopen(lpPath, "rb")) == NULL)
            {
            #ifdef _LOGDEBUG
                logmsg("Can't open DLL file \"%s\".", lpPath);
            #endif
                break;
            }

            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            data = (uint8_t *) SYS_MALLOC(size);
            fseek(fp, 0, SEEK_SET);
            read = fread(data, 1, size, fp);
            fclose(fp);

            handle = memLoadLibrary(data, size);
            if (handle == NULL)
            {
            #ifdef _LOGDEBUG
                logmsg("memLoadLibrary(%s) fail\n", lpPath);
            #endif
                break;
            }
            envPtrA = (pSetEnv) memGetProcAddress(handle, "_putenv");
            *n = 1;
        }
    } while (0);
    if (NULL != data)
    {
        SYS_FREE(data);
    }
    return handle;
}

static void /* 导入env字段下的环境变量 */
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
    return;
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
    return;
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
    return;
}

void WINAPI
set_envp(void *non_use)
{
    int fn_load;
    HMODULE m_crt = NULL;
    do
    {
        if ((m_crt = init_mscrt(&fn_load)) == NULL)
        {
            break;
        }

        if (NULL != envPtrA)
        {
            foreach_env();
            set_plugins();
            pentadactyl_fixed();
        }
    } while (0);
    if (m_crt)
    {
        fn_load ? memFreeLibrary(m_crt) : memDefaultFreeLibrary(m_crt, NULL);
    }
    return;
}
