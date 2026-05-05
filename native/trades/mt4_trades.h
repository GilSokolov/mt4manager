#pragma once

#include <mutex>
#include <memory>
#include <string>
#include <functional>

#include "trade_request.h"

#include "../include/mt4_sdk.h"

#include "../MT4PumpSubscriber.h"

using TradeHandler = std::function<void(const TradeRecord *)>;

class MT4Client;

class MT4Trades : public MT4PumpSubscriber<MT4Trades, TradeHandler>
{
    friend class MT4PumpSubscriber<MT4Trades, TradeHandler>;

public:
    ~MT4Trades();

    TradeRecord Get(int login) const;
    int Execute(const TradeRequest &payload);
    void HandleBidAskUpdate(const std::string &symbol);

private:
    explicit MT4Trades(const std::shared_ptr<MT4Client> &client);

    void HandleEvent(int code, int type, void *data);

    void HandleTradeUpdate(int type, void *data);
};