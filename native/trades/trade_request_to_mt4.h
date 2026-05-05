#pragma once

#include "../include/mt4_sdk.h"
#include "trade_request.h"

TradeTransInfo ToMt4TradeTransInfo(const TradeRequest &request);
int ToMt4TransactionType(const std::string &mode);