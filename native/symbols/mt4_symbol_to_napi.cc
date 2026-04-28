// native/symbols/mt4_symbol_to_napi.cc
#include "./mt4_symbol_to_napi.h"

#include "../utils/napi_converter_utils.h"

Napi::Object ToNapiSymbol(Napi::Env env, const SymbolInfo &symbol)
{
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("symbol", SafeString(symbol.symbol, sizeof(symbol.symbol)));
    obj.Set("digits", Napi::Number::New(env, symbol.digits));
    obj.Set("visible", Napi::Boolean::New(env, symbol.visible != 0));
    obj.Set("type", Napi::Number::New(env, symbol.type));
    obj.Set("point", Napi::Number::New(env, symbol.point));
    obj.Set("spread", Napi::Number::New(env, symbol.spread));
    obj.Set("direction", Napi::Number::New(env, symbol.direction));
    obj.Set("lastTime", ToJsDateOrNull(env, symbol.lasttime));
    obj.Set("bid", Napi::Number::New(env, symbol.bid));
    obj.Set("ask", Napi::Number::New(env, symbol.ask));
    obj.Set("high", Napi::Number::New(env, symbol.high));
    obj.Set("low", Napi::Number::New(env, symbol.low));
    obj.Set("commission", Napi::Number::New(env, symbol.commission));
    obj.Set("commissionType", Napi::Number::New(env, symbol.comm_type));

    return obj;
}

Napi::Object ToNapiSymbolConfig(Napi::Env env, const ConSymbol &s)
{
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("symbol", SafeString(s.symbol, sizeof(s.symbol)));
    obj.Set("description", SafeString(s.description, sizeof(s.description)));
    obj.Set("currency", SafeString(s.currency, sizeof(s.currency)));

    obj.Set("digits", Napi::Number::New(env, s.digits));
    obj.Set("point", Napi::Number::New(env, s.point));

    obj.Set("spread", Napi::Number::New(env, s.spread));

    obj.Set("contractSize", Napi::Number::New(env, s.contract_size));
    obj.Set("tickValue", Napi::Number::New(env, s.tick_value));
    obj.Set("tickSize", Napi::Number::New(env, s.tick_size));

    obj.Set("stopsLevel", Napi::Number::New(env, s.stops_level));

    obj.Set("marginCurrency", SafeString(s.margin_currency, sizeof(s.margin_currency)));
    obj.Set("marginInitial", Napi::Number::New(env, s.margin_initial));
    obj.Set("marginMaintenance", Napi::Number::New(env, s.margin_maintenance));

    return obj;
}