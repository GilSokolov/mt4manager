#pragma once

#include <napi.h>
#include <utility>
#include <functional>
#include <type_traits>

#include "async_job.h"
#include "async_result_job.h"

namespace async_utils
{

    template <typename Fn>
    Napi::Promise QueuePromise(
        Napi::Env env,
        const char *name,
        Fn &&work)
    {
        auto deferred = Napi::Promise::Deferred::New(env);

        AsyncJob *job = new AsyncJob(
            env,
            name,
            std::forward<Fn>(work),
            deferred);

        job->Queue();
        return deferred.Promise();
    }

    template <typename Fn, typename Complete>
    Napi::Promise QueuePromiseResult(
        Napi::Env env,
        const char *name,
        Fn &&work,
        Complete &&complete)
    {
        using Result = std::invoke_result_t<Fn>;

        auto deferred = Napi::Promise::Deferred::New(env);

        auto *job = new AsyncResultJob<Result>(
            env,
            name,
            std::function<Result()>(std::forward<Fn>(work)),
            std::function<Napi::Value(Napi::Env, Result &)>(
                std::forward<Complete>(complete)),
            deferred);

        job->Queue();
        return deferred.Promise();
    }

} // namespace async_utils