#include <iostream>

#include "mt4_users.h"
#include "mt4_users_wrap.h"

#include "./mt4_user_converter.h"

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

    if (update_tsfn_)
    {
        update_tsfn_.Release();
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
    // Get the N-API environment (needed for all JS interactions)
    Napi::Env env = info.Env();

    // Validate input: expect a single argument which must be a function
    if (info.Length() < 1 || !info[0].IsFunction())
    {
        Napi::TypeError::New(env, "Expected (handler: function)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    // If a previous ThreadSafeFunction exists, release it to avoid leaks
    if (update_tsfn_)
    {
        update_tsfn_.Release();
        update_tsfn_ = Napi::ThreadSafeFunction(); // reset to empty
    }

    // Extract the JS callback function
    Napi::Function jsHandler = info[0].As<Napi::Function>();

    // Create a new ThreadSafeFunction:
    // - allows calling JS from a non-JS thread (e.g., MT4 callback thread)
    // - queue size = 0 (unlimited)
    // - max threads = 1 (only one thread will use it)
    update_tsfn_ = Napi::ThreadSafeFunction::New(
        env,
        jsHandler,
        "MT4UsersUpdateHandler", // resource name (for debugging)
        0,
        1);

    // Register native update handler in MT4Users
    // This lambda will be called from a native thread when a user update occurs
    users_->SetUpdateHandler(
        [this](const UserRecord *user)
        {
            // Safety check:
            // - ensure TSFN is still valid
            // - ensure user pointer is not null
            if (!update_tsfn_ || !user)
            {
                return;
            }

            // Copy the user data because the original pointer may not be valid later
            // This payload will be passed to the JS thread
            auto *payload = new UserRecord(*user);

            // Call into JS thread safely using ThreadSafeFunction
            napi_status status = update_tsfn_.BlockingCall(
                payload,
                [](Napi::Env env, Napi::Function jsCallback, UserRecord *payload)
                {
                    // Use RAII to ensure payload is always deleted (even on exceptions)
                    std::unique_ptr<UserRecord> guard(payload);

                    // Convert native UserRecord to JS object
                    Napi::Object obj = ToNapiUser(env, *guard);

                    // Call the original JS handler with the user object
                    jsCallback.Call({obj});
                });

            // If the call failed, the callback above will NOT run,
            // so we must manually free the payload to avoid a memory leak
            if (status != napi_ok)
            {
                delete payload;
            }
        });

    // Return undefined to JS (no value expected)
    return env.Undefined();
}