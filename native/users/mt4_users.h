#pragma once

#include <memory>
#include <string>
#include <functional>
#include "../include/mt4_sdk.h"

class MT4Client;

class MT4Users
{
public:
    using UpdateHandler = std::function<void(const UserRecord *)>;
    explicit MT4Users(const std::shared_ptr<MT4Client> &client);

    UserRecord Get(int login) const;

    void SetUpdateHandler(UpdateHandler handler);

private:
    void HandleEvent(int code, int type, void *data, void *param);

private:
    std::shared_ptr<MT4Client> client_;
    UpdateHandler update_handler_;
};