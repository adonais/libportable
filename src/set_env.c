#define ENV_EXTERN

#include "inipara.h"
#include "set_env.h"
#include "MinHook.h"
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <process.h>
#include <stdio.h>

#define	  MAX_ENV_SIZE 8192
#define   CRT_LEN      16

typedef	int (__cdecl *MOZ_SETENV)(const wchar_t *env);

extern WCHAR ini_path[MAX_PATH+1];
static MOZ_SETENV moz_put_env;

/* c风格的unicode字符串替换函数 */
static WCHAR* 
dull_replace(WCHAR *in, size_t in_size, const WCHAR *pattern, const WCHAR *by)
{
    WCHAR* in_ptr = in;
    WCHAR  res[MAX_PATH+1] = {0};
    size_t resoffset = 0;
    WCHAR  *needle;
    while ( (needle = StrStrW(in, pattern)) && 
            resoffset < in_size ) 
    {
        /* copy everything up to the pattern */
        wcsncpy(res + resoffset, in, needle - in);
        resoffset += needle - in;

        /* skip the pattern in the input-string */
        in = needle + wcslen(pattern);
        /* copy the pattern */
        wcsncpy(res + resoffset, by, wcslen(by));
        resoffset += wcslen(by);
    }
    /* copy the remaining input */
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
        char*            pszDllName = NULL;
        char             name[CRT_LEN+1] = {0};
        IMAGE_THUNK_DATA *pThunk  = (PIMAGE_THUNK_DATA)(pImport->Characteristics);
        IMAGE_THUNK_DATA *pThunkIAT = (PIMAGE_THUNK_DATA)(pImport->FirstThunk);
        if(pThunk == 0 && pThunkIAT == 0) break;
        pszDllName = (char*)((BYTE*)hMod+pImport->Name);
#ifdef _LOGDEBUG
        logmsg("dllname:  [%s]\n",(const char *)pszDllName);
#endif
        if ( PathMatchSpecA(pszDllName,"msvcr*.dll") )
        {
            strncpy(name,pszDllName,CRT_LEN);
            strncpy(crt_buf,CharLowerA(name),len);
            ret = true;
            break;
        }
        pImport++;
    }
    FreeLibrary(hMod);
    return ret;
}

/* 加载 msvcrt */
static HMODULE 
init_mscrt(const char* crt_name)
{
    HMODULE hcrt = NULL;
    do
    {
        if ( NULL != moz_put_env )
        {
            break;
        }
        if ( (hcrt = LoadLibraryExA(crt_name ,NULL, 0)) == NULL )
        {
            break;
        }
        if ( (moz_put_env = (MOZ_SETENV)GetProcAddress(hcrt,"_wputenv")) == NULL )
        {
            hcrt = NULL;
            break;
        }
    }while(0);
    return hcrt;
}

unsigned WINAPI 
pentadactyl_fixed(void * pParam)
{
    WCHAR *rc_path = NULL;
    WCHAR m_env[VALUE_LEN+1] = {0};
    WCHAR m_value[VALUE_LEN+1] = {0};
    if ( !read_appkey( L"Env",L"VimpPentaHome", m_value, sizeof(m_value), NULL ) )
    {
        return (0);
    }
    if ( (rc_path = (WCHAR *)SYS_MALLOC(VALUE_LEN+1)) == NULL )
    {
        return (0);
    }
    if ( true )
    {
        PathToCombineW( m_value, VALUE_LEN );
        _snwprintf(m_env, VALUE_LEN, L"%ls%ls", L"HOME=", m_value);
        moz_put_env( m_env );
        SHCreateDirectoryExW( NULL, m_value, NULL );
        dull_replace(m_value, VALUE_LEN, L"\\", L"\\\\");
        _snwprintf(m_env, VALUE_LEN,L"%ls%ls",L"PENTADACTYL_RUNTIME=", m_value);
        moz_put_env( m_env );
        _snwprintf(rc_path, VALUE_LEN, L"%ls\\\\_pentadactylrc", m_value);
    }
    if ( !PathFileExistsW(rc_path) )
    {
        DWORD  m_bytes;
        const  char* desc = "\" File created by libportable.\r\n";
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
    
    if ( wcslen(rc_path) > 1 && \
        _snwprintf( m_env,
                    VALUE_LEN,
                    L"%ls%ls",
                    L"PENTADACTYL_INIT=:source ",
                    rc_path
                   ) > 0
        )
    {
        moz_put_env( m_env );
    }
    SYS_FREE(rc_path);
    return (1);
}

unsigned WINAPI 
foreach_env(void *pParam)
{
    LPWSTR env_buf = NULL;
    LPWSTR m_key;
    if ( (env_buf = (LPWSTR)SYS_MALLOC((MAX_ENV_SIZE+1)*sizeof(WCHAR))) == NULL )
    {
        return (0);
    }
    if ( GetPrivateProfileSectionW(L"Env", env_buf, MAX_ENV_SIZE, ini_path) < 4 )
    {
        SYS_FREE(env_buf);
        return (0);
    }
    m_key = env_buf;
    while(*m_key != L'\0')
    {
        if ( _wcsnicmp( m_key, L"NpluginPath", wcslen(L"NpluginPath") ) == 0 ||
             _wcsnicmp( m_key, L"VimpPentaHome", wcslen(L"VimpPentaHome") ) == 0 || 
             _wcsnicmp(m_key, L"TmpDataPath", wcslen(L"TmpDataPath")) == 0
           )
        {
            ;
        }
        else
        {
            moz_put_env( m_key );
        }
        m_key += wcslen(m_key)+1;
    }
    SYS_FREE(env_buf);
    return (1);
}

unsigned WINAPI 
set_plugins(void *pParam)
{
    WCHAR   val_str[VALUE_LEN+1] = {0};
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
            if ( moz_put_env( env_str ) == -1 )
            {
            #ifdef _LOGDEBUG
                logmsg("_wputenv(%ls) return false!\n", env_str);
            #endif
            }
        }
    }
    return (1);
}

unsigned WINAPI set_envp(void *pParam)
{
    char    crt[CRT_LEN+1] = {0};
    HMODULE m_crt = NULL;
    HANDLE  m_tsk[3] = { NULL, NULL, NULL };
    do
    {
        if ( !find_msvcrt(crt,CRT_LEN) )
        {
            break;
        }
        if ( (m_crt = init_mscrt(crt)) == NULL )
        {
            break;
        }
        m_tsk[0] = (HANDLE)_beginthreadex(NULL,0,&set_plugins,NULL,0,NULL);
        m_tsk[1] = (HANDLE)_beginthreadex(NULL,0,&foreach_env,NULL,0,NULL);
        m_tsk[2] = (HANDLE)_beginthreadex(NULL,0,&pentadactyl_fixed,NULL,0,NULL);
        if ( MsgWaitForMultipleObjects(3, m_tsk, true, INFINITE, QS_ALLINPUT) == WAIT_OBJECT_0 )
        {
            CloseHandle(m_tsk[0]);
            CloseHandle(m_tsk[1]);
            CloseHandle(m_tsk[2]);
        }
    }while(0);
    if ( m_crt )
    {
        FreeLibrary(m_crt);
    }
    return (1);
}
