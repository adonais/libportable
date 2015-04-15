#define INI_EXTERN

#include "inipara.h"
#include "MinHook.h"
#include <shlwapi.h>
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

typedef	int   (__cdecl *_PR_setenv)(const char *envA);
typedef DWORD (WINAPI *PFNGFVSW)(LPCWSTR, LPDWORD);
typedef DWORD (WINAPI *PFNGFVIW)(LPCWSTR, DWORD, DWORD, LPVOID);
typedef bool  (WINAPI *PFNVQVW)(LPCVOID, LPCWSTR, LPVOID, PUINT);

typedef struct _LANGANDCODEPAGE
{
    uint16_t wLanguage;
    uint16_t wCodePage;
} LANGANDCODEPAGE;

extern WCHAR  ini_path[MAX_PATH+1];

static PFNGFVSW	  pfnGetFileVersionInfoSizeW;
static PFNGFVIW	  pfnGetFileVersionInfoW;
static PFNVQVW	  pfnVerQueryValueW;
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

bool WINAPI 
PathToCombineW(LPWSTR lpfile, int len)
{
    int n = 1;
    EnterSpinLock();
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
            PathRemoveFileSpecW(buf_modname);
            if ( PathCombineW(tmp_path,buf_modname,lpfile) )
            {
                n = _snwprintf(lpfile,len,L"%ls",tmp_path);
            }
        }
    }
    LeaveSpinLock();
    return (n>0 && n<len);
}

HWND WINAPI 
get_moz_hwnd(LPWNDINFO pInfo)
{
    HWND hwnd = NULL;
    EnterSpinLock();
    while ( !pInfo->hFF )                 /* 等待主窗口并获取句柄 */
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
        Sleep(800);
    }
    LeaveSpinLock();
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

static bool __inline 
is_ff_dev(void)
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

bool WINAPI 
is_browser(void)
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
    if ( (m_section = (LPWSTR)SYS_MALLOC(MAX_ALLSECTIONS*sizeof(WCHAR)+1)) != NULL )
    {
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
                    if ( wcscmp(values, moz_values)==0 && \
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
    }
    return ret;
}

