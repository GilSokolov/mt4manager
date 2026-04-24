#include <iostream>

#include "mt4_users.h"
#include "mt4_users_wrap.h"

#include "../mt4_client.h"
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
    wrap->update_bridge_ = std::make_shared<JsCallbackBridge<UserPayload>>("MT4UsersUpdateHandler");

    return scope.Escape(napi_value(obj)).ToObject();
}

MT4UsersWrap::MT4UsersWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<MT4UsersWrap>(info)
{
}

MT4UsersWrap::~MT4UsersWrap()
{

    if (users_)
    {
        users_->SetUpdateHandler(nullptr);
    }
}

Napi::Value MT4UsersWrap::Get(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        const int login = napi_utils::GetInt32(info, 0, "login");
        const UserRecord user = users_->Get(login);
        return ToNapiUser(env, user);
    }
    catch (const std::exception &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
}

Napi::Value MT4UsersWrap::Create(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        Napi::Object userObject = napi_utils::GetObject(info, 0, "user");

        UserRecord user = FromNapiUser(env, userObject);
        const int login = users_->Create(user);

        return Napi::Number::New(env, login);
    }
    catch (const std::exception &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
}

Napi::Value MT4UsersWrap::Update(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        Napi::Object userObject = napi_utils::GetObject(info, 0, "user");
        UserRecord user = FromNapiUser(env, userObject);

        if (user.login <= 0)
        {
            return napi_utils::ThrowError(env, "Expected user.login for update");
        }

        users_->Update(user);
        return env.Undefined();
    }
    catch (const std::exception &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
}

Napi::Value MT4UsersWrap::SetUpdateHandler(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        Napi::Function handler = napi_utils::GetFunction(info, 0, "handler");

        update_bridge_->SetHandler(env, handler);

        users_->SetUpdateHandler(
            [bridge = update_bridge_](const UserRecord *user)
            {
                bridge->CallJs(UserPayload{*user}, BuildUserArgs);
            });

        return env.Undefined();
    }
    catch (const std::exception &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
}
