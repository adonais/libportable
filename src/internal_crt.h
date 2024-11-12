#ifndef _INTERNAL_CRT_H_
#define _INTERNAL_CRT_H_

#ifdef __cplusplus
extern "C" {
#endif

SIZE_T api_strlen(const char *str);
SIZE_T api_wcslen(const WCHAR *str);
const WCHAR *api_wcschr(const WCHAR *str, const WCHAR ch);
const WCHAR *api_wcsrchr(const WCHAR *str, const WCHAR ch);
const WCHAR *api_wcsstr(const WCHAR *str, const WCHAR *substr);
WCHAR  *api_wcscat(PWSTR psz1, PCWSTR psz2);
WCHAR  *api_wcsncat(PWSTR psz1, PCWSTR psz2, SIZE_T num);
void   api_fclose(HANDLE f);
HANDLE api_wfopen(const WCHAR *path, const WCHAR *attrs);
SIZE_T api_fwrite(const void *buffer, SIZE_T size, SIZE_T count, HANDLE hfile);
SIZE_T api_fread(void *buffer, SIZE_T size, SIZE_T count, HANDLE hfile);
int    api_fprintf(HANDLE fp, const char *s, ...);
int    api_snprintf(LPSTR str, SIZE_T cch, LPCSTR fmt, ...);
int    api_snwprintf(LPWSTR str, SIZE_T cch, LPCWSTR fmt, ...);
char  *api_strncpy(char *dest, const char *src, SIZE_T n);
WCHAR *api_wcsncpy(WCHAR *dest, const WCHAR *src, SIZE_T n);
int    api_wcscmp(const WCHAR *s1, const WCHAR *s2);
int    api_wcsicmp(const WCHAR *s1, const WCHAR *s2);
int    api_wcsncmp(const WCHAR *s1, const WCHAR *s2, SIZE_T n);
int    api_wcsnicmp(const WCHAR *s1, const WCHAR *s2, SIZE_T n);
int    api_memcmp(const void *b1, const void *b2, SIZE_T n);
void  *api_memset(void *dst, int val, SIZE_T size);
void  *api_memcpy(void *dst, const void *src, SIZE_T size);
void  *api_memmove(void *dst, const void *src, SIZE_T count);
char  *api_strlwr(char *s);
char  *api_strupr(char *s);
WCHAR *api_wcslwr(WCHAR *s);
WCHAR *api_wcsupr(WCHAR *s);

#ifdef __cplusplus
}
#endif

#endif /* end _INTERNAL_CRT_H_ */
