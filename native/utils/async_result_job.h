#pragma once

#include <napi.h>
#include <functional>
#include <optional>
#include <string>

template <typename Result>
class AsyncResultJob : public Napi::AsyncWorker
{
public:
    using ExecuteFn = std::function<Result()>;
    using CompleteFn = std::function<Napi::Value(Napi::Env, Result &)>;

    AsyncResultJob(
        Napi::Env env,
        std::string resource_name,
        ExecuteFn execute_fn,
        CompleteFn complete_fn,
        Napi::Promise::Deferred deferred)
        : Napi::AsyncWorker(env, resource_name.c_str()),
          execute_fn_(std::move(execute_fn)),
          complete_fn_(std::move(complete_fn)),
          deferred_(deferred)
    {
    }

    void Execute() override
    {
        result_ = execute_fn_();
    }

    void OnOK() override
    {
        deferred_.Resolve(complete_fn_(Env(), *result_));
    }

    void OnError(const Napi::Error &error) override
    {
        deferred_.Reject(error.Value());
    }

private:
    ExecuteFn execute_fn_;
    CompleteFn complete_fn_;
    Napi::Promise::Deferred deferred_;
    std::optional<Result> result_;
};