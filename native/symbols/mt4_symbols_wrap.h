#pragma once

#include <memory>
#include <napi.h>
#include <stdexcept>

#include "mt4_symbol_convertor.h"
#include "../utils/js_callback_bridge.h"

class MT4Client;
class MT4Symbols;

class MT4SymbolsWrap : public Napi::ObjectWrap<MT4SymbolsWrap>
{
public:
    static Napi::Function Init(Napi::Env env);

    static Napi::Object NewInstance(
        Napi::Env env,
        const std::shared_ptr<MT4Client> &client);

    explicit MT4SymbolsWrap(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;

    Napi::Value Get(const Napi::CallbackInfo &info);
    Napi::Value GetAll(const Napi::CallbackInfo &info);
    Napi::Value SetTickHandler(const Napi::CallbackInfo &info);
    Napi::Value Subscribe(const Napi::CallbackInfo &info);
    Napi::Value Unsubscribe(const Napi::CallbackInfo &info);

    std::shared_ptr<MT4Symbols> symbols_;
    std::shared_ptr<JsCallbackBridge<SymbolPayload>> bridge_;
};