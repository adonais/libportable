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

typedef BOOL     (WINAPI *_pGdiFlush)(void);
typedef DWORD    (WINAPI *_pGdiSetBatchLimit)(DWORD);
typedef BOOL     (WINAPI *_pInitializeCriticalSectionEx)(CRITICAL_SECTION *lpCriticalSection,DWORD dwSpinCount,DWORD Flags);

BOOL WINAPI ini_ready(LPWSTR inifull_name,DWORD str_len)
{
	BOOL rect = FALSE;
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
	wcsncpy(prefstring,lpstring,bufsize/sizeof(wchar_t)-1);
	prefstring[res] = '\0';
	SYS_FREE(lpstring);
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
					 wchar_t (*lpdata)[VALUE_LEN+1],	/* 二维数组首地址,保存多个段值 */
					 int line							/* 二维数组行数 */
					 )
{
	DWORD	res = 0;
	LPWSTR	lpstring;
	LPWSTR	strKey;
	int		i = 0;
	const	wchar_t delim[] = L"=";
	DWORD	num = VALUE_LEN*sizeof(wchar_t)*line;
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
				wchar_t t_str[VALUE_LEN] = {0};
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
    wchar_t *pptr, *sptr, *start;

    for (start = (wchar_t *)Str; *start != '\0'; start++)
    {
        for ( ; ((*start!='\0') && (toupper(*start) != toupper(*Pat))); start++);
        if ('\0' == *start) return NULL;
        pptr = (wchar_t *)Pat;
        sptr = (wchar_t *)start;
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
	}
	while (lp!=NULL);
	return;
}

BOOL PathToCombineW(LPWSTR lpfile, size_t str_len)
{
	int n = 1;
	if ( dll_module && lpfile[1] != L':' )
	{
		wchar_t buf_modname[VALUE_LEN+1] = {0};
		charTochar(lpfile);
		if ( GetModuleFileNameW( dll_module, buf_modname, VALUE_LEN) > 0)
		{
			wchar_t tmp_path[MAX_PATH] = {0};
			PathRemoveFileSpecW(buf_modname);
			if ( PathCombineW(tmp_path,buf_modname,lpfile) )
			{
				n = _snwprintf(lpfile,str_len,L"%ls",tmp_path);
			}
		}
	}
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
	int     cpu_z = 0;
	HANDLE	hc = (HANDLE)pParam;
	if (hc)
	{
		SuspendThread(hc);
		cpu_z = GetNumberOfWorkers();
		if ( !cpu_z )
		{
			cpu_z = read_appint(L"General",L"ProcessAffinityMask");
		}
		if ( cpu_z>5 )
		{
			SetThreadAffinityMask(hc, 0x1c); 
		}
		else if ( cpu_z>3 )
		{
			SetThreadAffinityMask(hc, 0xe); 
		}
		else if ( cpu_z>2 )
		{
			SetThreadAffinityMask(hc, 0x6); 
		}
		else
		{
			SetThreadAffinityMask(hc, 0x1); 
		}
		ResumeThread(hc);
		CloseHandle(hc);
	}
	return (1);
}

