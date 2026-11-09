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
  show_password_ = GetIniInt(L"general", L"show_password", 1) > 0;

  // tabs
  hover_tab_ = GetIniInt(L"tabs", L"hover_tab", 0);
  right_click_close_ = GetIniInt(L"tabs", L"right_click_close", 0) > 0;
  wheel_tab_when_press_rbutton_ = GetIniInt(L"tabs", L"right_click_mousewheel", 0) > 0;
  // update
  browser_updates_ = GetIniInt(L"general", L"update", 0) > 0;
  ready_updates_ = GetIniInt(L"update", L"be_ready", 0) > 0;
  last_check_ = GetIniI64(L"update", L"last_check", 0);
}

std::optional<std::wstring> Config::LoadDirPath(const std::wstring& dir_type) {
  std::wstring path = CanonicalizePath(GetAppDir() + L"\\..\\" + dir_type);
  std::wstring dir_key = dir_type + L"_dir";
  std::wstring dir_buffer = GetIniString(L"general", dir_key.c_str(), path.c_str());

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
