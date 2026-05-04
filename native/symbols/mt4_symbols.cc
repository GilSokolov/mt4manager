#include "./mt4_symbols.h"

#include <stdexcept>
#include <utility>

#include "../mt4_client.h"
#include "../utils/mt4_errors.h"
#include "../utils/mt4_log.h"
#include "../utils/napi_converter_utils.h"

MT4Symbols::MT4Symbols(const std::shared_ptr<MT4Client> &client)
    : MT4PumpSubscriber(client)
{
    MT4_DEBUG_LOG("MT4Symbols created");
}

MT4Symbols::~MT4Symbols()
{
    MT4_DEBUG_LOG("MT4Symbols destroyed");
    DetachPumpListener();
}

SymbolInfo MT4Symbols::Get(const std::string &symbol) const
{
    if (!client_)
    {
        MT4_ERROR_LOG("Symbols.Get failed: client is not initialized");
        throw std::runtime_error("MT4 client is not initialized");
    }

    CManagerInterface *manager = client_->Manager();
    if (!manager)
    {
        MT4_ERROR_LOG("Symbols.Get failed: manager is not initialized");
        throw std::runtime_error("MT4 manager is not initialized");
    }

    MT4_DEBUG_LOG("Symbols.Get begin symbol=" << symbol);

    SymbolInfo info{};
    const int code = manager->SymbolInfoGet(symbol.c_str(), &info);

    MT4_DEBUG_LOG("SymbolInfoGet returned code=" << code
                                                 << " symbol=" << symbol
                                                 << " resultSymbol=" << info.symbol);

    ThrowMt4Error("SymbolInfoGet", code, manager);

    if (info.symbol[0] == '\0')
    {
        MT4_ERROR_LOG("Symbols.Get failed: empty symbol returned symbol=" << symbol);
        throw std::runtime_error("SymbolInfoGet returned empty symbol");
    }

    MT4_DEBUG_LOG(
        "Symbols.Get success symbol=" << info.symbol
                                      << " bid=" << info.bid
                                      << " ask=" << info.ask
                                      << " digits=" << info.digits);

    return info;
}

std::vector<ConSymbol> MT4Symbols::GetAll() const
{
    if (!client_)
    {
        MT4_ERROR_LOG("Symbols.GetAll failed: client is not initialized");
        throw std::runtime_error("MT4 client is not initialized");
    }

    CManagerInterface *manager = client_->Manager();
    if (!manager)
    {
        MT4_ERROR_LOG("Symbols.GetAll failed: manager is not initialized");
        throw std::runtime_error("MT4 manager is not initialized");
    }

    int total = 0;

    MT4_DEBUG_LOG("Symbols.GetAll begin");

    ConSymbol *records = manager->SymbolsGetAll(&total);

    MT4_DEBUG_LOG("SymbolsGetAll returned total=" << total << " records=" << records);

    if (!records || total <= 0)
    {
        if (records)
            manager->MemFree(records);

        throw std::runtime_error("No symbols available (is pumping enabled with ticks?)");
    }

    std::vector<ConSymbol> result;
    result.reserve(total);

    for (int i = 0; i < total; ++i)
    {
        result.push_back(records[i]);
    }

    manager->MemFree(records);

    return result;
}

void MT4Symbols::HandleEvent(int code, int type, void *data)
{
    if (code != PUMP_UPDATE_BIDASK)
    {
        return;
    }

    auto handler = GetHandlerCopy();

    if (!handler)
    {
        return;
    }

    CManagerInterface *manager = client_->Manager();
    if (!manager)
    {
        return;
    }

    SymbolInfo symbols[32];

    while (true)
    {
        int count = 32;
        count = manager->SymbolInfoUpdated(symbols, count);

        if (count <= 0)
        {
            break;
        }

        MT4_DEBUG_LOG("Symbols.HandleEvent updated count=" << count);

        for (int i = 0; i < count; ++i)
        {
            MT4_DEBUG_LOG(
                "Symbol tick symbol=" << symbols[i].symbol
                                      << " bid=" << symbols[i].bid
                                      << " ask=" << symbols[i].ask);

            handler(&symbols[i]);
        }

        if (count < 32)
        {
            break;
        }
    }
}

void MT4Symbols::Subscribe(const std::string &symbol) const
{
    CManagerInterface *manager = client_->Manager();
    if (!manager)
    {
        throw std::runtime_error("MT4 manager is not initialized");
    }

    MT4_DEBUG_LOG("Symbols.Subscribe symbol=" << symbol);

    const int code = manager->SymbolAdd(symbol.c_str());
    ThrowMt4Error("SymbolAdd", code, manager);
}

void MT4Symbols::Unsubscribe(const std::string &symbol) const
{
    CManagerInterface *manager = client_->Manager();
    if (!manager)
    {
        throw std::runtime_error("MT4 manager is not initialized");
    }

    MT4_DEBUG_LOG("Symbols.Unsubscribe symbol=" << symbol);

    const int code = manager->SymbolHide(symbol.c_str());
    ThrowMt4Error("SymbolHide", code, manager);
}