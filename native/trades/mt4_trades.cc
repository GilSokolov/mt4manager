#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "mt4_trades.h"

#include "trade_request_to_mt4.h"

#include "../mt4_client.h"

#include "../include/mt4_sdk.h"

#include "../utils/mt4_errors.h"
#include "../utils/mt4_log.h"
#include "../utils/mt4_error_helpers.h"

MT4Trades::MT4Trades(const std::shared_ptr<MT4Client> &client)
    : MT4PumpSubscriber(client)
{
    MT4_DEBUG_LOG("MT4Trades created");
}

MT4Trades::~MT4Trades()
{
    MT4_DEBUG_LOG("MT4Trades destroyed");
    DetachPumpListener();
}

void MT4Trades::HandleEvent(int code, int type, void *data)
{
    if (code != PUMP_UPDATE_TRADES || !data)
    {
        return;
    }

    HandleTradeUpdate(type, data);
}

void MT4Trades::HandleTradeUpdate(int type, void *data)
{
    if (!data)
    {
        return;
    }

    const TradeRecord *trade = static_cast<const TradeRecord *>(data);

    auto handler = GetHandlerCopy();
    if (!handler)
    {
        return;
    }

    MT4_DEBUG_LOG(
        "Trades pump update login=" << trade->login
                                    << " type=" << type);

    handler(trade);
}

void MT4Trades::HandleBidAskUpdate(const std::string &symbol)
{
    CManagerInterface *manager = client_->Manager();
    if (!manager)
    {
        return;
    }

    int total = 0;
    TradeRecord *trades = manager->TradesGetBySymbol(
        const_cast<char *>(symbol.c_str()),
        &total);

    if (!trades || total <= 0)
    {
        if (trades)
        {
            manager->MemFree(trades);
        }

        return;
    }

    for (int i = 0; i < total; ++i)
    {
        HandleTradeUpdate(PUMP_UPDATE_BIDASK, &trades[i]);
    }

    manager->MemFree(trades);
}

TradeRecord MT4Trades::Get(int order) const
{
    const char *context = "Trades.Get";

    MT4_INFO_LOG(context << " start order=" << order);

    MT4_ENSURE_CLIENT(context, client_);

    CManagerInterface *manager = client_->Manager();

    MT4_ENSURE_MANAGER(context, manager);

    client_->EnsureNotPumping();

    const int orders[] = {order};
    int total = 1;

    MT4_DEBUG_LOG(context << " calling TradeRecordsRequest order=" << order);

    TradeRecord *records = manager->TradeRecordsRequest(orders, &total);

    MT4_DEBUG_LOG(context << " TradeRecordsRequest returned total="
                          << total << " records=" << records);

    if (!records || total <= 0)
    {
        MT4_THROW_NOT_FOUND_WITH_ID(context, "TradeRecord", order);
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
        MT4_THROW_NOT_FOUND_WITH_ID(context, "TradeRecord", order);
    }

    TradeRecord record = *found;

    manager->MemFree(records);

    MT4_INFO_LOG(context << " success order=" << order);

    return record;
}

int MT4Trades::Execute(const TradeRequest &request)
{
    const char *context = "Trades.Execute";

    MT4_INFO_LOG(context << " start mode=" << request.mode);

    MT4_ENSURE_CLIENT(context, client_);

    CManagerInterface *manager = client_->Manager();

    MT4_ENSURE_MANAGER(context, manager);

    TradeTransInfo info = ToMt4TradeTransInfo(request);

    int result = manager->TradeTransaction(&info);

    if (result != RET_OK)
    {
        ThrowMt4Error("TradeTransaction", result, manager);
    }

    return info.order;
}
