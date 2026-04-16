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
            Napi::TypeError::New(env, std::string("Expected number for argument: ") + name)
                .ThrowAsJavaScriptException();
            return 0;
        }

        return info[index].As<Napi::Number>().Int32Value();
    }

    inline std::string GetString(const Napi::CallbackInfo &info, size_t index, const char *name)
    {
        Napi::Env env = info.Env();

        if (info.Length() <= index || !info[index].IsString())
        {
            Napi::TypeError::New(env, std::string("Expected string for argument: ") + name)
                .ThrowAsJavaScriptException();
            return {};
        }

        return info[index].As<Napi::String>().Utf8Value();
    }

    inline bool HasPendingException(Napi::Env env)
    {
        return env.IsExceptionPending();
    }

} // namespace napi_utils