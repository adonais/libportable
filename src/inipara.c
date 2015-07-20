#include "inipara.h"
#include <shlwapi.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#ifdef _MSC_VER
#include <stdarg.h>
#endif

#define MAX_ALLSECTIONS 320
#define SECTION_NAMES 32
#define MAX_SECTION 10

typedef DWORD (WINAPI *PFNGFVSW)(LPCWSTR, LPDWORD);
typedef DWORD (WINAPI *PFNGFVIW)(LPCWSTR, DWORD, DWORD, LPVOID);
typedef bool  (WINAPI *PFNVQVW)(LPCVOID, LPCWSTR, LPVOID, PUINT);

typedef struct _LANGANDCODEPAGE
{
    uint16_t wLanguage;
    uint16_t wCodePage;
} LANGANDCODEPAGE;

extern WCHAR      ini_path[MAX_PATH+1];
extern char       logfile_buf[VALUE_LEN+1];

static PFNGFVSW   pfnGetFileVersionInfoSizeW;
static PFNGFVIW   pfnGetFileVersionInfoW;
static PFNVQVW    pfnVerQueryValueW;
_NtLoadLibraryExW OrgiLoadLibraryExW = NULL;
HMODULE           dll_module         = NULL;

bool WINAPI
init_parser(LPWSTR inifull_name,DWORD len)
{
    bool ret = false;
    GetModuleFileNameW(dll_module,inifull_name,len);
    PathRemoveFileSpecW(inifull_name);
    PathAppendW(inifull_name,L"portable.ini");
    ret = PathFileExistsW(inifull_name);
    if (!ret)
    {
        if ( PathRemoveFileSpecW(inifull_name) )
        {
            PathAppendW(inifull_name,L"tmemutil.ini");
            ret = PathFileExistsW(inifull_name);
        }
    }
    return ret;
}

bool WINAPI
read_appkey(LPCWSTR lpappname,           /* 区段名 */
            LPCWSTR lpkey,               /* 键名  */
            LPWSTR  prefstring,          /* 保存值缓冲区 */
            DWORD   bufsize,             /* 缓冲区大小 */
            void*   filename             /* 文件名,默认为空 */
           )
{
    DWORD   res = 0;
    LPWSTR  lpstring;
    LPCWSTR pfile = (LPCWSTR)filename;
    lpstring = (LPWSTR)SYS_MALLOC(bufsize);
    if ( pfile == NULL )
    {
        res = GetPrivateProfileStringW(lpappname, 
                                       lpkey ,
                                       L"", 
                                       lpstring, 
                                       bufsize, 
                                       ini_path);
    }
    else
    {
        res = GetPrivateProfileStringW(lpappname, 
                                       lpkey ,
                                       L"", 
                                       lpstring, 
                                       bufsize, 
                                       pfile);
    }
    if (res == 0 && GetLastError() != 0x0)
    {
        SYS_FREE(lpstring);
        return false;
    }
    wcsncpy(prefstring,lpstring,bufsize/sizeof(WCHAR)-1);
    prefstring[res] = L'\0';
    return ( res>0 );
}

int  WINAPI 
read_appint(LPCWSTR cat,LPCWSTR name)
{
    return GetPrivateProfileIntW(cat, name, -1, ini_path);
}

bool WINAPI
foreach_section(LPCWSTR cat,                     /* ini 区段 */
                WCHAR (*lpdata)[VALUE_LEN+1],    /* 二维数组首地址,保存多个段值 */
                int line                         /* 二维数组行数 */
               )
{
    DWORD	res = 0;
    LPWSTR	lpstring;
    LPWSTR	m_key;
    int		i = 0;
    const	WCHAR delim[] = L"=";
    DWORD	num = VALUE_LEN*sizeof(WCHAR)*line;
    if ( (lpstring = (LPWSTR)SYS_MALLOC(num)) != NULL )
    {
        if ( (res = GetPrivateProfileSectionW(cat, 
                                              lpstring, 
                                              num, 
                                              ini_path)
             ) > 0 
           )
        {
            fzero(*lpdata,num);
            m_key = lpstring;
            while(*m_key != L'\0'&& i < line)
            {
                LPWSTR strtmp;
                WCHAR t_str[VALUE_LEN] = {0};
                wcsncpy(t_str,m_key,VALUE_LEN-1);
                strtmp = StrStrW(t_str, delim);
                if (strtmp)
                {
                    wcsncpy(lpdata[i],&strtmp[1],VALUE_LEN-1);
                }
                m_key += wcslen(m_key)+1;
                ++i;
            }
        }
        SYS_FREE(lpstring);
    }
    return ( res>0 );
}

