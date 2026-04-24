#include "mt4_manager_wrap.h"

#include <string>

#include "async_job.h"

#include "pumping_options_napi.h"

#include "mt4_client_config.h"

#include "./utils/napi_utils.h"

#include "./utils/async_utils.h"

#include "users/mt4_users_wrap.h"

Napi::FunctionReference MT4ManagerWrap::constructor;

// Register MT4Manager class and export it to JS.
Napi::Object MT4ManagerWrap::Init(Napi::Env env, Napi::Object exports)
{
  // Ensure dependent modules are initialized first.
  MT4UsersWrap::Init(env);

  // Define JS class and bind native methods.
  Napi::Function klass = DefineClass(
      env,
      "MT4Manager",
      {
          InstanceMethod("connect", &MT4ManagerWrap::Connect),
          InstanceMethod("login", &MT4ManagerWrap::Login),
          InstanceMethod("disconnect", &MT4ManagerWrap::Disconnect),
          InstanceMethod("close", &MT4ManagerWrap::Close),
          InstanceMethod("startPumping", &MT4ManagerWrap::StartPumping),
          InstanceMethod("stopPumping", &MT4ManagerWrap::StopPumping),
      });

  // Keep constructor alive for future instantiation.
  constructor = Napi::Persistent(klass);

  // Export to JS module.
  exports.Set("MT4Manager", klass);
  return exports;
}

MT4ManagerWrap::MT4ManagerWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<MT4ManagerWrap>(info)
{
  Napi::Env env = info.Env();

  try
  {
    // Extract the DLL path from JS constructor arguments.
    // Expected usage from JS: new MT4Manager(dllPath)
    Napi::Object configObj = napi_utils::GetObject(info, 0, "config");
    MT4ClientConfig config = FromNapiClientConfig(env, configObj);

    // Create the core MT4 client.
    // Stored as shared_ptr so it can be safely shared across submodules (e.g. users).
    client_ = std::make_shared<MT4Client>(config);

    // Create the "users" wrapper object and inject the same client instance.
    // This ensures all modules operate on the same underlying MT4 connection.
    Napi::Object users = MT4UsersWrap::NewInstance(env, client_);

    // Hold a persistent reference to prevent the JS "users" object from being
    // garbage collected while this manager is still alive.
    users_ = Napi::Persistent(users);

    // Attach "users" to the JS object so it is accessible as:
    // const manager = new MT4Manager(...);
    // manager.users
    Value().Set("users", users);
  }
  catch (const std::exception &ex)
  {
    napi_utils::ThrowError(env, ex);
  }
}

MT4ManagerWrap::~MT4ManagerWrap()
{
  if (client_)
  {
    client_->Close();
  }
}

Napi::Value MT4ManagerWrap::Connect(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  try
  {
    const std::string server = napi_utils::GetString(info, 0, "server");

    auto task = [client = client_, server]()
    {
      client->Connect(server);
    };

    return async_utils::QueuePromise(env, "MT4Manager::connect", task);
  }
  catch (const std::exception &ex)
  {
    return napi_utils::ThrowError(env, ex);
  }
}

Napi::Value MT4ManagerWrap::Login(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  try
  {
    const int login = napi_utils::GetInt32(info, 0, "login");
    const std::string password = napi_utils::GetString(info, 1, "password");

    auto task = [client = client_, login, password]()
    {
      client->Login(login, password);
    };

    return async_utils::QueuePromise(env, "MT4Manager::login", task);
  }
  catch (const std::exception &ex)
  {
    return napi_utils::ThrowError(env, ex);
  }
}

Napi::Value MT4ManagerWrap::Disconnect(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  try
  {
    auto task = [client = client_]()
    {
      client->Disconnect();
    };

    return async_utils::QueuePromise(env, "MT4Manager::disconnect", task);
  }
  catch (const std::exception &ex)
  {
    return napi_utils::ThrowError(env, ex);
  }
}

Napi::Value MT4ManagerWrap::Close(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  try
  {
    auto task = [client = client_]()
    {
      client->Close();
    };

    return async_utils::QueuePromise(env, "MT4Manager::close", task);
  }
  catch (const std::exception &ex)
  {
    return napi_utils::ThrowError(env, ex);
  }
}

Napi::Value MT4ManagerWrap::StartPumping(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  PumpingOptions options{};
  if (info.Length() > 0)
  {
    options = ParsePumpingOptions(info[0]);
  }

  auto task = [client = client_, options]()
  {
    client->StartPumping(options);
  };

  return async_utils::QueuePromise(env, "MT4Manager::StartPumping", task);
}

Napi::Value MT4ManagerWrap::StopPumping(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  auto task = [client = client_]()
  {
    client->StopPumping();
  };

  return async_utils::QueuePromise(env, "MT4Manager::StopPumping", task);
}

// Napi::Value MT4ManagerWrap::IsPumping(const Napi::CallbackInfo &info)
// {
//   Napi::Env env = info.Env();

//   try
//   {
//     client_->IsPumping();
//     return env.Undefined();
//   }
//   catch (const std::exception &ex)
//   {
//     Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
//     return env.Null();
//   }
// }