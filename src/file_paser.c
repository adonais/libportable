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

static char *
mychr_replace(const char *path)
{
    char *ret = NULL;
    char *lp = NULL;
    intptr_t pos;
    if (path)
    {
        ret = _strdup(path);
    }
    do
    {
        lp = strchr(ret, '\\');
        if (lp)
        {
            pos = lp - ret;
            ret[pos] = '/';
        }
    } while (lp != NULL);
    return ret;
}

static const char *
mystr_replace(char *in, size_t in_size, const char *sub, const char *by)
{
    char *in_ptr = in;
    char res[MAX_PATH + 1] = { 0 };
    size_t resoffset = 0;
    char *needle;
    while ((needle = strstr(in, sub)) && resoffset < in_size)
    {
        strncpy(res + resoffset, in, needle - in);
        resoffset += needle - in;
        in = needle + (int) strlen(sub);
        strncpy(res + resoffset, by, strlen(by));
        resoffset += (int) strlen(by);
    }
    strcpy(res + resoffset, in);
    _snprintf(in_ptr, (int) in_size, "%s", res);
    return in_ptr;
}

static bool
value_repalce(cJSON *parent, const char *item, const char *sub, const char *by)
{
    bool ret = false;
    cJSON *path = cJSON_GetObjectItem(parent, item);
    if (path && strstr(path->valuestring, sub))
    {
        size_t len = strlen(path->valuestring) + 32;
        char *new_str = calloc(1, len);
        assert(new_str != NULL);
        strcpy(new_str, path->valuestring);
        mystr_replace(new_str, len, sub, by);
        cJSON_ReplaceItemInObject(parent, item, cJSON_CreateString(new_str));
        free(new_str);
        ret = true;
    }
    return ret;
}

static bool
node_path_fix(cJSON *addons, const char *item, const char *sub, const char *by)
{
    int size = cJSON_GetArraySize(addons);
    if (size > 0)
    {
        for (int i = 0; i < size; i++)
        {
            cJSON *tnode = cJSON_GetArrayItem(addons, i);
            value_repalce(tnode, item, sub, by);
        }
        return true;
    }
    return false;
}

static int
lookup_json(const char *file, const char *save_name, const char *win_app, const char *win_profiles)
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
    if ((fp = fopen(file, "rb")) == NULL)
    {
        return ret;
    }
    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    bytes = fread(moz, 1, 8, fp);
    if (strcmp(moz, moz_magic) == 0)
    {
        unsigned char numbers[4];
        bytes = fread(numbers, 1, 4, fp);
        out_len = bytes2int(numbers);
        dest = (char *) calloc(len, sizeof(char));
        assert(dest != NULL);
        bytes = fread(dest, 1, len, fp);
        assert(bytes > 0);
        regen_buffer = calloc(out_len, sizeof(char));
        assert(regen_buffer != NULL);
        const int decompressed_size = LZ4_decompress_safe(dest, regen_buffer, (int) bytes, out_len);
        if (decompressed_size > 0)
        {
            char *win_profiles_path = NULL;
            char *unix_profiles_path = NULL;
            char *win_app_path = NULL;
            char *unix_app_path = NULL;
            char *unix_profiles = mychr_replace(win_profiles);
            char *unix_app = mychr_replace(win_app);
            json = cJSON_Parse(regen_buffer);
            assert(json != NULL);
            cJSON *app_profile = cJSON_GetObjectItem(json, "app-profile");
            cJSON *system_default = cJSON_GetObjectItem(json, "app-system-defaults");
            cJSON *system_addon = cJSON_GetObjectItem(json, "app-system-addons");
            if (app_profile && (win_profiles_path = _strdup(cJSON_GetObjectItem(app_profile, "path")->valuestring)) != NULL)
            {
                if (strrchr(win_profiles_path, '\\') != NULL)
                {
                    *strrchr(win_profiles_path, '\\') = '\0';
                }
                unix_profiles_path = mychr_replace(win_profiles_path);
                cJSON *addons = cJSON_GetArrayItem(app_profile, 0);
                if (addons)
                {
                    node_path_fix(addons, "rootURI", unix_profiles_path, unix_profiles);
                }
                value_repalce(app_profile, "path", win_profiles_path, win_profiles);
                if (system_addon)
                {
                    cJSON *addons = cJSON_GetArrayItem(system_addon, 0);
                    if (addons)
                    {
                        node_path_fix(addons, "rootURI", unix_profiles_path, unix_profiles);
                    }
                    value_repalce(system_addon, "path", win_profiles_path, win_profiles);
                }
                if (system_default && (win_app_path = _strdup(cJSON_GetObjectItem(system_default, "path")->valuestring)) != NULL)
                {
                    if (strrchr(win_app_path, '\\') != NULL)
                    {
                        *strrchr(win_app_path, '\\') = '\0';
                        if (strrchr(win_app_path, '\\') != NULL)
                        {
                            *strrchr(win_app_path, '\\') = '\0';
                        }
                    }
                    unix_app_path = mychr_replace(win_app_path);
                    cJSON *addons = cJSON_GetArrayItem(system_default, 0);
                    if (addons)
                    {
                        node_path_fix(addons, "rootURI", unix_app_path, unix_app);
                    }
                    value_repalce(system_default, "path", win_app_path, win_app);
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
                if (unix_app_path)
                {
                    free(unix_app_path);
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
                const uint32_t src_size = (int) strlen(out_json);
                const uint32_t max_dst_size = LZ4_compressBound(src_size);
                char *compressed_data = calloc((size_t) max_dst_size, 1);
                assert(compressed_data != NULL);
                const int compressed_data_size = LZ4_compress_default(out_json, compressed_data, src_size, max_dst_size);
                if (compressed_data_size > 0 && (out = fopen(save_name, "wb")) != NULL)
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
            }
        }
    }
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
    char win_file[MAX_PATH + 1] = { 0 };
    char win_save[MAX_PATH + 1] = { 0 };
    char win_app[MAX_PATH + 1] = { 0 };
    if (!WideCharToMultiByte(CP_UTF8, 0, moz_profile, -1, win_profile, MAX_PATH, NULL, NULL))
    {
        return false;
    }
    if (!GetModuleFileNameA(NULL, win_app, MAX_PATH))
    {
        return false;
    }
    if (strrchr(win_app, '\\') != NULL)
    {
        *strrchr(win_app, '\\') = '\0';
    }
    _snprintf(win_file, MAX_PATH, "%s\\%s", win_profile, "addonStartup.json.lz4");
    _snprintf(win_save, MAX_PATH, "%s\\%s", win_profile, "addonStartup.json.lz4.new");
    if (lookup_json(win_file, win_save, win_app, win_profile) != 0)
    {
        return false;
    }
    return MoveFileExA(win_save, win_file, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
}
#ifdef __GNUC__
#pragma GCC diagnostic pop /* -Wstringop-truncation -Wstringop-overflow */
#endif
