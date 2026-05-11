#include "mt4_users_wrap.h"

#include <iostream>

#include "../mt4_client.h"
#include "../utils/env_registry.h"
#include "../utils/napi_utils.h"
#include "mt4_users.h"

// Persistent reference to the JS constructor function for MT4Users.
// Stored so we can create new instances from C++ (see NewInstance).

Napi::Function MT4UsersWrap::Init(Napi::Env env) {
  // Define the JS class "MT4Users" and bind its methods to C++ implementations.
  Napi::Function klass = DefineClass(
      env, "MT4Users",
      {
          InstanceMethod("get", &MT4UsersWrap::Get),
          InstanceMethod("getMarginLevel", &MT4UsersWrap::GetMargin),
          InstanceMethod("create", &MT4UsersWrap::Create),
          InstanceMethod("update", &MT4UsersWrap::Update),
          InstanceMethod("onEvent", &MT4UsersWrap::SetHandler),
          InstanceMethod("handleTradeUpdate", &MT4UsersWrap::HandleTradeUpdate),
      });

  // Store constructor in a persistent reference so it can be reused
  // later to instantiate objects from native code.

  EnvRegistry::Get(env).mt4Users = Napi::Persistent(klass);

  return klass;
}

Napi::Object MT4UsersWrap::NewInstance(
    Napi::Env env, const std::shared_ptr<MT4Client>& client) {
  // --- Why EscapableHandleScope? ---
  // In N-API, every JS object created in C++ is tracked by a "handle scope" —
  // think of it like a temporary stack frame for JS objects. When the scope
  // closes (end of function), any JS objects created inside it become eligible
  // for garbage collection.
  //
  // EscapableHandleScope is a special variant that lets you "escape" exactly
  // one object out before the scope closes — keeping it alive for the caller.
  // Without this, returning `obj` below would be unsafe.
  Napi::EscapableHandleScope scope(env);

  // --- Why construct with New({}) instead of passing `client` directly? ---
  // We can't pass a C++ shared_ptr through the JS constructor boundary —
  // JS only understands JS values. So we create a bare JS object first
  // (equivalent to `new MT4Users()` in JS with no arguments), then manually
  // inject the C++ dependencies below via Unwrap().
  Napi::Object obj = EnvRegistry::Get(env).mt4Users.New({});

  // --- What is Unwrap()? ---
  // When N-API creates a JS object from an ObjectWrap class, it secretly
  // stores a pointer to the paired C++ instance inside the JS object.
  // Unwrap() retrieves that hidden pointer so we can access the C++ fields.
  // At this point `wrap` IS the C++ object behind `obj` — same instance,
  // just accessed from the C++ side.
  MT4UsersWrap* wrap = Napi::ObjectWrap<MT4UsersWrap>::Unwrap(obj);

  // Inject the shared MT4 client connection into the C++ instance.
  // shared_ptr means MT4Client stays alive as long as either the manager
  // or any sub-module (users/trades/symbols) still holds a reference.
  wrap->users_ = MT4Users::CreateShared(client);

  // Set up the event bridge that forwards native C++ events (e.g. user updates
  // coming from the MT4 server on a background thread) to a JS callback safely.
  // Regular function calls aren't thread-safe across the JS/C++ boundary —
  // this bridge handles the synchronization internally.
  wrap->bridge_ =
      std::make_shared<JsCallbackBridge<UserPayload>>("MT4UsersUpdateHandler");

  // --- Why scope.Escape()? ---
  // Promotes `obj` out of the local handle scope so it survives after this
  // function returns. The cast to napi_value is needed because Escape() accepts
  // the raw base handle type, not the typed Napi::Object wrapper.
  // Without this line, `obj` could be garbage collected the moment we return.
  return scope.Escape(napi_value(obj)).ToObject();
}

MT4UsersWrap::MT4UsersWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<MT4UsersWrap>(info) {}

MT4UsersWrap::~MT4UsersWrap() {
  if (users_) {
    users_->SetHandler(nullptr);
  }
}

Napi::Value MT4UsersWrap::Get(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  try {
    const int login = napi_utils::GetInt32(info, 0, "login");
    const UserRecord user = users_->Get(login);
    return ToNapiUser(env, user);
  } catch (const MT4Error& ex) {
    return napi_utils::ThrowError(env, ex);
  } catch (const std::exception& ex) {
    return napi_utils::ThrowError(env, ex);
  }
}

Napi::Value MT4UsersWrap::GetMargin(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  try {
    const int login = napi_utils::GetInt32(info, 0, "login");
    const MarginLevel margin = users_->GetMargin(login);
    return ToNapiMarginLevel(env, margin);
  } catch (const MT4Error& ex) {
    return napi_utils::ThrowError(env, ex);
  } catch (const std::exception& ex) {
    return napi_utils::ThrowError(env, ex);
  }
}

Napi::Value MT4UsersWrap::Create(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  try {
    Napi::Object userObject = napi_utils::GetObject(info, 0, "user");

    UserRecord data = FromNapiUser(env, userObject);

    const int login = users_->Create(data);

    UserRecord user = users_->Get(login);

    return ToNapiUser(env, user);
  } catch (const std::exception& ex) {
    return napi_utils::ThrowError(env, ex);
  }
}

Napi::Value MT4UsersWrap::Update(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  try {
    const int login = napi_utils::GetInt32(info, 0, "login");
    Napi::Object patch = napi_utils::GetObject(info, 1, "data");

    UserRecord user = users_->Get(login);

    ApplyNapiUserPatch(env, patch, user);

    user.login = login;

    users_->Update(user);

    return ToNapiUser(env, user);
  } catch (const std::exception& ex) {
    return napi_utils::ThrowError(env, ex);
  }
}

Napi::Value MT4UsersWrap::SetHandler(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  try {
    Napi::Function handler = napi_utils::GetFunction(info, 0, "handler");

    bridge_->SetHandler(env, handler);

    users_->SetHandler([bridge = bridge_](const UserRecord* user, int type)
    {
      bridge->CallJs(UserPayload{*user, type}, BuildUserArgs);
    });

    return env.Undefined();
  } catch (const std::exception& ex) {
    return napi_utils::ThrowError(env, ex);
  }
}

Napi::Value MT4UsersWrap::HandleTradeUpdate(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  try {
    const int login = napi_utils::GetInt32(info, 0, "login");
    users_->HandleTradeUpdate(login);

    return env.Undefined();
  } catch (const MT4Error& ex) {
    return napi_utils::ThrowError(env, ex);
  } catch (const std::exception& ex) {
    return napi_utils::ThrowError(env, ex);
  }
}