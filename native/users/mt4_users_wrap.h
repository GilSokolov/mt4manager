#pragma once

#include <napi.h>

#include <memory>
#include <stdexcept>

#include "../utils/js_callback_bridge.h"
#include "./mt4_user_converter.h"

class MT4Client;
class MT4Users;

class MT4UsersWrap : public Napi::ObjectWrap<MT4UsersWrap> {
 public:
  ~MT4UsersWrap();
  static Napi::Function Init(Napi::Env env);
  static Napi::Object NewInstance(Napi::Env env,
                                  const std::shared_ptr<MT4Client>& client);

  MT4UsersWrap(const Napi::CallbackInfo& info);

 private:
  static Napi::FunctionReference constructor;

  Napi::Value Get(const Napi::CallbackInfo& info);
  Napi::Value GetMargin(const Napi::CallbackInfo& info);
  Napi::Value Create(const Napi::CallbackInfo& info);
  Napi::Value Update(const Napi::CallbackInfo& info);
  Napi::Value SetHandler(const Napi::CallbackInfo& info);
  Napi::Value HandleTradeUpdate(const Napi::CallbackInfo& info);

  std::shared_ptr<MT4Users> users_;
  std::shared_ptr<JsCallbackBridge<UserPayload>> bridge_;
};