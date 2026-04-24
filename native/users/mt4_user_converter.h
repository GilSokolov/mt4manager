#pragma once

#include <napi.h>

#include "../include/mt4_sdk.h"

Napi::Object ToNapiUser(Napi::Env env, const UserRecord &user);

UserRecord FromNapiUser(Napi::Env env, const Napi::Object &obj);

void ApplyNapiUserPatch(
    Napi::Env env,
    const Napi::Object &obj,
    UserRecord &user);

struct UserPayload
{
    UserRecord user;
};

std::vector<napi_value> BuildUserArgs(
    Napi::Env env,
    const UserPayload &payload);