#pragma once

#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "../include/mt4_sdk.h"

class MT4Client;

class MT4Symbols : public std::enable_shared_from_this<MT4Symbols>
{
public:
    ~MT4Symbols();
    using TickHandler = std::function<void(const SymbolInfo *)>;

    static std::shared_ptr<MT4Symbols> CreateShared(const std::shared_ptr<MT4Client> &client);

    SymbolInfo Get(const std::string &symbol) const;
    std::vector<ConSymbol> GetAll() const;
    void Subscribe(const std::string &symbol) const;
    void Unsubscribe(const std::string &symbol) const;
    void SetTickHandler(TickHandler handler);

private:
    explicit MT4Symbols(const std::shared_ptr<MT4Client> &client);

    void HandleEvent(int code, int type, void *data);

    void AttachPumpListener();
    void DetachPumpListener();

private:
    std::shared_ptr<MT4Client> client_;

    TickHandler tick_handler_;
    mutable std::mutex tick_handler_mutex_;
    int pump_listener_id_ = -1;
};