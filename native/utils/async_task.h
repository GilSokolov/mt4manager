#include <napi.h>

#include <functional>
#include <optional>
#include <stdexcept>

#include "./mt4_errors.h"

// ─────────────────────────────────────────────────────────────────────────────
// Primary Template — for jobs that produce a Result
// ─────────────────────────────────────────────────────────────────────────────
// optional custom error converter
using ErrorFn =
    std::function<Napi::Value(Napi::Env, const std::exception_ptr&)>;

template <typename Result>
class AsyncTask : public Napi::AsyncWorker {
 public:
  // A function that runs on the background thread and returns a Result
  using ExecuteFn = std::function<Result()>;

  // A function that runs on the main thread and converts Result → Napi::Value
  // Optional: if not provided, the Promise resolves with undefined
  using CompleteFn = std::function<Napi::Value(Napi::Env, Result&)>;

  // ── Static factory ────────────────────────────────────────────────────────
  // Preferred way to create and queue a job.
  // Returns the Promise immediately; the job runs in the background.
  static Napi::Promise Run(
      Napi::Env env,              // The current JS environment
      std::string resource_name,  // Label shown in profilers and stack traces
      ExecuteFn execute_fn,       // The background work
      CompleteFn complete_fn = nullptr,  // Optional: how to convert result → JS
      ErrorFn error_fn = nullptr)        // optional custom error converter
  {
    // Create a Promise/resolver pair owned by this job
    auto deferred = Napi::Promise::Deferred::New(env);

    // Allocate the worker on the heap — Node-API will delete it after
    // OnOK or OnError finishes, so we must not delete it ourselves
    auto* job = new AsyncTask<Result>(
        env, std::move(resource_name), std::move(execute_fn),
        std::move(complete_fn), std::move(error_fn), deferred);

    // Hand the job off to libuv's thread pool; Execute() will run shortly
    job->Queue();

    // Return the Promise to JS so the caller can await it
    return deferred.Promise();
  }

  // ── Execute ───────────────────────────────────────────────────────────────
  // Runs on a libuv background thread.
  // ⚠️ Never touch Napi::* objects here — they are not thread-safe.
  void Execute() override {
    try {
      // Run the user-supplied work and store the result in the optional.
      // std::optional avoids requiring Result to be default-constructible.
      result_ = execute_fn_();
    } catch (...) {
      // Catch any non-standard throws so the process doesn't crash
      // Store the exception instead of SetError()
      // so we can access the full type in OnOK()
      exception_ = std::current_exception();
    }
  }

  // ── OnOK ─────────────────────────────────────────────────────────────────
  // Runs on the main JS thread after Execute() succeeds (no SetError called).
  // Safe to call Napi::* here.
  void OnOK() override {
    // HandleScope ensures all Napi handles created here are released
    // when the scope exits, preventing handle leaks
    // Napi::HandleScope scope(Env());

    try {
      if (exception_) {
        Napi::Value rejected =
            error_fn_ ? error_fn_(Env(), exception_) : DefaultError(exception_);

        deferred_.Reject(rejected);

        return;
      }
      // If the caller provided a complete_fn, use it to convert the
      // result into a Napi::Value; otherwise resolve with undefined
      Napi::Value resolved =
          complete_fn_
              ? complete_fn_(Env(), *result_)  // dereference optional safely —
                                               // Execute succeeded
              : Env().Undefined();  // no converter → resolve with undefined

      // Fulfil the Promise on the JS side
      deferred_.Resolve(resolved);
    } catch (const std::exception& e) {
      // If the conversion itself throws, reject instead of crashing
      deferred_.Reject(Napi::Error::New(Env(), e.what()).Value());
    }
  }

  // ── OnError ───────────────────────────────────────────────────────────────
  // Runs on the main JS thread when SetError() was called in Execute().
  void OnError(const Napi::Error& error) override {
    // Reject the Promise with the error that was set in Execute()
    deferred_.Reject(error.Value());
  }

 private:
  // Stored as a member so its lifetime extends past the constructor init list,
  // making the resource_name_.c_str() pointer passed to AsyncWorker safe
  std::string resource_name_;

  // The background work function — captured by value in the lambda at the call
  // site
  ExecuteFn execute_fn_;

  // The JS-conversion function — nullptr if the caller omitted it
  CompleteFn complete_fn_;

  // Holds the resolve/reject handles for the Promise returned to JS
  Napi::Promise::Deferred deferred_;

  ErrorFn error_fn_;

  // Wraps the result so we don't require Result to be default-constructible.
  // Empty until Execute() succeeds; always valid when OnOK() runs.
  std::optional<Result> result_;

  std::exception_ptr exception_ = nullptr;  // stores exception from Execute()

