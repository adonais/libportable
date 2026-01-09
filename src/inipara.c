#include "inipara.h"
#include "confpath.h"
#include "internal_crt.h"
#include <shlwapi.h>
#include <tlhelp32.h>
#include <shlobj.h>

#ifdef _MSC_VER
#include <stdarg.h>
#endif

#define MAX_ALLSECTIONS 320
#define SECTION_NAMES 32
#define MAX_SECTION 10

extern WCHAR    ini_path[MAX_PATH+1];
extern WCHAR    logfile_buf[MAX_PATH+1];
extern volatile INT_PTR mpv_window_hwnd;

WCHAR *WINAPI
mp_make_u16(const char *utf8, WCHAR *utf16, int len)
{
    *utf16 = 0;
    int m = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, utf16, len);
    if (m > 0 && m <= len)
    {
        utf16[m-1] = 0;
    }
    else if (len > 0)
    {
        utf16[len-1] = 0;
    }
    return utf16;
}

char *WINAPI
mp_make_u8(const WCHAR *utf16, char *utf8, int len)
{
    *utf8 = 0;
    int m = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, utf8, len, NULL, NULL);
    if (m > 0 && m <= len)
    {
        utf8[m-1] = 0;
    }
    else if (len > 0)
    {
        utf8[len-1] = 0;
    }
    return utf8;
}

bool WINAPI
init_parser(LPWSTR inifull_name, const int len)
{
    return find_mpv_conf(inifull_name, len);
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
    LPCWSTR pfile = (LPCWSTR)filename;
    if (pfile == NULL)
    {
        res = GetPrivateProfileStringW(lpappname, 
                                       lpkey ,
                                       L"", 
                                       prefstring, 
                                       bufsize, 
                                       ini_path);
    }
    else
    {
        res = GetPrivateProfileStringW(lpappname, 
                                       lpkey ,
                                       L"", 
                                       prefstring, 
                                       bufsize, 
                                       pfile);
    }
    if (res == 0 && GetLastError() != 0x0)
    {
        return false;
    }
    prefstring[res] = L'\0';
    return (res > 0);
}

int WINAPI 
read_appint(LPCWSTR cat, LPCWSTR name)
{
    int ret = -1;
    if (ini_path[1] != L':')
    {
        if (!find_mpv_conf(ini_path, MAX_PATH))
        {
            return ret;
        }
    }
    ret = GetPrivateProfileIntW(cat, name, 0, ini_path);
    return ret;
}

#ifdef _LOGDEBUG
void WINAPI 
init_logs(void)
{
    if (*logfile_buf == '\0' && GetEnvironmentVariableW(L"APPDATA", logfile_buf, MAX_PATH) > 0)
    {
        api_wcsncat(logfile_buf, L"\\", MAX_PATH);
        api_wcsncat(logfile_buf, LOG_FILE, MAX_PATH);
        if (PathFileExistsW(logfile_buf))
        {
            DeleteFileW(logfile_buf);
        }
    }
}

void __cdecl
logmsg(const char * format, ...)
{
    va_list args;
    int	    len	= 0;
    char    buffer[MAX_BUFFER+3];
    va_start (args, format);
    if (api_wcslen(logfile_buf) > 0)
    {
        HANDLE *pfile = NULL;
        len = wvnsprintfA(buffer, MAX_BUFFER, format, args);
        buffer[len++] = '\n';
        buffer[len] = '\0';
        if ((pfile = api_wfopen(logfile_buf, L"a+")) != NULL)
        {
            DWORD dw;
            WriteFile(pfile, buffer, len, &dw, NULL);
            api_fclose(pfile);
        }
        va_end(args);
    }
}
#endif

static void 
unix2win(LPWSTR path)        /* 替换unix风格的路径符号 */
{
    LPWSTR   lp = NULL;
    INT_PTR pos;
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
get_basedir(LPWSTR name, const int wlen)
{
    int   i = 0;
    if (GetModuleFileNameW(NULL, name, (DWORD)wlen) > 0)
    {
        for (i = (int)api_wcslen(name); i > 0; i--)
        {
            if (name[i] == L'\\')
            {
                name[i] = L'\0';
                break;
            }
        }
    }
    return (i > 0 && i < wlen);
}
