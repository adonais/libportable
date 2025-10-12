#ifndef _INI_PARSER_H
#define _INI_PARSER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define LEN_SECTION 64
#define LEN_STRINGS 128
#define MAX_BUFFER_SIZE (16 * 1024)
#define ini_safe_free(p) ((p) ? ((free((void *)(p))), ((p) = NULL)) : (void *)(p))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    CHR_UNKOWN = 0,
    CHR_WIN,
    CHR_UNIX
} str_line;

typedef enum
{
    E_OTHER = 0,
    E_ANSI,
    E_UNICODE,
    E_UNICODE_BIG,
    E_UTF8,
    E_UTF8_BOM,
    E_ZERO,
} str_encoding;

typedef struct _node
{
    struct _node *next;
    char content[1];
} node, *position;

typedef struct _list
{
    str_encoding codes;
    str_line breaks;
    bool write;
    FILE *pf;
    node *pd;
} ini_list, *ini_cache;

extern bool __stdcall ini_foreach_key(const char *sec, char (*lpdata)[LEN_STRINGS], int line, const char *path, const bool isfile);
extern bool __stdcall ini_foreach_wkey(const char *sec, wchar_t (*lpdata)[LEN_STRINGS], int line, const char *path, const bool isfile);
extern bool __stdcall inicache_foreach_key(const char *sec, char (*lpdata)[LEN_STRINGS], int line, ini_cache *ini);
extern bool __stdcall inicache_foreach_wkey(const char *sec, wchar_t (*lpdata)[LEN_STRINGS], int line, ini_cache *ini);
extern bool __stdcall ini_foreach_string(const char *sec, char (*lpdata)[LEN_STRINGS], int line, const char *path, const bool isfile);
extern bool __stdcall ini_foreach_wstring(const char *sec, wchar_t (*lpdata)[LEN_STRINGS], int line, const char *path, const bool isfile);
extern bool __stdcall inicache_foreach_string(const char *sec, char (*lpdata)[LEN_STRINGS], int line, ini_cache *ini);
extern bool __stdcall inicache_foreach_wstring(const char *sec, wchar_t (*lpdata)[LEN_STRINGS], int line, ini_cache *ini);
extern bool __stdcall ini_read_string(const char *sec, const char *key, char **buf, const char *path, const bool isfile);
extern bool __stdcall inicache_read_string(const char *sec, const char *key, char **buf, ini_cache *ini);
extern bool __stdcall ini_write_string(const char *sec, const char *key, const char *new_value, const char *path);
extern bool __stdcall inicache_write_string(const char *sec, const char *key, const char *new_value, ini_cache *ini);
extern bool __stdcall ini_new_section(const char *value, const char *path);
extern bool __stdcall inicache_new_section(const char *value, ini_cache *ini);
extern bool __stdcall ini_delete_section(const char *sec, const char *path);
extern bool __stdcall inicache_delete_section(const char *sec, ini_cache *ini);
extern bool __stdcall ini_search_string(const char *key, char **buf, const char *path, const bool isfile);
extern bool __stdcall inicache_search_string(const char *key, char **buf, ini_cache *ini);
extern int  __stdcall inicache_readint_value(const char *sec, const char *key, ini_cache *ini, int default_value);
extern int  __stdcall inicache_read_int(const char *sec, const char *key, ini_cache *ini);
extern int  __stdcall ini_read_int(const char *sec, const char *key, const char *path, const bool isfile);
extern bool __stdcall inicache_sort_section(const char *sec, ini_cache *ini);
extern bool __stdcall ini_sort_section(const char *sec, const char *path);
extern void __stdcall inicache_foreach_section(char (*lpdata)[LEN_SECTION], const int line, ini_cache *ini);
extern void __stdcall ini_foreach_section(char (*lpdata)[LEN_SECTION], const int line, const char *path, const bool isfile);
extern char* __stdcall ini_utf16_utf8(const wchar_t *utf16, size_t *out_len);
extern char* __stdcall ini_utf16_mbcs(int codepage, const wchar_t *utf16, size_t *out_len);
extern wchar_t* __stdcall ini_mbcs_utf16(int codepage, const char *ansi, size_t *out_len);
extern char* __stdcall ini_mbcs_utf8(int codepage, const char *ansi, size_t *out_len);
extern wchar_t* __stdcall ini_utf8_utf16(const char *utf8, size_t *out_len);
extern char* __stdcall ini_utf8_mbcs(int codepage, const char *utf8, size_t *out_len);
extern uint64_t __stdcall inicache_read_uint64(const char *sec, const char *key, ini_cache *ini);
extern uint64_t __stdcall ini_read_uint64(const char *sec, const char *key, const char *path, const bool isfile);
extern ini_cache __stdcall iniparser_create_cache(const char *ini, const int access_or_size, const bool isfile);
extern void __stdcall iniparser_destroy_cache(ini_cache *li);

#ifdef __cplusplus
}
#endif

#endif   /* _INI_PARSER_H */
