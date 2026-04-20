#include "./mt4_users_wrap.h"

#include <memory>

#include "../mt4_client.h"
#include "mt4_users.h"

#include <iostream>

#include "./mt4_user_converter.h"
#include "../utils/napi_utils.h"

Napi::FunctionReference MT4UsersWrap::constructor;

Napi::Function MT4UsersWrap::Init(Napi::Env env)
{
    Napi::Function klass = DefineClass(
        env,
        "MT4Users",
        {
            InstanceMethod("get", &MT4UsersWrap::Get),
            InstanceMethod("subscribe", &MT4UsersWrap::Subscribe),
            InstanceMethod("unsubscribe", &MT4UsersWrap::Unsubscribe),
            InstanceMethod("unsubscribeAll", &MT4UsersWrap::UnsubscribeAll),
        });

    constructor = Napi::Persistent(klass);
    constructor.SuppressDestruct();

    return klass;
}

Napi::Object MT4UsersWrap::NewInstance(
    Napi::Env env,
    const std::shared_ptr<MT4Client> &client)
{
    Napi::EscapableHandleScope scope(env);

    Napi::Object obj = constructor.New({});
    MT4UsersWrap *wrap = Napi::ObjectWrap<MT4UsersWrap>::Unwrap(obj);
    wrap->users_ = std::make_shared<MT4Users>(client);

    return scope.Escape(napi_value(obj)).ToObject();
}

MT4UsersWrap::MT4UsersWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<MT4UsersWrap>(info)
{
}

Napi::Value MT4UsersWrap::Get(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    const int login = napi_utils::GetInt32(info, 0, "login");

    if (napi_utils::HasPendingException(env))
    {
        return env.Null();
    }

    try
    {
        const UserRecord user = users_->Get(login);
        return ToNapiUser(env, user);
    }
    catch (const std::exception &ex)
    {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MT4UsersWrap::Subscribe(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    const int login = napi_utils::GetInt32(info, 0, "login");

    if (napi_utils::HasPendingException(env))
    {
        return env.Null();
    }

    try
    {
        users_->Subscribe(login);
        return env.Undefined();
    }
    catch (const std::exception &ex)
    {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MT4UsersWrap::Unsubscribe(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    const int login = napi_utils::GetInt32(info, 0, "login");

    if (napi_utils::HasPendingException(env))
    {
        return env.Null();
    }

    try
    {
        users_->Unsubscribe(login);
        return env.Undefined();
    }
    catch (const std::exception &ex)
    {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MT4UsersWrap::UnsubscribeAll(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        users_->UnsubscribeAll();
        return env.Undefined();
    }
    catch (const std::exception &ex)
    {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}