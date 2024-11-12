#include <shlwapi.h>
#include <windows.h>
#ifdef _MSC_VER
#include <stdarg.h>
#endif

#undef stdin
#undef stdout
#undef stderr
#ifndef MAX_BUFFER
#define MAX_BUFFER 1024
#endif
#define API_BIT_ENABLED(mask, bit)  (((DWORD)(mask) & (DWORD)(bit)) != 0)
#define API_BIT_DISABLED(mask, bit) (((DWORD)(mask) & (DWORD)(bit)) == 0)
#define API_SET_BITS(mask, bit)     ((mask) |= (DWORD)(bit))
#define API_CLR_BITS(mask, bit)     ((mask) &= ~(DWORD)(bit))
#define stdin                       (GetStdHandle(STD_INPUT_HANDLE))
#define stdout                      (GetStdHandle(STD_OUTPUT_HANDLE))
#define stderr                      (GetStdHandle(STD_ERROR_HANDLE))

static void api_mode_converter(const WCHAR *mode, DWORD *access, DWORD *create, BOOL *end);

char *api_strlwr(char *s)
{
    CharLowerBuffA(s, lstrlenA(s));
    return s;
}

char *api_strupr(char *s)
{
    CharUpperBuffA(s, lstrlenA(s));
    return s;
}

WCHAR *api_wcslwr(WCHAR *s)
{
    CharLowerBuffW(s, lstrlenW(s));
    return s;
}

WCHAR *api_wcsupr(WCHAR *s)
{
    CharUpperBuffW(s, lstrlenW(s));
    return s;
}

SIZE_T api_strlen(const char *str)
{
    return ((SIZE_T)lstrlenA(str));
}

SIZE_T api_wcslen(const WCHAR *str)
{
    return ((SIZE_T)lstrlenW(str));
}

const WCHAR *api_wcschr(const WCHAR *str, const WCHAR ch)
{
    return StrChrW(str, (WCHAR)ch);
}

const WCHAR *api_wcsrchr(const WCHAR *str, const WCHAR ch)
{
    return StrRChrW(str, NULL, ch);
}

const WCHAR *api_wcsstr(const WCHAR *str, const WCHAR *substr)
{
    return StrStrW(str, substr);
}

WCHAR *api_wcscat(PWSTR psz1, PCWSTR psz2)
{
    return StrCatW(psz1, psz2);
}

WCHAR *api_wcsncat(PWSTR psz1, PCWSTR psz2, SIZE_T num)
{
    return StrNCatW(psz1, psz2, (int)num);
}

SIZE_T api_fwrite(const void *buffer, SIZE_T size, SIZE_T count, HANDLE hfile)
{
    DWORD bw = 0;
    if (WriteFile(hfile, buffer, (DWORD)(size * count), &bw, 0))
    {
        return bw / size;
    }
    return (0);
}

int api_fprintf(HANDLE fp, const char *s, ...)
{
    va_list args;
    va_start(args, s);
    char buf[MAX_BUFFER];
    int  len = wvnsprintfA(buf, MAX_BUFFER, s, args);
    buf[len] = 0;
    va_end(args);
    api_fwrite(buf, 1, len + 1, fp);
    return len;
}

void api_fclose(HANDLE f)
{
    CloseHandle(f);
}

