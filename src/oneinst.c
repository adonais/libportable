#ifndef _CRT_MEMORY_DEFINED
#define _CRT_MEMORY_DEFINED
#endif

#include "inipara.h"
#include "oneinst.h"
#include "bosskey.h"
#include "internal_crt.h"
#include "MinHook.h"

#define SECTOR_SIZE 2048
#define TAG_7Z    "\x37\x7A\xBC\xAF\x27\x1C"
#define DVD_17G   0x440000000
#define PL_STR    (L"--playlist=\"%s\"")
#define BD_STR    (L"bd:// --bluray-device=\"%s\"")
#define IPC_STR   (L" --input-ipc-server=libumpv")
#define IPC_NAME  (L"\\\\.\\pipe\\libumpv")
#define IPC_CMD   (L"{\"command\": [\"loadfile\", \"%s\"]}\n")
#define IPC_QUIT  ("{\"command\": [\"quit\"]}\n")
#define IPC_PAUSE ("{\"command\": [\"set_property\", \"pause\", %s]}\n")

static CommandLineToArgvWptr pCommandLineToArgvW,sCommandLineToArgvWstub;
static volatile long locks = 0;
extern volatile INT_PTR mpv_window_hwnd;
extern volatile DWORD lib_pid;
extern WCHAR ini_path[MAX_PATH+1];

// 字符数组赋初值时gcc自动调用memset优化,
// 但我们不链接crt, 导致链接器找不到memeset, 这里实现memeset骗过链接器
// extern void *memset(void *dst, int val, SIZE_T size){return NULL;}

int WINAPI
mp_argument_cmp(LPCWSTR s1, LPCWSTR s2)
{
    WCHAR *p = (WCHAR *)s1;
    size_t n = s1 != NULL && s2 != NULL ? api_wcslen(s2) : 0;
    if (!n || api_wcslen(s1) < 2 || s1[0] != L'-')
    {
        return -1;
    }
    if (s1[1] == L'-')
    {
        p += 2;
    }
    else
    {
        ++p;
    }
    return api_wcsnicmp(p, s2, n);
}

