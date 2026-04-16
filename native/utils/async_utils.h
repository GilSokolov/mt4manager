#pragma once

#include <napi.h>
#include <utility>

#include "async_job.h"

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

} // namespace async_utils