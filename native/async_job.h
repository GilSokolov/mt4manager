#pragma once

#include <napi.h>
#include <functional>
#include <memory>
#include <string>

class AsyncJob final : public Napi::AsyncWorker {
 public:
  AsyncJob(Napi::Env env,
           std::string resource_name,
           std::function<void()> execute_fn,
           Napi::Promise::Deferred deferred);

  void Execute() override;
  void OnOK() override;
  void OnError(const Napi::Error& error) override;

 private:
  std::function<void()> execute_fn_;
  Napi::Promise::Deferred deferred_;
};
