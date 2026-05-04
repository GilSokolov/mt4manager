#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "mt4_positions.h"

#include "../mt4_client.h"

#include "../trades/payload_to_mt4.h"
#include "../trades/trade_from_napi.h"
#include "../trades/trade_record_utils.h"
#include "../trades/trade_transaction_utils.h"

#include "../include/mt4_sdk.h"

#include "../utils/mt4_errors.h"
#include "../utils/mt4_log.h"
#include "../utils/mt4_error_helpers.h"

#include "../utils/napi_converter_utils.h" // test

MT4Positions::MT4Positions(const std::shared_ptr<MT4Client> &client)
    : MT4PumpSubscriber(client)
{
    MT4_DEBUG_LOG("MT4Positions created");
}

MT4Positions::~MT4Positions()
{
    MT4_DEBUG_LOG("MT4Positions destroyed");
    DetachPumpListener();
}

void MT4Positions::HandleEvent(int code, int type, void *data)
{
    if (code != PUMP_UPDATE_TRADES || !data)
    {
        return;
    }

    const TradeRecord *trade = static_cast<const TradeRecord *>(data);

    MT4_DEBUG_LOG(
        "Trades pump update"
        << " login=" << trade->login
        << " type=" << type);

    auto handler = GetHandlerCopy();

    if (handler)
    {
        MT4_DEBUG_LOG("Calling trades update handler login=" << trade->login);
        handler(trade);
    }
    else
    {
        MT4_DEBUG_LOG("No users update handler registered");
    }
}

TradeRecord MT4Positions::Get(int order) const
{
    return GetTradeRecordByOrder(
        "Positions.Get",
        client_,
        order,
        "Position");
}

TradeRecord MT4Positions::Create(const TradePayload &payload)
{
    const char *context = "Positions.Create";

    MT4_INFO_LOG(context
                 << " start login=" << payload.login
                 << " symbol=" << payload.symbol);

    TradeTransInfo info = BuildTradeTransInfo(payload);

    return ExecuteTradeTransaction(
        context,
        client_,
        info,
        "Open position failed",
        "Position");
}
