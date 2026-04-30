// trade_to_mt4.h
#pragma once

#include "../include/mt4_sdk.h"
#include "trade_types.h"

TradeTransInfo BuildTradeTransInfo(const TradePayload &p);
int ToOpenTransactionType(const std::string &execution);