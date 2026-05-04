#pragma once

#include <memory>
#include <mutex>
#include <utility>

#include "mt4_client.h"

#include "utils/mt4_log.h"

template <typename Derived, typename Handler>
class MT4PumpSubscriber : public std::enable_shared_from_this<Derived>
{
public:
    static std::shared_ptr<Derived> CreateShared(
        const std::shared_ptr<MT4Client> &client)
    {
        auto ptr = std::shared_ptr<Derived>(new Derived(client));
        ptr->AttachPumpListener();
        return ptr;
    }

    void SetHandler(Handler handler)
    {
        std::lock_guard<std::mutex> lock(handler_mutex_);
        handler_ = std::move(handler);
    }

protected:
    explicit MT4PumpSubscriber(const std::shared_ptr<MT4Client> &client)
        : client_(client)
    {
    }

    // Important: do NOT detach here yet.
    ~MT4PumpSubscriber() = default;

    void DetachPumpListener()
    {
        if (client_ && pump_listener_id_ != -1)
        {
            MT4_DEBUG_LOG("Removing pump listener id=" << pump_listener_id_);
            client_->RemovePumpListener(pump_listener_id_);
            pump_listener_id_ = -1;
        }
    }

    Handler GetHandlerCopy() const
    {
        std::lock_guard<std::mutex> lock(handler_mutex_);
        return handler_;
    }

    std::shared_ptr<MT4Client> client_;

private:
    void AttachPumpListener()
    {
        MT4_DEBUG_LOG("Attaching pump listener");

        std::weak_ptr<Derived> weak_self = this->weak_from_this();

        pump_listener_id_ = client_->AddPumpListener(
            [weak_self](int code, int type, void *data)
            {
                if (auto self = weak_self.lock())
                {
                    self->HandleEvent(code, type, data);
                }
            });

        MT4_DEBUG_LOG("Attached pump listener id=" << pump_listener_id_);
    }

private:
    Handler handler_;
    mutable std::mutex handler_mutex_;
    int pump_listener_id_ = -1;
};