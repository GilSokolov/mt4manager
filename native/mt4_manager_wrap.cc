#include "mt4_manager_wrap.h"

#include <string>

#include "async_job.h"

#include "./utils/napi_utils.h"

#include "./utils/async_utils.h"

#include "users/mt4_users_wrap.h"

Napi::FunctionReference MT4ManagerWrap::constructor;

Napi::Object MT4ManagerWrap::Init(Napi::Env env, Napi::Object exports)
{
  MT4UsersWrap::Init(env);

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

  constructor = Napi::Persistent(klass);
  constructor.SuppressDestruct();

  exports.Set("MT4Manager", klass);
  return exports;
}

MT4ManagerWrap::MT4ManagerWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<MT4ManagerWrap>(info)
{
  Napi::Env env = info.Env();

  const std::string dllPath = napi_utils::GetString(info, 0, "dllPath");

  if (napi_utils::HasPendingException(env))
  {
    return;
  }

  try
  {
    client_ = std::make_shared<MT4Client>(dllPath);
    Napi::Object users = MT4UsersWrap::NewInstance(env, client_);
    users_ = Napi::Persistent(users);
    users_.SuppressDestruct();
    Value().Set("users", users);
  }
  catch (const std::exception &ex)
  {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
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

  const std::string server = napi_utils::GetString(info, 0, "server");

  if (napi_utils::HasPendingException(env))
  {
    return env.Null();
  }

  std::shared_ptr<MT4Client> client = client_;

  auto task = [client, server]()
  {
    client->Connect(server);
  };

  return async_utils::QueuePromise(env, "MT4Manager::connect", task);
}

Napi::Value MT4ManagerWrap::Login(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  const int login = napi_utils::GetInt32(info, 0, "login");
  const std::string password = napi_utils::GetString(info, 1, "password");

  if (napi_utils::HasPendingException(env))
  {
    return env.Null();
  }

  std::shared_ptr<MT4Client> client = client_;

  auto task = [client, login, password]()
  {
    client->Login(login, password);
  };

  return async_utils::QueuePromise(env, "MT4Manager::login", task);
}

Napi::Value MT4ManagerWrap::Disconnect(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  std::shared_ptr<MT4Client> client = client_;

  auto task = [client]()
  {
    client->Disconnect();
  };

  return async_utils::QueuePromise(env, "MT4Manager::disconnect", task);
}

Napi::Value MT4ManagerWrap::Close(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  std::shared_ptr<MT4Client> client = client_;

  auto task = [client]()
  {
    client->Close();
  };

  return async_utils::QueuePromise(env, "MT4Manager::close", task);
}

Napi::Value MT4ManagerWrap::StartPumping(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  std::shared_ptr<MT4Client> client = client_;

  auto task = [client]()
  {
    client->StartPumping();
  };

  return async_utils::QueuePromise(env, "MT4Manager::StartPumping", task);
}

Napi::Value MT4ManagerWrap::StopPumping(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  try
  {
    client_->StopPumping();
    return env.Undefined();
  }
  catch (const std::exception &ex)
  {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}