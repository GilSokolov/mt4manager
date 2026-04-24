#pragma once
#include <napi.h>

struct MT4ClientConfig
{
    std::string dllPath;
    bool debug = false;
};

MT4ClientConfig FromNapiClientConfig(Napi::Env env, const Napi::Object &obj);