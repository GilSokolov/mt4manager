// native/trades/trade_transaction_utils.h
#pragma once

#include <memory>

#include "./trade_record_utils.h"

#include "../mt4_client.h"

#include "../include/mt4_sdk.h"

#include "../utils/mt4_errors.h"

inline TradeRecord ExecuteTradeTransaction(
    const char *context,
    const std::shared_ptr<MT4Client> &client,
    TradeTransInfo &info,
    const char *failureMessage,
    const char *entityName)
{
    CManagerInterface *manager = RequireManager(context, client);

    MT4_INFO_LOG(
        context
        << " TradeTransInfo"
        << " type=" << (int)info.type
        << " flags=" << (int)info.flags
        << " cmd=" << info.cmd
        << " order=" << info.order
        << " orderby=" << info.orderby
        << " symbol=" << info.symbol
        << " volume=" << info.volume
        << " price=" << info.price
        << " sl=" << info.sl
        << " tp=" << info.tp
        << " deviation=" << info.ie_deviation
        << " comment=" << info.comment
        << " expiration=" << info.expiration
        << " crc=" << info.crc);

    int result = manager->TradeTransaction(&info);

    if (result != RET_OK)
    {
        ThrowMt4Error(failureMessage, result, manager);
    }

    return GetTradeRecordByOrder(
        context,
        client,
        info.order,
        entityName);
}