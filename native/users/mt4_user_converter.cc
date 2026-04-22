#include "./mt4_user_converter.h"

inline std::string SafeString(const char *data, size_t size)
{
    return std::string(data, strnlen(data, size));
}

namespace
{
    template <size_t N>
    void CopyString(char (&dest)[N], const std::string &src)
    {
        std::memset(dest, 0, N);
#ifdef _MSC_VER
        strncpy_s(dest, src.c_str(), N - 1);
#else
        std::strncpy(dest, src.c_str(), N - 1);
#endif
    }

    std::string GetRequiredString(Napi::Env env, const Napi::Object &obj, const char *key)
    {
        if (!obj.Has(key) || !obj.Get(key).IsString())
        {
            throw std::runtime_error(std::string("Expected string field: ") + key);
        }
        return obj.Get(key).As<Napi::String>().Utf8Value();
    }

    int GetRequiredInt(Napi::Env env, const Napi::Object &obj, const char *key)
    {
        if (!obj.Has(key) || !obj.Get(key).IsNumber())
        {
            throw std::runtime_error(std::string("Expected number field: ") + key);
        }
        return obj.Get(key).As<Napi::Number>().Int32Value();
    }

    int GetOptionalInt(const Napi::Object &obj, const char *key, int fallback = 0)
    {
        if (!obj.Has(key))
        {
            return fallback;
        }
        Napi::Value value = obj.Get(key);
        if (!value.IsNumber())
        {
            throw std::runtime_error(std::string("Expected number field: ") + key);
        }
        return value.As<Napi::Number>().Int32Value();
    }

    std::string GetOptionalString(const Napi::Object &obj, const char *key)
    {
        if (!obj.Has(key))
        {
            return "";
        }
        Napi::Value value = obj.Get(key);
        if (!value.IsString())
        {
            throw std::runtime_error(std::string("Expected string field: ") + key);
        }
        return value.As<Napi::String>().Utf8Value();
    }

    int GetOptionalBoolAsInt(const Napi::Object &obj, const char *key, int fallback = 0)
    {
        if (!obj.Has(key))
            return fallback;

        auto v = obj.Get(key);

        if (v.IsBoolean())
            return v.As<Napi::Boolean>().Value() ? 1 : 0;

        if (v.IsNumber())
            return v.As<Napi::Number>().Int32Value();

        throw std::runtime_error(std::string("Expected boolean or number: ") + key);
    }

    Napi::Value ToJsDateOrNull(Napi::Env env, int timestampSeconds)
    {
        if (timestampSeconds <= 0)
            return env.Null();

        return Napi::Date::New(env, static_cast<double>(timestampSeconds) * 1000.0);
    }

    int GetOptionalDateAsUnixSeconds(const Napi::Object &obj, const char *key, int fallback = 0)
    {
        if (!obj.Has(key))
            return fallback;

        Napi::Value value = obj.Get(key);

        if (value.IsDate())
        {
            double ms = value.As<Napi::Date>().ValueOf();
            return static_cast<int>(ms / 1000.0);
        }

        if (value.IsNumber())
        {
            double num = value.As<Napi::Number>().DoubleValue();

            // heuristic: if it looks like JS milliseconds, convert to seconds
            if (num > 100000000000.0)
                return static_cast<int>(num / 1000.0);

            return static_cast<int>(num);
        }

        if (value.IsNull())
            return 0;

        throw std::runtime_error(std::string("Expected Date, number, or null field: ") + key);
    }
}

Napi::Object ToNapiUser(Napi::Env env, const UserRecord &user)
{
    Napi::Object obj = Napi::Object::New(env);

    // --- common settings
    obj.Set("login", Napi::Number::New(env, user.login));
    obj.Set("group", SafeString(user.group, sizeof(user.group)));
    // obj.Set("password", SafeString(user.password, sizeof(user.password)));

    // --- access flags
    obj.Set("enable", Napi::Boolean::New(env, user.enable != 0));
    obj.Set("enableChangePassword", Napi::Boolean::New(env, user.enable_change_password != 0));
    obj.Set("enableReadOnly", Napi::Boolean::New(env, user.enable_read_only != 0));
    obj.Set("enableOtp", Napi::Boolean::New(env, user.enable_otp != 0));

    // --- passwords
    // obj.Set("passwordInvestor", SafeString(user.password_investor, sizeof(user.password_investor)));
    // obj.Set("passwordPhone", SafeString(user.password_phone, sizeof(user.password_phone)));

    // --- personal info
    obj.Set("name", SafeString(user.name, sizeof(user.name)));
    obj.Set("country", SafeString(user.country, sizeof(user.country)));
    obj.Set("city", SafeString(user.city, sizeof(user.city)));
    obj.Set("state", SafeString(user.state, sizeof(user.state)));
    obj.Set("zipcode", SafeString(user.zipcode, sizeof(user.zipcode)));
    obj.Set("address", SafeString(user.address, sizeof(user.address)));
    obj.Set("leadSource", SafeString(user.lead_source, sizeof(user.lead_source)));
    obj.Set("phone", SafeString(user.phone, sizeof(user.phone)));
    obj.Set("email", SafeString(user.email, sizeof(user.email)));
    obj.Set("comment", SafeString(user.comment, sizeof(user.comment)));
    obj.Set("id", SafeString(user.id, sizeof(user.id)));
    obj.Set("status", SafeString(user.status, sizeof(user.status)));

    // --- dates
    obj.Set("regdate", ToJsDateOrNull(env, user.regdate));
    obj.Set("lastdate", ToJsDateOrNull(env, user.lastdate));
    obj.Set("timestamp", ToJsDateOrNull(env, user.timestamp));

    // --- trade settings
    obj.Set("leverage", Napi::Number::New(env, user.leverage));
    obj.Set("agentAccount", Napi::Number::New(env, user.agent_account));
    obj.Set("lastIp", Napi::Number::New(env, user.last_ip));

    obj.Set("balance", Napi::Number::New(env, user.balance));
    obj.Set("prevMonthBalance", Napi::Number::New(env, user.prevmonthbalance));
    obj.Set("prevBalance", Napi::Number::New(env, user.prevbalance));
    obj.Set("credit", Napi::Number::New(env, user.credit));
    obj.Set("interestRate", Napi::Number::New(env, user.interestrate));
    obj.Set("taxes", Napi::Number::New(env, user.taxes));
    obj.Set("prevMonthEquity", Napi::Number::New(env, user.prevmonthequity));
    obj.Set("prevEquity", Napi::Number::New(env, user.prevequity));

    // --- security
    obj.Set("otpSecret", SafeString(user.otp_secret, sizeof(user.otp_secret)));

    // --- misc
    obj.Set("sendReports", Napi::Number::New(env, user.send_reports));
    obj.Set("mqid", Napi::Number::New(env, user.mqid));
    obj.Set("userColor", Napi::Number::New(env, user.user_color));

    // --- api
    obj.Set("apiData", SafeString(user.api_data, sizeof(user.api_data)));

    return obj;
}

