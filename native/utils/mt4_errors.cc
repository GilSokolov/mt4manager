#include "utils/mt4_errors.h"

#include <sstream>
#include <stdexcept>

#include "../include/mt4_sdk.h"
#include "./mt4_log.h"

void ThrowMt4Error(const char *action, int code, CManagerInterface *manager)
{
    if (code == RET_OK)
    {
        return;
    }

    std::ostringstream oss;
    oss << action << " failed with MT4 error code " << code;

    const char *desc = manager ? manager->ErrorDescription(code) : nullptr;
    if (desc && desc[0])
    {
        oss << " (" << desc << ")";
    }

    const std::string message = oss.str();

    MT4_ERROR_LOG(message);
    throw std::runtime_error(message);
}