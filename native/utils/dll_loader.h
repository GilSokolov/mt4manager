#pragma once

#include <windows.h>

#include <stdexcept>
#include <string>

HMODULE LoadDynamicLibrary(const std::wstring &dllPath);
void FreeDynamicLibrary(HMODULE module);

template <typename T>
T LoadSymbol(HMODULE module, const char *symbolName)
{
    FARPROC proc = GetProcAddress(module, symbolName);
    if (!proc)
    {
        throw std::runtime_error(std::string("Missing DLL export: ") + symbolName);
    }

    return reinterpret_cast<T>(proc);
}