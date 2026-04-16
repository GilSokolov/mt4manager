#include "utils/win32_error.h"

std::string GetWin32ErrorMessage(DWORD code)
{
    LPSTR raw = nullptr;

    DWORD size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&raw),
        0,
        nullptr);

    if (size == 0 || raw == nullptr)
    {
        return "Unknown Win32 error";
    }

    std::string message(raw, raw + size);
    LocalFree(raw);
    return message;
}