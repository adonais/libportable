#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <windows.h>
#include "lz4.h"
#include "cjson.h"

#ifdef __GNUC__
#define GCC_VERSION ((__GNUC__ * 10000) + (__GNUC_MINOR__ * 1000) + (__GNUC_PATCHLEVEL__ * 100))
#pragma GCC diagnostic push
#if GCC_VERSION >= 80100
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
#if GCC_VERSION >= 80200
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
#endif

#ifdef _LOGDEBUG
extern void     __cdecl logmsg(const char * format, ...);
#endif

static unsigned char *
int2bytes(uint32_t value)
{
    unsigned char *src = (unsigned char *) calloc(1, 4);
    src[3] = (byte)((value >> 24) & 0xFF);
    src[2] = (byte)((value >> 16) & 0xFF);
    src[1] = (byte)((value >> 8) & 0xFF);
    src[0] = (byte)(value & 0xFF);
    return src;
}

static uint32_t
bytes2int(unsigned char *src)
{
    int value;
    value = (uint32_t)((src[0] & 0xFF) | ((src[1] & 0xFF) << 8) | ((src[2] & 0xFF) << 16) | ((src[3] & 0xFF) << 24));
    return value;
}

static const char*
fie2url(char* str)
{
    int i;
    int j = 0;
    int m_size=0;
    char *result = NULL;
    if (str == NULL)
    {
        return NULL;
    }
    m_size=(int)strlen(str);
    if ((result = (char *)malloc(3*m_size)) == NULL) 
    {
        return NULL;
    }
    for (i=0; i<m_size; ++i) 
    {
        char ch = str[i];
        if (((ch>='A') && (ch<='Z')) ||
            ((ch>='a') && (ch<='z')) ||
            ((ch>='0') && (ch<='9'))) 
        {
            result[j++] = ch;
        }
        else if (ch == ':' || ch == '/' || ch == '\\' || ch == '.' || ch == '-' || ch == '_' || ch == '&' ||
                ch == '$' || ch == '[' || ch == ']' || ch == '@' || ch == '!' || ch == '=' ||
                ch == ',' || ch == '~' || ch == '+' || ch == '\'') 
        {
            result[j++] = ch;
        } else 
        {
            sprintf(result+j, "%%%02X", (unsigned char)ch);
            j += 3;
        }
    }
    result[j] = '\0';
    strcpy(str,result);
    free(result);
    return str;
}

static char *
mychr_replace(const char *path)
{
    char *ret = NULL;
    char *lp = NULL;
    intptr_t pos;
    if (path == NULL)
    {
        return ret;
    }
    if ((ret = (char *)calloc(1, MAX_PATH)) == NULL)
    {
        return ret;
    }
    strncpy(ret, path, MAX_PATH);
    do
    {
        lp = strchr(ret, '\\');
        if (lp)
        {
            pos = lp - ret;
            ret[pos] = '/';
        }
    } while (lp != NULL);
    if (ret)
    {
        fie2url(ret);
    }
    return ret;
}

static char *
mystr_replace(const char *in, size_t in_size, const char *sub, const char *by)
{
    char *res = NULL;
    size_t resoffset = 0;
    char *needle;
    if ((res = (char *)calloc(1, in_size)) == NULL)
    {
        return NULL;
    }
    while ((needle = strstr(in, sub)) && resoffset < in_size)
    {
        strncpy(res + resoffset, in, needle - in);
        resoffset += needle - in;
        in = needle + (int) strlen(sub);
        strncpy(res + resoffset, by, strlen(by));
        resoffset += (int) strlen(by);
    }
    strcpy(res + resoffset, in);
    return res;
}

static bool
value_repalce(cJSON *parent, const char *item, const char *sub1, const char *sub2, const char *by)
{
    bool ret = false;
    const char *sub = NULL;
    cJSON *path = cJSON_GetObjectItem(parent, item);
    if (path == NULL)
    {
        return false;
    }
    sub = strstr(path->valuestring, sub1)?sub1:NULL;
    if (sub == NULL && sub2 != NULL)
    {
        sub = strstr(path->valuestring, sub2)?sub2:NULL;
    }
    if (sub)
    {
        size_t len = strlen(path->valuestring) + strlen(sub) + 4;
        char *new_str = mystr_replace(path->valuestring, len, sub, by);
        if (new_str != NULL)
        {
            cJSON_ReplaceItemInObject(parent, item, cJSON_CreateString(new_str));
            free(new_str);
            ret = true;
        }
    }
    return ret;
}

static bool
node_path_fix(cJSON *addons, const char *item, const char *sub1, const char *sub2, const char *by)
{
    int size = cJSON_GetArraySize(addons);
    if (size > 0)
    {
        for (int i = 0; i < size; i++)
        {
            cJSON *tnode = cJSON_GetArrayItem(addons, i);
            value_repalce(tnode, item, sub1, sub2, by);
        }
        return true;
    }
    return false;
}

