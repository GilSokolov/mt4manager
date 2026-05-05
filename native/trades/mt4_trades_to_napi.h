#pragma once

#include <napi.h>

#include "trade_payload.h"

#include "../include/mt4_sdk.h"

std::vector<napi_value> BuildTradeArgs(
    Napi::Env env,
    const TradePayload &payload);

Napi::Object ToNapiTrade(Napi::Env env, const TradeRecord &t);