#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <string>

#include "../include/mt4_sdk.h"

class MT4Client;

class MT4Users : public std::enable_shared_from_this<MT4Users>
{
public:
    using UpdateHandler = std::function<void(const UserRecord *)>;

    static std::shared_ptr<MT4Users> CreateShared(const std::shared_ptr<MT4Client> &client);

    UserRecord Get(int login) const;

    void SetUpdateHandler(UpdateHandler handler);

    int Create(UserRecord &user) const;
    void Update(const UserRecord &user) const;

private:
    explicit MT4Users(const std::shared_ptr<MT4Client> &client);
    void AttachPumpListener();
    void HandleEvent(int code, int type, void *data);

private:
    std::shared_ptr<MT4Client> client_;
    UpdateHandler update_handler_;
    mutable std::mutex update_handler_mutex_;
};