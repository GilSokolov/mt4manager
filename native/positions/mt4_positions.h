#pragma once

#include <mutex>
#include <memory>
#include <string>
#include <functional>

#include "../include/mt4_sdk.h"
#include "../trades/trade_types.h"

#include "../MT4PumpSubscriber.h"

using TradeHandler = std::function<void(const TradeRecord *)>;

class MT4Client;

class MT4Positions : public MT4PumpSubscriber<MT4Positions, TradeHandler>
{
    friend class MT4PumpSubscriber<MT4Positions, TradeHandler>;

public:
    ~MT4Positions();

    TradeRecord Get(int login) const;
    TradeRecord Create(const TradePayload &payload);

private:
    explicit MT4Positions(const std::shared_ptr<MT4Client> &client);

    void HandleEvent(int code, int type, void *data);
};