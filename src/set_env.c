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
typedef	int (__cdecl *MOZ_SETENV)(const wchar_t *env);
extern  WCHAR ini_path[MAX_PATH+1];

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

 /* 不硬编码文件名,而是从输入表查找CRT版本 */
static bool find_msvcrt(char *crt_buf, int len)
{
    bool ret = false;
    IMAGE_DOS_HEADER         *pDos;
    IMAGE_OPTIONAL_HEADER    *pOptHeader;
    IMAGE_IMPORT_DESCRIPTOR  *pImport ;
    HMODULE hMod=LoadLibraryExW(L"mozglue.dll" ,NULL, 0);   
    if (!hMod)
    {
        return ret;
    }
    pDos = (IMAGE_DOS_HEADER *)hMod;
    pOptHeader = (IMAGE_OPTIONAL_HEADER *)(
                 (BYTE *)hMod
                 + pDos->e_lfanew
                 + SIZE_OF_NT_SIGNATURE
                 + sizeof(IMAGE_FILE_HEADER)
                 );
    pImport = (IMAGE_IMPORT_DESCRIPTOR * )(
               (BYTE *)hMod
               + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
              ) ;
    while( true )
    {
        bool  vc14 = false;
        char* pszDllName = NULL;
        char  name[CRT_LEN+1] = {0};
        IMAGE_THUNK_DATA *pThunk  = (PIMAGE_THUNK_DATA)(pImport->Characteristics);
        IMAGE_THUNK_DATA *pThunkIAT = (PIMAGE_THUNK_DATA)(pImport->FirstThunk);
        if(pThunk == 0 && pThunkIAT == 0) break;
        pszDllName = (char*)((BYTE*)hMod+pImport->Name);
        vc14 = PathMatchSpecA(pszDllName,"vcruntime*.dll");
        if ( vc14 || PathMatchSpecA(pszDllName,"msvcr*.dll") )
        {
            if ( vc14 )
            {
                const char* ucrt = "ucrtbase.dll";
                strncpy(crt_buf,ucrt,len);
            }
            else
            {
                strncpy(name,pszDllName,CRT_LEN);
                strncpy(crt_buf,CharLowerA(name),len);
            }
            ret = true;
        #ifdef _LOGDEBUG
            logmsg("crt_buf[%s]\n", crt_buf);
        #endif
            break;
        }
        pImport++;
    }
    FreeLibrary(hMod);
    return ret;
}

/* 加载 msvcrt */
static HMODULE
init_mscrt(const char *filename, MOZ_SETENV *wput_env)
{
    FILE *fp;
    unsigned char *data=NULL;
    long size;
    size_t read;
    HMEMORYMODULE handle = NULL;
    if ( GetOsVersion()>603 || *filename == 'm' )
    {
        UNREFERENCED_PARAMETER(fp);
        UNREFERENCED_PARAMETER(size);
        UNREFERENCED_PARAMETER(read);
        do
        {
            if ( (handle = memDefaultLoadLibrary(filename ,NULL)) == NULL )
            {
                break;
            }
            if ( (*wput_env = (MOZ_SETENV)memDefaultGetProcAddress(handle,"_wputenv",NULL)) == NULL )
            {
                handle = NULL;
                break;
            }
        }while(0);

    }
    else
    {
        do
        {
            fp = fopen(filename, "rb");
            if (fp == NULL)
            {
            #ifdef _LOGDEBUG
                logmsg("Can't open DLL file \"%s\".", filename);
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
                break;
            }

            *wput_env = (MOZ_SETENV)memGetProcAddress(handle, "_wputenv");
            if (*wput_env == NULL) 
            {
                handle = NULL;
                break;
            }

        }while(0);
    }
    if ( NULL != data)
    {
        SYS_FREE(data);
    }    
    return handle;
}

