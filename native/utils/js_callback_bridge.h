#pragma once

#include <napi.h>

#include <memory>
#include <utility>
#include <vector>

template <typename Payload>
class JsCallbackBridge {
 public:
  explicit JsCallbackBridge(const char* resource_name)
      : resource_name_(resource_name) {}

  JsCallbackBridge(const JsCallbackBridge&) = delete;
  JsCallbackBridge& operator=(const JsCallbackBridge&) = delete;

  void SetHandler(Napi::Env env, Napi::Function handler) {
    // Release any previous JS callback before replacing it.
    Reset();

    // ThreadSafeFunction lets native threads safely schedule calls
    // back onto the Node.js JavaScript thread.
    tsfn_ =
        Napi::ThreadSafeFunction::New(env, handler, resource_name_,
                                      0,  // unlimited queue
                                      1   // one native thread uses this bridge
        );
  }

  void Reset() {
    if (tsfn_) {
      // Release the JS callback reference held by the TSFN.
      tsfn_.Release();

      // Reset to an empty TSFN so HasHandler() becomes false.
      tsfn_ = Napi::ThreadSafeFunction();
    }
  }

  bool HasHandler() const { return static_cast<bool>(tsfn_); }

  using ArgsBuilder = std::vector<napi_value> (*)(Napi::Env, const Payload&);
  void CallJs(Payload payload, ArgsBuilder build_args) const {
    // No JS handler has been registered yet.
    if (!tsfn_) {
      return;
    }

    // Move/copy payload to heap because it must survive until
    // Node.js runs the callback on the JS thread.
    auto* heap_payload = new Payload(std::move(payload));

    // Queue a callback onto the JS thread.
    napi_status status = tsfn_.BlockingCall(
        heap_payload, [build_args](Napi::Env env, Napi::Function js_callback,
                                   Payload* payload)
    {
      // Ensure payload is deleted when this JS-thread callback exits,
      // even if conversion or the JS call throws.
      std::unique_ptr<Payload> guard(payload);

      // Build JS arguments on the JS thread.
      // This is where Napi::Object, Napi::Number, etc. are safe.
      std::vector<napi_value> args = build_args(env, *guard);

      // Equivalent to calling:
      // handler(...args)
      js_callback.Call(args);
    });

    // If BlockingCall fails, the JS-thread callback will not run,
    // so we must manually delete the heap payload.
    if (status != napi_ok) {
      delete heap_payload;
    }
  }

 private:
  const char* resource_name_;
  Napi::ThreadSafeFunction tsfn_;
};