#include "mt4_symbol_to_napi.h"
#include "../include/mt4_sdk.h"

struct SymbolPayload
{
    SymbolInfo symbol;
};

std::vector<napi_value> BuildSymbolArgs(
    Napi::Env env,
    const SymbolPayload &payload);