#ifdef _LOGDEBUG
void __cdecl 
logmsg(const char * format, ...)
{
    va_list args;
    int	    len	 ;
    char    buffer[VALUE_LEN+3];
    va_start (args, format);
    len = _vscprintf(format, args);
    if (len > 0 && len < VALUE_LEN && strlen(logfile_buf) > 0)
    {
        FILE *pFile = NULL;
        len = _vsnprintf(buffer,len,format, args);
        buffer[len++] = '\n';
        buffer[len] = '\0';
        if ( (pFile = fopen(logfile_buf,"a+")) != NULL )
        {
            fprintf(pFile,buffer);
            fclose(pFile);
        }
        va_end(args);
    }
    return;
}
#endif

static HMODULE 
init_verinfo(void)          /* 初始化version.dll里面的三个函数 */
{
    HMODULE h_ver = LoadLibraryW(L"version.dll");
    if (h_ver != NULL)
    {
        pfnGetFileVersionInfoSizeW = (PFNGFVSW)GetProcAddress( \
                                     h_ver, "GetFileVersionInfoSizeW");
        pfnGetFileVersionInfoW = (PFNGFVIW)GetProcAddress( \
                                 h_ver, "GetFileVersionInfoW");
        pfnVerQueryValueW = (PFNVQVW)GetProcAddress( \
                            h_ver, "VerQueryValueW");
        if ( !(pfnGetFileVersionInfoSizeW && 
               pfnGetFileVersionInfoW && 
               pfnVerQueryValueW) 
           )
        {
            FreeLibrary(h_ver);
            h_ver = NULL;
        }
    }
    return h_ver;
}

static bool 
get_productname(LPCWSTR filepath, LPWSTR out_string, size_t len)
{
    HMODULE  h_ver = NULL;
    bool     ret = false;
    DWORD    dwHandle = 0;
    DWORD    dwSize = 0;
    uint16_t i;
    uint32_t cbTranslate = 0;
    LPWSTR   pBuffer = NULL;
    PVOID    pTmp = NULL;
    WCHAR    dwBlock[NAMES_LEN+1] = {0};
    LANGANDCODEPAGE *lpTranslate = NULL;
    do
    {
        if ( (h_ver = init_verinfo()) == NULL )
        {
            break;
        }
        if ( (dwSize = pfnGetFileVersionInfoSizeW(filepath, &dwHandle)) == 0 )
        {
        #ifdef _LOGDEBUG
            logmsg("pfnGetFileVersionInfoSizeW return false\n");
        #endif
            break;
        }
        if ( ( pBuffer = (LPWSTR)SYS_MALLOC(dwSize*sizeof(WCHAR)) ) == NULL )
        {
            break;
        }
        if( !pfnGetFileVersionInfoW(filepath,0,dwSize,(LPVOID)pBuffer) )
        {
        #ifdef _LOGDEBUG
            logmsg("pfnpfnGetFileVersionInfoW return false\n");
        #endif
            break;
        }
        pfnVerQueryValueW((LPCVOID)pBuffer,
                          L"\\VarFileInfo\\Translation",
                          (LPVOID*)&lpTranslate,&cbTranslate
                         );
        if ( NULL == lpTranslate )
        {
            break;
        }
        for ( i=0; i < (cbTranslate/sizeof(LANGANDCODEPAGE)); i++ )
        {
            _snwprintf(dwBlock,
                       NAMES_LEN,
                       L"\\StringFileInfo\\%04x%04x\\ProductName",
                       lpTranslate[i].wLanguage, 
                       lpTranslate[i].wCodePage
                      );
            ret = pfnVerQueryValueW((LPCVOID)pBuffer,
                                    (LPCWSTR)dwBlock,
                                    (LPVOID *)&pTmp, 
                                    &cbTranslate);
            if (ret)
            {
                out_string[0] = L'\0';
                wcsncpy(out_string, (LPCWSTR)pTmp, len);
                ret = wcslen(out_string) > 1;
                if (ret) break;
            }
        }
    } while (0);
    if ( pBuffer )
    {
        SYS_FREE(pBuffer);
    }
    if ( h_ver )
    {
        FreeLibrary(h_ver);
    }
    return ret;
}

