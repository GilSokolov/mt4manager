// native/symbols/mt4_symbol_to_napi.h
#pragma once

#include <napi.h>

#include "../include/mt4_sdk.h"

Napi::Object ToNapiSymbol(Napi::Env env, const SymbolInfo &symbol);
Napi::Object ToNapiSymbolConfig(Napi::Env env, const ConSymbol &s);