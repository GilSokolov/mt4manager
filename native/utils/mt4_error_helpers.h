// mt4_error_helpers.h
#pragma once

#include "./mt4_errors.h"
#include "./mt4_log.h"

#define MT4_THROW_ERROR(code, message) \
    do                                 \
    {                                  \
        MT4_ERROR_LOG(message);        \
        throw MT4Error(code, message); \
    } while (0)

#define MT4_THROW_CLIENT_NOT_INITIALIZED(context) \
    MT4_THROW_ERROR(                              \
        MT4ErrorCode::ClientNotInitialized,       \
        std::string(context) + " failed: client is not initialized")

#define MT4_THROW_MANAGER_NOT_INITIALIZED(context) \
    MT4_THROW_ERROR(                               \
        MT4ErrorCode::ManagerNotInitialized,       \
        std::string(context) + " failed: manager is not initialized")

#define MT4_THROW_NOT_FOUND(context, entity) \
    MT4_THROW_ERROR(                         \
        MT4ErrorCode::NotFound,              \
        std::string(context) + " failed: " + std::string(entity) + " not found")

#define MT4_THROW_NOT_FOUND_WITH_ID(context, entity, id) \
    MT4_THROW_ERROR(                                     \
        MT4ErrorCode::NotFound,                          \
        std::string(context) + " failed: " +             \
            std::string(entity) + " not found id=" +     \
            std::to_string(id))

#define MT4_ENSURE_CLIENT(context, client)             \
    do                                                 \
    {                                                  \
        if (!(client))                                 \
        {                                              \
            MT4_THROW_CLIENT_NOT_INITIALIZED(context); \
        }                                              \
    } while (0)

#define MT4_ENSURE_MANAGER(context, manager)            \
    do                                                  \
    {                                                   \
        if (!(manager))                                 \
        {                                               \
            MT4_THROW_MANAGER_NOT_INITIALIZED(context); \
        }                                               \
    } while (0)