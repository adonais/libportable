#ifndef _FILE_PASER_H_
#  define _FILE_PASER_H_

#ifdef __cplusplus
extern "C" {
#endif

extern bool     __stdcall json_parser(void *json, const WCHAR *xre_dir, const char *app_dir);
extern void*    __stdcall json_lookup(const WCHAR *file, const WCHAR *xre_path, const char *path);
extern unsigned __stdcall fn_update(void *lparam);

#ifdef __cplusplus
}
#endif

#endif   /* end _FILE_PASER_H_ */