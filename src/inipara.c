#define INI_EXTERN

#include "inipara.h"
#include <shlwapi.h>
#include <tlhelp32.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef _MSC_VER
#  include <stdarg.h>
#endif

typedef	int (__cdecl *_PR_setenv)(const char* envA);

/* only init once */
static WCHAR profile_path[MAX_PATH+1];

volatile LONG g_locked = FALSE;
static VOID EnterSpinLock(VOID)
{
    while (_InterlockedCompareExchange(&g_locked, TRUE, FALSE) != FALSE)
    {
        Sleep(1);
    }
}
static VOID LeaveSpinLock(VOID)
{
    _InterlockedExchange(&g_locked, FALSE);
}

BOOL WINAPI ini_ready(LPWSTR inifull_name,DWORD str_len)
{
    BOOL rect = FALSE;
    EnterSpinLock();
#ifdef LIBPORTABLE_STATIC
    dll_module = NULL;
#endif
    GetModuleFileNameW(dll_module,inifull_name,str_len);
    PathRemoveFileSpecW(inifull_name);
    PathAppendW(inifull_name,L"portable.ini");
    rect = PathFileExistsW(inifull_name);
    if (!rect)
    {
        if ( PathRemoveFileSpecW(inifull_name) )
        {
            PathAppendW(inifull_name,L"tmemutil.ini");
            rect = PathFileExistsW(inifull_name);
        }
    }
    LeaveSpinLock();
    return rect;
}

BOOL read_appkey(LPCWSTR lpappname,              /* 区段名 */
                 LPCWSTR lpkey,					 /* 键名  */
                 LPWSTR  prefstring,			 /* 保存值缓冲区 */
                 DWORD   bufsize				 /* 缓冲区大小 */
                )
{
    DWORD  res = 0;
    LPWSTR lpstring;
    if ( profile_path[1] != L':' )
    {
        if (!ini_ready(profile_path,MAX_PATH))
        {
            return res;
        }
    }
    lpstring = (LPWSTR)SYS_MALLOC(bufsize);
    res = GetPrivateProfileStringW(lpappname, lpkey ,L"", lpstring, bufsize, profile_path);
    if (res == 0 && GetLastError() != 0x0)
    {
        SYS_FREE(lpstring);
        return FALSE;
    }
    wcsncpy(prefstring,lpstring,bufsize/sizeof(WCHAR)-1);
    prefstring[res] = '\0';
    return ( res>0 );
}

int read_appint(LPCWSTR cat,LPCWSTR name)
{
    int res = -1;
    if ( profile_path[1] != L':' )
    {
        if (!ini_ready(profile_path,MAX_PATH))
        {
            return res;
        }
    }
    res = GetPrivateProfileIntW(cat, name, -1, profile_path);
    return res;
}

BOOL foreach_section(LPCWSTR cat,						/* ini 区段 */
                     WCHAR (*lpdata)[VALUE_LEN+1],	    /* 二维数组首地址,保存多个段值 */
                     int line							/* 二维数组行数 */
                    )
{
    DWORD	res = 0;
    LPWSTR	lpstring;
    LPWSTR	strKey;
    int		i = 0;
    const	WCHAR delim[] = L"=";
    DWORD	num = VALUE_LEN*sizeof(WCHAR)*line;
    if ( profile_path[1] != L':' )
    {
        if (!ini_ready(profile_path,MAX_PATH))
        {
            return res;
        }
    }
    if ( (lpstring = (LPWSTR)SYS_MALLOC(num)) != NULL )
    {
        if ( (res = GetPrivateProfileSectionW(cat, lpstring, num, profile_path)) > 0 )
        {
            fzero(*lpdata,num);
            strKey = lpstring;
            while(*strKey != L'\0'&& i < line)
            {
                LPWSTR strtmp;
                WCHAR t_str[VALUE_LEN] = {0};
                wcsncpy(t_str,strKey,VALUE_LEN-1);
                strtmp = StrStrW(t_str, delim);
                if (strtmp)
                {
                    wcsncpy(lpdata[i],&strtmp[1],VALUE_LEN-1);
                }
                strKey += wcslen(strKey)+1;
                ++i;
            }
        }
        SYS_FREE(lpstring);
    }
    return (BOOL)res;
}

