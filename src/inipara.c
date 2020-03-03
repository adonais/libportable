#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <shlwapi.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include "inipara.h"

#ifdef _MSC_VER
#include <stdarg.h>
#endif

#define MAX_MESSAGE 1024
#define MAX_ALLSECTIONS 640
#define SECTION_NAMES 32
#define MAX_SECTION 16

typedef DWORD(WINAPI *PFNGFVSW)(LPCWSTR, LPDWORD);
typedef DWORD(WINAPI *PFNGFVIW)(LPCWSTR, DWORD, DWORD, LPVOID);
typedef bool(WINAPI *PFNVQVW)(LPCVOID, LPCWSTR, LPVOID, PUINT);
extern bool __stdcall json_parser(wchar_t *moz_profile);

typedef struct _LANGANDCODEPAGE
{
    uint16_t wLanguage;
    uint16_t wCodePage;
} LANGANDCODEPAGE;

static PFNGFVSW pfnGetFileVersionInfoSizeW;
static PFNGFVIW pfnGetFileVersionInfoW;
static PFNVQVW pfnVerQueryValueW;
LoadLibraryExPtr sLoadLibraryExStub = NULL;

#ifdef _LOGDEBUG
static char logfile_buf[MAX_PATH + 1];

void WINAPI
init_logs(void)
{
    if (*logfile_buf == '\0' && GetEnvironmentVariableA("APPDATA", logfile_buf, MAX_PATH) > 0)
    {
        strncat(logfile_buf, "\\", MAX_PATH);
        strncat(logfile_buf, LOG_FILE, MAX_PATH);
    }
}

void __cdecl 
logmsg(const char *format, ...)
{
    va_list args;
    char buffer[MAX_MESSAGE];
    va_start(args, format);
    if (strlen(logfile_buf) > 0)
    {
        FILE *pFile = NULL;
        int len = wvnsprintfA(buffer, MAX_MESSAGE, format, args);
        if (len > 0 && len < MAX_MESSAGE)
        {
            buffer[len] = '\n';
            buffer[len + 1] = '\0';
            if ((pFile = fopen(logfile_buf, "a+")) != NULL)
            {
                fwrite(buffer, strlen(buffer), 1, pFile);
                fclose(pFile);
            }
        }
    }
    va_end(args);
    return;
}
#endif

bool WINAPI
get_ini_path(WCHAR *ini, int len)
{
    bool ret = false;
    if (!GetModuleFileNameW(dll_module, ini, len))
    {
        return ret;
    }
    if ((ret = PathRemoveFileSpecW(ini) && PathAppendW(ini, L"portable.ini") && PathFileExistsW(ini)) == false)
    {
        WCHAR example[MAX_PATH + 1] = { 0 };
        wcsncpy(example, ini, MAX_PATH);
        ret = PathRemoveFileSpecW(example) && PathAppendW(example, L"tmemutil.ini") && PathFileExistsW(example);
        if (ret)
        {
            wcsncpy(ini, example, len);
        }
        else if (PathRemoveFileSpecW(example) && PathAppendW(example, L"portable(example).ini") && PathFileExistsW(example))
        {
            ret = CopyFileW(example, ini, true);
        }
    }
    return ret;
}

