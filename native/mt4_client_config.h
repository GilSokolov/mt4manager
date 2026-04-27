#pragma once
#include <napi.h>
#include "utils/mt4_logger.h"

struct MT4ClientConfig
{
    std::string dllPath;
    MT4LogLevel logLevel = MT4LogLevel::Error;
};

MT4ClientConfig FromNapiClientConfig(Napi::Env env, const Napi::Object &obj);