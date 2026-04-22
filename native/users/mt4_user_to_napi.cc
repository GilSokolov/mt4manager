#include "./mt4_user_converter.h"
#include "../utils/napi_converter_utils.h"

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
