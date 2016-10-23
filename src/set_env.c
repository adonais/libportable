#include "inipara.h"
#include "set_env.h"
#include "load_module.h"
#include "MinHook.h"
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <process.h>
#include <stdio.h>

#define	MAX_ENV_SIZE 8192
typedef	int (__cdecl *pSetEnv)(const char *env);
extern  WCHAR ini_path[MAX_PATH+1];
static  pSetEnv envPtrA = NULL;

#if _MSC_VER
// Disable warning about DWORD -> void* on vc14
#pragma warning(disable:4312)
#endif

/* c风格的unicode字符串替换函数 */
static WCHAR* 
dull_replace( WCHAR *in,              /* 目标字符串 */      
              size_t in_size,         /* 字符串长度 */
              const WCHAR *pattern,
              const WCHAR *by
            )
{
    WCHAR* in_ptr = in;
    WCHAR  res[MAX_PATH+1] = {0};
    size_t resoffset = 0;
    WCHAR  *needle;
    while ( (needle = StrStrW(in, pattern)) && 
            resoffset < in_size ) 
    {
        wcsncpy(res + resoffset, in, needle - in);
        resoffset += needle - in;
        in = needle + wcslen(pattern);
        wcsncpy(res + resoffset, by, wcslen(by));
        resoffset += wcslen(by);
    }
    wcscpy(res + resoffset, in);
    _snwprintf(in_ptr, in_size, L"%ls", res);
    return in_ptr;
}

static int envPtrw(LPCWSTR env)
{
    int  len = MAX_PATH;
    char lpkey[MAX_PATH+1] = {0};
    WideCharToMultiByte(CP_ACP, 0, env, -1, lpkey, len, NULL, NULL);
    return envPtrA( lpkey );
}

