#pragma once

#include <iostream>
#include "mt4_logger.h"

#define MT4_ERROR_LOG(msg)                      \
    do                                          \
    {                                           \
        std::ostringstream oss;                 \
        oss << msg;                             \
        MT4Logger::Instance().Error(oss.str()); \
    } while (0)

#define MT4_INFO_LOG(msg)                      \
    do                                         \
    {                                          \
        std::ostringstream oss;                \
        oss << msg;                            \
        MT4Logger::Instance().Info(oss.str()); \
    } while (0)

#define MT4_DEBUG_LOG(msg)                      \
    do                                          \
    {                                           \
        std::ostringstream oss;                 \
        oss << msg;                             \
        MT4Logger::Instance().Debug(oss.str()); \
    } while (0)