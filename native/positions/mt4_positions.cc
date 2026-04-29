#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "mt4_positions.h"

#include "../mt4_client.h"

#include "../include/mt4_sdk.h"

#include "../utils/mt4_errors.h"
#include "../utils/mt4_log.h"
#include "../utils/mt4_error_helpers.h"

MT4Positions::MT4Positions(const std::shared_ptr<MT4Client> &client)
    : client_(client)
{
    MT4_DEBUG_LOG("MT4Positions created");
}

MT4Positions::~MT4Positions()
{
    MT4_DEBUG_LOG("MT4Positions destroyed");
}

std::shared_ptr<MT4Positions> MT4Positions::CreateShared(const std::shared_ptr<MT4Client> &client)
{
    auto positions = std::shared_ptr<MT4Positions>(new MT4Positions(client));
    return positions;
}

TradeRecord MT4Positions::Get(int order) const
{
    const char *context = "Positions.Get";

    MT4_INFO_LOG(context << " start order=" << order);

    MT4_ENSURE_CLIENT(context, client_);

    CManagerInterface *manager = client_->Manager();
    MT4_ENSURE_MANAGER(context, manager);

    client_->EnsureNotPumping();

    const int orders[] = {order};
    int total = 1;

    MT4_DEBUG_LOG("Calling TradeRecordsRequest order=" << order);

    TradeRecord *records = manager->TradeRecordsRequest(orders, &total);

    MT4_DEBUG_LOG("TradeRecordsRequest returned total=" << total << " records=" << records);

    if (!records || total <= 0)
    {
        MT4_THROW_NOT_FOUND_WITH_ID(context, "Position", order);
    }

    const TradeRecord *found = nullptr;
    for (int i = 0; i < total; ++i)
    {
        if (records[i].order == order)
        {
            found = &records[i];
            break;
        }
    }

    if (!found)
    {
        manager->MemFree(records);

        MT4_THROW_NOT_FOUND_WITH_ID(context, "Position", order);
    }

    TradeRecord record = *found;
    manager->MemFree(records);

    MT4_INFO_LOG(context << " success order=" << order);

    return record;
}