UserRecord FromNapiUser(Napi::Env env, const Napi::Object &obj)
{
    UserRecord user{};
    std::memset(&user, 0, sizeof(UserRecord));

    // --- CORE ---
    if (obj.Has("group"))
        CopyString(user.group, GetOptionalString(obj, "group"));

    if (obj.Has("name"))
        CopyString(user.name, GetOptionalString(obj, "name"));

    if (obj.Has("password"))
        CopyString(user.password, GetOptionalString(obj, "password"));

    if (obj.Has("passwordInvestor"))
        CopyString(user.password_investor, GetOptionalString(obj, "passwordInvestor"));

    if (obj.Has("email"))
        CopyString(user.email, GetOptionalString(obj, "email"));

    if (obj.Has("phone"))
        CopyString(user.phone, GetOptionalString(obj, "phone"));

    if (obj.Has("leverage"))
        user.leverage = GetOptionalInt(obj, "leverage");

    if (obj.Has("login"))
        user.login = GetOptionalInt(obj, "login");

    // --- FLAGS ---
    if (obj.Has("enable"))
        user.enable = GetOptionalBoolAsInt(obj, "enable");

    if (obj.Has("enableChangePassword"))
        user.enable_change_password = GetOptionalBoolAsInt(obj, "enableChangePassword");

    if (obj.Has("enableReadOnly"))
        user.enable_read_only = GetOptionalBoolAsInt(obj, "enableReadOnly");

    if (obj.Has("enableOtp"))
        user.enable_otp = GetOptionalBoolAsInt(obj, "enableOtp");

    if (obj.Has("passwordPhone"))
        CopyString(user.password_phone, GetOptionalString(obj, "passwordPhone"));

    // --- PERSONAL ---
    if (obj.Has("country"))
        CopyString(user.country, GetOptionalString(obj, "country"));

    if (obj.Has("city"))
        CopyString(user.city, GetOptionalString(obj, "city"));

    if (obj.Has("state"))
        CopyString(user.state, GetOptionalString(obj, "state"));

    if (obj.Has("zipcode"))
        CopyString(user.zipcode, GetOptionalString(obj, "zipcode"));

    if (obj.Has("address"))
        CopyString(user.address, GetOptionalString(obj, "address"));

    if (obj.Has("leadSource"))
        CopyString(user.lead_source, GetOptionalString(obj, "leadSource"));

    if (obj.Has("comment"))
        CopyString(user.comment, GetOptionalString(obj, "comment"));

    if (obj.Has("id"))
        CopyString(user.id, GetOptionalString(obj, "id"));

    if (obj.Has("status"))
        CopyString(user.status, GetOptionalString(obj, "status"));

    // --- DATES ---
    if (obj.Has("regdate"))
        user.regdate = GetOptionalDateAsUnixSeconds(obj, "regdate");

    if (obj.Has("lastdate"))
        user.lastdate = GetOptionalDateAsUnixSeconds(obj, "lastdate");

    if (obj.Has("timestamp"))
        user.timestamp = GetOptionalDateAsUnixSeconds(obj, "timestamp");

    // --- RELATIONS ---
    if (obj.Has("agentAccount"))
        user.agent_account = GetOptionalInt(obj, "agentAccount");

    if (obj.Has("taxes"))
        user.taxes = GetOptionalInt(obj, "taxes");

    // --- SETTINGS ---
    if (obj.Has("sendReports"))
        user.send_reports = GetOptionalInt(obj, "sendReports");

    if (obj.Has("mqid"))
        user.mqid = GetOptionalInt(obj, "mqid");

    if (obj.Has("userColor"))
        user.user_color = GetOptionalInt(obj, "userColor");

    return user;
}