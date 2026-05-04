#pragma once

#include <napi.h>

#include "../include/mt4_sdk.h"

struct TradeEventPayload
{
    TradeRecord trade;
};

std::vector<napi_value> BuildTradeArgs(
    Napi::Env env,
    const TradeEventPayload &payload);

Napi::Object ToNapiPosition(Napi::Env env, const TradeRecord &t);