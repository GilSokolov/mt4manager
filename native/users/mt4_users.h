#pragma once

#include <memory>
#include <string>
#include "../include/mt4_sdk.h"

class MT4Client;

class MT4Users
{
public:
    explicit MT4Users(const std::shared_ptr<MT4Client> &client);

    UserRecord Get(int login) const;

private:
    std::shared_ptr<MT4Client> client_;
};