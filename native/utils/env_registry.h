

#pragma once
#include <napi.h>

// One struct holds all per-environment constructor references.
// Stored once via env.SetInstanceData and retrieved anywhere via
// GetRegistry(env).
struct EnvRegistry {
  Napi::FunctionReference mt4Users;
  Napi::FunctionReference mt4Trades;
  Napi::FunctionReference mt4Symbols;

  static EnvRegistry& Get(Napi::Env env) {
    auto* reg = env.GetInstanceData<EnvRegistry>();
    if (!reg) {
      reg = new EnvRegistry();
      env.SetInstanceData<EnvRegistry>(reg);
    }
    return *reg;
  }
};