#pragma once

#include <memory>
#include <string>
#include "../include/mt4_sdk.h"
#include "../utils/subscription_set.h"

class MT4Client;

class MT4Users
{
public:
    explicit MT4Users(const std::shared_ptr<MT4Client> &client);

    UserRecord Get(int login) const;

    void Subscribe(int login);
    void Unsubscribe(int login);
    void UnsubscribeAll();

    bool IsSubscribed(int login) const;

private:
    std::shared_ptr<MT4Client> client_;
    SubscriptionSet<int> subscriptions_;
};