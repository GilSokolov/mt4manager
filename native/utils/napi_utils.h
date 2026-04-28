#pragma once

#include <napi.h>
#include <string>

#include "./mt4_errors.h"

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

    inline Napi::Function GetFunction(
        const Napi::CallbackInfo &info,
        size_t index,
        const char *name)
    {
        if (info.Length() <= index || !info[index].IsFunction())
        {
            throw std::invalid_argument(std::string("Expected function for argument: ") + name);
        }

        return info[index].As<Napi::Function>();
    }

    inline Napi::Value ThrowErrorWithCode(
        Napi::Env env,
        const char *message,
        const char *code)
    {
        Napi::Error error = Napi::Error::New(env, message);
        error.Value().Set("code", code);
        error.ThrowAsJavaScriptException();
        return env.Undefined();
    }

    inline Napi::Value ThrowError(Napi::Env env, const MT4Error &ex)
    {
        return ThrowErrorWithCode(env, ex.what(), ex.CodeString());
    }

    inline Napi::Value ThrowError(Napi::Env env, const std::exception &ex)
    {
        return ThrowErrorWithCode(env, ex.what(), "UNKNOWN");
    }

    inline Napi::Value ThrowError(Napi::Env env, const char *message)
    {
        return ThrowErrorWithCode(env, message, "UNKNOWN");
    }

    inline Napi::Value ThrowError(Napi::Env env, const std::string &message)
    {
        return ThrowError(env, message.c_str());
    }

} // namespace napi_utils