#include <iostream>

#include "mt4_symbols.h"
#include "mt4_symbols_wrap.h"

#include "../mt4_client.h"
#include "../utils/napi_utils.h"

Napi::FunctionReference MT4SymbolsWrap::constructor;

Napi::Function MT4SymbolsWrap::Init(Napi::Env env)
{
    Napi::Function klass = DefineClass(
        env,
        "MT4Symbols",
        {
            InstanceMethod("get", &MT4SymbolsWrap::Get),
            InstanceMethod("getAll", &MT4SymbolsWrap::GetAll),
            InstanceMethod("_setTickHandler", &MT4SymbolsWrap::SetTickHandler),
            InstanceMethod("subscribe", &MT4SymbolsWrap::Subscribe),
            InstanceMethod("unsubscribe", &MT4SymbolsWrap::Unsubscribe),
        });

    constructor = Napi::Persistent(klass);
    constructor.SuppressDestruct();

    return klass;
}

MT4SymbolsWrap::MT4SymbolsWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<MT4SymbolsWrap>(info)
{
}

Napi::Object MT4SymbolsWrap::NewInstance(
    Napi::Env env,
    const std::shared_ptr<MT4Client> &client)
{
    Napi::EscapableHandleScope scope(env);

    Napi::Object obj = constructor.New({});
    MT4SymbolsWrap *wrap = Napi::ObjectWrap<MT4SymbolsWrap>::Unwrap(obj);

    wrap->symbols_ = MT4Symbols::CreateShared(client);
    wrap->bridge_ =
        std::make_shared<JsCallbackBridge<SymbolPayload>>("MT4SymbolsTickHandler");

    return scope.Escape(obj).ToObject();
}

Napi::Value MT4SymbolsWrap::Get(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        const std::string symbol = napi_utils::GetString(info, 0, "symbol");

        const SymbolInfo result = symbols_->Get(symbol);

        return ToNapiSymbol(env, result, true);
    }
    catch (const std::exception &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
}

Napi::Value MT4SymbolsWrap::GetAll(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        const auto symbols = symbols_->GetAll();

        Napi::Array arr = Napi::Array::New(env, symbols.size());

        for (size_t i = 0; i < symbols.size(); ++i)
        {
            arr.Set(i, ToNapiSymbolConfig(env, symbols[i]));
        }

        return arr;
    }
    catch (const std::exception &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
}

Napi::Value MT4SymbolsWrap::SetTickHandler(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        Napi::Function handler = napi_utils::GetFunction(info, 0, "handler");

        bridge_->SetHandler(env, handler);

        symbols_->SetTickHandler(
            [bridge = bridge_](const SymbolInfo *symbol)
            {
                bridge->CallJs(SymbolPayload{*symbol}, BuildSymbolArgs);
            });

        return env.Undefined();
    }
    catch (const std::exception &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
}

Napi::Value MT4SymbolsWrap::Subscribe(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        const std::string symbol = napi_utils::GetString(info, 0, "symbol");
        symbols_->Subscribe(symbol);
        return env.Undefined();
    }
    catch (const std::exception &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
}

Napi::Value MT4SymbolsWrap::Unsubscribe(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        const std::string symbol = napi_utils::GetString(info, 0, "symbol");
        symbols_->Unsubscribe(symbol);
        return env.Undefined();
    }
    catch (const std::exception &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
}