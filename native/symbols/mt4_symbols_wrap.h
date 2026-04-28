// native/symbols/mt4_symbols_wrap.h
#pragma once

#include <memory>
#include <napi.h>

#include "./mt4_symbols.h"

class MT4SymbolsWrap : public Napi::ObjectWrap<MT4SymbolsWrap>
{
public:
    static Napi::Function Init(Napi::Env env);

    static Napi::Object NewInstance(
        Napi::Env env,
        const std::shared_ptr<MT4Client> &client);

    explicit MT4SymbolsWrap(const Napi::CallbackInfo &info);

private:
    Napi::Value Get(const Napi::CallbackInfo &info);
    Napi::Value GetAll(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;

    std::shared_ptr<MT4Symbols> symbols_;
};