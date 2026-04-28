// native/symbols/mt4_symbols.h
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../include/mt4_sdk.h"

class MT4Client;

class MT4Symbols : public std::enable_shared_from_this<MT4Symbols>
{
public:
    static std::shared_ptr<MT4Symbols> CreateShared(const std::shared_ptr<MT4Client> &client);

    SymbolInfo Get(const std::string &symbol) const;
    std::vector<ConSymbol> GetAll() const;

private:
    explicit MT4Symbols(const std::shared_ptr<MT4Client> &client);

private:
    std::shared_ptr<MT4Client> client_;
};