static bool
is_utf8_string(const char *utf)
{
#define CHECK_LENGTH MAX_PATH
    int length = (int)strlen(utf);
    int check_sub = 0;
    int i = 0;

    if (length > CHECK_LENGTH) // 取前面特定长度的字符来验证
    {
        length = CHECK_LENGTH;
    }
    for (; i < length; i++)
    {
        if (check_sub == 0)
        {
            if ((utf[i] >> 7) == 0) // 0xxx xxxx
            {
                continue;
            }
            else if ((utf[i] & 0xE0) == 0xC0) // 110x xxxx
            {
                check_sub = 1;
            }
            else if ((utf[i] & 0xF0) == 0xE0) // 1110 xxxx
            {
                check_sub = 2;
            }
            else if ((utf[i] & 0xF8) == 0xF0) // 1111 0xxx
            {
                check_sub = 3;
            }
            else if ((utf[i] & 0xFC) == 0xF8) // 1111 10xx
            {
                check_sub = 4;
            }
            else if ((utf[i] & 0xFE) == 0xFC) // 1111 110x
            {
                check_sub = 5;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if ((utf[i] & 0xC0) != 0x80)
            {
                return false;
            }
            check_sub--;
        }
    }
    return true;
#undef CHECK_LENGTH
}

bool WINAPI
read_appkeyW(LPCWSTR lpappname, /* 区段名 */
             LPCWSTR lpkey,     /* 键名  */
             LPWSTR prefstring, /* 保存值缓冲区 */
             DWORD bufsize,     /* 缓冲区大小 */
             void *filename     /* 文件名,默认为空 */
)
{
    DWORD res = 0;
    LPCWSTR pfile = (LPCWSTR) filename;
    WCHAR ini[MAX_PATH] = { 0 };
    if (!pfile)
    {
        pfile = get_ini_path(ini, MAX_PATH) ? ini : NULL;
    }
    if (pfile != NULL)
    {
        res = GetPrivateProfileStringW(lpappname, lpkey, L"", prefstring, bufsize, pfile);
    }
    if ((res == 0 && GetLastError() != 0x0) || *prefstring == L'\0')
    {
        return false;
    }
    prefstring[res] = L'\0';
    return (res > 0);
}

bool WINAPI
read_appkeyA(LPCSTR lpappname,  /* 区段名 */
             LPCSTR lpkey,      /* 键名  */
             LPWSTR prefstring, /* 保存值缓冲区 */
             DWORD bufsize,     /* 缓冲区大小 */
             void *filename     /* 文件名,默认为空 */
)

{
    DWORD res = 0;
    LPCWSTR pfile = (LPCWSTR) filename;
    WCHAR ini[MAX_PATH] = { 0 };
    char u8_path[MAX_PATH] = { 0 };
    char a8_ini[MAX_PATH] = { 0 };
    if (!pfile)
    {
        pfile = get_ini_path(ini, MAX_PATH) ? ini : NULL;
    }
    if (pfile == NULL)
    {
        return false;
    }
    if (!WideCharToMultiByte(CP_ACP, 0, pfile, -1, a8_ini, MAX_PATH, NULL, NULL))
    {
    #ifdef _LOGDEBUG
        logmsg("WideCharToMultiByte false in %s\n", __FUNCTION__);
    #endif
        return false;
    }
    res = GetPrivateProfileStringA(lpappname, lpkey, "", u8_path, MAX_PATH, a8_ini);
    if ((res == 0 && GetLastError() != 0x0) || *u8_path == '\0')
    {
    #ifdef _LOGDEBUG
        logmsg("GetPrivateProfileStringA false in %s , ini = %s\n", __FUNCTION__, a8_ini);
    #endif
        return false;
    }
    if (is_utf8_string(u8_path))
    {
        res = MultiByteToWideChar(CP_UTF8, 0, u8_path, -1, prefstring, bufsize);
    }
    else
    {
        res = MultiByteToWideChar(CP_ACP, 0, u8_path, -1, prefstring, bufsize);
    }
    if (res > 0)
    {
        prefstring[res] = L'\0';
    }
    else
    {
        *prefstring = L'\0';
    }
    return (res > 0);
}

int WINAPI
read_appint(LPCWSTR cat, LPCWSTR name)
{
    WCHAR pfile[MAX_PATH] = { 0 };
    if (!get_ini_path(pfile, MAX_PATH))
    {
        return -1;
    }
    return GetPrivateProfileIntW(cat, name, 0, pfile);
}

bool WINAPI
foreach_section(LPCWSTR cat,                    /* ini 区段 */
                WCHAR (*lpdata)[VALUE_LEN + 1], /* 二维数组首地址,保存多个段值 */
                int line                        /* 二维数组行数 */
)
{
    DWORD res = 0;
    LPWSTR lpstring;
    LPWSTR m_key;
    int i = 0;
    const WCHAR delim[] = L"=";
    DWORD num = VALUE_LEN * sizeof(WCHAR) * line;
    WCHAR pfile[MAX_PATH] = { 0 };
    if (!get_ini_path(pfile, MAX_PATH))
    {
        return false;
    }
    if ((lpstring = (LPWSTR) SYS_MALLOC(num)) == NULL)
    {
        return false;
    }
    if ((res = GetPrivateProfileSectionW(cat, lpstring, num, pfile)) > 0)
    {
        fzero(*lpdata, num);
        m_key = lpstring;
        while (*m_key != L'\0' && i < line)
        {
            LPWSTR strtmp;
            WCHAR t_str[VALUE_LEN + 1] = { 0 };
            wcsncpy(t_str, m_key, VALUE_LEN);
            strtmp = StrStrW(t_str, delim);
            if (strtmp)
            {
                wcsncpy(lpdata[i], &strtmp[1], VALUE_LEN);
            }
            m_key += wcslen(m_key) + 1;
            ++i;
        }
    }
    SYS_FREE(lpstring);
    return (res > 0);
}

static HMODULE
init_verinfo(void) /* 初始化version.dll里面的三个函数 */
{
    HMODULE h_ver = LoadLibraryW(L"version.dll");
    if (h_ver != NULL)
    {
        pfnGetFileVersionInfoSizeW = (PFNGFVSW) GetProcAddress(h_ver, "GetFileVersionInfoSizeW");
        pfnGetFileVersionInfoW = (PFNGFVIW) GetProcAddress(h_ver, "GetFileVersionInfoW");
        pfnVerQueryValueW = (PFNVQVW) GetProcAddress(h_ver, "VerQueryValueW");
        if (!(pfnGetFileVersionInfoSizeW && pfnGetFileVersionInfoW && pfnVerQueryValueW))
        {
            FreeLibrary(h_ver);
            h_ver = NULL;
        }
    }
    return h_ver;
}

static bool
get_productname(LPCWSTR filepath, LPWSTR out_string, size_t len, bool plugin)
{
    HMODULE h_ver = NULL;
    bool ret = false;
    DWORD dwHandle = 0;
    DWORD dwSize = 0;
    uint16_t i;
    uint32_t cbTranslate = 0;
    LPWSTR pBuffer = NULL;
    PVOID pTmp = NULL;
    WCHAR dwBlock[NAMES_LEN + 1] = { 0 };
    LANGANDCODEPAGE *lpTranslate = NULL;
    do
    {
        if ((h_ver = init_verinfo()) == NULL)
        {
            break;
        }
        if ((dwSize = pfnGetFileVersionInfoSizeW(filepath, &dwHandle)) == 0)
        {
        #ifdef _LOGDEBUG
            logmsg("pfnGetFileVersionInfoSizeW return false\n");
        #endif
            break;
        }
        if ((pBuffer = (LPWSTR) SYS_MALLOC(dwSize * sizeof(WCHAR))) == NULL)
        {
            break;
        }
        if (!pfnGetFileVersionInfoW(filepath, 0, dwSize, (LPVOID) pBuffer))
        {
        #ifdef _LOGDEBUG
            logmsg("pfnpfnGetFileVersionInfoW return false\n");
        #endif
            break;
        }
        pfnVerQueryValueW((LPCVOID) pBuffer, L"\\VarFileInfo\\Translation", (LPVOID *) &lpTranslate, &cbTranslate);
        if (NULL == lpTranslate)
        {
            break;
        }
        for (i = 0; i < (cbTranslate / sizeof(LANGANDCODEPAGE)); i++)
        {
            if (plugin)
            {
                wnsprintfW(dwBlock, NAMES_LEN, L"\\StringFileInfo\\%ls\\ProductName", L"040904e4");
            }
            else
            {
                wnsprintfW(dwBlock,
                           NAMES_LEN,
                           L"\\StringFileInfo\\%04x%04x\\ProductName",
                           lpTranslate[i].wLanguage,
                           lpTranslate[i].wCodePage);
            }
            ret = pfnVerQueryValueW((LPCVOID) pBuffer, (LPCWSTR) dwBlock, (LPVOID *) &pTmp, &cbTranslate);
            if (ret)
            {
                out_string[0] = L'\0';
                wcsncpy(out_string, (LPCWSTR) pTmp, len);
                ret = wcslen(out_string) > 1;
                if (ret) break;
            }
        }
    } while (0);
    if (pBuffer)
    {
        SYS_FREE(pBuffer);
    }
    if (h_ver)
    {
        FreeLibrary(h_ver);
    }
    return ret;
}

int WINAPI
get_file_version(void)
{
    HMODULE h_ver = NULL;
    DWORD dwHandle = 0;
    DWORD dwSize = 0;
    VS_FIXEDFILEINFO *pversion = NULL;
    LPWSTR pbuffer = NULL;
    WCHAR dw_block[NAMES_LEN] = { 0 };
    WCHAR filepath[MAX_PATH] = { 0 };
    uint32_t cb = 0;
    int ver = 0;
    do
    {
        if (GetModuleFileNameW(NULL, filepath, MAX_PATH) == 0)
        {
            break;
        }
        if ((h_ver = init_verinfo()) == NULL)
        {
            break;
        }
        if ((dwSize = pfnGetFileVersionInfoSizeW(filepath, &dwHandle)) == 0)
        {
        #ifdef _LOGDEBUG
            logmsg("pfnGetFileVersionInfoSizeW return false\n");
        #endif
            break;
        }
        if ((pbuffer = (LPWSTR) SYS_MALLOC(dwSize * sizeof(WCHAR))) == NULL)
        {
            break;
        }
        if (!pfnGetFileVersionInfoW(filepath, 0, dwSize, (LPVOID) pbuffer))
        {
        #ifdef _LOGDEBUG
            logmsg("pfnpfnGetFileVersionInfoW return false\n");
        #endif
            break;
        }
        if (!pfnVerQueryValueW((LPCVOID) pbuffer, L"\\", (void **) &pversion, &cb))
        {
        #ifdef _LOGDEBUG
            logmsg("pfnVerQueryValueW return false\n");
        #endif
            break;
        }
        if (pversion != NULL)
        {
            wnsprintfW(dw_block, NAMES_LEN, L"%d%d", HIWORD(pversion->dwFileVersionMS), LOWORD(pversion->dwFileVersionMS));
            ver = _wtoi(dw_block);
        }
    } while (0);
    if (pbuffer)
    {
        SYS_FREE(pbuffer);
    }
    if (h_ver)
    {
        FreeLibrary(h_ver);
    }
    return ver;
}

static void
wchr_replace(LPWSTR path) /* 替换unix风格的路径符号 */
{
    LPWSTR lp = NULL;
    intptr_t pos;
    do
    {
        lp = StrChrW(path, L'/');
        if (lp)
        {
            pos = lp - path;
            path[pos] = L'\\';
        }
    } while (lp != NULL);
    return;
}

/* c风格的unicode字符串替换函数 */
LPWSTR WINAPI
wstr_replace(LPWSTR in, size_t in_size, LPCWSTR pattern, LPCWSTR by)
{
    WCHAR *in_ptr = in;
    WCHAR res[MAX_PATH + 1] = { 0 };
    size_t resoffset = 0;
    WCHAR *needle;
    while ((needle = StrStrW(in, pattern)) && resoffset < in_size)
    {
        wcsncpy(res + resoffset, in, needle - in);
        resoffset += needle - in;
        in = needle + (int) wcslen(pattern);
        wcsncpy(res + resoffset, by, wcslen(by));
        resoffset += (int) wcslen(by);
    }
    wcscpy(res + resoffset, in);
    wnsprintfW(in_ptr, (int) in_size, L"%ls", res);
    return in_ptr;
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
create_dir(LPCWSTR dir)
{
    LPWSTR p = NULL;
    WCHAR tmp_name[MAX_PATH];
    wcscpy(tmp_name, dir);
    p = wcschr(tmp_name, L'\\');
    for (; p != NULL; *p = L'\\', p = wcschr(p + 1, L'\\'))
    {
        *p = L'\0';
        if (exists_dir(tmp_name))
        {
            continue;
        }
        if (!CreateDirectoryW(tmp_name, NULL))
        {
            return false;
        }
    }
    return (exists_dir(tmp_name)? true: (CreateDirectoryW(tmp_name, NULL)));
}

static WCHAR *
rand_str(WCHAR *str, const int len)
{
    int i;
    for (i = 0; i < len; ++i)
        str[i] = 'A' + rand() % 26;
    str[len] = '\0';
    return str;
}

bool WINAPI
test_path(LPCWSTR dir)
{
#define LEN_NAME 6    
    HANDLE pfile = INVALID_HANDLE_VALUE;
    WCHAR dist_path[MAX_PATH] = { 0 };
    WCHAR temp[LEN_NAME + 1] = { 0 };
    if (exists_dir(dir) || create_dir(dir))
    {
        wnsprintfW(dist_path,MAX_PATH,L"%ls\\%ls", dir, rand_str(temp, LEN_NAME));  
        pfile = CreateFileW(dist_path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
        if (pfile == INVALID_HANDLE_VALUE)
        {
        #ifdef _LOGDEBUG
            logmsg("create %ls failed\n", dist_path);
        #endif
        }
        CloseHandle(pfile);
    }
    return (pfile != INVALID_HANDLE_VALUE);
#undef LEN_NAME    
}

bool WINAPI
path_to_absolute(LPWSTR path, int len)
{
    WCHAR lpfile[MAX_PATH + 1] = { 0 };
    int n = 1;
    if (NULL == path || *path == L'\0' || *path == L' ')
    {
        return false;
    }
    if ((*path != L'"' || *path != L'%') && wcslen(path) > 1 && path[1] == L':')
    {
        return true;
    }
    if (*path == L'"')
    {
        if (wcslen(path) < 3 || path[wcslen(path) - 1] != L'"')
        {
            return false;
        }
        wnsprintfW(lpfile, MAX_PATH, L"%ls", &path[1]);
        lpfile[wcslen(lpfile) - 1] = L'\0';
    }
    else
    {
        wnsprintfW(lpfile, MAX_PATH, L"%ls", path);
    }
    wchr_replace(lpfile);
    if (lpfile[0] == L'%')
    {
        WCHAR buf_env[VALUE_LEN + 1] = { 0 };
        while (lpfile[n] != L'\0')
        {
            if (lpfile[n++] == L'%')
            {
                break;
            }
        }
        if (n < len)
        {
            wnsprintfW(buf_env, n + 1, L"%ls", lpfile);
        }
        if (wcslen(buf_env) > 1 && ExpandEnvironmentStringsW(buf_env, buf_env, VALUE_LEN) > 0)
        {
            n = wnsprintfW(path, len, L"%ls%ls", buf_env, &lpfile[n]);
        }
    }
    else if (lpfile[1] != L':')
    {
        WCHAR modname[VALUE_LEN + 1] = { 0 };
        if (GetModuleFileNameW(NULL, modname, VALUE_LEN) > 0)
        {
            if (PathRemoveFileSpecW(modname) && PathCombineW(lpfile, modname, lpfile))
            {
                n = wnsprintfW(path, len, L"%ls", lpfile);
            }
        }
    }
    else
    {
        n = wnsprintfW(path, len, L"%ls", lpfile);
    }
    return (n > 0 && n < len);
}

HWND WINAPI
get_moz_hwnd(LPWNDINFO pInfo)
{
    HWND hwnd = NULL;
    int i = 10;
    while (!pInfo->hFF && i--) /* 等待主窗口并获取句柄,增加线程退出倒计时8s */
    {
        bool m_loop = false;
        DWORD dwProcessId = 0;
        hwnd = FindWindowExW(NULL, hwnd, L"MozillaWindowClass", NULL);
        GetWindowThreadProcessId(hwnd, &dwProcessId);
        m_loop = (dwProcessId > 0 && dwProcessId == pInfo->hPid);
        if (!m_loop)
        {
            pInfo->hFF = NULL;
        }
        if (NULL != hwnd && m_loop)
        {
            pInfo->hFF = hwnd;
            break;
        }
        SleepEx(800, false);
    }
    return (hwnd != NULL ? hwnd : pInfo->hFF);
}

bool WINAPI
is_gui(LPCWSTR lpFileName)
{
    IMAGE_DOS_HEADER dos_header;
    IMAGE_NT_HEADERS pe_header;
    bool ret = false;
    HANDLE hFile = CreateFileW(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!goodHandle(hFile))
    {
        return ret;
    }
    do
    {
        DWORD readed = 0;
        DWORD m_ptr = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        if (INVALID_SET_FILE_POINTER == m_ptr)
        {
            break;
        }
        ret = ReadFile(hFile, &dos_header, sizeof(IMAGE_DOS_HEADER), &readed, NULL);
        if (ret && readed != sizeof(IMAGE_DOS_HEADER) && dos_header.e_magic != 0x5a4d)
        {
            break;
        }
        m_ptr = SetFilePointer(hFile, dos_header.e_lfanew, NULL, FILE_BEGIN);
        if (INVALID_SET_FILE_POINTER == m_ptr)
        {
            break;
        }
        ret = ReadFile(hFile, &pe_header, sizeof(IMAGE_NT_HEADERS), &readed, NULL);
        if (ret && readed != sizeof(IMAGE_NT_HEADERS))
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
    int i = 0;
    WCHAR lpFullPath[MAX_PATH + 1] = { 0 };
    if (GetModuleFileNameW(NULL, lpFullPath, MAX_PATH) > 0)
    {
        for (i = (int) wcslen(lpFullPath); i > 0; i--)
        {
            if (lpFullPath[i] == L'\\') break;
        }
        if (i > 0)
        {
            i = wnsprintfW(lpstrName, wlen, L"%ls", lpFullPath + i + 1);
        }
    }
    return (i > 0 && i < (int) wlen);
}

bool WINAPI
getw_cwd(LPWSTR lpstrName, DWORD wlen)
{
    int i = 0;
    WCHAR lpFullPath[MAX_PATH + 1] = { 0 };
    if (GetModuleFileNameW(NULL, lpFullPath, MAX_PATH) > 0)
    {
        for (i = (int) wcslen(lpFullPath); i > 0; i--)
        {
            if (lpFullPath[i] == L'\\')
            {
                lpFullPath[i] = L'\0';
                break;
            }
        }
        if (i > 0)
        {
            i = wnsprintfW(lpstrName, wlen, L"%ls", lpFullPath);
        }
    }
    return (i > 0 && i < (int) wlen);
}

LIB_INLINE
static bool
is_ff_dev(void)
{
    bool ret = false;
    WCHAR process_name[VALUE_LEN + 1];
    WCHAR product_name[NAMES_LEN + 1] = { 0 };
    if (GetCurrentProcessName(process_name, VALUE_LEN) && get_productname(process_name, product_name, NAMES_LEN, false))
    {
        ret = _wcsicmp(L"FirefoxDeveloperEdition", product_name) == 0;
    }
    return ret;
}

static bool
get_module_name(uintptr_t caller, WCHAR *out, int len)
{
    bool res = false;
    HMODULE module = NULL;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR) caller, &module))
    {
        *out = L'\0';
        if (GetModuleFileNameW(module, out, len) > 0)
        {
            res = true;
        }
    }
    if (module)
    {
        res = FreeLibrary(module);
    }
    return res;
}

bool WINAPI
is_specialapp(LPCWSTR appname)
{
    WCHAR process_name[VALUE_LEN + 1];
    GetCurrentProcessName(process_name, VALUE_LEN);
    return (_wcsicmp(process_name, appname) == 0);
}

bool WINAPI
is_browser(void *path)
{
    LPCWSTR cpath = (LPCWSTR) path;
    if (cpath)
    {
        WCHAR current[MAX_PATH] = { 0 };
        GetModuleFileNameW(NULL, current, MAX_PATH);
        return (_wcsicmp(current, cpath) == 0);
    }
    return is_specialapp(L"Iceweasel.exe") || is_specialapp(L"firefox.exe");
}

bool WINAPI
is_specialdll(uintptr_t caller, LPCWSTR files)
{
    bool res = false;
    WCHAR module[VALUE_LEN + 1] = { 0 };
    if (get_module_name(caller, module, VALUE_LEN))
    {
        if (StrChrW(files, L'*') || StrChrW(files, L'?'))
        {
            if (PathMatchSpecW(module, files))
            {
                res = true;
            }
        }
        else if (StrStrIW(module, files))
        {
            res = true;
        }
    }
    return res;
}

bool WINAPI
is_flash_plugins(uintptr_t caller)
{
    bool res = false;
    WCHAR dll_name[VALUE_LEN + 1];
    WCHAR product_name[NAMES_LEN + 1] = { 0 };
    if (get_module_name(caller, dll_name, VALUE_LEN) && get_productname(dll_name, product_name, NAMES_LEN, true))
    {
        res = _wcsicmp(L"Shockwave Flash", product_name) == 0;
    }
    return res;
}

/* 获取profiles.ini文件绝对路径,保存到in_dir数组 */
static bool
get_mozilla_profile(LPWSTR in_dir, int len, LPCWSTR appdt)
{
    int m = 0;
    if (is_specialapp(L"thunderbird.exe"))
    {
        m = wnsprintfW(in_dir, (size_t) len, L"%ls%ls", appdt, L"\\Thunderbird\\profiles.ini");
    }
    else
    {
        m = wnsprintfW(in_dir, (size_t) len, L"%ls%ls", appdt, L"\\Mozilla\\Firefox\\profiles.ini");
    }
    return (m > 0 && m < len);
}

/* 查找moz_values所在段,并把段名保存在out_names数组
 * 函数成功返回值为0,返回任何其他值意味着段没有找到
 */
static int
search_section_names(LPCWSTR moz_profile, LPCWSTR moz_values, LPWSTR out_names, int len)
{
    int ret = -1;
    LPWSTR m_section, u_section = NULL;
    if ((m_section = (LPWSTR) SYS_MALLOC(MAX_ALLSECTIONS * sizeof(WCHAR) + 1)) == NULL)
    {
        return ret;
    }
    if (GetPrivateProfileSectionNamesW(m_section, MAX_ALLSECTIONS, moz_profile) > 0)
    {
        int i = 0;
        LPCWSTR pf = L"Profile";
        size_t j = wcslen(pf);
        u_section = m_section;
        while (*u_section != L'\0' && i < MAX_SECTION)
        {
            WCHAR values[SECTION_NAMES] = { 0 };
            if (wcsncmp(u_section, pf, j) == 0)
            {
                char section[NAMES_LEN] = { 0 };
                if (!WideCharToMultiByte(CP_ACP, 0, u_section, (int)wcslen(u_section), section, NAMES_LEN, NULL, NULL))
                {
                    break;
                }
                if (!read_appkeyA(section, "Name", values, SECTION_NAMES, (void *) moz_profile))
                {
                    break;
                }
                if (wcsncmp(values, moz_values, wcslen(moz_values)) == 0 && wnsprintfW(out_names, (size_t) len, L"%ls", u_section) > 0)
                {
                    ret = 0;
                    break;
                }
                else
                {
                    ret = StrToIntW(&u_section[j]);
                    if (ret >= 0) ++ret;
                }
            }
            u_section += wcslen(u_section) + 1;
            ++i;
        }
    }
    SYS_FREE(m_section);
    return ret;
}

static bool 
get_profile_arg(LPCWSTR moz_profile, LPWSTR out_path, int len)
{
    LPWSTR  *args = NULL;
    int     m_arg = 0;
    bool    ret = false;
    args = CommandLineToArgvW(GetCommandLineW(), &m_arg);
    if ( NULL != args )
    {
        int i;
        for (i = 1; i < m_arg; ++i)
        {
            if ((_wcsicmp(args[i], L"-p") == 0  || _wcsicmp(args[i], L"-profile") == 0 ) && i < m_arg - 1)
            {
                WCHAR m_sec[NAMES_LEN+1] = { 0 };
                if (!search_section_names(moz_profile, args[i+1], m_sec, NAMES_LEN))
                {
                    char  a_sec[NAMES_LEN+1] = { 0 };
                    WideCharToMultiByte(CP_ACP, 0, m_sec, -1, a_sec, NAMES_LEN, NULL, NULL);
                    if (read_appkeyA(a_sec, "Path", out_path, len, (void *)moz_profile))
                    {
                        ret = true;
                    }                    
                }
                break;
            }
        }
        LocalFree(args);
    }
    return ret;
}

static bool
find_locked_path(LPCWSTR moz_profile, LPWSTR out_path, int len)
{
    bool ret = false;
    LPWSTR m_section, u_section = NULL;
    if ((m_section = (LPWSTR) SYS_MALLOC(MAX_ALLSECTIONS * sizeof(WCHAR) + 1)) == NULL)
    {
        return ret;
    }
    u_section = m_section;
    if (GetPrivateProfileSectionNamesW(u_section, MAX_ALLSECTIONS, moz_profile) > 0)
    {
        int i = 0;
        while (*u_section != L'\0' && i < MAX_SECTION)
        {
            if (GetPrivateProfileIntW(u_section, L"Locked", 0, moz_profile) > 0)
            {
                char  a_sec[NAMES_LEN+1] = { 0 };
                WideCharToMultiByte(CP_ACP, 0, u_section, -1, a_sec, NAMES_LEN, NULL, NULL);
                if (read_appkeyA(a_sec, "Default", out_path, len, (void *)moz_profile))
                {
                    ret = true;
                }
            #ifdef _LOGDEBUG
                logmsg("out_path =  %ls\n", out_path);
            #endif
                break;
            }
            u_section += wcslen(u_section) + 1;
            ++i;
        }
    }
    SYS_FREE(m_section);
    return ret;
}

static bool
get_profile_path(LPWSTR in_dir, int len, LPCWSTR appdt)
{
    WCHAR path[MAX_PATH] = { 0 };
    if (!get_mozilla_profile(in_dir, len, appdt))
    {
        return false;
    }
    if (!(get_profile_arg(in_dir, path, MAX_PATH) || find_locked_path(in_dir, path, MAX_PATH) || read_appkeyA("Profile0", "Path", path, MAX_PATH, in_dir)))
    {
    #ifdef _LOGDEBUG
        logmsg("get_profile_path false in %s\n", __FUNCTION__);
    #endif        
        return false;
    }
    wchr_replace(path);
    if (path[0] == L'.')
    {
        PathRemoveFileSpecW(in_dir);
        PathAppendW(in_dir, path);
        PathCombineW(in_dir, NULL, in_dir);
    }
    else
    {
        PathRemoveFileSpecW(in_dir);
        PathAppendW(in_dir, path);
    }
    if (in_dir[wcslen(in_dir) - 1] == L'\\')
    {
        in_dir[wcslen(in_dir) - 1] = L'\0';
    }
    return true;
}

static void
write_ini_file(LPCWSTR path)
{
    WritePrivateProfileSectionW(L"General", L"StartWithLastProfile=1\r\n\0", path);
    if (is_specialapp(L"thunderbird.exe"))
    {
        WritePrivateProfileSectionW(L"Profile0", L"Name=default\r\nIsRelative=1\r\nPath=../../\r\nDefault=1\r\n\0", path);
    }
    else if (is_ff_dev())
    {
        WritePrivateProfileSectionW(L"Profile0", L"Name=dev-edition-default\r\nIsRelative=1\r\nPath=../../../\r\n\0", path);
    }
    else
    {
        WritePrivateProfileSectionW(L"Profile0", L"Name=default\r\nIsRelative=1\r\nPath=../../../\r\nDefault=1\r\n\0", path);
    }
}

static void
clean_files(LPCWSTR appdt)
{
    WCHAR path[MAX_PATH] = { 0 };
    WCHAR temp[MAX_PATH] = { 0 };
    WCHAR cmp_ini[MAX_PATH] = { 0 };
    if (!(getw_cwd(temp, MAX_PATH) && get_profile_path(path, MAX_PATH, appdt)))
    {
    #ifdef _LOGDEBUG
        logmsg("profile path[%ls]\n", path);
    #endif        
        return;
    }    
    if (!(wnsprintfW(cmp_ini, MAX_PATH, L"%ls", path) > 0 && PathAppendW(cmp_ini, L"compatibility.ini")))
    {
        return;
    }
    if (!(read_appkeyA("Compatibility", "LastPlatformDir", cmp_ini, MAX_PATH, cmp_ini) && _wcsicmp(temp, cmp_ini)))
    {
    #ifdef _LOGDEBUG
        logmsg("no movement of position,do nothing.\n");
    #endif
        return;
    }
    if (true)
    {
    #ifdef _LOGDEBUG
        logmsg("we need rewrite addonStartup.json.lz4\n");
    #endif
        json_parser(path);
    }
    return;
}

unsigned WINAPI
write_file(void *p)
{
    bool ret = false;
    LPWSTR szDir = NULL;
    LPCWSTR appdt = (LPCWSTR) p;
    WCHAR moz_profile[MAX_PATH + 1] = { 0 };
    SetEnvironmentVariableW(L"LIBPORTABLE_FILEIO_DEFINED", L"1");
    if (read_appint(L"General", L"Portable") <= 0)
    {
        return (0);
    }
    if (get_mozilla_profile(moz_profile, MAX_PATH, appdt) && PathFileExistsW(moz_profile))
    {
        WCHAR app_names[MAX_PATH + 1] = { 0 };
        WCHAR m_profile[10] = { L'P', L'r', L'o', L'f', L'i', L'l', L'e', };
        int f = search_section_names(moz_profile, L"default", app_names, SECTION_NAMES);
        if (f)
        {
            write_ini_file(moz_profile);
        }
        else if (is_specialapp(L"thunderbird.exe"))
        {
            ret = WritePrivateProfileStringW(app_names, L"Path", L"../../", moz_profile);
        }
        else if (is_ff_dev())
        {
            int m = search_section_names(moz_profile, L"dev-edition-default", app_names, SECTION_NAMES);
            if (m > 0)
            {
                /* 更新dev版配置文件 */
                wnsprintfW(m_profile + wcslen(m_profile), 2, L"%d", m);
                ret = WritePrivateProfileSectionW(m_profile, L"Name=dev-edition-default\r\nIsRelative=1\r\nPath=../../../\r\n\0", moz_profile);
            }
        }
        else
        {
            ret = WritePrivateProfileStringW(app_names, L"IsRelative", L"1", moz_profile);
            ret = WritePrivateProfileStringW(app_names, L"Path", L"../../../", moz_profile);
        }
    }
    else
    {
        if (wcslen(moz_profile) > 1 && (szDir = (LPWSTR) SYS_MALLOC(sizeof(moz_profile))) != NULL)
        {
            wcsncpy(szDir, moz_profile, MAX_PATH);
        }
        if (szDir && PathRemoveFileSpecW(szDir) && create_dir(szDir))
        {
            write_ini_file(moz_profile);
        }
    }
    if (szDir)
    {
        SYS_FREE(szDir);
    }
    if (true)
    {
        clean_files(appdt);
    }
    return (1);
}

bool WINAPI
get_appdt_path(WCHAR *path, int len)
{
    WCHAR ini[MAX_PATH] = { 0 };
    if (!get_ini_path(ini, MAX_PATH))
    {
        return false;
    }
    if (!read_appkeyW(L"General", L"PortableDataPath", path, len, ini))
    {
        wnsprintfW(path, MAX_PATH, L"%ls", L"../Profiles");
    }
    if (path[1] != L':')
    {
        path_to_absolute(path, len);
    }
    return !!wcsncat(path, L"\\AppData", len);
}

bool WINAPI
get_localdt_path(WCHAR *path, int len)
{
    WCHAR ini[MAX_PATH] = { 0 };
    if (!get_ini_path(ini, MAX_PATH))
    {
        return false;
    }
    if (read_appkeyW(L"Env", L"TmpDataPath", path, len, ini))
    {
        ;
    }
    else if (!read_appkeyW(L"General", L"PortableDataPath", path, len, ini))
    {
        wnsprintfW(path, MAX_PATH, L"%ls", L"../Profiles");
    }
    if (path[1] != L':')
    {
        path_to_absolute(path, len);
    }
    return !!wcsncat(path, L"\\LocalAppData\\Temp\\Fx", len);
}

DWORD WINAPI
get_os_version(void)
{
	typedef void (WINAPI *RtlGetNtVersionNumbersPtr)(DWORD*, DWORD*, DWORD*);
    RtlGetNtVersionNumbersPtr fnRtlGetNtVersionNumbers = NULL;
    DWORD dwMajorVer, dwMinorVer, dwBuildNumber;
	DWORD ver = 0;
	HMODULE nt_dll = GetModuleHandleW(L"ntdll.dll");
    if (nt_dll)
    {
	    fnRtlGetNtVersionNumbers = (RtlGetNtVersionNumbersPtr)GetProcAddress(nt_dll, "RtlGetNtVersionNumbers");
    }
    if (fnRtlGetNtVersionNumbers)
    {
	#define VER_NUM 5
		WCHAR pszOS[VER_NUM] = { 0 };
	    fnRtlGetNtVersionNumbers(&dwMajorVer, &dwMinorVer,&dwBuildNumber);
		wnsprintfW(pszOS, VER_NUM, L"%lu%d%lu", dwMajorVer, 0, dwMinorVer);
		ver = wcstol(pszOS, NULL, 10);
	#undef VER_NUM
    }
    return ver;
}

bool WINAPI
print_process_module(DWORD pid)
{
    BOOL ret = true;
    HANDLE hmodule = NULL;
    MODULEENTRY32 me32 = { 0 };
    hmodule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hmodule == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    me32.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(hmodule, &me32))
    {
        do
        {
        #ifdef _LOGDEBUG
            logmsg("szModule = %ls, start_addr = 0x%x, end_addr = 0x%x\n", me32.szExePath, me32.modBaseAddr, me32.modBaseAddr + me32.modBaseSize);
        #endif
        } while (Module32Next(hmodule, &me32));
    }
    else
    {
        ret = false;
    }
    CloseHandle(hmodule);
    return ret;
}