BOOL inifile_exist(void)
{
    WCHAR file[VALUE_LEN+1] = {0};
    if ( profile_path[1] == L':' )
        return PathFileExistsW(profile_path);
    else
        return ini_ready(file,VALUE_LEN);
}

#ifdef _LOGDEBUG
void __cdecl logmsg(const char * format, ...)
{
    va_list args;
    int		len	 ;
    char	buffer[VALUE_LEN+3];
    va_start (args, format);
    len	 =	_vscprintf(format, args);
    if (len > 0 && len < VALUE_LEN && strlen(logfile_buf) > 0)
    {
        FILE	*pFile = NULL;
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

LPWSTR stristrW(LPCWSTR Str, LPCWSTR Pat)
{
    WCHAR *pptr, *sptr, *start;

    for (start = (WCHAR *)Str; *start != '\0'; start++)
    {
        for ( ; ((*start!='\0') && (toupper(*start) != toupper(*Pat))); start++);
        if ('\0' == *start) return NULL;
        pptr = (WCHAR *)Pat;
        sptr = (WCHAR *)start;
        while (toupper(*sptr) == toupper(*pptr))
        {
            sptr++;
            pptr++;
            if ('\0' == *pptr) return (start);
        }
    }
    return NULL;
}

void WINAPI charTochar(LPWSTR path)
{
    LPWSTR	lp = NULL;
    INT_PTR	pos;
    do
    {
        lp =  StrChrW(path,L'/');
        if (lp)
        {
            pos = lp-path;
            path[pos] = L'\\';
        }
    }while (lp!=NULL);
    return;
}

BOOL PathToCombineW(LPWSTR lpfile, size_t str_len)
{
    size_t n = 1;
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
        if ( n < str_len )
        {
            _snwprintf(buf_env, n+1 ,L"%ls", lpfile);
        }
        if ( wcslen(buf_env) > 1 && ExpandEnvironmentStringsW(buf_env,buf_env,VALUE_LEN) > 0 )
        {
            WCHAR tmp_env[VALUE_LEN+1] = {0};
            _snwprintf(tmp_env, str_len ,L"%ls%ls", buf_env, &lpfile[n+1]);
            n = _snwprintf(lpfile, str_len ,L"%ls", tmp_env);
        }
    }
    if ( lpfile[1] != L':' )
    {
        WCHAR buf_modname[VALUE_LEN+1] = {0};
        charTochar(lpfile);
        if ( GetModuleFileNameW( dll_module, buf_modname, VALUE_LEN) > 0)
        {
            WCHAR tmp_path[MAX_PATH] = {0};
            PathRemoveFileSpecW(buf_modname);
            if ( PathCombineW(tmp_path,buf_modname,lpfile) )
            {
                n = _snwprintf(lpfile,str_len,L"%ls",tmp_path);
            }
        }
    }
    LeaveSpinLock();
    return (n>0);
}

static int GetNumberOfWorkers(void)
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (int)(si.dwNumberOfProcessors);
}

unsigned WINAPI SetCpuAffinity_tt(void * pParam)
{
    DWORD_PTR   mask = 0L;
    int         cpu_z = GetNumberOfWorkers();
    /* 设置进程总是在除开CPU0之外的其他核心运行 */
    switch (cpu_z)
    {
    case 2:
        mask = 0x02;
        break;
    case 3:
        mask = 0x06;
        break;
    case 4:
        mask = 0x0e;
        break;
    case 6:
        mask = 0x3e;
        break;
    case 8:
        mask = 0xfe;
        break;
    default:
        break;
    }
    if (mask)
    {
        SetProcessAffinityMask(GetCurrentProcess(),mask);
    }
    return (1);
}

