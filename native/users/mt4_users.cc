#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "./mt4_users.h"
#include "../include/mt4_sdk.h"
#include "../mt4_client.h"
#include "../utils/mt4_errors.h"
#include "../utils/mt4_log.h"
#include "../utils/mt4_error_helpers.h"

MT4Users::MT4Users(const std::shared_ptr<MT4Client> &client)
    : MT4PumpSubscriber(client)
{
    MT4_DEBUG_LOG("MT4Users created");
}

MT4Users::~MT4Users()
{
    MT4_DEBUG_LOG("MT4Users destroyed");
    DetachPumpListener();
}

void MT4Users::HandleEvent(int code, int type, void *data)
{
    if (code != PUMP_UPDATE_USERS || !data)
    {
        return;
    }

    const UserRecord *user = static_cast<const UserRecord *>(data);

    MT4_DEBUG_LOG(
        "User pump update"
        << " login=" << user->login
        << " type=" << type);

    UserHandler handler = GetHandlerCopy();

    if (handler)
    {
        MT4_DEBUG_LOG("Calling users update handler login=" << user->login);
        handler(user, type);
    }
    else
    {
        MT4_DEBUG_LOG("No users update handler registered");
    }
}

UserRecord MT4Users::Get(int login) const
{
    const char *context = "Users.Get";

    MT4_INFO_LOG(context << " start login=" << login);

    MT4_ENSURE_CLIENT(context, client_);

    CManagerInterface *manager = client_->Manager();
    MT4_ENSURE_MANAGER(context, manager);

    client_->EnsureNotPumping();

    const int logins[] = {login};
    int total = 1;

    MT4_DEBUG_LOG("Calling UserRecordsRequest login=" << login);

    UserRecord *records = manager->UserRecordsRequest(logins, &total);

    MT4_DEBUG_LOG("UserRecordsRequest returned total=" << total << " records=" << records);

    if (!records || total <= 0)
    {
        MT4_THROW_NOT_FOUND_WITH_ID(context, "User", login);
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

        MT4_THROW_NOT_FOUND_WITH_ID(context, "User", login);
    }

    UserRecord user = *found;
    manager->MemFree(records);

    MT4_INFO_LOG(context << " success login=" << login);

    return user;
}

int MT4Users::Create(UserRecord &user) const
{
    MT4_INFO_LOG("Users.Create start group=" << user.group << " email=" << user.email);

    if (!client_)
    {
        MT4_ERROR_LOG("Users.Create failed: client is not initialized");
        throw std::runtime_error("MT4 client is not initialized");
    }

    CManagerInterface *manager = client_->Manager();
    if (!manager)
    {
        MT4_ERROR_LOG("Users.Create failed: manager is not initialized");
        throw std::runtime_error("MT4 manager is not initialized");
    }

    client_->EnsureNotPumping();

    int result = manager->UserRecordNew(&user);

    MT4_DEBUG_LOG("UserRecordNew returned code=" << result << " login=" << user.login);

    ThrowMt4Error("UserRecordNew", result, manager);

    MT4_INFO_LOG("Users.Create success login=" << user.login);

    return user.login;
}

void MT4Users::Update(const UserRecord &user) const
{
    MT4_INFO_LOG("Users.Update start login=" << user.login);

    if (!client_)
    {
        MT4_ERROR_LOG("Users.Update failed: client is not initialized");
        throw std::runtime_error("MT4 client is not initialized");
    }

    CManagerInterface *manager = client_->Manager();
    if (!manager)
    {
        MT4_ERROR_LOG("Users.Update failed: manager is not initialized");
        throw std::runtime_error("MT4 manager is not initialized");
    }

    client_->EnsureNotPumping();

    int result = manager->UserRecordUpdate(&user);

    MT4_DEBUG_LOG("UserRecordUpdate returned code=" << result << " login=" << user.login);

    ThrowMt4Error("UserRecordUpdate", result, manager);

    MT4_INFO_LOG("Users.Update success login=" << user.login);
}