#pragma once

#include <atomic>
#include <mutex>
#include <string>

#include "../include/mt4_sdk.h"
#include "utils/mt4_factory.h"

class MT4Client
{
public:
  explicit MT4Client(const std::string &dllPath);
  ~MT4Client();

  void Connect(const std::string &server);
  void Login(int login, const std::string &password);
  void Disconnect();
  void Close();

private:
  void LoadApi();
  void UnloadApi() noexcept;
  void EnsureOpen() const;
  void EnsureManager() const;

private:
  std::mutex mutex_;
  std::atomic<bool> closed_{false};

  std::string dllPath_;
  CManagerFactory *factory_{nullptr};
  CManagerInterface *manager_{nullptr};

  bool connected_{false};
  bool loggedIn_{false};
};