LPWSTR WINAPI
stristrW(LPCWSTR Str, LPCWSTR Pat)       /* 忽略大小写查找子串,功能同StrStrIW函数 */
{
    WCHAR *pptr, *sptr, *start;

    for (start = (WCHAR *)Str; *start != L'\0'; start++)
    {
        for ( ; ((*start!=L'\0') && (toupper(*start) != toupper(*Pat))); start++);
        if (L'\0' == *start) return NULL;
        pptr = (WCHAR *)Pat;
        sptr = (WCHAR *)start;
        while (toupper(*sptr) == toupper(*pptr))
        {
            sptr++;
            pptr++;
            if (L'\0' == *pptr) return (start);
        }
    }
    return NULL;
}

static void 
replace_separator(LPWSTR path)        /* 替换unix风格的路径符号 */
{
    LPWSTR   lp = NULL;
    intptr_t pos;
    do
    {
        lp =  StrChrW(path,L'/');
        if (lp)
        {
            pos = lp-path;
            path[pos] = L'\\';
        }
    } while (lp!=NULL);
    return;
}

bool WINAPI 
exists_dir(LPCWSTR path) 
{
    DWORD fileattr = GetFileAttributesW(path);
    if (fileattr != INVALID_FILE_ATTRIBUTES)
    {
        return (fileattr & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
    return false;
}

bool WINAPI 
create_dir(LPCWSTR full_path) 
{
    int err;
    if (exists_dir(full_path))
    {
        return true;
    }
    err = SHCreateDirectoryExW(NULL, full_path, NULL);
    return err == ERROR_SUCCESS;
}

bool WINAPI 
PathToCombineW(LPWSTR lpfile, int len)
{
    int n = 1;
    if ( NULL == lpfile || *lpfile == L' ' )
    {
        return false;
    }
    if ( lpfile[0] == L'%' )
    {
        WCHAR buf_env[VALUE_LEN+1] = {0};
        while ( lpfile[n] != L'\0' )
        {
            if ( lpfile[n] == L'%' )
            {
                break;
            }
            ++n;
        }
        if ( n < len )
        {
            _snwprintf(buf_env, n+1 ,L"%ls", lpfile);
        }
        if ( wcslen(buf_env) > 1 &&
             ExpandEnvironmentStringsW(buf_env,buf_env,VALUE_LEN) > 0
           )
        {
            WCHAR tmp_env[VALUE_LEN+1] = {0};
            _snwprintf(tmp_env, len ,L"%ls%ls", buf_env, &lpfile[n+1]);
            n = _snwprintf(lpfile, len ,L"%ls", tmp_env);
        }
    }
    if ( lpfile[1] != L':' )
    {
        WCHAR buf_modname[VALUE_LEN+1] = {0};
        replace_separator(lpfile);
        if ( GetModuleFileNameW( dll_module, buf_modname, VALUE_LEN) > 0)
        {
            WCHAR tmp_path[MAX_PATH] = {0};
            if ( PathRemoveFileSpecW(buf_modname) && 
                 PathCombineW(tmp_path,buf_modname,lpfile) )
            {
                n = _snwprintf(lpfile,len,L"%ls",tmp_path);
            }
        }
    }
    return (n>0 && n<len);
}

HWND WINAPI
get_moz_hwnd(LPWNDINFO pInfo)
{
    HWND hwnd = NULL;
    while ( !pInfo->hFF )    /* 等待主窗口并获取句柄 */
    {
        bool  m_loop = false;
        DWORD dwProcessId = 0;
        hwnd = FindWindowExW( NULL, hwnd, L"MozillaWindowClass", NULL );
        GetWindowThreadProcessId(hwnd, &dwProcessId);
        m_loop = (dwProcessId > 0 && dwProcessId == pInfo->hPid);
        if ( !m_loop )
        {
            pInfo->hFF = NULL;
        }
        if (NULL != hwnd && m_loop)
        {
            pInfo->hFF = hwnd;
            break;
        }
        SleepEx(800,false);
    }
    return (hwnd!=NULL?hwnd:pInfo->hFF);
}

bool WINAPI
IsGUI(LPCWSTR lpFileName)
{
    IMAGE_DOS_HEADER dos_header;
    IMAGE_NT_HEADERS pe_header;
    bool	ret = false;
    HANDLE	hFile = CreateFileW(lpFileName,GENERIC_READ,
                                FILE_SHARE_READ,NULL,OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,NULL);
    if( !goodHandle(hFile) )
    {
        return ret;
    }
    do
    {
        DWORD readed = 0;
        DWORD m_ptr  = SetFilePointer( hFile,0,NULL,FILE_BEGIN );
        if ( INVALID_SET_FILE_POINTER == m_ptr )
        {
            break;
        }
        ret = ReadFile( hFile,&dos_header,sizeof(IMAGE_DOS_HEADER),&readed,NULL );
        if( ret && readed != sizeof(IMAGE_DOS_HEADER) && \
            dos_header.e_magic != 0x5a4d )
        {
            break;
        }
        m_ptr = SetFilePointer( hFile,dos_header.e_lfanew,NULL,FILE_BEGIN );
        if ( INVALID_SET_FILE_POINTER == m_ptr )
        {
            break;
        }
        ret = ReadFile( hFile,&pe_header,sizeof(IMAGE_NT_HEADERS),&readed,NULL );
        if( ret && readed != sizeof(IMAGE_NT_HEADERS) )
        {
            break;
        }
        ret = pe_header.OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI;
    } while (0);
    CloseHandle(hFile);
    return ret;
}

static bool 
GetCurrentProcessName(LPWSTR lpstrName, DWORD wlen)
{
    int   i = 0;
    WCHAR lpFullPath[MAX_PATH+1]= {0};
    if ( GetModuleFileNameW(NULL,lpFullPath,MAX_PATH)>0 )
    {
        for( i=(int)wcslen(lpFullPath); i>0; i-- )
        {
            if (lpFullPath[i] == L'\\')
                break;
        }
        if ( i > 0 )
        {
            i = _snwprintf(lpstrName,wlen,L"%ls",lpFullPath+i+1);
        }
    }
    return (i>0 && i<(int)wlen);
}

bool WINAPI 
GetCurrentWorkDir(LPWSTR lpstrName, DWORD wlen)
{
    int   i = 0;
    WCHAR lpFullPath[MAX_PATH+1] = {0};
    if ( GetModuleFileNameW(NULL,lpFullPath,MAX_PATH)>0 )
    {
        for( i=(int)wcslen(lpFullPath); i>0; i-- )
        {
            if (lpFullPath[i] == L'\\')
            {
                lpFullPath[i] = L'\0';
                break;
            }
        }
        if ( i > 0 )
        {
            i = _snwprintf(lpstrName,wlen,L"%ls",lpFullPath);
        }
    }
    return (i>0 && i<(int)wlen);
}

static __inline bool is_ff_dev(void)
{
    bool     ret = false;
    WCHAR    process_name[VALUE_LEN+1];
    WCHAR    product_name[NAMES_LEN+1] = {0};
    if ( GetCurrentProcessName(process_name,VALUE_LEN) && \
         get_productname(process_name, product_name, NAMES_LEN) )
    {
        ret = _wcsicmp(L"FirefoxDeveloperEdition", product_name) == 0;
    }
    return ret;
}

bool WINAPI is_browser(void)
{
    WCHAR process_name[VALUE_LEN+1];
    GetCurrentProcessName(process_name,VALUE_LEN);
    return ( !(_wcsicmp(process_name, L"Iceweasel.exe") &&
               _wcsicmp(process_name, L"firefox.exe")	&&
               _wcsicmp(process_name, L"lawlietfox.exe") )
           );
}

bool WINAPI 
is_specialapp(LPCWSTR appname)
{
    WCHAR process_name[VALUE_LEN+1];
    GetCurrentProcessName(process_name,VALUE_LEN);
    return ( _wcsicmp(process_name, appname) == 0 );
}

bool WINAPI 
is_specialdll(uintptr_t callerAddress,LPCWSTR dll_file)
{
    bool    ret = false;
    HMODULE hCallerModule = NULL;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, 
                          (LPCWSTR)callerAddress, 
                          &hCallerModule))
    {
        WCHAR szModuleName[VALUE_LEN+1] = {0};
        if ( GetModuleFileNameW(hCallerModule, szModuleName, VALUE_LEN) )
        {
            if ( StrChrW(dll_file,L'*') || StrChrW(dll_file,L'?') )
            {
                if ( PathMatchSpecW(szModuleName, dll_file) )
                {
                    ret = true;
                }
            }
            else if ( stristrW(szModuleName, dll_file) )
            {
                ret = true;
            }
        }
    }
    if ( hCallerModule )
    {
        FreeLibrary(hCallerModule);
    }
    return ret;
}

