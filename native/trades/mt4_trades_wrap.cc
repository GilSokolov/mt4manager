#include "mt4_trades_wrap.h"

#include "../utils/env_registry.h"
#include "../utils/napi_utils.h"
#include "mt4_trades_from_napi.h"
#include "mt4_trades_to_napi.h"
#include "trade_request.h"

Napi::Object MT4TradesWrap::Init(Napi::Env env) {
  Napi::Function klass =
      DefineClass(env, "MT4Trades",
                  {
                      InstanceMethod("get", &MT4TradesWrap::Get),
                      InstanceMethod("execute", &MT4TradesWrap::Execute),
                      InstanceMethod("onEvent", &MT4TradesWrap::SetHandler),
                      InstanceMethod("handleBidAskUpdate",
                                     &MT4TradesWrap::HandleBidAskUpdate),
                  });

  EnvRegistry::Get(env).mt4Trades = Napi::Persistent(klass);
  return klass;
}

MT4TradesWrap::MT4TradesWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<MT4TradesWrap>(info) {}

Napi::Object MT4TradesWrap::NewInstance(
    Napi::Env env, const std::shared_ptr<MT4Client>& client) {
  Napi::EscapableHandleScope scope(env);

  Napi::Object obj = EnvRegistry::Get(env).mt4Trades.New({});

  MT4TradesWrap* wrap = Napi::ObjectWrap<MT4TradesWrap>::Unwrap(obj);

  wrap->trades_ = MT4Trades::CreateShared(client);

  wrap->bridge_ =
      std::make_shared<JsCallbackBridge<TradePayload>>("MT4TradeHandler");

  return scope.Escape(obj).ToObject();
}

MT4TradesWrap::~MT4TradesWrap() {
  if (trades_) {
    trades_->SetHandler(nullptr);
  }
}

Napi::Value MT4TradesWrap::SetHandler(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  try {
    Napi::Function handler = napi_utils::GetFunction(info, 0, "handler");

    bridge_->SetHandler(env, handler);

    trades_->SetHandler([bridge = bridge_](const TradeRecord* trade, int type)
    {
      bridge->CallJs(TradePayload{*trade, type}, BuildTradeArgs);
    });

    return env.Undefined();
  } catch (const std::exception& ex) {
    return napi_utils::ThrowError(env, ex);
  }
}

Napi::Value MT4TradesWrap::Get(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  try {
    const int order = napi_utils::GetInt32(info, 0, "login");
    const TradeRecord trade = trades_->Get(order);
    return ToNapiTrade(env, trade);
  } catch (const MT4Error& ex) {
    return napi_utils::ThrowError(env, ex);
  } catch (const std::exception& ex) {
    return napi_utils::ThrowError(env, ex);
  }
}

Napi::Value MT4TradesWrap::Execute(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  try {
    TradeRequest request = FromNapiTrade(info[0]);
    const int orderId = trades_->Execute(request);

    if (orderId == 0) {
      return Napi::Boolean::New(env, true);
    }

    const TradeRecord trade = trades_->Get(orderId);
    return ToNapiTrade(env, trade);
  } catch (const MT4Error& ex) {
    return napi_utils::ThrowError(env, ex);
  } catch (const std::exception& ex) {
    return napi_utils::ThrowError(env, ex);
  }
}

Napi::Value MT4TradesWrap::HandleBidAskUpdate(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  try {
    const std::string symbol = napi_utils::GetString(info, 0, "symbol");
    trades_->HandleBidAskUpdate(symbol);

    return env.Undefined();
  } catch (const MT4Error& ex) {
    return napi_utils::ThrowError(env, ex);
  } catch (const std::exception& ex) {
    return napi_utils::ThrowError(env, ex);
  }
}