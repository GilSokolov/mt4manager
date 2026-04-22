#include "pumping_options_napi.h"

#include <stdexcept>
#include <string>

namespace
{
    bool GetOptionalBool(const Napi::Object &obj, const char *key, bool defaultValue = false)
    {
        if (!obj.Has(key))
            return defaultValue;

        Napi::Value value = obj.Get(key);
        if (!value.IsBoolean())
        {
            throw std::runtime_error(std::string("Expected boolean for '") + key + "'");
        }

        return value.As<Napi::Boolean>().Value();
    }
}

PumpingOptions ParsePumpingOptions(const Napi::Value &value)
{
    PumpingOptions options{};

    if (value.IsUndefined() || value.IsNull())
        return options;

    if (!value.IsObject())
        throw std::runtime_error("Expected pumping options object");

    Napi::Object obj = value.As<Napi::Object>();

    options.ticks = GetOptionalBool(obj, "ticks");
    options.news = GetOptionalBool(obj, "news");
    options.fullNews = GetOptionalBool(obj, "fullNews");
    options.mail = GetOptionalBool(obj, "mail");
    options.onlineUsers = GetOptionalBool(obj, "onlineUsers");
    options.users = GetOptionalBool(obj, "users");

    if (options.fullNews && !options.news)
    {
        throw std::runtime_error("fullNews=true requires news=true");
    }

    return options;
}