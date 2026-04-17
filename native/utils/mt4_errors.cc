#include "utils/mt4_errors.h"

#include <sstream>
#include <stdexcept>

#include "../include/mt4_sdk.h"

void ThrowMt4Error(const char *action, int code, CManagerInterface *manager)
{
    if (code == RET_OK)
    {
        return;
    }

    std::ostringstream message;
    message << action << " failed with MT4 error code " << code;

    const char *desc = manager ? manager->ErrorDescription(code) : nullptr;
    if (desc && desc[0])
    {
        message << " (" << desc << ")";
    }

    throw std::runtime_error(message.str());
}