#include <cstring>
#include <stdexcept>

#include "payload_to_mt4.h"

#include "../utils/napi_converter_utils.h"

int ToOpenTransactionType(const std::string &mode)
{
    if (mode == "open")
        return TT_BR_ORDER_OPEN;

    if (mode == "close")
        return TT_BR_ORDER_CLOSE;

    if (mode == "delete")
        return TT_BR_ORDER_DELETE;

    if (mode == "close_by")
        return TT_BR_ORDER_CLOSE_BY;

    if (mode == "close_all")
        return TT_BR_ORDER_CLOSE_ALL;

    if (mode == "modify")
        return TT_BR_ORDER_MODIFY;

    if (mode == "activate")
        return TT_BR_ORDER_ACTIVATE;

    if (mode == "modify_comment")
        return TT_BR_ORDER_COMMENT;

    if (mode == "balance")
        return TT_BR_BALANCE;

    throw std::runtime_error("Invalid execution mode: " + mode);
}

TradeTransInfo BuildTradeTransInfo(const TradePayload &p)
{
    TradeTransInfo t{0};

    t.type = ToOpenTransactionType(p.mode);
    t.flags = static_cast<char>(p.flags);
    t.cmd = static_cast<short>(p.cmd);

    t.order = p.id;
    t.orderby = p.login;

    CopyString(t.symbol, p.symbol);

    t.volume = p.volume;
    t.price = p.price;
    t.sl = p.sl;
    t.tp = p.tp;

    t.ie_deviation = p.deviation;

    CopyString(t.comment, p.comment);

    t.expiration = p.expiration;

    return t;
}