unsigned WINAPI 
pentadactyl_fixed(void * pParam)
{
    HMEMORYMODULE m_crt  = NULL;
    WCHAR         *rc_path = NULL;
    WCHAR         m_env[VALUE_LEN+1] = {0};
    WCHAR         m_value[VALUE_LEN+1] = {0};
    const char*   crt_names = (const char *)pParam;
    MOZ_SETENV    my_putenv = NULL;
    if ( !read_appkey( L"Env",L"VimpPentaHome", m_value, sizeof(m_value), NULL ) )
    {
        return (0);
    }
    if ( (m_crt = init_mscrt(crt_names, &my_putenv)) == NULL )
    {
        return (0);
    }
    if ( !(PathToCombineW(m_value, VALUE_LEN) && create_dir(m_value)) )
    {
        return (0);
    }
    if ( (rc_path = (WCHAR *)SYS_MALLOC(VALUE_LEN+1)) == NULL )
    {
        return 0;
    }
    do
    {
        int m = _snwprintf(m_env, VALUE_LEN, L"%ls%ls", L"HOME=", m_value);
        if ( m > 0 &&  m < VALUE_LEN )
        {
            my_putenv( m_env );
        }
        dull_replace(m_value, VALUE_LEN, L"\\", L"\\\\");
        m = _snwprintf(m_env, VALUE_LEN,L"%ls%ls",L"PENTADACTYL_RUNTIME=", m_value);
        if ( m > 0 &&  m < VALUE_LEN )
        {
            my_putenv( m_env );
        }
        m = _snwprintf(rc_path, VALUE_LEN, L"%ls\\\\_pentadactylrc", m_value);
        if ( !(m > 0 &&  m < VALUE_LEN) )
        {
            break;
        }
        m = _snwprintf(m_env, VALUE_LEN, L"%ls%ls", L"PENTADACTYL_INIT=:source ", rc_path);
        if ( m > 0 &&  m < VALUE_LEN )
        {
            my_putenv( m_env );
        }
        if ( rc_path[1] == L':' && !exists_dir(rc_path) )
        {
            DWORD  m_bytes;
            const  char* desc = "\" File created by libportable.\r\n"
                                "loadplugins \'\\.(js|penta)$\'\r\n";
            HANDLE h_file = CreateFileW(rc_path,
                            GENERIC_WRITE,
                            FILE_SHARE_WRITE,
                            NULL,
                            OPEN_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
            if ( goodHandle(h_file) )
            {
                WriteFile(h_file, desc, (DWORD)strlen(desc), &m_bytes, NULL);
                CloseHandle(h_file);
            }
        }
    }while (0);
    SYS_FREE(rc_path);
    if ( m_crt )
    {
        if ( GetOsVersion()>=603 || *crt_names == 'm' )
            memDefaultFreeLibrary(m_crt, NULL);
        else
            memFreeLibrary(m_crt);
    }
    return (1);
}

static void 
foreach_env(void *pParam)
{
    LPWSTR     m_key;
    /* 使用栈空间, 节省申请堆的时间 */
    WCHAR      env_buf[MAX_ENV_SIZE+1];
    MOZ_SETENV my_putenv = (MOZ_SETENV)pParam;
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
            my_putenv( m_key );
        }
        m_key += wcslen(m_key)+1;
    }
    return;
}

static void 
set_plugins(void *pParam)
{
    WCHAR      val_str[VALUE_LEN+1] = {0};
    MOZ_SETENV my_putenv = (MOZ_SETENV)pParam;
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
            my_putenv( env_str );
        }
    }
    return;
}

void WINAPI set_envp(char *crt_names, int len)
{
    HMEMORYMODULE m_crt = NULL;
    MOZ_SETENV    moz_put_env = NULL;
    do
    {
        if ( !find_msvcrt(crt_names, len) )
        {
            break;
        }
        
        if ( (m_crt = init_mscrt(crt_names, &moz_put_env)) 
              == NULL )
        {
            break;
        }
        foreach_env(moz_put_env);
        set_plugins(moz_put_env); 
    }while(0);
    if ( m_crt )
    {
        if ( GetOsVersion()>603 || *crt_names == 'm' )
            memDefaultFreeLibrary(m_crt, NULL);
        else
            memFreeLibrary(m_crt);
    } 
    return;
}
