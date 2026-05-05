#pragma once

#include <napi.h>
#include <memory>

#include "mt4_trades.h"
#include "trade_payload.h"

#include "../utils/js_callback_bridge.h"

class MT4TradesWrap : public Napi::ObjectWrap<MT4TradesWrap>
{
public:
    static Napi::Object Init(Napi::Env env);
    static Napi::Object NewInstance(
        Napi::Env env,
        const std::shared_ptr<MT4Client> &client);

    MT4TradesWrap(const Napi::CallbackInfo &info);
    ~MT4TradesWrap();

private:
    static Napi::FunctionReference constructor;

    Napi::Value Get(const Napi::CallbackInfo &info);
    Napi::Value Execute(const Napi::CallbackInfo &info);
    Napi::Value SetHandler(const Napi::CallbackInfo &info);
    Napi::Value HandleBidAskUpdate(const Napi::CallbackInfo &info);

    std::shared_ptr<MT4Trades> trades_;
    std::shared_ptr<JsCallbackBridge<TradePayload>> bridge_;
};