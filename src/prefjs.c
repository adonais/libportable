#include "inipara.h"
#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <errno.h>
#include <wchar.h>

#define VER_LEN 16
#define USER_PREF(name, value) { name, value, 0 }
#define USER_FILE_TT (L"user.js")
#define DIST_FILE_TT (L"user.js.tmp001")
#define INFO_FILE_TT (L"gmpopenh264.info")

typedef struct {
    const char* name;
    const char* value;
    int   status;
} NamedStrRef;

static NamedStrRef NamedEntities[] = {
    USER_PREF("media.gmp-gmpopenh264.path", "%s"),
    USER_PREF("media.gmp-gmpopenh264.version", "%f"),
    USER_PREF("media.gmp-manager.url", "")
};

void gmp_write(LPCWSTR profile_dir,LPCWSTR path,LPCSTR version_str)
{
    int     i = 0;
    FILE*   pfile = NULL;
    WCHAR   userjs_file[MAX_PATH+1] = { L'\0' };
    WCHAR   examp_file[MAX_PATH+1] = { L'\0' };
    char*   gmp_path = NULL;
    LPCSTR  newline = "\n";
    do
    {
        gmp_path = unicode_ansi(path);
        if ( !gmp_path )
        {
            break;
        }
        i = _snwprintf(userjs_file,MAX_PATH,L"%ls\\%ls",profile_dir,USER_FILE_TT);
        if ( !(i > 1 && i < MAX_PATH) )
        {
            break;
        }
        i = _snwprintf(examp_file,MAX_PATH,L"%ls\\%ls",profile_dir,DIST_FILE_TT);
        if ( !(i > 1 && i < MAX_PATH) )
        {
            break;
        }
        pfile = _wfopen(examp_file, L"a+");
        if ( NULL == pfile )
        {
        #ifdef _LOGDEBUG
            logmsg("%ls open false\n",examp_file);
        #endif 
            break;
        }
        for ( i=0; i<sizeof(NamedEntities)/sizeof(NamedEntities[0]) ; ++i)
        {
            if ( strcmp("%s", NamedEntities[i].value) == 0 )
            {
                NamedEntities[i].value = gmp_path;
            }
            if ( strcmp("%f", NamedEntities[i].value) == 0 )
            {
                NamedEntities[i].value = version_str;
            }
            if ( !NamedEntities[i].status )
            {
                char line_buf[VALUE_LEN+1] = { '\0' };
                int  m = _snprintf(line_buf, VALUE_LEN, "%suser_pref(\"%s\", \"%s\");", \
                                   newline,NamedEntities[i].name,NamedEntities[i].value);
                if ( m>1 && m < VALUE_LEN )
                {
                    line_buf[m] = '\0';
                    fwrite(line_buf,sizeof(char), strlen(line_buf), pfile);
                    NamedEntities[i].status = 1;
                }
            }
        }
    }while (0);
    if ( gmp_path )
    {
        SYS_FREE(gmp_path);
    }
    if ( pfile )
    {
        fclose(pfile);
        MoveFileExW(examp_file, userjs_file, MOVEFILE_REPLACE_EXISTING);
    }
    return;
}

void prefs_write(LPCWSTR dist_dir, FILE* stream)
{
    char*     line_buf   = NULL;;
    WCHAR*    examp_file = NULL;
    FILE*     m_file     = NULL;
    int       array_c    = sizeof(NamedEntities)/sizeof(NamedEntities[0]);
    if ( (examp_file=(WCHAR*)SYS_MALLOC((MAX_PATH+1)*sizeof(WCHAR))) == NULL )
    {
        return;
    }
    if ( _snwprintf(examp_file,MAX_PATH,L"%ls\\%ls",dist_dir,DIST_FILE_TT) > 0 )
    {
        m_file = _wfopen(examp_file, L"wb+");
    }
    SYS_FREE(examp_file);
    do
    {
        if ( !m_file )
        {
            break;
        }
        if ( (line_buf=(char*)SYS_MALLOC(MAX_PATH+1)) == NULL )
        {
            break;
        }
        while( fgets(line_buf, MAX_PATH-1, stream) && errno != EINTR )
        {
            int   i = 0;
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
                fputs(line_buf, m_file);
            }
        }
    }while (0);
    if (line_buf)
    {
        SYS_FREE(line_buf);
    }
    if ( m_file )
    {
        fflush(m_file);
        fclose(m_file);
    }
}

