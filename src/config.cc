#include "config.h"

#include <windows.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "utils.h"

Config& Config::Instance() {
  static Config instance;
  return instance;
}

Config::Config() {
  LoadConfig();
}

void Config::LoadConfig() {
  // general
  command_line_ = GetIniString(L"general", L"command_line", L"");
  launch_on_startup_ = GetIniString(L"general", L"launch_on_startup", L"");
  launch_on_exit_ = GetIniString(L"general", L"launch_on_exit", L"");
  user_data_dir_ = LoadDirPath(L"data");
  disk_cache_dir_ = LoadDirPath(L"cache");
  boss_key_ = GetIniString(L"general", L"boss_key", L"");
  show_password_ = ::GetPrivateProfileIntW(L"general", L"show_password", 1,
                                           GetIniPath().c_str()) != 0;
  // tabs
  hover_tab_ = ::GetPrivateProfileIntW(L"tabs", L"hover_tab", 0, GetIniPath().c_str());
  right_click_close_ = ::GetPrivateProfileIntW(L"tabs", L"right_click_close", 0,
                                               GetIniPath().c_str()) != 0;
}

std::optional<std::wstring> Config::LoadDirPath(const std::wstring& dir_type) {
  std::wstring path = CanonicalizePath(GetAppDir() + L"\\..\\" + dir_type);
  std::wstring dir_key = dir_type + L"_dir";
  std::wstring dir_buffer = GetIniString(L"general", dir_key, path);

  if (dir_buffer == L"none") {
    return std::nullopt;
  }

  if (dir_buffer.empty()) {
    dir_buffer = path;
  }

  std::wstring expanded_path = ExpandEnvironmentPath(dir_buffer);
  ReplaceStringInPlace(expanded_path, L"%app%", GetAppDir());
  return GetAbsolutePath(expanded_path);
}

const Config& config = Config::Instance();
