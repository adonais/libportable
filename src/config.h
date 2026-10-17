#ifndef CHROME_PLUS_SRC_CONFIG_H_
#define CHROME_PLUS_SRC_CONFIG_H_

#include <optional>
#include <string>
#include <utility>
#include <vector>

class Config {
 public:
  static Config& Instance();

  // general
  const std::wstring& GetCommandLine() const { return command_line_; }
  const std::wstring& GetLaunchOnStartup() const { return launch_on_startup_; }
  const std::wstring& GetLaunchOnExit() const { return launch_on_exit_; }
  const std::optional<std::wstring>& GetUserDataDir() const {
    return user_data_dir_;
  }
  const std::optional<std::wstring>& GetDiskCacheDir() const {
    return disk_cache_dir_;
  }
  const std::wstring& GetBossKey() const { return boss_key_; }
  bool IsShowPassword() const { return show_password_; }

  // tabs
  bool IsHoverTab() const { return hover_tab_ > 0; }
  bool IsRightClickClose() const { return right_click_close_; }
  int GetHoverTabDelay() const { return hover_tab_; }

 private:
  Config();
  ~Config() = default;
  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;

  void LoadConfig();

  std::optional<std::wstring> LoadDirPath(const std::wstring& dir_type);

 private:
  // general
  std::wstring command_line_;
  std::wstring launch_on_startup_;
  std::wstring launch_on_exit_;
  std::optional<std::wstring> user_data_dir_;
  std::optional<std::wstring> disk_cache_dir_;
  std::wstring boss_key_;
  bool show_password_;

  // tabs
  int hover_tab_;
  bool right_click_close_;
};

extern const Config& config;

#endif  // CHROME_PLUS_SRC_CONFIG_H_
