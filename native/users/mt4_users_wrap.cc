#include "users/mt4_users_wrap.h"

#include <memory>

#include "../mt4_client.h"

Napi::FunctionReference MT4UsersWrap::constructor;

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
    wrap->client_ = client;

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

    // Simple stub first, just to make the native shape work end-to-end.
    // Replace this later with real MT4 SDK user lookup.
    Napi::Object user = Napi::Object::New(env);
    user.Set("login", Napi::Number::New(env, login));
    user.Set("group", Napi::String::New(env, "demo\\group"));
    user.Set("name", Napi::String::New(env, "Test User"));
    user.Set("email", Napi::String::New(env, "test@example.com"));
    user.Set("leverage", Napi::Number::New(env, 100));

    return user;
}