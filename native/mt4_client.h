#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <functional>
#include "../include/mt4_sdk.h"

class MT4Client
{
public:
  explicit MT4Client(const std::string &dllPath);
  ~MT4Client();

  void Connect(const std::string &server);
  void Login(int login, const std::string &password);
  void Disconnect();
  void Close();
  void StartPumping();
  void StopPumping();
  bool IsPumping() const;

  using PumpListener = std::function<void(int, int, void *, void *)>;
  void AddPumpListener(PumpListener listener);

  CManagerInterface *Manager() const { return manager_; }

private:
  void LoadApi();
  void UnloadApi() noexcept;
  void EnsureOpen() const;
  void EnsureManager() const;
  void ValidateDllPath() const;

private:
  static void __stdcall PumpCallback(int code, int type, void *data, void *param);

  void HandlePumpEvent(int code, int type, void *data, void *param);

private:
  std::mutex mutex_;
  std::atomic<bool> closed_{false};
  std::atomic<bool> pumping_{false};
  std::mutex pump_listeners_mutex_;
  std::vector<PumpListener> pump_listeners_;
  std::string dllPath_;

  static MT4Client *active_pump_client_;

  CManagerFactory *factory_{nullptr};
  CManagerInterface *manager_{nullptr};

  bool connected_{false};
  bool loggedIn_{false};
  bool winsockStarted_{false};

  int pump_flags_ = 0;
};