static void  
pentadactyl_fixed(void)
{
    WCHAR rc_path[VALUE_LEN+1] = {0};
    WCHAR m_env[VALUE_LEN+1] = {0};
    WCHAR m_value[VALUE_LEN+1] = {0};
    if ( !read_appkey( L"Env",L"VimpPentaHome", m_value, sizeof(m_value), NULL ) )
    {
        return;
    } 
    if ( !PathToCombineW(m_value, VALUE_LEN) )
    {
        return;
    }
    
    if ( !(PathToCombineW(m_value, VALUE_LEN) && create_dir(m_value)) )
    {
        return;
    }
    do
    {  
        int m = _snwprintf(m_env, VALUE_LEN, L"%ls%ls", L"HOME=", m_value);
        if ( m > 0 &&  m < VALUE_LEN )
        {
            envPtrw( m_env );
        }
        dull_replace(m_value, VALUE_LEN, L"\\", L"\\\\");
        m = _snwprintf(m_env, VALUE_LEN,L"%ls%ls",L"PENTADACTYL_RUNTIME=", m_value);
        if ( m > 0 &&  m < VALUE_LEN )
        {
            envPtrw( m_env );
        }
        
        m = _snwprintf(rc_path, VALUE_LEN, L"%ls\\\\_pentadactylrc", m_value);
        if ( !(m > 0 &&  m < VALUE_LEN) )
        {
            break;
        }
        m = _snwprintf(m_env, VALUE_LEN, L"%ls%ls", L"PENTADACTYL_INIT=:source ", rc_path);
        if ( m > 0 &&  m < VALUE_LEN )
        {
            envPtrw( m_env );
        }
        if ( rc_path[1] == L':' && !exists_dir(rc_path) )
        {
            const  char* desc = "\" File created by libportable.\r\n"
                                "loadplugins \'\\.(js|penta)$\'\r\n";
            HANDLE h_file = CreateFileW(rc_path,
                            GENERIC_WRITE,
                            0,
                            NULL,
                            OPEN_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
            if ( goodHandle(h_file) )
            {
                DWORD  m_bytes;
                WriteFile(h_file, desc, (DWORD)strlen(desc), &m_bytes, NULL);
                CloseHandle(h_file);
            }
        }
    }while (0);
    return;
}

static bool 
find_ucrt(LPCSTR names, char *ucrt_path, int len)
{
    bool ret = false;   
    do
    {
        const char* path = "%SystemRoot%";
        char  lpPath[MAX_PATH+1] = {0};
        if ( !GetCurrentWorkDirA(ucrt_path, len) )
        {
            break;
        }
        if ( !PathAppendA(ucrt_path, names) )
        {
            break;
        }
        if ( !!(ret = PathFileExistsA(ucrt_path)) )
        {
            break;
        }
        if ( !ExpandEnvironmentStringsA(path, lpPath, MAX_PATH) )
        {
            break;
        }
        if ( ucrt_path )
        {
            ucrt_path[len-1] = '\0';
        }
        if ( is_wow64() )
        {
            _snprintf(ucrt_path, len, "%s\\%s\\%s", lpPath, "SysWOW64", names);
        }
        else
        {
            _snprintf(ucrt_path, len, "%s\\%s\\%s", lpPath, "System32", names);
        }
        ret = ucrt_path[len-1] == '\0' && PathFileExistsA(ucrt_path);
    } while (0);
    return ret;
}

/* 加载 nss3.dll */
static HMODULE init_mscrt(int *n)
{
    FILE *fp = NULL;
    unsigned char *data=NULL;
    long size;
    size_t read;
    HMEMORYMODULE handle = NULL;
    UNREFERENCED_PARAMETER(n);
    UNREFERENCED_PARAMETER(fp);
    UNREFERENCED_PARAMETER(size);
    UNREFERENCED_PARAMETER(read);
    do
    {
        if ( (handle = memDefaultLoadLibrary("nss3.dll" ,NULL)) != NULL )
        {
            if ( (envPtrA = (pSetEnv)memDefaultGetProcAddress(handle,"PR_SetEnv",NULL)) != NULL )
            {
                break;
            }
        }
    #ifdef _LOGDEBUG
        logmsg("memDefaultLoadLibrary(nss3.dll) faild[%lu]\n", GetLastError());
    #endif
        if ( true )
        {
            char  lpPath[MAX_PATH+1] = {0};
            if ( !find_ucrt("ucrtbase.dll", lpPath, MAX_PATH) )
            {
                break;
            }
            if ( (fp = fopen(lpPath, "rb")) == NULL )
            {
            #ifdef _LOGDEBUG
                logmsg("Can't open DLL file \"%s\".", lpPath);
            #endif
                break;
            }

            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            data = (unsigned char *)SYS_MALLOC(size);
            fseek(fp, 0, SEEK_SET);
            read = fread(data, 1, size, fp);
            fclose(fp);

            handle = memLoadLibrary(data, size);
            if (handle == NULL)
            {
            #ifdef _LOGDEBUG
                logmsg("memLoadLibrary(%s) faild\n", lpPath);
            #endif
                break;
            }
            if ((envPtrA = (pSetEnv)memGetProcAddress(handle, "_putenv")) == NULL)
            {
                memDefaultFreeLibrary(handle, NULL);
                handle = NULL;
                break;
            }
            *n = 1;
        }
    } while (0);
    if ( NULL != data)
    {
        SYS_FREE(data);
    }    
    return handle;
}

static void 
foreach_env(void)
{
    LPWSTR     m_key;
    /* 使用栈空间, 节省申请堆的时间 */
    WCHAR      env_buf[MAX_ENV_SIZE+1];
    if ( GetPrivateProfileSectionW(L"Env", env_buf, MAX_ENV_SIZE, ini_path) < 4 )
    {
        return;
    }
    m_key = env_buf;
    while(*m_key != L'\0')
    {
        if ( _wcsnicmp( m_key, L"NpluginPath", wcslen(L"NpluginPath") ) != 0 ||
             _wcsnicmp( m_key, L"VimpPentaHome", wcslen(L"VimpPentaHome") ) != 0 || 
             _wcsnicmp( m_key, L"TmpDataPath", wcslen(L"TmpDataPath") ) != 0
           )
        {
            envPtrw( m_key );
        }
        m_key += wcslen(m_key)+1;
    }
    return;
}

static void 
set_plugins(void)
{
    WCHAR      val_str[VALUE_LEN+1] = {0};
    if ( read_appkey( L"Env",L"NpluginPath", val_str, sizeof(val_str), NULL ) )
    {
        WCHAR env_str[VALUE_LEN+1] = {0};
        PathToCombineW( val_str, VALUE_LEN );
        if ( _snwprintf(env_str,
                       VALUE_LEN,
                       L"%ls%ls",
                       L"MOZ_PLUGIN_PATH=",
                       val_str
                      ) > 0
          )
        {
            envPtrw( env_str );
        }
    }
    return;
}

void WINAPI set_envp(void * non_use)
{
    int     fn_load = 0;
    HMODULE m_crt = NULL;
    do
    {
        if ( (m_crt = init_mscrt(&fn_load)) == NULL )
        {
            break;
        }
     
        if ( NULL != envPtrA )
        {
            foreach_env();
            set_plugins();
            pentadactyl_fixed();
        }
    }while(0);
    if ( m_crt )
    {
        fn_load?memFreeLibrary(m_crt):memDefaultFreeLibrary(m_crt, NULL);
    }
    return;
}
