#ifndef _INTERNAL_CRT_H_
#define _INTERNAL_CRT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <shlwapi.h>
#include <windows.h>
#include <intrin.h>
#ifdef _MSC_VER
#include <stdarg.h>
#endif

#undef stdin
#undef stdout
#undef stderr
#undef MAX_BUFFER

#define MAX_BUFFER 1024
#define API_BIT_ENABLED(mask, bit)  (((DWORD)(mask) & (DWORD)(bit)) != 0)
#define API_BIT_DISABLED(mask, bit) (((DWORD)(mask) & (DWORD)(bit)) == 0)
#define API_SET_BITS(mask, bit)     ((mask) |= (DWORD)(bit))
#define API_CLR_BITS(mask, bit)     ((mask) &= ~(DWORD)(bit))
#define stdin                       (GetStdHandle(STD_INPUT_HANDLE))
#define stdout                      (GetStdHandle(STD_OUTPUT_HANDLE))
#define stderr                      (GetStdHandle(STD_ERROR_HANDLE))

#undef api_malloc
#undef api_free
#define api_malloc(x)  (HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (x)))
#define api_free(x)    (HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, (x)),(x = NULL))

static void api_mode_converter(const WCHAR *mode, DWORD *access, DWORD *create, BOOL *end);

static inline char *api_strlwr(char *s)
{
    CharLowerBuffA(s, lstrlenA(s));
    return s;
}

static inline char *api_strupr(char *s)
{
    CharUpperBuffA(s, lstrlenA(s));
    return s;
}

static inline WCHAR *api_wcslwr(WCHAR *s)
{
    CharLowerBuffW(s, lstrlenW(s));
    return s;
}

static inline WCHAR *api_wcsupr(WCHAR *s)
{
    CharUpperBuffW(s, lstrlenW(s));
    return s;
}

static inline size_t api_strlen(const char *str)
{
    return ((size_t)lstrlenA(str));
}

static inline size_t api_wcslen(const WCHAR *str)
{
    return ((size_t)lstrlenW(str));
}

static inline const WCHAR *api_wcschr(const WCHAR *str, const WCHAR ch)
{
    return StrChrW(str, (WCHAR)ch);
}

static inline const WCHAR *api_wcsrchr(const WCHAR *str, const WCHAR ch)
{
    return StrRChrW(str, NULL, ch);
}

static inline const WCHAR *api_wcsstr(const WCHAR *str, const WCHAR *substr)
{
    return StrStrW(str, substr);
}

static inline WCHAR *api_wcscat(PWSTR psz1, PCWSTR psz2)
{
    return StrCatW(psz1, psz2);
}

static inline WCHAR *api_wcsncat(PWSTR psz1, PCWSTR psz2, size_t num)
{
    return StrNCatW(psz1, psz2, (int)num);
}

static inline int api_memcmp(const void *b1, const void *b2, size_t n)
{
    const BYTE *p1 = (const BYTE *)b1;
    const BYTE *p2 = (const BYTE *)b2;
    if (!n)
    {
        return 0;
    }
    for (size_t i = 0; i < n; ++i)
    {
        if (p1[i] != p2[i])
        {
            return (int)(p1[i] - p2[i]);
        }
    }
    return 0;
}

static inline void *api_memset(void *dst, int val, size_t size)
{
    __stosb((unsigned char *)dst, (unsigned char)val, size);
    return dst;
}

static inline void *api_memcpy(void *dst, const void *src, size_t size)
{
    __movsb((unsigned char*)dst, (unsigned const char*)src, size);
    return dst;
}

static inline void *api_memmove(void *dst, const void *src, size_t count)
{
    MoveMemory(dst, src, count);
    return dst;
}

static inline size_t api_fwrite(const void *buffer, size_t size, size_t count, HANDLE hfile)
{
    DWORD bw = 0;
    if (WriteFile(hfile, buffer, (DWORD)(size * count), &bw, 0))
    {
        return bw / size;
    }
    return (0);
}

static inline int api_fprintf(HANDLE fp, const char *s, ...)
{
    int  len = 0;
    va_list args;
    va_start(args, s);
    char buf[MAX_BUFFER];
    api_memset(buf, 0, MAX_BUFFER);
    len = wvnsprintfA(buf, MAX_BUFFER - 1, s, args);
    va_end(args);
    api_fwrite(buf, 1, len + 1, fp);
    return len;
}

static inline void api_fclose(HANDLE f)
{
    if (0 < (intptr_t)f) CloseHandle(f);
}

static inline HANDLE api_wfopen(const WCHAR *path, const WCHAR *attrs)
{
    HANDLE hfile;
    DWORD  access = 0, disp = 0;
    BOOL   end = FALSE;
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

static inline size_t api_fread(void *buffer, size_t size, size_t count, HANDLE hfile)
{
    DWORD bw = 0;
    if (ReadFile(hfile, buffer, (DWORD)(size * count), &bw, 0))
    {
        return bw / size;
    }
    return (0);
}

static inline int64_t api_fseek(HANDLE f, int64_t offset, DWORD origin)
{
    LARGE_INTEGER li;
    li.QuadPart = offset;
    li.LowPart = SetFilePointer(f, li.LowPart, &li.HighPart, origin);
    if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
    {
        li.QuadPart = -1;
    }
    return li.QuadPart;
}

static inline char *api_strncpy(char *dest, const char *src, size_t n)
{
    size_t len = api_strlen(src);
    api_memcpy(dest, src, len);
    if (n > len)
    {
        api_memset(&dest[len], 0, n - len);
    }
    return dest;
}

static inline int api_snprintf(LPSTR str, size_t cch, LPCSTR fmt, ...)
{
    int     count;
    va_list ap;
    va_start(ap, fmt);
    count = wvnsprintfA(str, (int)cch, fmt, ap);
    va_end(ap);
    return count;
}

static inline int api_snwprintf(LPWSTR str, size_t cch, LPCWSTR fmt, ...)
{
    int     count;
    va_list ap;
    va_start(ap, fmt);
    count = wvnsprintfW(str, (int)cch, fmt, ap);
    va_end(ap);
    return count;
}

static inline WCHAR *api_wcsncpy(WCHAR *dest, const WCHAR *src, size_t n)
{
    size_t len = api_wcslen(src);
    api_memcpy(dest, src, len * sizeof(WCHAR));
    if (n > len)
    {
        api_memset(&dest[len], 0, (n - len) * sizeof(WCHAR));
    }
    return dest;
}

static inline int api_wcscmp(const WCHAR *s1, const WCHAR *s2)
{
    return lstrcmpW(s1, s2);
}

static inline int api_wcsicmp(const WCHAR *s1, const WCHAR *s2)
{
    return lstrcmpiW(s1, s2);
}

static inline int api_wcsncmp(const WCHAR *s1, const WCHAR *s2, size_t n)
{
    return StrCmpNW(s1, s2, (int)n);
}

static inline int api_wcsnicmp(const WCHAR *s1, const WCHAR *s2, size_t n)
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


#ifdef __cplusplus
}
#endif

#endif /* end _INTERNAL_CRT_H_ */
