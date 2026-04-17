#pragma once

#include <iostream>

// Enable debug logs by defining MT4_DEBUG (e.g. in build flags)

#ifdef MT4_DEBUG
#define MT4_DEBUG_LOG(msg)                                \
    do                                                    \
    {                                                     \
        std::cerr << "[mt4][debug] " << msg << std::endl; \
    } while (0)
#else
#define MT4_DEBUG_LOG(msg) \
    do                     \
    {                      \
    } while (0)
#endif

#define MT4_ERROR_LOG(msg)                                \
    do                                                    \
    {                                                     \
        std::cerr << "[mt4][error] " << msg << std::endl; \
    } while (0)

#define MT4_INFO_LOG(msg)                          \
    do                                             \
    {                                              \
        std::cerr << "[mt4] " << msg << std::endl; \
    } while (0)