bool WINAPI 
WaitWriteFile(LPCWSTR app_path)
{
    bool  ret = false;
    WCHAR moz_profile[MAX_PATH+1] = {0};
    FROZEN_THREADS threads;
    if ( !get_mozilla_profile(app_path, moz_profile, MAX_PATH) )
    {
        return ret;
    }
    Freezex(&threads);
    if ( PathFileExistsW(moz_profile) )
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
        LPWSTR szDir;
        if ( (szDir = (LPWSTR)SYS_MALLOC( sizeof(moz_profile) ) ) != NULL )
        {
            wcsncpy (szDir, moz_profile, MAX_PATH);
            PathRemoveFileSpecW( szDir );
            SHCreateDirectoryExW(NULL,szDir,NULL);
            SYS_FREE(szDir);
            WritePrivateProfileSectionW(\
            L"General",
            L"StartWithLastProfile=1\r\n\0",
            moz_profile);
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
    Unfreeze(&threads);
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

char* WINAPI 
unicode_ansi(LPCWSTR pwszUnicode)
{
    int   iSize;
    char* pszByte = NULL;
    iSize = WideCharToMultiByte(CP_ACP, 0, pwszUnicode, -1, NULL, 0, NULL, NULL);
    pszByte = (char*)SYS_MALLOC( iSize+sizeof(char) );
    if ( !pszByte )
    {
        return NULL;
    }
    if ( !WideCharToMultiByte(CP_ACP, 0, pwszUnicode, -1, pszByte, iSize, NULL, NULL) )
    {
        SYS_FREE(pszByte);
    }
    return pszByte;
}

static int __cdecl 
write_env(WCHAR* env)
{
    int     ret = -1;
    HMODULE hcrt = NULL;
    char*   envA = NULL;
    WCHAR   dll_path[MAX_PATH+1] = {0};
    static  _PR_setenv moz_put_env;
    do
    {
        if ( NULL == moz_put_env )
        {   
            if ( !GetCurrentWorkDir(dll_path, MAX_PATH) )
            {
                break;
            }
            if ( !PathAppendW(dll_path,L"nss3.dll") )
            {
                break;
            }
            hcrt = OrgiLoadLibraryExW?OrgiLoadLibraryExW(dll_path,NULL,0):\
                   LoadLibraryExW(dll_path,NULL,0);
            if ( hcrt == NULL )
            {
            #ifdef _LOGDEBUG
                logmsg("LoadLibraryW in %s return false\n", __FUNCTION__);
            #endif
                break;
            }
            if ( (moz_put_env = (_PR_setenv)GetProcAddress(hcrt,"PR_SetEnv")) == NULL )
            {
            #ifdef _LOGDEBUG
                logmsg("GetProcAddress in %s return false\n", __FUNCTION__);
            #endif
                break;
            }
        }
        if ( (envA = unicode_ansi(env)) == NULL )
        {
            break;
        }
    } while (0);
    if ( envA )
    {
        ret = moz_put_env(envA);
        SYS_FREE(envA);
    }
    if ( hcrt )
    {
        FreeLibrary(hcrt);
    }
    return ret;
}

static void 
pentadactyl_fixed(WCHAR* m_value, size_t m_size)
{
    WCHAR *rc_path = NULL;
    WCHAR env_str[VALUE_LEN+1] = {0};
    if ( (rc_path = (WCHAR *)SYS_MALLOC(VALUE_LEN+1)) == NULL )
    {
        return;
    }
    dull_replace(m_value, m_size, L"\\", L"\\\\");
    if ( _snwprintf(env_str,
                    m_size,
                    L"%ls%ls",
                    L"PENTADACTYL_RUNTIME=",
                    m_value
                   ) > 0
        )
    {
        write_env( env_str );
    }
    _snwprintf(rc_path,
               VALUE_LEN, 
               L"%ls\\\\_pentadactylrc",
               m_value
              );
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
        _snwprintf( env_str,
                    m_size,
                    L"%ls%ls",
                    L"PENTADACTYL_INIT=:source ",
                    rc_path
                   ) > 0
        )
    {
        write_env( env_str );
    }
    SYS_FREE(rc_path);
    return;
}

static void 
foreach_env(LPWSTR m_key)
{
    while(*m_key != L'\0')
    {
        WCHAR val_str[VALUE_LEN+1] = {0};
        WCHAR env_str[VALUE_LEN+1] = {0};
        if ( _wcsnicmp( m_key, L"NpluginPath", wcslen(L"NpluginPath") ) == 0 && \
             read_appkey( L"Env",L"NpluginPath", val_str, sizeof(val_str), NULL )
           )
        {
            PathToCombineW( val_str, VALUE_LEN );
            if ( _snwprintf(env_str,
                            VALUE_LEN,
                            L"%ls%ls",
                            L"MOZ_PLUGIN_PATH=",
                            val_str
                           ) > 0
               )
            {
                write_env( env_str );
            }
        }
        else if ( _wcsnicmp( m_key, L"VimpPentaHome", wcslen(L"VimpPentaHome") ) == 0 && \
                  read_appkey( L"Env", L"VimpPentaHome", val_str, sizeof(val_str), NULL)
                )
        {
            PathToCombineW( val_str, VALUE_LEN );
            if ( _snwprintf(env_str,
                            VALUE_LEN,
                            L"%ls%ls",
                            L"HOME=",
                            val_str
                           ) > 0
                )
            {
                write_env( env_str );
                SHCreateDirectoryExW( NULL, val_str, NULL );
                pentadactyl_fixed( val_str, VALUE_LEN );
            }

        }
        else if ( _wcsnicmp(m_key, L"TmpDataPath", wcslen(L"TmpDataPath")) == 0 )
        {
            /* ignore the variables can result in init_global_env function */
        }
        else
        {
            write_env( m_key );
        }
        m_key += wcslen(m_key)+1;
    }
}

unsigned WINAPI 
SetPluginPath(void * pParam)
{
    int    ret = 0;
    LPWSTR lpstring = NULL;
    do
    {
        if ( (lpstring = (LPWSTR)SYS_MALLOC(MAX_ENV_SIZE)) == NULL )
        {
            break;
        }
        ret = GetPrivateProfileSectionW(L"Env", lpstring, MAX_ENV_SIZE-1, ini_path);
        if ( ret < 4 )
        {
            break;
        }
        foreach_env(lpstring);
    }while (0);
    SYS_FREE(lpstring);
    return (ret);
}
