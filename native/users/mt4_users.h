#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <string>

#include "../include/mt4_sdk.h"

#include "../MT4PumpSubscriber.h"

using UserHandler = std::function<void(const UserRecord *, int)>;

class MT4Client;

class MT4Users : public MT4PumpSubscriber<MT4Users, UserHandler>
{
    friend class MT4PumpSubscriber<MT4Users, UserHandler>;

public:
    ~MT4Users();

    UserRecord Get(int login) const;

    int Create(UserRecord &user) const;
    void Update(const UserRecord &user) const;
    void HandleTradeUpdate(int login);

private:
    explicit MT4Users(const std::shared_ptr<MT4Client> &client);

    void HandleEvent(int code, int type, void *data);
    void HandleUserUpdate(int type, void *data);
};