#pragma once

#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "../include/mt4_sdk.h"

#include "../MT4PumpSubscriber.h"

using TickHandler = std::function<void(const SymbolInfo *)>;

class MT4Client;

class MT4Symbols : public MT4PumpSubscriber<MT4Symbols, TickHandler>
{
    friend class MT4PumpSubscriber<MT4Symbols, TickHandler>;

public:
    ~MT4Symbols();

    SymbolInfo Get(const std::string &symbol) const;
    std::vector<ConSymbol> GetAll() const;
    void Subscribe(const std::string &symbol) const;
    void Unsubscribe(const std::string &symbol) const;

private:
    explicit MT4Symbols(const std::shared_ptr<MT4Client> &client);

    void HandleEvent(int code, int type, void *data);
};