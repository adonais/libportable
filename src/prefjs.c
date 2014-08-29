#define PREJS_EXTERN

#include "prefjs.h"
#include "inipara.h"
#include <stdio.h>
#include <wchar.h>
#include <windows.h>
#include <shlwapi.h>

extern WCHAR appdata_path[VALUE_LEN+1];

#define USER_PREF(name, value) { name, value, 0 }
#define USER_FILE (L"user.js")
#define DIST_FILE (L"user.js.tmp001")

typedef struct {
    const char* name;
    const char* value;
    int   status;
} NamedStrRef;

static NamedStrRef NamedEntities[] = {
    USER_PREF("media.gmp-gmpopenh264.path", "%s"),
    USER_PREF("media.gmp-gmpopenh264.version", "9.9"),
    USER_PREF("media.gmp-manager.url", "")
};

static const char* newline = "\n";

/* 定位user.js文件的位置 */
BOOL WINAPI getProfilesDir(WCHAR *app, WCHAR *dist_buf, size_t len)
{
    BOOL ret = FALSE;
    dist_buf[0] = L'\0';
    if ( read_appint(L"General", L"Portable") > 0 &&
         read_appint(L"General", L"Nocompatete") > 0 &&
         _snwprintf(dist_buf,len,L"%ls",app) >0 )
    {
        if ( PathRemoveFileSpecW(dist_buf) )
        {
            ret = TRUE;
        }
    }
    else if ( app[1] == L':' )     /* Nocompatete=0? */
    {
        WCHAR profiles[VALUE_LEN+1];
        WCHAR prefs_path[NAMES_LEN+1];
        unsigned short count = 16;
        DWORD m;
        if ( !get_mozilla_profile(app, profiles, VALUE_LEN) )
        {
            return ret;
        }
        for ( ; count > 0; --count )
        {
            prefs_path[0] = L'\0';
            m=GetPrivateProfileStringW(L"Profile0",L"Path",NULL,prefs_path,NAMES_LEN,profiles);
            if ( m>0 && GetLastError() == 0x0 && PathRemoveFileSpecW(profiles) )
            {
                _snwprintf(dist_buf, len,L"%s\\%s", profiles, prefs_path);
                ret = TRUE;
                break;
            }
            else
            {
#ifdef _LOGDEBUG
                logmsg("GetPrivateProfileStringW function falsed\n");
#endif
                Sleep(400);
            }
        }
    }
    else
    {
        /* Portable=0? */
    }
    return ret;
}

BOOL gmp_write(LPCWSTR profile_dir,LPCWSTR path)
{
    int     i;
    FILE*   pfile;
    WCHAR   userjs_file[MAX_PATH+1] = {0};
    WCHAR   examp_file[MAX_PATH+1] = {0};
    char*   gmp_path = unicode_ansi(path);
    _snwprintf(userjs_file,MAX_PATH,L"%ls\\%ls",profile_dir,USER_FILE);
    _snwprintf(examp_file,MAX_PATH,L"%ls\\%ls",profile_dir,DIST_FILE);
    pfile = _wfopen(examp_file, L"a+");
    if ( !goodHandle(pfile) )
    {
    #ifdef _LOGDEBUG
        logmsg("%ls open false\n",examp_file);
    #endif
        return FALSE;
    }
    for ( i=0; i<sizeof(NamedEntities)/sizeof(NamedEntities[0]) ; ++i)
    {
        if ( strcmp("%s", NamedEntities[i].value) == 0 )
        {
            NamedEntities[i].value = gmp_path;
        }
        if ( !NamedEntities[i].status )
        {
            fprintf(pfile,"%suser_pref(\"%s\", \"%s\");", newline,NamedEntities[i].name,NamedEntities[i].value);
            NamedEntities[i].status = 1;
        }
    }
    if (gmp_path)
    {
        SYS_FREE(gmp_path);
    }
    if (pfile)
    {
        fclose(pfile),pfile = NULL;
    }
 #ifdef _LOGDEBUG
     logmsg("examp_file: %ls\nuserjs_file: %ls\n",examp_file,userjs_file);
 #endif     
    return MoveFileExW(examp_file, userjs_file, MOVEFILE_REPLACE_EXISTING);
}

