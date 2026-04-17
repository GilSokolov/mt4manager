#pragma once

#include <memory>
#include <string>

class MT4Client;

struct MT4UserRecord
{
    int login = 0;
    std::string group;
    std::string name;
    std::string email;
    int leverage = 0;
};

class MT4Users
{
public:
    explicit MT4Users(const std::shared_ptr<MT4Client> &client);

    MT4UserRecord Get(int login) const;

private:
    std::shared_ptr<MT4Client> client_;
};