BOOL WINAPI IsGUI(LPCWSTR lpFileName)
{
    IMAGE_DOS_HEADER dos_header;
    IMAGE_NT_HEADERS pe_header;
    DWORD	readed;
    BOOL	ret     = FALSE;
    HANDLE	hFile	=  CreateFileW(lpFileName,GENERIC_READ,
                                   FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        return ret;
    }
    SetFilePointer(hFile,0,0,FILE_BEGIN);

    ReadFile(hFile,&dos_header,sizeof(IMAGE_DOS_HEADER),&readed,NULL);
    if(readed != sizeof(IMAGE_DOS_HEADER))
    {
        CloseHandle(hFile);
        return ret;
    }
    if(dos_header.e_magic != 0x5a4d)
    {
        CloseHandle(hFile);
        return ret;
    }
    SetFilePointer(hFile,dos_header.e_lfanew,NULL,FILE_BEGIN);
    ReadFile(hFile,&pe_header,sizeof(IMAGE_NT_HEADERS),&readed,NULL);
    if(readed != sizeof(IMAGE_NT_HEADERS))
    {
        CloseHandle(hFile);
        return ret;
    }
    CloseHandle(hFile);
    if(pe_header.OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
    {
        ret = TRUE;
    }
    return ret;
}

BOOL WINAPI GetCurrentProcessName(LPWSTR lpstrName, DWORD wlen)
{
    size_t i = 0;
    WCHAR lpFullPath[MAX_PATH+1]= {0};
    if ( GetModuleFileNameW(NULL,lpFullPath,MAX_PATH)>0 )
    {
        for( i=wcslen(lpFullPath); i>0; i-- )
        {
            if (lpFullPath[i] == L'\\')
                break;
        }
        if ( i > 0 )
        {
            i = _snwprintf(lpstrName,wlen,L"%ls",lpFullPath+i+1);
        }
    }
    return (i>0 && i<wlen);
}

BOOL WINAPI GetCurrentWorkDir(LPWSTR lpstrName, DWORD wlen)
{
    size_t i = 0;
    WCHAR lpFullPath[MAX_PATH+1] = {0};
    if ( GetModuleFileNameW(NULL,lpFullPath,MAX_PATH)>0 )
    {
        for( i=wcslen(lpFullPath); i>0; i-- )
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
    return (i>0 && i<wlen);
}

BOOL is_nplugins(void)
{
    WCHAR	process_name[VALUE_LEN+1] ;
    GetCurrentProcessName(process_name,VALUE_LEN);
    return ( _wcsicmp(process_name, L"plugin-container.exe") == 0 );
}

BOOL is_thunderbird(void)
{
    WCHAR	process_name[VALUE_LEN+1];
    GetCurrentProcessName(process_name,VALUE_LEN);
    return ( _wcsicmp(process_name, L"thunderbird.exe") == 0 );
}

BOOL is_browser(void)
{
    WCHAR	process_name[VALUE_LEN+1];
    GetCurrentProcessName(process_name,VALUE_LEN);
    return ( !(_wcsicmp(process_name, L"Iceweasel.exe") &&
               _wcsicmp(process_name, L"firefox.exe")	&&
               _wcsicmp(process_name, L"lawlietfox.exe") )
           );
}

BOOL WINAPI is_specialdll(UINT_PTR callerAddress,LPCWSTR dll_file)
{
    BOOL	ret = FALSE;
    HMODULE hCallerModule = NULL;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)callerAddress, &hCallerModule))
    {
        WCHAR szModuleName[VALUE_LEN+1] = {0};
        if ( GetModuleFileNameW(hCallerModule, szModuleName, VALUE_LEN) )
        {
            if ( StrChrW(dll_file,L'*') || StrChrW(dll_file,L'?') )
            {
                if ( PathMatchSpecW(szModuleName, dll_file) )
                {
                    ret = TRUE;
                }
            }
            else if ( stristrW(szModuleName, dll_file) )
            {
                ret = TRUE;
            }
        }
    }
    return ret;
}

BOOL WINAPI get_mozilla_profile(LPCWSTR app, LPWSTR in_dir, size_t len)
{
    int m = 0;
    if (is_thunderbird())
    {
        m = _snwprintf(in_dir,len,L"%ls%ls",app,L"\\Thunderbird\\profiles.ini");
    }
    else if (is_browser())
    {
        m = _snwprintf(in_dir,len,L"%ls%ls",app,L"\\Mozilla\\Firefox\\profiles.ini");
    }
    return (m>0 && m < (int)len);
}

