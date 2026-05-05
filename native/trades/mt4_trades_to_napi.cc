#include "mt4_trades_to_napi.h"

#include "../utils/napi_converter_utils.h"

std::vector<napi_value> BuildTradeArgs(
    Napi::Env env,
    const TradePayload &payload)
{
    return {
        ToNapiTrade(env, payload.trade),
        Napi::Number::New(env, payload.type)};
}

Napi::Object ToNapiTrade(Napi::Env env, const TradeRecord &trade)
{
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("id", trade.order);
    obj.Set("login", trade.login);
    obj.Set("symbol", SafeString(trade.symbol, sizeof(trade.symbol)));
    obj.Set("digits", trade.digits);
    obj.Set("cmd", trade.cmd);
    obj.Set("volume", trade.volume);

    obj.Set("openTime", ToJsDateOrNull(env, trade.open_time));
    obj.Set("openPrice", trade.open_price);
    obj.Set("sl", trade.sl);
    obj.Set("tp", trade.tp);
    obj.Set("closeTime", ToJsDateOrNull(env, trade.close_time));
    obj.Set("closePrice", trade.close_price);
    obj.Set("expiration", ToJsDateOrNull(env, trade.expiration));
    obj.Set("reason", static_cast<int>(trade.reason));

    obj.Set("convRates", ToNapiNumberArray(env, trade.conv_rates, 2));

    obj.Set("commission", trade.commission);
    obj.Set("commissionAgent", trade.commission_agent);
    obj.Set("swap", trade.storage);
    obj.Set("profit", trade.profit);
    obj.Set("taxes", trade.taxes);
    obj.Set("magic", trade.magic);
    obj.Set("comment", SafeString(trade.comment, sizeof(trade.comment)));
    obj.Set("gwOrder", trade.gw_order);
    obj.Set("gwVolume", trade.gw_volume);
    obj.Set("gwOpenPrice", trade.gw_open_price);
    obj.Set("gwClosePrice", trade.gw_close_price);
    obj.Set("marginRate", trade.margin_rate);
    obj.Set("timestamp", ToJsDateOrNull(env, trade.timestamp));
    obj.Set("activation", trade.activation);
    obj.Set("state", trade.state);
    obj.Set("apiData", ToNapiNumberArray(env, trade.api_data, 4));

    return obj;
}