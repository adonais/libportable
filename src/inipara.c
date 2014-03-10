#define INI_EXTERN

#include "inipara.h"
#include <shlwapi.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef _MSC_VER
#  include <stdarg.h>
#endif

typedef BOOL  (WINAPI *_pGdiFlush)(void);
typedef DWORD (WINAPI *_pGdiSetBatchLimit)(DWORD);
typedef BOOL  (WINAPI *_pInitializeCriticalSectionEx)(CRITICAL_SECTION *lpCriticalSection,DWORD dwSpinCount,DWORD Flags);

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
				int n = _snwprintf(lpfile,str_len,L"%ls",tmp_path);
				lpfile[n] = L'\0';
			}
		}
	}
	return TRUE;
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
			i = _snwprintf(lpstrName,wlen-1,L"%ls",lpFullPath+i+1);
			lpstrName[i] = L'\0';
		}
	}
	return !!i;
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
