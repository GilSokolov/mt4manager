#pragma once

#include <napi.h>
#include "trade_types.h"

TradePayload FromNapiTrade(const Napi::Value &value);