void PrefsRewrite(LPCWSTR dist_dir,FILE* stream)
{
    char*     line_buf   = NULL;;
    WCHAR*    examp_file = NULL;
    HANDLE    hFile      = NULL;
    int       array_c    = sizeof(NamedEntities)/sizeof(NamedEntities[0]);
    if ( (examp_file=(WCHAR*)SYS_MALLOC((MAX_PATH+1)*sizeof(WCHAR))) == NULL )
    {
        return;
    }
    if ( _snwprintf(examp_file,MAX_PATH,L"%ls\\%ls",dist_dir,DIST_FILE) > 0 )
    {
        hFile = CreateFileW(examp_file,
                    GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_ALWAYS,
                    0,
                    NULL);
    }
    SYS_FREE(examp_file);
    if ( !goodHandle(hFile) )
    {
        return;
    }
    if ( (line_buf=(char*)SYS_MALLOC(MAX_PATH+1)) == NULL )
    {
        CloseHandle(hFile);
        return;
    }
    while( fgets(line_buf, VALUE_LEN, stream) )
    {
        int   i;
        char* skip_str = NULL;
        for ( i=0; i<array_c ; ++i)
        {
            skip_str = strstr(line_buf,NamedEntities[i].name);
            if ( skip_str != NULL )
            {
                break;
            }
        }
        if ( !isspace(line_buf[0]) && line_buf[0] != '/' && line_buf[0] != '*' && !skip_str )
        {
            DWORD dw_bytes;
            WriteFile(hFile, line_buf,(DWORD)strlen(line_buf), &dw_bytes, NULL);
        }
        ZeroMemory(line_buf,VALUE_LEN+1);
    }
    if (line_buf)
    {
        SYS_FREE(line_buf);
    }
    if ( goodHandle(hFile) )
    {
        CloseHandle(hFile);
    }
    if ( goodHandle(stream) )
    {
        fclose(stream);
    }
}

/* 打开目标文件 */
BOOL openPrefsFile(LPCWSTR dist, FILE** pp_file)
{
    WCHAR* user_fs;
    HANDLE hFile = NULL;
    if ( (user_fs = (WCHAR*)SYS_MALLOC((MAX_PATH+1)*sizeof(WCHAR))) == NULL )
    {
        return FALSE;
    }
    if ( _snwprintf(user_fs,MAX_PATH,L"%ls\\%ls",dist,USER_FILE) >0 )
    {
        *pp_file = _wfopen(user_fs, L"r");
    }   
    if (user_fs)
    {
        SYS_FREE(user_fs);
    }
    return (goodHandle(*pp_file));
}

unsigned WINAPI gmpservice_check(void * pParam)
{
    WCHAR   gmp_path[VALUE_LEN+1];
    WCHAR   dist_dir[VALUE_LEN+1];
    if ( read_appkey(L"Env",L"MOZ_GMP_PATH",gmp_path,sizeof(gmp_path)) )
    {
        FILE*   m_file = NULL;
        if ( getProfilesDir(appdata_path, dist_dir, VALUE_LEN) )
        {
        #ifdef _LOGDEBUG
            logmsg("profile:\n %ls\n",dist_dir);
        #endif
            PathToCombineW(gmp_path, VALUE_LEN);
            if ( openPrefsFile(dist_dir, &m_file) )
            {
                PrefsRewrite(dist_dir,m_file);
            }
            if ( !gmp_write(dist_dir, gmp_path) )
            {
            #ifdef _LOGDEBUG
                logmsg("gmp_write() return false\n");
            #endif
            }
        }
        if ( goodHandle(m_file) )
        {
            fclose(m_file);
        }
    }
    return (1);
}