/* 打开目标文件 */
BOOL open_prefs_file(LPCWSTR dist, FILE** pp_file)
{
    WCHAR* user_fs;
    if ( (user_fs = (WCHAR*)SYS_MALLOC((MAX_PATH+1)*sizeof(WCHAR))) == NULL )
    {
        return FALSE;
    }
    if ( _snwprintf(user_fs,MAX_PATH,L"%ls\\%ls",dist,USER_FILE_TT) > 0 )
    {
        *pp_file = _wfopen(user_fs, L"rb");
    }   
    SYS_FREE(user_fs);
    return (*pp_file != NULL);
}

BOOL get_gmp_version(WCHAR* gmp_info, char* version_str, int len)
{
    int   ret = 0;
    FILE* p_file = NULL;
    char  line_buf[NAMES_LEN+1] = { '\0' };
    do
    {
        const char* sub_str = "Version:";
        if ( !PathFileExistsW(gmp_info) )
        {
            break;
        }
        p_file = _wfopen(gmp_info, L"rb");
        if ( NULL == p_file )
        {
            break;
        }
        while( !feof(p_file) )
        {
            char* dist_str = NULL;
            fgets(line_buf, NAMES_LEN-1, p_file);
            if ( (dist_str = strstr(line_buf,sub_str)) != NULL )
            {
                int i = 0;
                dist_str += strlen(sub_str);
                while ( isspace(dist_str[i++]) )
                {
                    dist_str++;
                }
                ret = _snprintf(version_str, (size_t)len, "%s", dist_str);
                /* Strip trailing CR (\r\n) */
                if ( ret>0 && ret<len && version_str[ret-1] == '\n' )
                {
                    ret--;    
                }
                if ( ret>0 && ret<len && version_str[ret-1] == '\r' )
                {
                    ret--;    
                }
                if ( ret>0 && ret<len )
                {
                    version_str[ret] = '\0';
                }
                break;
            }
        }
    }while (0);
    if (p_file)
    {
        fclose(p_file);
    }
    return (ret>0 && ret<len);
}

void WINAPI gmpservice_check(LPCWSTR app_path, LPCWSTR gmp_path)
{
    WCHAR   dist_dir[MAX_PATH+1]  = { L'\0' };
    WCHAR   gmp_info[MAX_PATH+1]  = { L'\0' };
    char    version_str[VER_LEN+1] = { '\0' };
    FILE*   m_file = NULL;
    do
    {
        int m;
        if ( gmp_path == NULL || gmp_path[1] != L':' )
        {
            break;
        }    
        m = _snwprintf(gmp_info,MAX_PATH,L"%ls\\%ls",gmp_path,INFO_FILE_TT);
        if ( !(m > 0 && m < MAX_PATH) )
        {
            break;
        }
        if ( !get_gmp_version(gmp_info,version_str,VER_LEN) )
        {
            break;
        }
        if ( get_mozprofiles_path(app_path, dist_dir, MAX_PATH) )
        {
        #ifdef _LOGDEBUG
            logmsg("profile:\n %ls\n",dist_dir);
        #endif
            if ( open_prefs_file(dist_dir,&m_file) && m_file != NULL )
            {
                prefs_write(dist_dir,m_file);
                fclose(m_file);
            }
            gmp_write(dist_dir, gmp_path, version_str);
        }
    }while (0);
}
