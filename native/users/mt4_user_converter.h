#pragma once

#include <napi.h>

#include "../include/mt4_sdk.h"

Napi::Object ToNapiUser(Napi::Env env, const UserRecord &user);

UserRecord FromNapiUser(Napi::Env env, const Napi::Object &obj);

Napi::Object ToNapiMarginLevel(Napi::Env env, const MarginLevel &margin);

void ApplyNapiUserPatch(
    Napi::Env env,
    const Napi::Object &obj,
    UserRecord &user);

struct UserPayload
{
    UserRecord user;
    int type;
};

std::vector<napi_value> BuildUserArgs(
    Napi::Env env,
    const UserPayload &payload);