/* 获取profiles.ini文件绝对路径,保存到in_dir数组 */
static __inline bool 
get_mozilla_profile(LPCWSTR app_path, LPWSTR in_dir, int len)
{
    int m = 0;
    if (is_specialapp(L"thunderbird.exe"))
    {
        m = _snwprintf(in_dir,
                       (size_t)len,
                       L"%ls%ls",
                       app_path,
                       L"\\Thunderbird\\profiles.ini"
                      );
    }
    else if (is_browser())
    {
        m = _snwprintf(in_dir,
                       (size_t)len,
                       L"%ls%ls",
                       app_path,
                       L"\\Mozilla\\Firefox\\profiles.ini"
                      );
    }
    in_dir[m] = L'\0';
    return (m>0 && m<len);
}

/* 获取配置文件夹所在路径,保存到dist_buf */
bool WINAPI 
get_mozprofiles_path(LPCWSTR app_path, WCHAR *dist_buf, int len)
{
    int ret = 0;
    do
    {
        if ( !dist_buf )
        {
            break;
        }
        if ( read_appint(L"General", L"Portable") <= 0 )
        {
            break;
        }
        ret = _snwprintf(dist_buf,(size_t)len,L"%ls",app_path);
        if ( ret > 1 && ret < len )
        {
            ret = PathRemoveFileSpecW(dist_buf);
        }
    } while (0);
    return (ret>0&&ret<len);
}

