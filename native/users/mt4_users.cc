#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "./mt4_users.h"
#include "../include/mt4_sdk.h"
#include "../mt4_client.h"
#include "../utils/mt4_errors.h"

MT4Users::MT4Users(const std::shared_ptr<MT4Client> &client)
    : client_(client)
{
}

std::shared_ptr<MT4Users> MT4Users::CreateShared(const std::shared_ptr<MT4Client> &client)
{
    auto users = std::shared_ptr<MT4Users>(new MT4Users(client));
    users->AttachPumpListener();
    return users;
}

void MT4Users::AttachPumpListener()
{
    std::weak_ptr<MT4Users> weak_self = shared_from_this();

    client_->AddPumpListener(
        [weak_self](int code, int type, void *data)
        {
            if (auto self = weak_self.lock())
            {
                self->HandleEvent(code, type, data);
            }
        });
}

void MT4Users::HandleEvent(int code, int type, void *data)
{
    if (code != PUMP_UPDATE_USERS || !data)
    {
        return;
    }

    const UserRecord *user = static_cast<const UserRecord *>(data);

    UpdateHandler handler;
    {
        std::lock_guard<std::mutex> lock(update_handler_mutex_);
        handler = update_handler_;
    }

    if (handler)
    {
        handler(user);
    }
}

void MT4Users::SetUpdateHandler(UpdateHandler handler)
{
    std::lock_guard<std::mutex> lock(update_handler_mutex_);
    update_handler_ = std::move(handler);
}

UserRecord MT4Users::Get(int login) const
{
    if (!client_)
    {
        throw std::runtime_error("MT4 client is not initialized");
    }

    CManagerInterface *manager = client_->Manager();
    if (!manager)
    {
        throw std::runtime_error("MT4 manager is not initialized");
    }

    const int logins[] = {login};
    int total = 1;

    UserRecord *records = manager->UserRecordsRequest(logins, &total);

    if (!records || total <= 0)
    {
        throw std::runtime_error("UserRecordsRequest returned no users");
    }

    const UserRecord *found = nullptr;
    for (int i = 0; i < total; ++i)
    {
        if (records[i].login == login)
        {
            found = &records[i];
            break;
        }
    }

    if (!found)
    {
        manager->MemFree(records);
        throw std::runtime_error("Requested user not found in UserRecordsRequest result");
    }

    UserRecord user = *found;
    manager->MemFree(records);
    return user;
}

int MT4Users::Create(UserRecord &user) const
{
    if (!client_)
    {
        throw std::runtime_error("MT4 client is not initialized");
    }

    CManagerInterface *manager = client_->Manager();
    if (!manager)
    {
        throw std::runtime_error("MT4 manager is not initialized");
    }

    ThrowMt4Error("UserRecordNew", manager->UserRecordNew(&user), manager);
    return user.login;
}

void MT4Users::Update(const UserRecord &user) const
{
    if (!client_)
    {
        throw std::runtime_error("MT4 client is not initialized");
    }

    CManagerInterface *manager = client_->Manager();
    if (!manager)
    {
        throw std::runtime_error("MT4 manager is not initialized");
    }

    ThrowMt4Error("UserRecordUpdate", manager->UserRecordUpdate(&user), manager);
}