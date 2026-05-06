#pragma once

#include <napi.h>

#include <functional>
#include <string>

class AsyncJob : public Napi::AsyncWorker
{
public:
    AsyncJob(
        Napi::Env env,
        std::string resource_name,
        std::function<void()> execute_fn,
        Napi::Promise::Deferred deferred)
        : Napi::AsyncWorker(env, resource_name.c_str()),
          execute_fn_(std::move(execute_fn)),
          deferred_(deferred)
    {
    }

    void Execute() override
    {
        execute_fn_();
    }

    void OnOK() override
    {
        deferred_.Resolve(Env().Undefined());
    }

    void OnError(const Napi::Error &error) override
    {
        deferred_.Reject(error.Value());
    }

private:
    std::function<void()> execute_fn_;
    Napi::Promise::Deferred deferred_;
};