HANDLE api_wfopen(const WCHAR *path, const WCHAR *attrs)
{
    DWORD  access = 0, disp = 0;
    BOOL   end = FALSE;
    HANDLE hfile = INVALID_HANDLE_VALUE;
    api_mode_converter(attrs, &access, &disp, &end);
    if ((hfile = CreateFileW(path, access, FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL, disp, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
    {
        if (end)
        {
            SetFilePointer(hfile, 0, NULL, FILE_END);
        }
    }
    return hfile;
}

SIZE_T api_fread(void *buffer, SIZE_T size, SIZE_T count, HANDLE hfile)
{
    DWORD bw = 0;
    if (ReadFile(hfile, buffer, (DWORD)(size * count), &bw, 0))
    {
        return bw / size;
    }
    return (0);
}

int api_memcmp(const void *b1, const void *b2, SIZE_T n)
{
    const BYTE *p1 = (const BYTE *)b1;
    const BYTE *p2 = (const BYTE *)b2;
    if (!n)
    {
        return 0;
    }
    for (SIZE_T i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
        {
            return p1[i] - p2[i];
        }
    }
    return 0;
}

void *api_memset(void *dst, int val, SIZE_T size)
{
    __stosb(dst, val, size);
    return dst;
}

void *api_memcpy(void *dst, const void *src, SIZE_T size)
{
    char       *_dst = (char *)dst;
    const char *_src = (char *)src;
    for (SIZE_T i = 0; i < size; i++)
    {
        _dst[i] = _src[i];
    }
    return dst;
}

void *api_memmove(void *dst, const void *src, SIZE_T count)
{
    void *ret = dst;
    if (dst <= src || (char *)dst >= ((char *)src + count))
    {
        // Non-Overlapping Buffers
        // copy from lower addresses to higher addresses
        while (count--)
        {
            *(char *)dst = *(char *)src;
            dst = (char *)dst + 1;
            src = (char *)src + 1;
        }
    }
    else
    {
        // Overlapping Buffers
        // copy from higher addresses to lower addresses
        dst = (char *)dst + count - 1;
        src = (char *)src + count - 1;

        while (count--)
        {
            *(char *)dst = *(char *)src;
            dst = (char *)dst - 1;
            src = (char *)src - 1;
        }
    }
    return ret;
}

char *api_strncpy(char *dest, const char *src, SIZE_T n)
{
    SIZE_T len = api_strlen(src);
    api_memcpy(dest, src, n);
    if (n > len)
    {
        api_memset(&dest[len], 0, n - len);
    }
    return dest;
}

int api_snprintf(LPSTR str, SIZE_T cch, LPCSTR fmt, ...)
{
    int     count;
    va_list ap;
    va_start(ap, fmt);
    count = wvnsprintfA(str, (int)cch, fmt, ap);
    va_end(ap);
    return count;
}

int api_snwprintf(LPWSTR str, SIZE_T cch, LPCWSTR fmt, ...)
{
    int     count;
    va_list ap;
    va_start(ap, fmt);
    count = wvnsprintfW(str, (int)cch, fmt, ap);
    va_end(ap);
    return count;
}

WCHAR *api_wcsncpy(WCHAR *dest, const WCHAR *src, SIZE_T n)
{
    SIZE_T len = api_wcslen(src);
    api_memcpy(dest, src, n * sizeof(WCHAR));
    if (n > len)
    {
        api_memset(&dest[len], 0, (n - len) * sizeof(WCHAR));
    }
    return dest;
}

int api_wcscmp(const WCHAR *s1, const WCHAR *s2)
{
    return lstrcmpW(s1, s2);
}

int api_wcsicmp(const WCHAR *s1, const WCHAR *s2)
{
    return lstrcmpiW(s1, s2);
}

int api_wcsncmp(const WCHAR *s1, const WCHAR *s2, SIZE_T n)
{
    return StrCmpNW(s1, s2, (int)n);
}

int api_wcsnicmp(const WCHAR *s1, const WCHAR *s2, SIZE_T n)
{
    return StrCmpNIW(s1, s2, (int)n);
}

static void api_mode_converter(const WCHAR *mode, DWORD *access, DWORD *create, BOOL *end)
{
    for (const WCHAR *x = mode; *x != 0; ++x)
    {
        switch (*x)
        {
        case TEXT('r'): {
            if (API_BIT_DISABLED(*access, GENERIC_READ))
            {
                API_SET_BITS(*access, GENERIC_READ);
            }
            if (*create == 0)
            {
                *create = OPEN_EXISTING;
            }
            break;
        }
        case TEXT('w'): {
            *access = GENERIC_READ | GENERIC_WRITE;
            *create = CREATE_ALWAYS;
            break;
        }
        case TEXT('a'): {
            *access = GENERIC_READ | GENERIC_WRITE;
            *create = OPEN_ALWAYS;
            break;
        }
        case TEXT('+'): {
            *access = GENERIC_READ | GENERIC_WRITE;
            if (api_wcschr(mode, L'a'))
            {
                *end = TRUE;
            }
            break;
        }
        default:
            break;
        }
    }
}
