#pragma once

#include <napi.h>
#include <memory>
#include "mt4_positions.h"
#include "../utils/js_callback_bridge.h"
#include "mt4_position_to_napi.h"

class MT4PositionsWrap : public Napi::ObjectWrap<MT4PositionsWrap>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(
        Napi::Env env,
        const std::shared_ptr<MT4Client> &client);

    MT4PositionsWrap(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;

    Napi::Value Get(const Napi::CallbackInfo &info);
    Napi::Value Create(const Napi::CallbackInfo &info);

    Napi::Value SetHandler(const Napi::CallbackInfo &info);

    std::shared_ptr<MT4Positions> positions_;
    std::shared_ptr<JsCallbackBridge<TradeEventPayload>> bridge_;
};