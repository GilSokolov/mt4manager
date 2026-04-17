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
    std::cerr << "[mt4][users] UsersRequest total=" << total << std::endl;
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
