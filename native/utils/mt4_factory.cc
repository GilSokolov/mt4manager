#include "utils/mt4_factory.h"

#include <stdexcept>
#include <iostream>

#include "utils/dll_loader.h"
#include "utils/mt4_errors.h"

namespace
{
    void StartWinsock(Mt4Factory &factory)
    {
        WSADATA wsa{};
        const int rc = WSAStartup(MAKEWORD(2, 2), &wsa);
        if (rc != 0)
        {
            throw std::runtime_error(
                "WSAStartup failed with WinSock error code " + std::to_string(rc));
        }
        factory.winsockStarted = true;
    }

    void StopWinsock(Mt4Factory &factory) noexcept
    {
        if (factory.winsockStarted)
        {
            WSACleanup();
            factory.winsockStarted = false;
        }
    }
} // namespace

Mt4Factory CreateMt4Factory(const std::wstring &dllPath)
{
    std::cerr << "[mt4] CreateMt4Factory start" << std::endl;

    Mt4Factory factory;

    StartWinsock(factory);
    std::cerr << "[mt4] winsock started" << std::endl;

    factory.dll = LoadDynamicLibrary(dllPath);
    std::cerr << "[mt4] dll loaded" << std::endl;

    try
    {
        factory.versionFn = LoadSymbol<int (*)()>(factory.dll, "MtManVersion");
        std::cerr << "[mt4] MtManVersion resolved" << std::endl;

        factory.createFn =
            LoadSymbol<int (*)(int, CManagerInterface **)>(factory.dll, "MtManCreate");
        std::cerr << "[mt4] MtManCreate resolved" << std::endl;
    }
    catch (...)
    {
        DestroyMt4Factory(factory);
        throw;
    }

    std::cerr << "[mt4] CreateMt4Factory done" << std::endl;
    return factory;
}

void DestroyMt4Factory(Mt4Factory &factory) noexcept
{
    if (factory.dll)
    {
        FreeDynamicLibrary(factory.dll);
        factory.dll = nullptr;
    }

    factory.versionFn = nullptr;
    factory.createFn = nullptr;

    StopWinsock(factory);
}