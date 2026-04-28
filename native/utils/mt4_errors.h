#pragma once

#include <string>
#include <stdexcept>

enum class MT4ErrorCode
{
    InvalidArgument,
    ClientNotInitialized,
    ManagerNotInitialized,
    PumpingActive,
    NotFound,
    Mt4RequestFailed,
};

inline const char *MT4ErrorCodeToString(MT4ErrorCode code)
{
    switch (code)
    {
    case MT4ErrorCode::InvalidArgument:
        return "INVALID_ARGUMENT";
    case MT4ErrorCode::ClientNotInitialized:
        return "CLIENT_NOT_INITIALIZED";
    case MT4ErrorCode::ManagerNotInitialized:
        return "MANAGER_NOT_INITIALIZED";
    case MT4ErrorCode::PumpingActive:
        return "PUMPING_ACTIVE";
    case MT4ErrorCode::NotFound:
        return "NOT_FOUND";
    case MT4ErrorCode::Mt4RequestFailed:
        return "MT4_REQUEST_FAILED";
    default:
        return "UNKNOWN";
    }
}

class MT4Error : public std::runtime_error
{
public:
    MT4Error(MT4ErrorCode code, const std::string &message)
        : std::runtime_error(message), code_(code)
    {
    }

    MT4ErrorCode Code() const
    {
        return code_;
    }

    const char *CodeString() const
    {
        return MT4ErrorCodeToString(code_);
    }

private:
    MT4ErrorCode code_;
};

class CManagerInterface;

void ThrowMt4Error(
    const char *action,
    int code,
    CManagerInterface *manager = nullptr);
