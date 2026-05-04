#include "mt4_positions_wrap.h"
#include "mt4_position_to_napi.h"

#include "../utils/napi_utils.h"
#include "../utils/mt4_log.h"

#include "../trades/trade_from_napi.h"

Napi::FunctionReference MT4PositionsWrap::constructor;

Napi::Object MT4PositionsWrap::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function klass = DefineClass(
        env,
        "MT4Positions",
        {
            InstanceMethod("get", &MT4PositionsWrap::Get),
            InstanceMethod("create", &MT4PositionsWrap::Create),
            InstanceMethod("_setUpdateHandler", &MT4PositionsWrap::SetHandler),
        });

    constructor = Napi::Persistent(klass);
    constructor.SuppressDestruct();

    exports.Set("MT4Positions", klass);
    return exports;
}

MT4PositionsWrap::MT4PositionsWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<MT4PositionsWrap>(info)
{
}

Napi::Object MT4PositionsWrap::NewInstance(
    Napi::Env env,
    const std::shared_ptr<MT4Client> &client)
{
    Napi::EscapableHandleScope scope(env);

    Napi::Object obj = constructor.New({});
    MT4PositionsWrap *wrap =
        Napi::ObjectWrap<MT4PositionsWrap>::Unwrap(obj);

    wrap->positions_ = MT4Positions::CreateShared(client);
    wrap->bridge_ =
        std::make_shared<JsCallbackBridge<TradeEventPayload>>("MT4TradeHandler");

    return scope.Escape(obj).ToObject();
}

Napi::Value MT4PositionsWrap::SetHandler(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        Napi::Function handler = napi_utils::GetFunction(info, 0, "handler");

        bridge_->SetHandler(env, handler);

        positions_->SetHandler(
            [bridge = bridge_](const TradeRecord *trade)
            {
                bridge->CallJs(TradeEventPayload{*trade}, BuildTradeArgs);
            });

        return env.Undefined();
    }
    catch (const std::exception &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
}

Napi::Value MT4PositionsWrap::Get(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        const int order = napi_utils::GetInt32(info, 0, "login");
        const TradeRecord position = positions_->Get(order);
        return ToNapiPosition(env, position);
    }
    catch (const MT4Error &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
    catch (const std::exception &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
}

Napi::Value MT4PositionsWrap::Create(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        auto payload = FromNapiTrade(info[0]);
        const TradeRecord position = positions_->Create(payload);
        return ToNapiPosition(env, position);
    }
    catch (const MT4Error &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
    catch (const std::exception &ex)
    {
        return napi_utils::ThrowError(env, ex);
    }
}