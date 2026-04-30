#pragma once

#include <mutex>
#include <memory>
#include <string>
#include <functional>

#include "../include/mt4_sdk.h"
#include "../trades/trade_types.h"

class MT4Client;

class MT4Positions
{
public:
    static std::shared_ptr<MT4Positions> CreateShared(const std::shared_ptr<MT4Client> &client);

    ~MT4Positions();

    TradeRecord Get(int login) const;
    TradeRecord Create(const TradePayload &payload);

private:
    explicit MT4Positions(const std::shared_ptr<MT4Client> &client);

    std::shared_ptr<MT4Client> client_;
};