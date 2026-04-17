#include "./mt4_user_converter.h"

Napi::Object ToNapiUser(Napi::Env env, const MT4UserRecord &user)
{
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("login", Napi::Number::New(env, user.login));
    obj.Set("group", Napi::String::New(env, user.group));
    obj.Set("name", Napi::String::New(env, user.name));
    obj.Set("email", Napi::String::New(env, user.email));
    obj.Set("leverage", Napi::Number::New(env, user.leverage));
    return obj;
}