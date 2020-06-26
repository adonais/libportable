#ifndef _INI_PARSER_H
#define _INI_PARSER_H

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    CHR_UNKOWN,
    CHR_WIN,
    CHR_UNIX
} str_line;

typedef enum
{
    E_OTHER,
    E_ANSI,
    E_UNICODE,
    E_UNICODE_BIG,
    E_UTF8,
    E_UTF8_BOM,
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

extern bool __stdcall ini_foreach_key(const char *sec, char (*lpdata)[129], int line, const char *path);
extern bool __stdcall ini_foreach_wkey(const char *sec, wchar_t (*lpdata)[129], int line, const char *path);
extern bool __stdcall inicache_foreach_key(const char *sec, char (*lpdata)[129], int line, ini_cache *ini);
extern bool __stdcall inicache_foreach_wkey(const char *sec, wchar_t (*lpdata)[129], int line, ini_cache *ini);
extern bool __stdcall ini_foreach_string(const char *sec, char (*lpdata)[129], int line, const char *path);
extern bool __stdcall ini_foreach_wstring(const char *sec, wchar_t (*lpdata)[129], int line, const char *path);
extern bool __stdcall inicache_foreach_string(const char *sec, char (*lpdata)[129], int line, ini_cache *ini);
extern bool __stdcall inicache_foreach_wstring(const char *sec, wchar_t (*lpdata)[129], int line, ini_cache *ini);
extern bool __stdcall ini_read_string(const char *sec, const char *key, char **buf, const char *path);
extern bool __stdcall inicache_read_string(const char *sec, const char *key, char **buf, ini_cache *ini);
extern bool __stdcall ini_write_string(const char *sec, const char *key, const char *new_value, const char *path);
extern bool __stdcall inicache_write_string(const char *sec, const char *key, const char *new_value, ini_cache *ini);
extern bool __stdcall ini_new_section(const char *value, const char *path);
extern bool __stdcall inicache_new_section(const char *value, ini_cache *ini);
extern bool __stdcall ini_delete_section(const char *sec, const char *path);
extern bool __stdcall inicache_delete_section(const char *sec, ini_cache *ini);
extern bool __stdcall ini_search_string(const char *key, char **buf, const char *path);
extern bool __stdcall inicache_search_string(const char *key, char **buf, ini_cache *ini);
extern ini_cache __stdcall iniparser_create_cache(const char *ini, bool write_access);
extern void __stdcall iniparser_destroy_cache(ini_cache *li);
extern int  __stdcall inicache_read_int(const char *sec, const char *key, ini_cache *ini);
extern int  __stdcall ini_read_int(const char *sec, const char *key, const char *path);
extern char*__stdcall utf16_to_utf8(const wchar_t *utf16);
extern char* __stdcall utf16_to_mbcs(const wchar_t *utf16);
extern wchar_t* __stdcall mbcs_to_utf16(const char *ansi);
extern char* __stdcall mbcs_to_utf8(const char *ansi);
extern wchar_t* __stdcall utf8_to_utf16(const char *utf8);
extern char* __stdcall utf8_to_mbcs(const char *utf8);

#ifdef __cplusplus
}
#endif

#endif   /* _INI_PARSER_H */