DWORD WINAPI GetOsVersion(void)
{
    OSVERSIONINFOEXA	osvi;
    BOOL				bOs = FALSE;
    DWORD				ver = 0L;
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

/* 必须使用进程依赖crt的wputenv函数追加环境变量 */
unsigned WINAPI SetPluginPath(void * pParam)
{
    int				ret = 0;
    HMODULE			hcrt =NULL;
    _PR_setenv		write_env = NULL;
    LPWSTR			lpstring;
    do
    {
        if ( (lpstring = (LPWSTR)SYS_MALLOC(MAX_ENV_SIZE)) == NULL )
        {
        #ifdef _LOGDEBUG
            logmsg("SYS_MALLOC(MAX_ENV_SIZE) return false\n");
        #endif
            break;
        }
        ret = GetPrivateProfileSectionW(L"Env", lpstring, MAX_ENV_SIZE-1, profile_path);

        if ( ret < 4 )
        {
        #ifdef _LOGDEBUG
            logmsg("GetPrivateProfileSectionW return false\n");
        #endif
            break;;
        }
        if ( (hcrt = LoadLibraryW(L"nss3.dll")) != NULL && \
             (write_env = (_PR_setenv)GetProcAddress(hcrt,"PR_SetEnv")) != NULL )
        {
            LPWSTR	strKey = lpstring;
            while(*strKey != L'\0')
            {
                WCHAR value_str[VALUE_LEN+1] = {0};
                WCHAR env_string[VALUE_LEN+1] = {0};
                char* envA = NULL;
                if ( _wcsnicmp(strKey, L"NpluginPath", wcslen(L"NpluginPath")) == 0 && \
                     read_appkey(L"Env",L"NpluginPath",value_str,sizeof(value_str)) )
                {
                    PathToCombineW(value_str, VALUE_LEN);
                    if ( _snwprintf(env_string,VALUE_LEN,L"%ls%ls",L"MOZ_PLUGIN_PATH=",value_str) > 0)
                    {
                        envA = unicode_ansi(env_string);
                        ret = write_env( envA );
                    }
                }
                else if	( _wcsnicmp(strKey, L"TmpDataPath", wcslen(L"TmpDataPath")) == 0 )
                {
                    /* 忽略此变量,已经由init_global_env函数处理完成 */
                }
                else
                {
                    envA = unicode_ansi(strKey);
                    ret = write_env( envA );
                }
                strKey += wcslen(strKey)+1;
                if (envA)
                {
                    SYS_FREE(envA);
                }
            }
            FreeLibrary(hcrt);
        }
    } while (0);
    if (lpstring)
    {
        SYS_FREE(lpstring);
    }
    return (ret);
}

BOOL WINAPI parse_shcommand(void)
{
    LPWSTR	*args = NULL;
    int		m_arg = 0;
    BOOL    ret = FALSE;
    args = CommandLineToArgvW(GetCommandLineW(), &m_arg);
    if ( NULL != args )
    {
        int i;
        for (i = 1; i < m_arg; ++i)
        {
            if ( StrStrIW(args[i],L"preferences") || StrStrIW(args[i],L"silent") )
            {
                ret = TRUE;
                break;
            }
        }
        LocalFree(args);
    }
    return ret;
}

void WINAPI refresh_tray(void)
{
    HWND hwnd ;          /* tray hwnd */
    RECT m_trayToolBar;
    int  x;
    hwnd = FindWindowW(L"Shell_TrayWnd", NULL);
    hwnd = FindWindowExW(hwnd, 0, L"TrayNotifyWnd", NULL);
    hwnd = FindWindowExW(hwnd, 0, L"SysPager", NULL);
    hwnd = FindWindowExW(hwnd, 0, L"ToolbarWindow32", NULL);
    GetClientRect(hwnd, &m_trayToolBar);
    for(x = 1; x < m_trayToolBar.right - 1; x++)
    {
        int y = m_trayToolBar.bottom / 2;
        PostMessage(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
    }
}

HANDLE WINAPI search_process(LPCWSTR lpstr, DWORD m_parent)
{
    BOOL   b_more;
    PROCESSENTRY32W pe32;
    HANDLE hSnapshot = INVALID_HANDLE_VALUE;
    DWORD  chi_pid[PROCESS_NUM] = {0};
    HANDLE m_handle = NULL;
    volatile int    i = 1;
    static   int    h_num = 1;
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if( hSnapshot == INVALID_HANDLE_VALUE )
    {
    #ifdef _LOGDEBUG
        logmsg("CreateToolhelp32Snapshot (of processes) error %lu\n",GetLastError() );
    #endif
        return m_handle;
    }
    chi_pid[0] = m_parent;
    pe32.dwSize=sizeof(pe32);
    b_more = Process32FirstW(hSnapshot,&pe32);
    while (b_more)
    {
        if ( m_parent == pe32.th32ParentProcessID )
        {
            chi_pid[i++] = pe32.th32ProcessID;
            if (i>=PROCESS_NUM)
            {
                break;
            }
        }
        if ( lpstr && pe32.th32ParentProcessID>4 && StrStrIW((LPWSTR)lpstr,(LPCWSTR)pe32.szExeFile) )
        {
            m_handle = (HANDLE)pe32.th32ProcessID;
            break;
        }
        b_more = Process32NextW(hSnapshot,&pe32);
    }
    CloseHandle(hSnapshot);
    if ( !m_handle && chi_pid[0] )
    {
        for ( i=1 ; i<PROCESS_NUM&&h_num<PROCESS_NUM; ++i )
        {
            HANDLE tmp = OpenProcess(PROCESS_TERMINATE, FALSE, chi_pid[i]);
            if ( NULL != tmp )
            {
                EnterSpinLock();
                g_handle[h_num++] =  tmp;
                LeaveSpinLock();
                search_process(NULL, chi_pid[i]);
            }
        }
    }
    return m_handle;
}

int WINAPI get_parameters(LPWSTR wdir, LPWSTR lpstrCmd, DWORD len)
{
    int		ret = -1;
    LPWSTR	lp = NULL;
    WCHAR   temp[VALUE_LEN+1]   = {0};
    WCHAR   m_para[VALUE_LEN+1] = {0};
    if ( read_appkey(L"attach",L"ExPath",temp,sizeof(temp)) )
    {
        wdir[0] = L'\0';
        lp =  StrChrW(temp,L',');
        if (isdigit(temp[wcslen(temp)-1])&&lp)
        {
            ret = temp[wcslen(temp)-1] - L'0';
            temp[lp-temp] = L'\0';
            lp =  StrChrW(temp,L' ');
            /* 如果第三方进程存在参数,工作目录设为浏览器主进程所在目录 */
            if ( lp )
            {
                temp[lp-temp] = L'\0';
                _snwprintf(m_para,VALUE_LEN,L" "L"%ls",lp+1);
                if ( !GetCurrentWorkDir(wdir,len) )
                {
                    wdir[0] = L'\0';
                }
            }
            _snwprintf(lpstrCmd,len,L"%ls",temp);
            if ( lpstrCmd[0] == L'.' || lpstrCmd[0] == L'%' )
            {
                PathToCombineW(lpstrCmd,VALUE_LEN);
            }
            wcsncat(lpstrCmd,m_para,len);
            if ( wcslen(wdir) == 0 )
            {
                _snwprintf(wdir,len,L"%ls",lpstrCmd);
                if ( !PathRemoveFileSpecW(wdir) )
                {
                    wdir[0] = L'\0';
                }
            }
        }
    }
    return ret;
}

unsigned WINAPI run_process(void * pParam)
{
    PROCESS_INFORMATION pi;
    STARTUPINFOW si;
    WCHAR wcmd[VALUE_LEN+1] = {0};
    WCHAR wdirectory[VALUE_LEN+1] = {0};
    DWORD dwCreat = 0;
    int flags = get_parameters(wdirectory, wcmd, VALUE_LEN);
    if (flags<0)
    {
        return (0);
    }
    /* 如果是预启动,直接返回 */
    if ( parse_shcommand() )
    {
        return (0);
    }
    if ( wcslen(wcmd)>0  && !search_process(wcmd,0) )
    {
        fzero(&si,sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_MINIMIZE;
        if (!flags)
        {
            si.wShowWindow = SW_HIDE;
            dwCreat |= CREATE_NEW_PROCESS_GROUP;
        }
        if(!CreateProcessW(NULL,
                           (LPWSTR)wcmd,
                           NULL,
                           NULL,
                           FALSE,
                           dwCreat,
                           NULL,
                           (LPCWSTR)wdirectory,
                           &si,&pi))
        {
        #ifdef _LOGDEBUG
            logmsg("CreateProcessW error %lu\n",GetLastError());
        #endif
            return (0);
        }
        g_handle[0] = pi.hProcess;
        CloseHandle(pi.hThread);
        if ( pi.dwProcessId >4 && (SleepEx(6000,FALSE) == 0) )
        {
            search_process(NULL, pi.dwProcessId);
        }
    }
    return (1);
}

char* WINAPI unicode_ansi(LPCWSTR pwszUnicode)
{
    int iSize; 
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
