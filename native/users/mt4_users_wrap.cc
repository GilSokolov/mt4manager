#include <iostream>

#include "mt4_users.h"
#include "mt4_users_wrap.h"

#include "../mt4_client.h"
#include "../utils/napi_utils.h"

// Persistent reference to the JS constructor function for MT4Users.
// Stored so we can create new instances from C++ (see NewInstance).
Napi::FunctionReference MT4UsersWrap::constructor;

Napi::Function MT4UsersWrap::Init(Napi::Env env)
{
    // Define the JS class "MT4Users" and bind its methods to C++ implementations.
    Napi::Function klass = DefineClass(
        env,
        "MT4Users",
        {
            InstanceMethod("get", &MT4UsersWrap::Get),
            InstanceMethod("create", &MT4UsersWrap::Create),
            InstanceMethod("update", &MT4UsersWrap::Update),
            InstanceMethod("_setUpdateHandler", &MT4UsersWrap::SetUpdateHandler),
        });

    // Store constructor in a persistent reference so it can be reused
    // later to instantiate objects from native code.
    constructor = Napi::Persistent(klass);

    return klass;
}

Napi::Object MT4UsersWrap::NewInstance(
    Napi::Env env,
    const std::shared_ptr<MT4Client> &client)
{
    // Handle scope that allows returning a safely escaped JS object.
    Napi::EscapableHandleScope scope(env);

    // Create a new JS instance: equivalent to `new MT4Users()`.
    Napi::Object obj = constructor.New({});

    // Unwrap the C++ instance associated with the JS object.
    MT4UsersWrap *wrap = Napi::ObjectWrap<MT4UsersWrap>::Unwrap(obj);

    // Initialize the underlying native users service.
    // Uses shared_ptr so it shares ownership of the MT4Client.
    wrap->users_ = MT4Users::CreateShared(client);

    // Create bridge responsible for forwarding native events (updates)
    // to a JS callback in a thread-safe way.
    wrap->update_bridge_ =
        std::make_shared<JsCallbackBridge<UserPayload>>("MT4UsersUpdateHandler");

    // Return the JS object, escaping it from the local handle scope.
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
    catch (const MT4Error &ex)
    {
        return napi_utils::ThrowError(env, ex);
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

        UserRecord data = FromNapiUser(env, userObject);

        const int login = users_->Create(data);

        UserRecord user = users_->Get(login);

        return ToNapiUser(env, user);
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
        const int login = napi_utils::GetInt32(info, 0, "login");
        Napi::Object patch = napi_utils::GetObject(info, 1, "data");

        UserRecord user = users_->Get(login);

        ApplyNapiUserPatch(env, patch, user);

        user.login = login;

        users_->Update(user);

        return ToNapiUser(env, user);
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
