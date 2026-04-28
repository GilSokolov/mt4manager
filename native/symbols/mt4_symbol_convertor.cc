#include "mt4_symbol_convertor.h"

std::vector<napi_value> BuildSymbolArgs(
    Napi::Env env,
    const SymbolPayload &payload)
{
    return {
        ToNapiSymbol(env, payload.symbol, false),
    };
}
