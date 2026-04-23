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
            InstanceMethod("create", &MT4UsersWrap::Create),
            InstanceMethod("update", &MT4UsersWrap::Update),
            InstanceMethod("_setUpdateHandler", &MT4UsersWrap::SetUpdateHandler),
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
    wrap->users_ = MT4Users::CreateShared(client);

    return scope.Escape(napi_value(obj)).ToObject();
}

MT4UsersWrap::MT4UsersWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<MT4UsersWrap>(info)
{
}

MT4UsersWrap::~MT4UsersWrap()
{
    if (update_tsfn_)
    {
        update_tsfn_.Release();
    }
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

Napi::Value MT4UsersWrap::Create(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsObject())
    {
        Napi::TypeError::New(env, "Expected (user: object)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    try
    {
        UserRecord user = FromNapiUser(env, info[0].As<Napi::Object>());
        const int login = users_->Create(user);

        Napi::Number result = Napi::Number::New(env, login);

        return result;
    }
    catch (const std::exception &ex)
    {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MT4UsersWrap::Update(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsObject())
    {
        Napi::TypeError::New(env, "Expected (user: object)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    try
    {
        UserRecord user = FromNapiUser(env, info[0].As<Napi::Object>());

        if (user.login <= 0)
        {
            Napi::TypeError::New(env, "Expected user.login for update")
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        users_->Update(user);
        return env.Undefined();
    }
    catch (const std::exception &ex)
    {
        Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value MT4UsersWrap::SetUpdateHandler(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsFunction())
    {
        Napi::TypeError::New(env, "Expected (handler: function)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (update_tsfn_)
    {
        update_tsfn_.Release();
        update_tsfn_ = Napi::ThreadSafeFunction();
    }

    Napi::Function jsHandler = info[0].As<Napi::Function>();

    update_tsfn_ = Napi::ThreadSafeFunction::New(
        env,
        jsHandler,
        "MT4UsersUpdateHandler",
        0,
        1);

    users_->SetUpdateHandler([this](const UserRecord *user)
                             {
        if (!update_tsfn_)
        {
            return;
        }

        auto *payload = new UserRecord(*user);

        napi_status status = update_tsfn_.BlockingCall(
            payload,
            [](Napi::Env env, Napi::Function jsCallback, UserRecord *payload)
            {
                Napi::Object obj = ToNapiUser(env, *payload);
                jsCallback.Call({obj});
                delete payload;
            });

        if (status != napi_ok)
        {
            delete payload;
        } });

    return env.Undefined();
}