static int
lookup_json(const wchar_t *file, const wchar_t *save_name, const char *win_app, const char *win_profiles)
{
    FILE *fp = NULL;
    FILE *out = NULL;
    char *out_json = NULL;
    uint32_t len = 0;
    const char moz_magic[] = "mozLz40";
    char moz[8] = { 0 };
    size_t bytes = 0;
    char *dest = NULL;
    char *regen_buffer = NULL;
    int32_t out_len = 0;
    int ret = -1;
    cJSON *json = NULL;
    do
    {
        unsigned char numbers[4];    
        int decompressed_size = 0;
        char *win_profiles_path = NULL;
        char *unix_profiles_path = NULL;
        char *win_app_path = NULL;
        char *u8_app_path = NULL;
        char *a8_app_path = NULL;
        char *unix_profiles = NULL;
        char *unix_app = NULL;
        cJSON *app_profile = NULL;
        cJSON *system_default = NULL;
        cJSON *system_addon = NULL; 
        int src_size = 0;
        int max_dst_size = 0;        
        char *compressed_data = NULL;
        int compressed_data_size = 0; 
        if ((fp = _wfopen(file, L"rb")) == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("fopen %s failed\n", file);
        #endif          
            break;
        }
        fseek(fp, 0L, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        bytes = fread(moz, 1, 8, fp);        
        if (strcmp(moz, moz_magic) != 0)
        {
            break;
        }
        bytes = fread(numbers, 1, 4, fp);
        out_len = bytes2int(numbers);
        dest = (char *) calloc(len, sizeof(char));
        if (dest == NULL)
        {
            break;
        }    
        bytes = fread(dest, 1, len, fp);
        if (bytes < 1)
        {
            break;
        }   
        regen_buffer = calloc(out_len, sizeof(char));
        if (regen_buffer == NULL)
        {
            break;
        }   
        decompressed_size = LZ4_decompress_safe(dest, regen_buffer, (int) bytes, out_len);
        if (decompressed_size < 1)
        {
        #ifdef _LOGDEBUG
            logmsg("LZ4_decompress_safe faild, decompressed_size = %d\n", decompressed_size);
        #endif 
            break;            
        } 
    #ifdef _LOGDEBUG
        FILE *tptp = NULL;
        if ((tptp = _wfopen(L"test.json", L"wb")) != NULL)
        {
            fwrite(regen_buffer, 1, decompressed_size, tptp);
            fclose(tptp);        
        } 
    #endif
        if ((json = cJSON_Parse(regen_buffer)) == NULL) 
        { 
            break;
        } 
        app_profile = cJSON_GetObjectItem(json, "app-profile");
        system_default = cJSON_GetObjectItem(json, "app-system-defaults");
        system_addon = cJSON_GetObjectItem(json, "app-system-addons");
        unix_profiles = mychr_replace(win_profiles);
        unix_app = mychr_replace(win_app);
        if (system_default && (win_app_path = _strdup(cJSON_GetObjectItem(system_default, "path")->valuestring)) != NULL)
        {
            wchar_t u_app_path[MAX_PATH+1] = {0};
            char ansi_app_path[MAX_PATH+1] = {0};                    
            if (strrchr(win_app_path, '\\') != NULL)
            {
                *strrchr(win_app_path, '\\') = '\0';
                if (strrchr(win_app_path, '\\') != NULL)
                {
                    *strrchr(win_app_path, '\\') = '\0';
                }
            }
            if (!MultiByteToWideChar(CP_UTF8,0,win_app_path,-1,u_app_path,MAX_PATH))
            {
                break;
            } 
            if (!WideCharToMultiByte(CP_ACP, 0, u_app_path, -1, ansi_app_path, MAX_PATH, NULL, NULL))
            {
                break;
            }                                     
            u8_app_path = mychr_replace(win_app_path);
            a8_app_path = mychr_replace(ansi_app_path);
        #ifdef _LOGDEBUG
            logmsg("u8_app_path = %s\n", u8_app_path);
            logmsg("a8_app_path = %s\n", a8_app_path);
        #endif            
            cJSON *addons = cJSON_GetArrayItem(system_default, 0);
            if (addons)
            {
                node_path_fix(addons, "rootURI", u8_app_path, a8_app_path, unix_app);
            }
            value_repalce(system_default, "path", win_app_path, NULL, win_app);
        }
        if (app_profile && (win_profiles_path = _strdup(cJSON_GetObjectItem(app_profile, "path")->valuestring)) != NULL)
        {            
            if (strrchr(win_profiles_path, '\\') != NULL)
            {
                *strrchr(win_profiles_path, '\\') = '\0';
            }               
            unix_profiles_path = mychr_replace(win_profiles_path);
        #ifdef _LOGDEBUG
            logmsg("unix_profiles_path = %s\n", unix_profiles_path);
            logmsg("unix_profiles = %s\n", unix_profiles);
        #endif                  
            cJSON *addons = cJSON_GetArrayItem(app_profile, 0);
            if (addons)
            {
                node_path_fix(addons, "rootURI", unix_profiles_path, NULL, unix_profiles);
            }
            value_repalce(app_profile, "path", win_profiles_path, NULL, win_profiles);
        }
        if (system_addon != NULL)
        {
            cJSON *addons = cJSON_GetArrayItem(system_addon, 0);
            if (addons)
            {
                node_path_fix(addons, "rootURI", unix_profiles_path, NULL, unix_profiles);
            }             
            value_repalce(system_addon, "path", win_profiles_path, NULL, win_profiles);
        }        
        if (win_profiles_path)
        {
            free(win_profiles_path);
        }
        if (win_app_path)
        {
            free(win_app_path);
        }
        if (unix_profiles_path)
        {
            free(unix_profiles_path);
        }
        if (u8_app_path)
        {
            free(u8_app_path);
        }
        if (a8_app_path)
        {
            free(a8_app_path);
        }        
        if (unix_profiles)
        {
            free(unix_profiles);
        }
        if (unix_app)
        {
            free(unix_app);
        }
        out_json = cJSON_PrintUnformatted(json);
        src_size = (int) strlen(out_json);
        max_dst_size = LZ4_compressBound(src_size);
        if ((compressed_data = calloc((size_t) max_dst_size, 1)) == NULL)
        {
            break;
        }
        compressed_data_size = LZ4_compress_default(out_json, compressed_data, src_size, max_dst_size);
        if (compressed_data_size > 0 && (out = _wfopen(save_name, L"wb")) != NULL)
        {
        #define MAGICNUMBER_SIZE 4
            unsigned char *moz_skip = int2bytes(src_size);
            fwrite(moz_magic, sizeof(moz_magic), 1, out);
            fwrite(moz_skip, MAGICNUMBER_SIZE, 1, out);
            fwrite(compressed_data, compressed_data_size, 1, out);
            fclose(out);
            free(moz_skip);
            free(compressed_data);
            ret = 0;
        #undef MAGICNUMBER_SIZE
        }
    }while(0); 
    if (fp)
    {
        fclose(fp);
    }
    if (dest)
    {
        free(dest);
    }
    if (regen_buffer)
    {
        free(regen_buffer);
    }
    if (out_json)
    {
        free(out_json);
    }
    if (json)
    {
        cJSON_Delete(json);
    }
    return ret;
}

bool __stdcall json_parser(wchar_t *moz_profile)
{
    char win_profile[MAX_PATH + 1] = { 0 };
    wchar_t u_file[MAX_PATH + 1] = { 0 };
    wchar_t u_save[MAX_PATH + 1] = { 0 };
    wchar_t u_app[MAX_PATH + 1] = { 0 };  
    char win_app[MAX_PATH + 1] = { 0 };
    if (!GetModuleFileNameW(NULL, u_app, MAX_PATH))
    {
        return false;
    }
    if (wcsrchr(u_app, '\\') != NULL)
    {
        *wcsrchr(u_app, '\\') = '\0';
    } 
#ifdef USE_UTF8    
    if (!WideCharToMultiByte(CP_UTF8, 0, u_app, -1, win_app, MAX_PATH, NULL, NULL))
#else
    if (!WideCharToMultiByte(CP_ACP, 0, u_app, -1, win_app, MAX_PATH, NULL, NULL))
#endif 
    {
        return false;
    }   
    _snwprintf(u_file, MAX_PATH, L"%ls\\%ls", moz_profile, L"addonStartup.json.lz4");
    _snwprintf(u_save, MAX_PATH, L"%ls\\%ls", moz_profile, L"addonStartup.json.lz4.new");
#ifdef USE_UTF8    
    if (!WideCharToMultiByte(CP_UTF8, 0, moz_profile, -1, win_profile, MAX_PATH, NULL, NULL))
#else
    if (!WideCharToMultiByte(CP_ACP, 0, moz_profile, -1, win_profile, MAX_PATH, NULL, NULL))
#endif                
    {
        return false;
    }
    if (lookup_json(u_file, u_save, win_app, win_profile) != 0)
    {
    #ifdef _LOGDEBUG
        logmsg("lookup_json return false.\n");
    #endif        
        return false;
    }
    return MoveFileExW(u_save, u_file, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
}
#ifdef __GNUC__
#pragma GCC diagnostic pop /* -Wstringop-truncation -Wstringop-overflow */
#endif