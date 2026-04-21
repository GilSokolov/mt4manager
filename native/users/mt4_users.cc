#include <stdexcept>
#include <cstring>
#include <iostream>

#include "./mt4_users.h"
#include "../mt4_client.h"
#include "../include/mt4_sdk.h"
#include "../utils/mt4_errors.h"

MT4Users::MT4Users(const std::shared_ptr<MT4Client> &client)
    : client_(client)
{

    client_->AddPumpListener([this](int code, int type, void *data, void *param)
                             { HandleEvent(code, type, data, param); });
}

void MT4Users::HandleEvent(int code, int type, void *data, void *param)
{
    if (code != PUMP_UPDATE_USERS || !data)
    {
        return;
    }

    const UserRecord *user = static_cast<const UserRecord *>(data);

    if (update_handler_)
    {
        update_handler_(user);
    }
}

void MT4Users::SetUpdateHandler(UpdateHandler handler)
{
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
