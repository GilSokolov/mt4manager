#include "utils/dll_loader.h"
#include "utils/win32_error.h"

HMODULE LoadDynamicLibrary(const std::wstring &dllPath)
{
    HMODULE module = LoadLibraryW(dllPath.c_str());
    if (!module)
    {
        throw std::runtime_error(
            "Failed to load MT4 Manager DLL: " +
            GetWin32ErrorMessage(GetLastError()));
    }

    return module;
}

void FreeDynamicLibrary(HMODULE module)
{
    if (module)
    {
        FreeLibrary(module);
    }
}