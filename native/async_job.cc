#include "async_job.h"

AsyncJob::AsyncJob(Napi::Env env,
                   std::string resource_name,
                   std::function<void()> execute_fn,
                   Napi::Promise::Deferred deferred)
    : Napi::AsyncWorker(env, resource_name.c_str()),
      execute_fn_(std::move(execute_fn)),
      deferred_(deferred) {}

void AsyncJob::Execute()
{
  execute_fn_();
}

void AsyncJob::OnOK()
{
  deferred_.Resolve(Env().Undefined());
}

void AsyncJob::OnError(const Napi::Error &error)
{
  deferred_.Reject(error.Value());
}
