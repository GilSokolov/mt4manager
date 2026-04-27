#pragma once

#include "mt4_client_config.h"

#include "utils/napi_converter_utils.h"

MT4ClientConfig FromNapiClientConfig(Napi::Env env, const Napi::Object &obj)
{
    MT4ClientConfig config{};

    config.dllPath = GetString(obj, "dllPath");

    int level = GetOptionalInt(obj, "logLevel", 1);

    if (level < 1 || level > 3)
    {
        throw std::runtime_error("Expected logLevel to be 1, 2, or 3");
    }

    config.logLevel = static_cast<MT4LogLevel>(level);

    return config;
}