  // Fallback: converts exception to a plain JS Error
  // Builds { message, code } JS object from any exception
  Napi::Value DefaultError(const std::exception_ptr& eptr) {
    try {
      std::rethrow_exception(eptr);
    } catch (const MT4Error& ex) {
      // Create a proper Error object first
      Napi::Error error = Napi::Error::New(Env(), ex.what());
      // Then attach extra fields to it
      error.Value().Set("code", std::string(ex.CodeString()));
      return error.Value();
    } catch (const std::exception& ex) {
      Napi::Error error = Napi::Error::New(Env(), ex.what());
      error.Value().Set("code", std::string("UNKNOWN"));
      return error.Value();
    } catch (...) {
      Napi::Error error = Napi::Error::New(Env(), "Unknown error");
      error.Value().Set("code", std::string("UNKNOWN"));
      return error.Value();
    }
  }

  // ── Private constructor ───────────────────────────────────────────────────
  // Called only from Run(); callers use the factory, not new directly.
  AsyncTask(Napi::Env env, std::string resource_name, ExecuteFn execute_fn,
            CompleteFn complete_fn, ErrorFn error_fn,
            Napi::Promise::Deferred deferred)
      : resource_name_(std::move(resource_name)),  // store first …
        Napi::AsyncWorker(
            env, resource_name_.c_str()),  // … then pass its c_str() safely
        execute_fn_(std::move(execute_fn)),
        complete_fn_(std::move(complete_fn)),
        error_fn_(std::move(error_fn)),
        deferred_(deferred) {}
};

// ─────────────────────────────────────────────────────────────────────────────
// void Specialization — for jobs that produce no result
// complete_fn is not needed; the Promise always resolves with undefined
// ─────────────────────────────────────────────────────────────────────────────

template <>
class AsyncTask<void> : public Napi::AsyncWorker {
 public:
  using ExecuteFn = std::function<void()>;

  static Napi::Promise Run(
      Napi::Env env, std::string resource_name, ExecuteFn execute_fn,
      ErrorFn error_fn = nullptr)  // ← add ErrorFn here too
  {
    auto deferred = Napi::Promise::Deferred::New(env);
    auto* job = new AsyncTask<void>(env, std::move(resource_name),
                                    std::move(execute_fn),
                                    std::move(error_fn),  // ← pass it through
                                    deferred);
    job->Queue();
    return deferred.Promise();
  }

  void Execute() override {
    try {
      execute_fn_();
    } catch (...) {
      exception_ = std::current_exception();  // ← store, don't SetError
    }
  }

  void OnOK() override {
    Napi::HandleScope scope(Env());
    try {
      if (exception_) {
        Napi::Value rejected =
            error_fn_ ? error_fn_(Env(), exception_)
                      : DefaultError(exception_);  // ← same DefaultError logic
        deferred_.Reject(rejected);
        return;
      }
      deferred_.Resolve(Env().Undefined());
    } catch (const std::exception& e) {
      deferred_.Reject(Napi::Error::New(Env(), e.what()).Value());
    }
  }

  void OnError(const Napi::Error& error) override {
    deferred_.Reject(error.Value());
  }

 private:
  std::string resource_name_;
  ExecuteFn execute_fn_;
  ErrorFn error_fn_;  // ← add this
  Napi::Promise::Deferred deferred_;
  std::exception_ptr exception_ = nullptr;  // ← add this

  // same DefaultError as primary template
  Napi::Value DefaultError(const std::exception_ptr& eptr) {
    Napi::HandleScope scope(Env());
    try {
      std::rethrow_exception(eptr);
    } catch (const MT4Error& ex) {
      Napi::Error error = Napi::Error::New(Env(), ex.what());
      error.Value().Set("code", std::string(ex.CodeString()));
      return error.Value();
    } catch (const std::exception& ex) {
      Napi::Error error = Napi::Error::New(Env(), ex.what());
      error.Value().Set("code", std::string("UNKNOWN"));
      return error.Value();
    } catch (...) {
      Napi::Error error = Napi::Error::New(Env(), "Unknown error");
      error.Value().Set("code", std::string("UNKNOWN"));
      return error.Value();
    }
  }

  AsyncTask(Napi::Env env, std::string resource_name, ExecuteFn execute_fn,
            ErrorFn error_fn, Napi::Promise::Deferred deferred)
      : resource_name_(std::move(resource_name)),
        Napi::AsyncWorker(env, resource_name_.c_str()),
        execute_fn_(std::move(execute_fn)),
        error_fn_(std::move(error_fn)),  // ← add this
        deferred_(deferred) {}
};

// With complete_fn
template <typename ExecuteFn, typename CompleteFn>
auto MakeAsyncTask(Napi::Env env, std::string resource_name,
                   ExecuteFn execute_fn, CompleteFn complete_fn,
                   ErrorFn error_fn = nullptr) {
  using Result = std::invoke_result_t<ExecuteFn>;
  return AsyncTask<Result>::Run(env, std::move(resource_name),
                                std::move(execute_fn), std::move(complete_fn),
                                std::move(error_fn));
}

// Without complete_fn — resolves with undefined
template <typename ExecuteFn>
auto MakeAsyncTask(Napi::Env env, std::string resource_name,
                   ExecuteFn execute_fn) {
  using Result = std::invoke_result_t<ExecuteFn>;
  return AsyncTask<Result>::Run(env, std::move(resource_name),
                                std::move(execute_fn));
}