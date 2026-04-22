#include "./mt4_user_converter.h"
#include "../utils/napi_converter_utils.h"

UserRecord FromNapiUser(Napi::Env env, const Napi::Object &obj)
{
    UserRecord user{}; // already zeroed

    // --- CORE ---
    CopyOptionalStringField(user.group, obj, "group");
    CopyOptionalStringField(user.name, obj, "name");
    CopyOptionalStringField(user.password, obj, "password");
    CopyOptionalStringField(user.password_investor, obj, "passwordInvestor");
    CopyOptionalStringField(user.email, obj, "email");
    CopyOptionalStringField(user.phone, obj, "phone");

    CopyOptionalIntField(user.login, obj, "login");
    CopyOptionalIntField(user.leverage, obj, "leverage");

    // --- FLAGS ---
    CopyOptionalBoolFieldAsInt(user.enable, obj, "enable");
    CopyOptionalBoolFieldAsInt(user.enable_change_password, obj, "enableChangePassword");
    CopyOptionalBoolFieldAsInt(user.enable_read_only, obj, "enableReadOnly");
    CopyOptionalBoolFieldAsInt(user.enable_otp, obj, "enableOtp");

    CopyOptionalStringField(user.password_phone, obj, "passwordPhone");

    // --- PERSONAL ---
    CopyOptionalStringField(user.country, obj, "country");
    CopyOptionalStringField(user.city, obj, "city");
    CopyOptionalStringField(user.state, obj, "state");
    CopyOptionalStringField(user.zipcode, obj, "zipcode");
    CopyOptionalStringField(user.address, obj, "address");
    CopyOptionalStringField(user.lead_source, obj, "leadSource");
    CopyOptionalStringField(user.comment, obj, "comment");
    CopyOptionalStringField(user.id, obj, "id");
    CopyOptionalStringField(user.status, obj, "status");

    // --- DATES ---
    CopyOptionalDateField(user.regdate, obj, "regdate");
    CopyOptionalDateField(user.lastdate, obj, "lastdate");
    CopyOptionalDateField(user.timestamp, obj, "timestamp");

    // --- RELATIONS ---
    CopyOptionalIntField(user.agent_account, obj, "agentAccount");

    // --- FINANCIAL ---
    CopyOptionalIntField(user.taxes, obj, "taxes");

    // --- SETTINGS ---
    CopyOptionalIntField(user.send_reports, obj, "sendReports");
    CopyOptionalIntField(user.mqid, obj, "mqid");
    CopyOptionalIntField(user.user_color, obj, "userColor");

    return user;
}