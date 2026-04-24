#pragma once

#include "mt4_client_config.h"

#include "utils/napi_converter_utils.h"

MT4ClientConfig FromNapiClientConfig(Napi::Env env, const Napi::Object &obj)
{
    MT4ClientConfig config{};

    config.dllPath = GetString(obj, "dllPath");
    config.debug = GetOptionalBool(obj, "debug");

    return config;
}