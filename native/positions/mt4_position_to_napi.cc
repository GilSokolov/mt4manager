#include "mt4_position_to_napi.h"

#include "../utils/napi_converter_utils.h"

Napi::Object ToNapiPosition(Napi::Env env, const TradeRecord &t)
{
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("order", t.order);
    obj.Set("login", t.login);
    obj.Set("symbol", t.symbol);
    obj.Set("volume", t.volume);
    obj.Set("openPrice", t.open_price);
    obj.Set("closePrice", t.close_price);
    obj.Set("profit", t.profit);

    obj.Set("openTime", ToJsDateOrNull(env, t.open_time));
    obj.Set("closeTime", ToJsDateOrNull(env, t.close_time));

    return obj;
}