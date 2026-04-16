#include <napi.h>

#include "mt4_manager_wrap.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return MT4ManagerWrap::Init(env, exports);
}

NODE_API_MODULE(mt4manager, InitAll)
