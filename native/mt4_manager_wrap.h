#pragma once

#include <napi.h>
#include <memory>

#include "mt4_client.h"

class MT4Client;

class MT4ManagerWrap : public Napi::ObjectWrap<MT4ManagerWrap>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  MT4ManagerWrap(const Napi::CallbackInfo &info);
  ~MT4ManagerWrap() override;

private:
  static Napi::FunctionReference constructor;

  Napi::Value Connect(const Napi::CallbackInfo &info);
  Napi::Value Login(const Napi::CallbackInfo &info);
  Napi::Value Disconnect(const Napi::CallbackInfo &info);
  Napi::Value Close(const Napi::CallbackInfo &info);
  Napi::Value StartPumping(const Napi::CallbackInfo &info);
  Napi::Value StopPumping(const Napi::CallbackInfo &info);

  std::shared_ptr<MT4Client> client_;
  Napi::ObjectReference users_;
  Napi::ObjectReference symbols_;
  Napi::ObjectReference positions_;
};
