#pragma once

#include <string>

struct TradePayload
{
    int id = 0;

    int login;

    std::string symbol;
    int volume;

    int cmd; // OP_BUY, etc.

    std::string mode;

    double price = 0;
    double sl = 0;
    double tp = 0;

    int deviation = 0;

    std::string comment;

    int expiration = 0; // unix time

    int flags = 0;
};