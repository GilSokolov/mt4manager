#pragma once

#include <napi.h>
#include "trade_request.h"

TradeRequest FromNapiTrade(const Napi::Value &value);