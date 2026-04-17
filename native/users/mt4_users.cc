#include "users/mt4_users.h"

#include "mt4_client.h"

MT4Users::MT4Users(const std::shared_ptr<MT4Client> &client)
    : client_(client)
{
}

MT4UserRecord MT4Users::Get(int login) const
{
    MT4UserRecord user{};
    user.login = login;
    user.group = "demo\\group";
    user.name = "Test User";
    user.email = "test@example.com";
    user.leverage = 100;
    return user;
}