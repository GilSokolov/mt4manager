// native/trades/trade_record_utils.h
#pragma once

#include <stdexcept>
#include <functional>

#include "../mt4_client.h"

#include "../include/mt4_sdk.h"

#include "../utils/mt4_errors.h"
#include "../utils/mt4_error_helpers.h"

inline CManagerInterface *RequireManager(
    const char *context,
    const std::shared_ptr<MT4Client> &client)
{
    MT4_ENSURE_CLIENT(context, client);

    CManagerInterface *manager = client->Manager();
    MT4_ENSURE_MANAGER(context, manager);

    return manager;
}

inline TradeRecord GetTradeRecordByOrder(
    const char *context,
    const std::shared_ptr<MT4Client> &client,
    int order,
    const char *entityName = "TradeRecord")
{
    MT4_INFO_LOG(context << " start order=" << order);

    CManagerInterface *manager = RequireManager(context, client);

    client->EnsureNotPumping();

    const int orders[] = {order};
    int total = 1;

    MT4_DEBUG_LOG(context << " calling TradeRecordsRequest order=" << order);

    TradeRecord *records = manager->TradeRecordsRequest(orders, &total);

    MT4_DEBUG_LOG(context << " TradeRecordsRequest returned total="
                          << total << " records=" << records);

    if (!records || total <= 0)
    {
        MT4_THROW_NOT_FOUND_WITH_ID(context, entityName, order);
    }

    TradeRecord *found = nullptr;

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
        MT4_THROW_NOT_FOUND_WITH_ID(context, entityName, order);
    }

    TradeRecord record = *found;

    manager->MemFree(records);

    MT4_INFO_LOG(context << " success order=" << order);

    return record;
}