unsigned WINAPI GdiSetLimit_tt(void * pParam)
{
	HANDLE	 hc = (HANDLE)pParam;
	HMODULE	 hGdi32 = GetModuleHandleW(L"gdi32.dll");
	int		 limit = read_appint(L"General",L"GdiBatchLimit");
	if (hc && hGdi32 && limit > 0)
	{
		_pGdiSetBatchLimit pfnGdiSetBatchLimit = (_pGdiSetBatchLimit)GetProcAddress(hGdi32, "GdiSetBatchLimit");
		_pGdiFlush pfnGdiFlush = (_pGdiFlush)GetProcAddress(hGdi32, "GdiFlush");
		if (pfnGdiSetBatchLimit && pfnGdiFlush)
		{
		#ifdef _LOGDEBUG
			logmsg("exec GdiSetBatchLimit()\n");
		#endif
			SuspendThread(hc);
			pfnGdiSetBatchLimit(limit);
			pfnGdiFlush();
			ResumeThread(hc);
		}
	}
	if (hc)
	{
		CloseHandle(hc);
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
	WCHAR lpFullPath[MAX_PATH+1]={0};
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
	return (i>0);
}

BOOL WINAPI GetCurrentWorkDir(LPWSTR lpstrName, DWORD wlen)
{
	size_t i = 0;
	WCHAR lpFullPath[MAX_PATH+1]={0};
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
	return (i>0);
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

/* 从输入表查找CRT版本 */
BOOL WINAPI find_msvcrt(char *crt_name,int len)
{
	BOOL			ret = FALSE;
	IMAGE_DOS_HEADER      *pDos;
	IMAGE_OPTIONAL_HEADER *pOptHeader;
	IMAGE_IMPORT_DESCRIPTOR    *pImport ;
	HMODULE hMod=GetModuleHandleW(NULL);
	if (!hMod)
	{
	#ifdef _LOGDEBUG
		logmsg("GetModuleHandleW false,hMod = 0\n");
	#endif
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
	while( TRUE )
	{
		char*		pszDllName = NULL;
		char		name[CRT_LEN+1] = {0};
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
			strncpy(crt_name,CharLowerA(name),len);
			ret = TRUE;
			break;
		}
		pImport++;
	}
	return ret;
}

/* 必须使用进程依赖crt的wputenv函数追加环境变量 */
unsigned WINAPI SetPluginPath(void * pParam)
{
	typedef			int (__cdecl *_pwrite_env)(LPCWSTR envstring);
	int				ret = 0;
	HMODULE			hCrt =NULL;
	_pwrite_env		write_env = NULL;
	char			msvc_crt[CRT_LEN+1] = {0};
	LPWSTR			lpstring;
	if ( !find_msvcrt(msvc_crt,CRT_LEN) )
	{
		return ((unsigned)ret);
	}
	if ( (hCrt = GetModuleHandleA(msvc_crt)) == NULL )
	{
		return ((unsigned)ret);
	}
	if ( profile_path[1] != L':' )
	{
		if (!ini_ready(profile_path,MAX_PATH))
		{
			return ((unsigned)ret);
		}
	}
	write_env = (_pwrite_env)GetProcAddress(hCrt,"_wputenv");
	if ( write_env )
	{
		if ( (lpstring = (LPWSTR)SYS_MALLOC(MAX_ENV_SIZE)) != NULL )
		{
			if ( (ret = GetPrivateProfileSectionW(L"Env", lpstring, MAX_ENV_SIZE-1, profile_path)) > 0 )
			{
				LPWSTR	strKey = lpstring;
				while(*strKey != L'\0') 
				{
					if ( stristrW(strKey, L"NpluginPath") )
					{
						WCHAR lpfile[VALUE_LEN+1];
						if ( read_appkey(L"Env",L"NpluginPath",lpfile,sizeof(lpfile)) )
						{
							WCHAR env_string[VALUE_LEN+1] = {0};
							PathToCombineW(lpfile, VALUE_LEN);
							if ( _snwprintf(env_string,VALUE_LEN,L"%ls%ls",L"MOZ_PLUGIN_PATH=",lpfile) > 0)
							{
								ret = write_env( (LPCWSTR)env_string );
							}
						}
					}
					else if	(stristrW(strKey, L"TmpDataPath"))
					{
						;
					}
					else
					{
						ret = write_env( (LPCWSTR)strKey );
					}
					strKey += wcslen(strKey)+1;
				}
			}
			SYS_FREE(lpstring);
		}
	}
	return ( (unsigned)ret );
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
		logmsg("CreateToolhelp32Snapshot (of processes) error %ld\n",GetLastError() );
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
				g_handle[h_num++] =  tmp;
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
			if (lpstrCmd[0] == L'.')
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
	// PathFileExistsW(wcmd)
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

BOOL WINAPI new_locks(LOCKS *lock)
{
	BOOL ok = TRUE;
	CRITICAL_SECTION *cs = &lock->mutex;
	LONG volatile *used = &lock->use;
	_pInitializeCriticalSectionEx pfnInitializeCriticalSectionEx = NULL;
	HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
	if ( hKernel32 && *used != 1)
	{
		pfnInitializeCriticalSectionEx = (_pInitializeCriticalSectionEx)GetProcAddress(hKernel32, \
			                             "InitializeCriticalSectionEx");
		if ( pfnInitializeCriticalSectionEx ) 
		{
			ok = pfnInitializeCriticalSectionEx(cs, LOCK_SPIN_COUNT,
												CRITICAL_SECTION_NO_DEBUG_INFO);
		} 
		else
		{
			ok = InitializeCriticalSectionAndSpinCount(cs, LOCK_SPIN_COUNT);
		}
		InterlockedExchange(used,(LONG)ok);
	}
	return ok;
}

BOOL WINAPI lc_lock(LOCKS *lock)
{
	BOOL  ok = FALSE;
	if (lock->use == 1)
	{
		EnterCriticalSection(&lock->mutex);
		ok = TRUE;
	}
	return ok;
}

BOOL WINAPI un_lock(LOCKS *lock)
{
	BOOL  ok = FALSE;
	DWORD_PTR threadId = (DWORD_PTR)lock->mutex.OwningThread;
	if ( threadId == GetCurrentThreadId() )
	{
		LeaveCriticalSection(&lock->mutex);
		ok = TRUE;
	}
	return ok;
}

void WINAPI free_locks(LOCKS *lock)
{
	LONG volatile *used = &lock->use;
	if (*used == 1)
	{
		DeleteCriticalSection(&(lock)->mutex);
		InterlockedExchange(used,(LONG)0);
	}
}
