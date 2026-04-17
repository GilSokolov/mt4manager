#include "./mt4_users_wrap.h"

#include <memory>

#include "../mt4_client.h"
#include "mt4_users.h"

#include <iostream>

Napi::FunctionReference MT4UsersWrap::constructor;

namespace
{
    Napi::Object ToNapiUser(Napi::Env env, const MT4UserRecord &user)
    {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("login", Napi::Number::New(env, user.login));
        obj.Set("group", Napi::String::New(env, user.group));
        obj.Set("name", Napi::String::New(env, user.name));
        obj.Set("email", Napi::String::New(env, user.email));
        obj.Set("leverage", Napi::Number::New(env, user.leverage));
        return obj;
    }
} // namespace

Napi::Function MT4UsersWrap::Init(Napi::Env env)
{
    Napi::Function func = DefineClass(
        env,
        "MT4Users",
        {
            InstanceMethod("get", &MT4UsersWrap::Get),
        });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    return func;
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

    if (info.Length() < 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected (login: number)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    const int login = info[0].As<Napi::Number>().Int32Value();
    std::cerr << "[mt4][users] Get start login=" << login << std::endl;
    try
    {
        const MT4UserRecord user = users_->Get(login);
        return ToNapiUser(env, user);
    }
    catch (const std::exception &ex)
    {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}