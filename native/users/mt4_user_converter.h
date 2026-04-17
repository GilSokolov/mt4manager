#pragma once

#include <napi.h>

#include "users/mt4_users.h"

Napi::Object ToNapiUser(Napi::Env env, const MT4UserRecord &user);