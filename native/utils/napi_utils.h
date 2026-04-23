#pragma once

#include <napi.h>
#include <string>

namespace napi_utils
{

    inline int GetInt32(const Napi::CallbackInfo &info, size_t index, const char *name)
    {
        Napi::Env env = info.Env();

        if (info.Length() <= index || !info[index].IsNumber())
        {
            throw std::invalid_argument(std::string("Expected number for argument: ") + name);
        }

        return info[index].As<Napi::Number>().Int32Value();
    }

    inline std::string GetString(const Napi::CallbackInfo &info, size_t index, const char *name)
    {
        Napi::Env env = info.Env();

        if (info.Length() <= index || !info[index].IsString())
        {
            throw std::invalid_argument(std::string("Expected string for argument: ") + name);
        }

        return info[index].As<Napi::String>().Utf8Value();
    }

    inline Napi::Object GetObject(const Napi::CallbackInfo &info, size_t index, const char *name)
    {
        if (info.Length() <= index || !info[index].IsObject())
        {
            throw std::invalid_argument(std::string("Expected object for argument: ") + name);
        }

        return info[index].As<Napi::Object>();
    }

    inline Napi::Value ThrowError(Napi::Env env, const std::string &msg)
    {
        Napi::Error::New(env, msg).ThrowAsJavaScriptException();
        return env.Null();
    }

    inline Napi::Value ThrowError(Napi::Env env, const char *msg)
    {
        return ThrowError(env, std::string(msg));
    }

    inline Napi::Value ThrowError(Napi::Env env, const std::exception &ex)
    {
        return ThrowError(env, ex.what());
    }

} // namespace napi_utils