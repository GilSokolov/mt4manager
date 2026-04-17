#pragma once

#include <memory>

#include <napi.h>

class MT4Client;

class MT4UsersWrap : public Napi::ObjectWrap<MT4UsersWrap>
{
public:
    static Napi::Function Init(Napi::Env env);
    static Napi::Object NewInstance(
        Napi::Env env,
        const std::shared_ptr<MT4Client> &client);

    MT4UsersWrap(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;

    Napi::Value Get(const Napi::CallbackInfo &info);

    std::shared_ptr<MT4Client> client_;
};