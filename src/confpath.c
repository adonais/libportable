#include "inipara.h"
#include "confpath.h"
#include "internal_crt.h"
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>

typedef	int    (__cdecl *set_env_ptr)(LPCWSTR env);
typedef	WCHAR* (__cdecl *get_env_ptr)(LPCWSTR env);

/* crt_wget_env */
WCHAR* __cdecl 
crt_wget_env(const WCHAR *varname, const WCHAR *crt)
{
    WCHAR* ret = NULL;
    do
    {
        HMODULE     hcrt = NULL;
        get_env_ptr wget_env = NULL;
        if ((hcrt = GetModuleHandleW(crt)) == NULL)
        {
            break;
        }
        if ((wget_env = (get_env_ptr)GetProcAddress(hcrt, "_wgetenv")) == NULL)
        {
            break;
        }
        ret = wget_env(varname);
    }while(0);
    return ret;
}

/* crt_wput_env */
int __cdecl 
crt_wput_env(const WCHAR *varname, const WCHAR *crt)
{
    int ret = -1;
    do
    {
        HMODULE     hcrt = NULL;
        set_env_ptr wput_env = NULL;
        if ((hcrt = GetModuleHandleW(crt)) == NULL)
        {
            break;
        }
        if ((wput_env = (set_env_ptr)GetProcAddress(hcrt,"_wputenv")) == NULL)
        {
            break;
        }
        ret = wput_env(varname);
    }while(0);
    return ret;
}

static bool
find_mscrt(void *hmod, WCHAR *crtbuf, int len)
{
    bool ret = false;
    IMAGE_OPTIONAL_HEADER *pheader;
    IMAGE_IMPORT_DESCRIPTOR *pimport;
    IMAGE_DOS_HEADER *pdos = (IMAGE_DOS_HEADER *) hmod;
    if (!pdos || !crtbuf || !len)
    {
        return false;
    }
    pheader = (IMAGE_OPTIONAL_HEADER *) ((BYTE *) hmod + pdos->e_lfanew + SIZE_OF_NT_SIGNATURE + sizeof(IMAGE_FILE_HEADER));
    pimport = (IMAGE_IMPORT_DESCRIPTOR *) ((BYTE *) hmod + pheader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    while (true)
    {
        IMAGE_THUNK_DATA *pThunk = (PIMAGE_THUNK_DATA)(UINT_PTR) pimport->Characteristics;
        IMAGE_THUNK_DATA *pThunkIAT = (PIMAGE_THUNK_DATA)(UINT_PTR) pimport->FirstThunk;
        if (pThunk == 0 && pThunkIAT == 0)
        {
            break;
        }
        const char *dll_name = (const char *) ((BYTE *) hmod + pimport->Name);
        if (PathMatchSpecA(dll_name, "api-ms-win-crt-environment-*.dll") || PathMatchSpecA(dll_name, "msvcr*.dll"))
        {
            char name[VALUE_LEN];
            api_strncpy(name, (*dll_name == 'a' ? "ucrtbase.dll" : dll_name), VALUE_LEN);
            ret = MultiByteToWideChar(CP_UTF8, 0, api_strlwr(name), -1, crtbuf, len) > 0;
        #ifdef _LOGDEBUG
            logmsg("[config_path] crtbuf[%s]\n", name);
        #endif
            break;
        }
        pimport++;
    }
    return ret;
}

static bool 
parse_config(LPCWSTR base, WCHAR (*lpconfig)[MAX_BUFFER])
{
    int    m = 0;
    bool   ret = false;
    WCHAR  temp[MAX_BUFFER];
    m = api_snwprintf(temp, MAX_BUFFER, L"%s\\%s", base, lpconfig[0]);
    temp[m] = 0;
    m = api_snwprintf(lpconfig[2], MAX_BUFFER, L"%s\\%s", base, lpconfig[1]);
    lpconfig[2][m] = 0;
    if (PathFileExistsW(temp))
    {
        m = api_snwprintf(lpconfig[2], MAX_BUFFER - 1, L"%s", temp);
        ret = (m > 0 && m < MAX_BUFFER);
    }
    else if (PathFileExistsW(lpconfig[2]))
    {
        ret = true;
    }
    else
    {
        *lpconfig[2] = 0;
    }
    return ret;
}

bool WINAPI
find_mpv_conf(WCHAR* path, const int len)
{
    int    ret = 0;
    WCHAR  conf[3][MAX_BUFFER];
    // 赋初值时gcc自动调用memset优化, 但我们不链接crt
    api_snwprintf(conf[0], MAX_BUFFER, L"%s", L"mpv.conf");
    api_snwprintf(conf[1], MAX_BUFFER, L"%s", L"config");
    api_memset(conf[2], 0, sizeof(conf[2]));
    do
    {
        WCHAR* tmp_env;
        WCHAR  mpv[MAX_BUFFER];
        WCHAR  crtname[VALUE_LEN];
        if (find_mscrt(GetModuleHandleW(NULL), crtname, VALUE_LEN))
        {
            if (*crtname && (tmp_env = crt_wget_env(L"MPV_HOME", crtname)) != NULL)
            {
                if (parse_config(tmp_env, &conf[0]))
                {
                    ret = 1;
                    break;
                }
            }
        }
        if (get_basedir(mpv, MAX_BUFFER))
        {
            WCHAR base[MAX_BUFFER];
            WCHAR portable[MAX_BUFFER];
            api_memset(base, 0, sizeof(base));
            api_memset(portable, 0, sizeof(portable));
            api_snwprintf(base, MAX_BUFFER - 1, L"%s", mpv);
            api_snwprintf(portable, MAX_BUFFER - 1, L"%s", mpv);
            api_wcsncat(portable, L"\\portable_config", MAX_BUFFER - 1);
            api_wcsncat(mpv, L"\\mpv", MAX_BUFFER - 1);
            if (parse_config(portable, &conf[0]))
            {
                ret = 1;
                break;
            }
            else if (parse_config(mpv, &conf[0]))
            {
                ret = 1;
                break;
            }
            else if (parse_config(base, &conf[0]))
            {
                ret = 1;
                break;
            }
        }
        if (SHGetFolderPathW(0, CSIDL_APPDATA, 0, SHGFP_TYPE_CURRENT, mpv) == S_OK)
        {
            api_wcsncat(mpv, L"\\mpv", MAX_BUFFER);
            if (parse_config(mpv, &conf[0]))
            {
                ret = 1;
                break;
            }
        }
    } while (0);
    if (ret && conf[2][1] == L':')
    {
        ret = api_snwprintf(path, (size_t)len, L"%s", conf[2]);
    #ifdef _LOGDEBUG
        char data[MAX_BUFFER];
        logmsg("[config_path] mpv.conf[%s]\n", mp_make_u8(path, data, MAX_BUFFER));
    #endif
    }
    return (ret > 0 && ret < len);
}
