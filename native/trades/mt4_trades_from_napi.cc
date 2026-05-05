#include "mt4_trades_from_napi.h"

#include "../utils/napi_converter_utils.h"

TradeRequest FromNapiTrade(const Napi::Value &value)
{
    if (!value.IsObject())
    {
        throw std::runtime_error("Expected object for trade request");
    }

    Napi::Object obj = value.As<Napi::Object>();

    TradeRequest p{};

    p.login = GetOptionalInt(obj, "login");
    p.symbol = GetOptionalString(obj, "symbol");
    p.volume = GetOptionalInt(obj, "volume");
    p.cmd = GetOptionalInt(obj, "cmd");
    p.mode = GetOptionalString(obj, "mode");
    p.id = GetOptionalInt(obj, "id", 0);
    p.price = GetOptionalDouble(obj, "price", 0.0);
    p.sl = GetOptionalDouble(obj, "sl", 0.0);
    p.tp = GetOptionalDouble(obj, "tp", 0.0);
    p.deviation = GetOptionalInt(obj, "deviation", 0);
    p.comment = GetOptionalString(obj, "comment");
    p.flags = GetOptionalInt(obj, "flags", 0);

    // --- expiration (already normalized to unix seconds in TS)
    p.expiration = GetOptionalInt(obj, "expiration", 0);

    return p;
}