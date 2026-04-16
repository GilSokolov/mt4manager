#include "utils/mt4_errors.h"

#include <sstream>
#include <stdexcept>

#include "../include/mt4_sdk.h"

bool IsMt4Success(int code)
{
    return code == 0;
}

void ThrowMt4Error(
    const char *action,
    int code,
    CManagerInterface *manager)
{
    if (IsMt4Success(code))
    {
        return;
    }

    std::ostringstream message;
    message << action << " failed with MT4 error code " << code;

    // Later, if your MT4 API exposes a readable error method, add it here.
    // Example:
    // if (manager) {
    //   const char* text = manager->ErrorDescription(code);
    //   if (text && *text) {
    //     message << " (" << text << ")";
    //   }
    // }

    throw std::runtime_error(message.str());
}