/* 查找moz_values所在段,并把段名保存在out_names数组
 * 函数成功返回值为0,返回任何其他值意味着段没有找到 
 */
static int 
search_section_names(LPCWSTR moz_profile, 
                     LPCWSTR moz_values, 
                     LPWSTR out_names, 
                     int len)
{
    int    ret = -1;
    LPWSTR m_section,str_section = NULL;
    if ( (m_section = (LPWSTR)SYS_MALLOC(MAX_ALLSECTIONS*sizeof(WCHAR)+1)) == NULL )
    {
        return ret;
    }
    if ( GetPrivateProfileSectionNamesW(m_section,MAX_ALLSECTIONS,moz_profile) > 0 )
    {
        int     i = 0;
        LPCWSTR pf = L"Profile";
        size_t  j  = wcslen(pf);
        str_section = m_section;
        while ( *str_section != L'\0' &&  i < MAX_SECTION )
        {
            WCHAR values[SECTION_NAMES] = {0};

            if ( wcsncmp(str_section,pf,j) == 0 && \
                 read_appkey(str_section,L"Name",values,sizeof(values),(void *)moz_profile) )
            {
                if ( wcsncmp(values, moz_values, wcslen(moz_values))==0 && \
                     _snwprintf(out_names,(size_t)len,L"%ls",str_section) > 0 )
                {
                    ret = 0;
                    break;
                }
                else
                {
                    ret = StrToIntW(&str_section[j]);
                    if ( ret>=0 ) ++ret;
                }
            }
            str_section += wcslen(str_section)+1;
            ++i;
        }
    }
    SYS_FREE(m_section);
    return ret;
}

