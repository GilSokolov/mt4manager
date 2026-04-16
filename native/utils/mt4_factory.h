#pragma once

#include <windows.h>
#include <string>

#include "../include/mt4_sdk.h"

struct Mt4Factory
{
    HMODULE dll{nullptr};

    int (*versionFn)() = nullptr;
    int (*createFn)(int version, CManagerInterface **man) = nullptr;

    bool winsockStarted{false};
};

// Create and initialize the MT4 factory
Mt4Factory CreateMt4Factory(const std::wstring &dllPath);

// Destroy factory (frees DLL)
void DestroyMt4Factory(Mt4Factory &factory) noexcept;