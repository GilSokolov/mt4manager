#pragma once

#include <napi.h>

#include "../include/mt4_sdk.h"

Napi::Object ToNapiPosition(Napi::Env env, const TradeRecord &t);