bool WINAPI 
WaitWriteFile(LPCWSTR app_path)
{
    bool   ret   = false;
    LPWSTR szDir = NULL;
    WCHAR  moz_profile[MAX_PATH+1] = {0};
    if ( !get_mozilla_profile(app_path, moz_profile, MAX_PATH) )
    {
        return ret;
    }
    if ( exists_dir(moz_profile) )
    {
        WCHAR app_names[MAX_PATH+1] = {0};
        WCHAR m_profile[10] = {L'P',L'r',L'o',L'f',L'i',L'l',L'e',};
        search_section_names(moz_profile, L"default", app_names, SECTION_NAMES);
        if ( is_specialapp(L"thunderbird.exe") )
        {
            ret = WritePrivateProfileStringW(app_names,L"Path",L"../../",moz_profile);
        }
        else if ( is_ff_dev() )
        {
            int m = search_section_names(\
                    moz_profile, 
                    L"dev-edition-default", 
                    app_names, 
                    SECTION_NAMES);
            if ( m > 0 )
            {
                /* 更新dev版配置文件 */
                _snwprintf(m_profile+wcslen(m_profile), 2, L"%d", m);
                ret = WritePrivateProfileSectionW(\
                      m_profile,
                      L"Name=dev-edition-default\r\nIsRelative=1\r\nPath=../../../\r\n\0",
                      moz_profile);
            }
        }
        ret = WritePrivateProfileStringW(app_names,
                                         L"IsRelative",
                                         L"1",
                                         moz_profile
                                        );
        ret = WritePrivateProfileStringW(app_names,
                                         L"Path",
                                         L"../../../",
                                         moz_profile
                                        );
    }
    else
    {
        if ( (szDir = (LPWSTR)SYS_MALLOC( sizeof(moz_profile) ) ) != NULL )
        {
            wcsncpy(szDir, moz_profile, MAX_PATH);
        }
        if ( szDir && PathRemoveFileSpecW( szDir ) && create_dir( szDir ) )
        {
            if ( !WritePrivateProfileSectionW(\
                 L"General",
                 L"StartWithLastProfile=1\r\n\0",
                 moz_profile))
            {
                SYS_FREE(szDir); return ret;
            }
            if ( is_specialapp(L"thunderbird.exe") )
            {
                ret = WritePrivateProfileSectionW(\
                      L"Profile0",
                      L"Name=default\r\nIsRelative=1\r\nPath=../../\r\nDefault=1\r\n\0",
                      moz_profile);
            }
            else if ( is_ff_dev() )
            {
                ret = WritePrivateProfileSectionW(\
                      L"Profile0",
                      L"Name=dev-edition-default\r\nIsRelative=1\r\nPath=../../../\r\n\0",
                      moz_profile);
            }
            else
            {
                ret = WritePrivateProfileSectionW(\
                      L"Profile0",
                      L"Name=default\r\nIsRelative=1\r\nPath=../../../\r\nDefault=1\r\n\0",
                      moz_profile);
            }
        }
    }
    if ( szDir ) SYS_FREE(szDir);
    return ret;
}

DWORD WINAPI 
GetOsVersion(void)
{
    OSVERSIONINFOEXA    osvi;
    bool                bOs = false;
    DWORD               ver = 0L;
    fzero(&osvi, sizeof(OSVERSIONINFOEXA));

    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
    if( GetVersionExA((OSVERSIONINFOA*)&osvi) )
    {
        if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId &&
             osvi.dwMajorVersion > 4 )
        {
            char pszOS[4] = {0};
            _snprintf(pszOS, 3, "%lu%d%lu", osvi.dwMajorVersion,0,osvi.dwMinorVersion);
            ver = strtol(pszOS, NULL, 10);
        }
    }
    return ver;
}
