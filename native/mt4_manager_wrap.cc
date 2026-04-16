#include "mt4_manager_wrap.h"

#include <string>

#include "async_job.h"

#include "./utils/napi_utils.h"

#include <iostream>

Napi::FunctionReference MT4ManagerWrap::constructor;

namespace
{
  std::string Utf8ToWide(const std::string &value)
  {
    return std::string(value.begin(), value.end());
  }
} // namespace

Napi::Object MT4ManagerWrap::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function func = DefineClass(
      env,
      "MT4Manager",
      {
          InstanceMethod("connect", &MT4ManagerWrap::Connect),
          InstanceMethod("login", &MT4ManagerWrap::Login),
          InstanceMethod("disconnect", &MT4ManagerWrap::Disconnect),
          InstanceMethod("close", &MT4ManagerWrap::Close),
      });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("MT4Manager", func);
  return exports;
}

MT4ManagerWrap::MT4ManagerWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<MT4ManagerWrap>(info)
{
  std::cerr << "[mt4] wrap ctor start" << std::endl;

  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsString())
  {
    Napi::TypeError::New(env, "Expected (dllPath: string)")
        .ThrowAsJavaScriptException();
    return;
  }

  const std::string dllPathUtf8 = info[0].As<Napi::String>().Utf8Value();
  std::cerr << "[mt4] got dll path string" << std::endl;

  try
  {
    std::string dllPath(dllPathUtf8.begin(), dllPathUtf8.end());
    client_ = std::make_shared<MT4Client>(dllPath);
    std::cerr << "[mt4] client created" << std::endl;
  }
  catch (const std::exception &ex)
  {
    std::cerr << "[mt4] wrap ctor exception: " << ex.what() << std::endl;
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
  if (info.Length() < 1 || !info[0].IsString())
  {
    throw Napi::TypeError::New(env, "server string is required");
  }

  std::string server = info[0].As<Napi::String>().Utf8Value();
  auto deferred = Napi::Promise::Deferred::New(env);
  auto client = client_;

  auto *job = new AsyncJob(
      env,
      "MT4Manager::connect",
      [client, server]()
      { client->Connect(server); },
      deferred);
  job->Queue();

  return deferred.Promise();
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

  auto deferred = Napi::Promise::Deferred::New(env);
  auto client = client_;

  auto *job = new AsyncJob(
      env,
      "MT4Manager::login",
      [client, login, password]()
      { client->Login(login, password); },
      deferred);
  job->Queue();

  return deferred.Promise();
}

Napi::Value MT4ManagerWrap::Disconnect(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  auto deferred = Napi::Promise::Deferred::New(env);
  auto client = client_;

  auto *job = new AsyncJob(
      env,
      "MT4Manager::disconnect",
      [client]()
      { client->Disconnect(); },
      deferred);
  job->Queue();

  return deferred.Promise();
}

Napi::Value MT4ManagerWrap::Close(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  auto deferred = Napi::Promise::Deferred::New(env);
  auto client = client_;

  auto *job = new AsyncJob(
      env,
      "MT4Manager::close",
      [client]()
      { client->Close(); },
      deferred);
  job->Queue();

  return deferred.Promise();
}
