#pragma once

#include <napi.h>

#include <cstring>
#include <stdexcept>
#include <string>

// ---------- LOW LEVEL ----------

inline std::string SafeString(const char *data, size_t size)
{
    return std::string(data, strnlen(data, size));
}

template <size_t N>
inline void CopyString(char (&dest)[N], const std::string &src)
{
    std::memset(dest, 0, N);
#ifdef _MSC_VER
    strncpy_s(dest, src.c_str(), N - 1);
#else
    std::strncpy(dest, src.c_str(), N - 1);
#endif
}

// ---------- READERS ----------

inline std::string GetOptionalString(const Napi::Object &obj, const char *key)
{
    if (!obj.Has(key))
        return "";

    Napi::Value v = obj.Get(key);
    if (!v.IsString())
        throw std::runtime_error(std::string("Expected string: ") + key);

    return v.As<Napi::String>().Utf8Value();
}

inline int GetOptionalInt(const Napi::Object &obj, const char *key, int fallback = 0)
{
    if (!obj.Has(key))
        return fallback;

    Napi::Value v = obj.Get(key);
    if (!v.IsNumber())
        throw std::runtime_error(std::string("Expected number: ") + key);

    return v.As<Napi::Number>().Int32Value();
}

inline int GetOptionalBoolAsInt(const Napi::Object &obj, const char *key, int fallback = 0)
{
    if (!obj.Has(key))
        return fallback;

    Napi::Value v = obj.Get(key);

    if (v.IsBoolean())
        return v.As<Napi::Boolean>().Value() ? 1 : 0;

    if (v.IsNumber())
        return v.As<Napi::Number>().Int32Value();

    throw std::runtime_error(std::string("Expected bool/number: ") + key);
}

inline int GetOptionalDateAsUnixSeconds(const Napi::Object &obj, const char *key, int fallback = 0)
{
    if (!obj.Has(key))
        return fallback;

    Napi::Value v = obj.Get(key);

    if (v.IsDate())
    {
        double ms = v.As<Napi::Date>().ValueOf();
        return static_cast<int>(ms / 1000.0);
    }

    if (v.IsNumber())
    {
        double num = v.As<Napi::Number>().DoubleValue();
        return num > 100000000000.0
                   ? static_cast<int>(num / 1000.0)
                   : static_cast<int>(num);
    }

    if (v.IsNull())
        return 0;

    throw std::runtime_error(std::string("Expected Date/number/null: ") + key);
}

// ---------- WRITERS ----------

inline Napi::Value ToJsDateOrNull(Napi::Env env, int ts)
{
    if (ts <= 0)
        return env.Null();

    return Napi::Date::New(env, static_cast<double>(ts) * 1000.0);
}

// ---------- HIGH LEVEL HELPERS ----------

template <size_t N>
inline void CopyOptionalStringField(
    char (&dest)[N],
    const Napi::Object &obj,
    const char *key)
{
    if (obj.Has(key))
        CopyString(dest, GetOptionalString(obj, key));
}

template <typename T>
inline void CopyOptionalIntField(
    T &dest,
    const Napi::Object &obj,
    const char *key)
{
    if (obj.Has(key))
        dest = static_cast<T>(GetOptionalInt(obj, key));
}

template <typename T>
inline void CopyOptionalBoolFieldAsInt(
    T &dest,
    const Napi::Object &obj,
    const char *key)
{
    if (obj.Has(key))
        dest = static_cast<T>(GetOptionalBoolAsInt(obj, key));
}

template <typename T>
inline void CopyOptionalDateField(
    T &dest,
    const Napi::Object &obj,
    const char *key)
{
    if (obj.Has(key))
        dest = static_cast<T>(GetOptionalDateAsUnixSeconds(obj, key));
}