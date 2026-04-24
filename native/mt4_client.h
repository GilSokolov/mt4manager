#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include "pumping_options.h"
#include "mt4_client_config.h"

#include "../include/mt4_sdk.h"

class MT4Client
{
public:
  using PumpListener = std::function<void(int, int, void *)>;

  explicit MT4Client(MT4ClientConfig config);
  ~MT4Client();

  void Connect(const std::string &server);
  void Login(int login, const std::string &password);
  void Disconnect();
  void Close();

  void StartPumping(const PumpingOptions &options);
  void StopPumping();
  bool IsPumping() const;
  int AddPumpListener(PumpListener listener);
  void RemovePumpListener(int id);

  CManagerInterface *Manager() const { return manager_; }

private: // callbacks
  static void __stdcall PumpCallback(int code, int type, void *data, void *param);

private: // helpers
  void LoadApi();
  void UnloadApi() noexcept;
  void EnsureOpen() const;
  void EnsureManager() const;
  void ValidateDllPath() const;

private: // pumping
  void TogglePumping(int flags);
  void NotifyPumpListeners(int code, int type, void *data);
  std::vector<PumpListener> CopyPumpListeners() const;

private: // state
  MT4ClientConfig config_;

  std::mutex mutex_;
  mutable std::mutex pump_listeners_mutex_;

  std::atomic<bool> closed_{false};
  std::atomic<bool> pumping_{false};

  std::vector<PumpListener> pump_listeners_;

  CManagerFactory *factory_{nullptr};
  CManagerInterface *manager_{nullptr};

  int pump_flags_{0};

  bool connected_{false};
  bool loggedIn_{false};
  bool winsockStarted_{false};
};