static bool
mp_exist_file(LPCWSTR path)
{
    DWORD fileattr = INVALID_FILE_ATTRIBUTES;
    if (path && (fileattr = GetFileAttributesW(path)) != INVALID_FILE_ATTRIBUTES)
    {
        return (fileattr & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }
    return false;
}

static inline bool
mp_exist_url(LPCWSTR purl)
{
    return (api_wcsncmp(purl, L"https://", 8) == 0||
            api_wcsncmp(purl, L"http://", 7) == 0 ||
            api_wcsncmp(purl, L"ytdl://", 7) == 0 ||
            api_wcsncmp(purl, L"file://", 7) == 0 ||
            api_wcsncmp(purl, L"smb://", 6) == 0);
}

static inline bool
mp_exist_protocols(LPCWSTR purl)
{
    return (api_wcsstr(purl, L"://"));
}

static inline bool
mp_file_rlt(LPCWSTR purl)
{
    return (api_wcslen(purl) > 7 && api_wcsncmp(purl, L"file://", 7) == 0 && purl[7] != L'/');
}

static inline bool
mp_exist_listplay(LPCWSTR purl)
{
    return (purl && mp_argument_cmp(purl, L"playlist") == 0);
}

static bool
mp_exist_listfile(LPCWSTR purl)
{
    const WCHAR *sep = NULL;
    const WCHAR *exts[] = {L".ini", L".pls", L".txt", NULL};
    if ((sep = api_wcsrchr(purl, L'.')) != NULL)
    {
        for (int n = 0; exts[n]; n++)
        {
            if (api_wcsicmp(sep, exts[n]) == 0)
            {
                return true;
            }
        }
    }
    return false;
}

static char *
mp_memstr(char *full_data, int full_data_len, const char *substr)
{
    if (full_data != NULL && full_data_len > 0 && substr != NULL && *substr != 0)
    {
        int sublen = (int)api_strlen(substr);
        char *cur = full_data;
        int last_possible = full_data_len - sublen + 1;
        for (int i = 0; i < last_possible; i++)
        {
            if (*cur == *substr)
            {
                if (api_memcmp(cur, substr, sublen) == 0)
                {
                    // found
                    return cur;
                }
            }
            cur++;
        }
    }
    return NULL;
}

static UINT64
get_file_size(const HANDLE hfile)
{  
    UINT64 size = 0;
    GetFileSizeEx(hfile, (LARGE_INTEGER *) &size);
    return size;
}

static bool
detect_media_dvd(const HANDLE fp, bool *udf)
{
    // DVD主描述符, 16扇区
    const int dvd_primary_sector = 16;
    unsigned char dvd_header[6];
    api_fseek(fp, dvd_primary_sector * SECTOR_SIZE, 0);
    if (api_fread(dvd_header, 1, sizeof(dvd_header), fp) == sizeof(dvd_header))
    {
        // 是否udf格式
        if (dvd_header[0] == 0x0 && api_memcmp(dvd_header + 1, "BEA01", 5) == 0)
        {
            *udf = true;
        }
        // DVD标识: 首字节为0x01且后续5字节为"CD001"
        else if (dvd_header[0] == 0x01 && api_memcmp(dvd_header + 1, "CD001", 5) == 0)
        {
            return true;
        }
    }
    return false;
}

static bool
detect_media_utf2(const HANDLE fp)
{
    // (位置: 17扇区)
    const int sector = 17;
    unsigned char vudf[6];
    api_fseek(fp, sector * SECTOR_SIZE, 0);
    if (api_fread(vudf, 1, sizeof(vudf), fp) == sizeof(vudf))
    {
        // NSR03, udf 2.x 版本
        if (vudf[0] == 0x0 && api_memcmp(vudf + 1, "NSR03", 5) == 0)
        {
            return true;
        }
    }
    return false;
}

static bool
mp_check_bdmv(const wchar_t *path)
{
    HANDLE f = NULL;
    bool iso_bd = false;
    while (path && path[0] && (f = api_wfopen(path, L"rb")) != NULL)
    {
        bool udf = false;
        UINT64 s = get_file_size(f);
        if (s <= 256 * SECTOR_SIZE + 16)
        {   // 过小, 不是蓝光
            break;
        }
        if (detect_media_dvd(f, &udf))
        {
            break;
        }
        if (s > (UINT64)DVD_17G)
        {
            iso_bd = true;
        }
        else if (udf && detect_media_utf2(f))
        {
            iso_bd = true;
        }
        break;
    }
    api_fclose(f);
    return iso_bd;
}

static bool
detect_type_7z(const wchar_t *path)
{
    HANDLE f = NULL;
    bool ret = false;
    if (path && path[0] && (f = api_wfopen(path, L"rb")) != NULL)
    {
        unsigned char header[6];
        if (api_fread(header, 1, sizeof(header), f) == sizeof(header))
        {
            if (api_memcmp(header, TAG_7Z, 6) == 0)
            {
                ret = true;
            }
        }
        api_fclose(f);
    }
    return ret;
}

static bool
mp_fake_7z(const wchar_t *path)
{
    return (!detect_type_7z(path) && mp_check_bdmv(path));
}

// 转换直接在in内进行, 所以in_size要能容纳替换后的内容, in_size为数组长度
static LPWSTR
mp_wstr_replace(WCHAR *in, const size_t in_size, LPCWSTR pattern, LPCWSTR by)
{
    const WCHAR *needle = NULL;
    WCHAR *res = (WCHAR *)api_malloc(in_size * sizeof(WCHAR));
    if (res != NULL)
    {
        WCHAR *in_ptr = in;
        size_t offset = 0;
        while ((needle = api_wcsstr(in_ptr, pattern)) != NULL && offset < in_size)
        {
            api_wcsncpy(res + offset, in_ptr, needle - in_ptr);
            offset += needle - in_ptr;
            in_ptr = (WCHAR *)(needle + api_wcslen(pattern));
            api_wcsncpy(res + offset, by, in_size - offset);
            offset +=  api_wcslen(by);
        }
        api_wcsncpy(res + offset, in_ptr, in_size - offset);
        api_snwprintf(in, in_size, L"%s", res);
        api_free(res);
    }
    return in;
}

static bool
mp_open_ipc(HANDLE *ptr_pipe)
{
    //等待连接命名管道
    if (!WaitNamedPipeW(IPC_NAME, NMPWAIT_WAIT_FOREVER))
    {
    #ifdef _LOGDEBUG
        logmsg("[hook_command] pipe does not exist\n");
    #endif
        return false;
    }
    //打开命名管道
    if((*ptr_pipe = CreateFileW(IPC_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
    {
    #ifdef _LOGDEBUG
        logmsg("[hook_command] Failed to open pipe\n");
    #endif
        return false;
    }
    return true;
}

static void
mp_write_ipc(const HANDLE hpipe, LPWSTR pfile)
{
    DWORD dw;
    char data[MAX_BUFFER];
    WCHAR pbuf[MAX_BUFFER];
    // file://向对路径
    const bool relative = mp_file_rlt(pfile);
    if (relative || (api_wcslen(pfile) > 1 && pfile[1] != L':' && !mp_exist_protocols(pfile)))
    {   // 命令行打开相对目录时没有标准路径
        GetCurrentDirectoryW(MAX_BUFFER - 1, pbuf);
        api_wcsncat(pbuf, L"\\", MAX_BUFFER - 1);
        api_wcsncat(pbuf, relative ? &pfile[7] : pfile, MAX_BUFFER - 1);
        api_snwprintf(pfile, MAX_BUFFER - 1, L"%s", pbuf);
    }
    mp_wstr_replace(pfile, MAX_BUFFER - 1, L"\\", L"\\\\");
    api_snwprintf(pbuf, MAX_BUFFER - 1, IPC_CMD, pfile);
    if (mp_make_u8(pbuf, data, MAX_BUFFER))
    {
    #ifdef _LOGDEBUG
        logmsg("[hook_command] write pipe<%s>\n", data);
    #endif
        if(!WriteFile(hpipe, data, (DWORD)api_strlen(data) + 1, &dw, NULL))
        {
        #ifdef _LOGDEBUG
            logmsg("[hook_command] Failed to write pipe\n");
        #endif
        }
    }
}

static bool
mp_write_quit(void)
{
    bool ret = false;
    HANDLE hpipe = INVALID_HANDLE_VALUE;
    if (mp_open_ipc(&hpipe))
    {
        DWORD dw;
        if(WriteFile(hpipe, IPC_QUIT, (DWORD)api_strlen(IPC_QUIT) + 1, &dw, NULL))
        {
            ret = true;
        }
    #ifdef _LOGDEBUG
        else
        {
            logmsg("[hook_command] Failed to write quit\n");
        }
    #endif
    }
    return ret;
}

static void
mp_read_ipc(const HANDLE hpipe)
{
    DWORD dw;
    char pbuf[MAX_BUFFER];
    if(ReadFile(hpipe, pbuf, MAX_BUFFER - 1, &dw, NULL))
    {
        pbuf[dw] = 0;
    }
}

//#ifdef _MSC_VER
//#pragma optimize("", off)
//#endif

static void
mp_path_replace(const WCHAR *format, const WCHAR *path, WCHAR *parg)
{
    const WCHAR *p = NULL;
    WCHAR temp[MAX_BUFFER];
    WCHAR protocol[MAX_BUFFER];
    if (parg && (p = api_wcsstr(parg, path)) != NULL)
    {
        api_memset(temp, 0, sizeof(temp));
        api_memset(protocol, 0, sizeof(protocol));
        if (parg < p - 1 && *(p - 1) == L'"')
        {
            api_snwprintf(temp, MAX_BUFFER - 1, L"\"%s", path);
            if (*(p + api_wcslen(path)) == L'"')
            {
                api_wcsncat(temp, L"\"", MAX_BUFFER - 1);
            }
        }
        else
        {
            api_snwprintf(temp, MAX_BUFFER - 1, L"%s", path);
        }
        api_snwprintf(protocol, MAX_BUFFER - 1, format, path);
        mp_wstr_replace(parg, MAX_BUFFER - 1, temp, protocol);
    }
}

//#ifdef _MSC_VER
//#pragma optimize("", on)
//#endif

static LPWSTR
mp_update_command(const WCHAR **szlist, const WCHAR *pline, const int num, const WCHAR *playlist, const WCHAR *iso_path, const bool use_ipc, const bool use_bd)
{
    DWORD size = 0;
    WCHAR *parg = NULL;
    if (*pline == 0)
    {
        size = (MAX_PATH + 1) + (use_ipc ? (DWORD)api_wcslen(IPC_STR) : 0) + (use_bd ? 32 : 0) + (playlist ? 16 : 0);
    }
    else
    {
        size = (DWORD)api_wcslen(pline) + 1 +  (use_ipc ? (DWORD)api_wcslen(IPC_STR) : 0) + (use_bd ? 32 : 0) + (playlist ? 16 : 0);
    }
    if ((parg = (WCHAR *)api_malloc(sizeof(WCHAR) * (size + 1))) != NULL)
    {
        if (*pline == 0)
        {
            GetModuleFileNameW(0, parg, size);
            if (use_ipc)
            {
                api_wcsncat(parg, IPC_STR, size);
            }
        }
        else if (num > 1 && api_wcscmp(szlist[1], L"--") == 0)
        {   // 某些版本的mpv命令行上只有前引号
            api_snwprintf(parg, size, L"%s", pline);
            if (use_ipc)
            {
                mp_wstr_replace(parg, size, L"--", IPC_STR);
            }
            else if (playlist || use_bd)
            {
                mp_wstr_replace(parg, size, L"--", L"");
            }
        }
        else
        {
            api_snwprintf(parg, size, L"%s%s", pline, use_ipc ? IPC_STR : L"");
        }
        if (playlist)
        {
            mp_path_replace(PL_STR, playlist, parg);
        }
        else if (use_bd && iso_path)
        {
            mp_path_replace(BD_STR, iso_path, parg);
        }
    }
    return parg;
}

static void
mp_window_setting(void)
{
    HWND htop = NULL;
    DWORD top_id = 0;
    if (0 != mpv_window_hwnd)
    {
        if (!IsWindowVisible((HWND)mpv_window_hwnd) || IsIconic((HWND)mpv_window_hwnd))
        {
        #ifdef _LOGDEBUG
            logmsg("[hook_command] main window[%08x] minimize or invisible\n", mpv_window_hwnd);
        #endif
            ShowWindow((HWND)mpv_window_hwnd, SW_SHOWNOACTIVATE);
            ShowWindow((HWND)mpv_window_hwnd, SW_SHOW);
        }
        //SwitchToThisWindow((HWND)mpv_window_hwnd, FALSE);
        htop = GetForegroundWindow();
        top_id = GetWindowThreadProcessId(htop, NULL);
        DWORD mvp_id = GetWindowThreadProcessId((HWND)mpv_window_hwnd, NULL);
        AttachThreadInput(top_id, mvp_id, TRUE);
        SetForegroundWindow((HWND)mpv_window_hwnd);
        AttachThreadInput(top_id, mvp_id, FALSE);
        SetWindowPos((HWND)mpv_window_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        SetWindowPos((HWND)mpv_window_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
}

static LPWSTR *
mp_CommandLineToArgvW(LPCWSTR pline, int *numargs)
{
    LPWSTR *szlist = NULL;
    const bool single = read_appint(L"libumpv", L"#OneInstance") > 0;
    const bool redirect = read_appint(L"libumpv", L"#ArgRedirect") > 0;
    if ((szlist = sCommandLineToArgvWstub(pline, numargs)) != NULL && (single || redirect) && *numargs > 0 && *numargs < 4)
    {
        bool mp_pts = false;
        bool mp_url = false;
        bool mp_list = false;
        bool mp_file = false;
        bool use_ipc = (bool)single;
        const WCHAR *iso_path = NULL;
        const WCHAR *list_path = NULL;
        for (int i = 1; i < *numargs; ++i)
        {
            if (mp_argument_cmp(szlist[i], L"input-ipc-server") == 0)
            {
                use_ipc = false;
            }
            if (mp_argument_cmp(szlist[i], L"playlist") == 0)
            {
                mp_url = false;
                mp_pts = false;
                mp_list = true;
                iso_path = NULL;
                list_path = szlist[i];
            }
            if (!mp_list)
            {
                if (!mp_pts && mp_exist_url(szlist[i]))
                {
                    mp_url = true;
                    if (mp_exist_listfile(szlist[i]))
                    {
                        list_path = szlist[i];
                    }
                }
                else if (mp_exist_protocols(szlist[i]))
                {
                    mp_url = false;
                    mp_pts = true;
                }
                if (!(mp_url || mp_pts) && szlist[i][0] != L'-' && mp_exist_file(szlist[i]))
                {
                    const WCHAR *p = NULL;
                    mp_file = true;
                    if (mp_exist_listfile(szlist[i]))
                    {
                        list_path = szlist[i];
                    }
                    else if ((p = api_wcsrchr(szlist[i], L'.')) != NULL && (api_wcsicmp(p, L".iso") == 0 || 
                            (api_wcsicmp(p, L".7z") == 0 && mp_fake_7z(szlist[i]))))
                    {
                        iso_path = (const WCHAR *)szlist[i];
                    }
                }
            }
        }
        do
        {
            bool use_bd = false;
            WCHAR *parg = NULL;
            DWORD cur_pid = GetCurrentProcessId();
            if (!(*numargs == 1 || mp_list || mp_url || mp_pts || mp_file))
            {
                break;
            }
            if (iso_path)
            {
                use_bd = mp_check_bdmv(iso_path);
            }
            if (cur_pid > 0x4 && cur_pid != lib_pid && mp_url && list_path)
            {   /* 从pipe不能加载远程list文件, 但可以加载其他媒体文件 */
                mp_list = true;
            }
            if (lib_pid == cur_pid)
            {
                if ((parg = mp_update_command((const WCHAR **)szlist, pline, *numargs, mp_exist_listplay(list_path) ? NULL: list_path, iso_path, use_ipc, use_bd)) != NULL)
                {
                    LocalFree(szlist);
                    szlist = sCommandLineToArgvWstub(parg, numargs);
                }
            }
            else if (!mp_list && !mp_pts && !use_bd && use_ipc)
            {
                WCHAR pfile[MAX_BUFFER];
                HANDLE hpipe = INVALID_HANDLE_VALUE;
                api_memset(pfile, 0, sizeof(pfile));
                if (mp_list)
                {
                    api_snwprintf(pfile, MAX_BUFFER - 1, L"%s", list_path);
                }
                else if (*numargs > 1 && api_wcscmp(szlist[1], L"--") != 0 && (mp_exist_file(szlist[1]) || mp_exist_url(szlist[1])))
                {
                    api_snwprintf(pfile, MAX_BUFFER - 1, L"%s", szlist[1]);
                }
                else if (*numargs > 2 && (mp_exist_file(szlist[2]) || mp_exist_url(szlist[2])))
                {
                    api_snwprintf(pfile, MAX_BUFFER - 1, L"%s", szlist[2]);
                }
                if (*pfile && mp_open_ipc(&hpipe))
                {
                    mp_write_ipc(hpipe, pfile);
                #ifdef _LOGDEBUG
                    mp_read_ipc(hpipe);
                #endif
                }
                if (hpipe != INVALID_HANDLE_VALUE)
                {
                    CloseHandle(hpipe);
                }
                mp_window_setting();
                szlist[0] = NULL, *numargs = 0;
            }
            else if ((mp_list || use_ipc || use_bd) && mpv_window_hwnd)
            {   /* 重写共享区的进程pid */
                if (use_ipc && (lib_pid = cur_pid) > 0x4u)
                {
                #ifdef _LOGDEBUG
                    logmsg("[hook_command] Close prewindow, first use ipc quit, then send wm_close\n");
                #endif
                    if (!mp_write_quit())
                    {
                        SendMessageW((HWND)mpv_window_hwnd, WM_CLOSE, 0, 0);
                    }
                    Sleep(1000);     // 等待ipc server关闭
                }
                if ((mp_list || use_bd) && (parg = mp_update_command((const WCHAR **)szlist, pline, *numargs, mp_exist_listplay(list_path) ? NULL: list_path, iso_path, use_ipc, use_bd)) != NULL)
                {
                    LocalFree(szlist);
                    szlist = sCommandLineToArgvWstub(parg, numargs);
                }
                if (use_ipc && szlist)
                {
                    init_bosskey();
                }
            }
            #ifdef _LOGDEBUG
            {
                char data[MAX_BUFFER];
                logmsg("[hook_command] pline = [%s]\n", mp_make_u8(pline , data, MAX_BUFFER));
                if (parg)
                {
                    logmsg("[hook_command] parg = [%s]\n", mp_make_u8(parg , data, MAX_BUFFER));
                }
                for (int i = 0; i < *numargs; ++i)
                {
                    logmsg("[hook_command] szlist[%d] = [%s]\n", i, mp_make_u8(szlist[i], data, MAX_BUFFER));
                }
            }
            #endif
            if (parg)
            {
                api_free(parg);
            }
        } while(0);
    }
    return szlist;
}

void
mp_command_pause(const bool enable)
{
    HANDLE hpipe = INVALID_HANDLE_VALUE;
    if (mp_open_ipc(&hpipe))
    {
        DWORD dw;
        char data[VALUE_LEN];
        api_memset(data, 0, sizeof(data));
        api_snprintf(data, VALUE_LEN - 1, IPC_PAUSE, enable ? "true" : "false");
        if(!WriteFile(hpipe, data, (DWORD)api_strlen(data) + 1, &dw, NULL))
        {
        #ifdef _LOGDEBUG
            logmsg("[hook_command] Failed to write pause\n");
        #endif
        }
    }
}

BOOL WINAPI
init_crthook(void)
{
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    do
    {
        if (!shell32)
        {
            break;
        }
        if (NULL == (pCommandLineToArgvW = (CommandLineToArgvWptr)GetProcAddress(shell32, "CommandLineToArgvW")))
        {
            break;
        }
        if (MH_CreateHook((LPVOID)pCommandLineToArgvW, (LPVOID)mp_CommandLineToArgvW, (LPVOID *)&sCommandLineToArgvWstub) != MH_OK)
        {
            break;
        }
        if (MH_EnableHook((LPVOID)pCommandLineToArgvW) != MH_OK)
        {
        #ifdef _LOGDEBUG
            logmsg("[hook_command] MH_EnableHook() fail!\n");
        #endif
            break;
        }
        return TRUE;
    } while (0);
    return FALSE;
}

void WINAPI
uninit_crthook(void)
{
    if (sCommandLineToArgvWstub)
    {
        MH_DisableHook((LPVOID)pCommandLineToArgvW);
    }
}
