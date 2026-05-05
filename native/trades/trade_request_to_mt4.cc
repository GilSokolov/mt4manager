#include <cstring>
#include <stdexcept>

#include "trade_request_to_mt4.h"
#include "../utils/napi_converter_utils.h"

int ToMt4TransactionType(const std::string &mode)
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

    throw std::runtime_error("Invalid trade mode: " + mode);
}

TradeTransInfo ToMt4TradeTransInfo(const TradeRequest &request)
{
    TradeTransInfo trans{0};

    trans.type = ToMt4TransactionType(request.mode);
    trans.flags = static_cast<char>(request.flags);
    trans.cmd = static_cast<short>(request.cmd);

    trans.order = request.id;
    trans.orderby = request.login;

    CopyString(trans.symbol, request.symbol);

    trans.volume = request.volume;
    trans.price = request.price;
    trans.sl = request.sl;
    trans.tp = request.tp;

    trans.ie_deviation = request.deviation;

    CopyString(trans.comment, request.comment);

    trans.expiration = request.expiration;

    return trans;
}