#pragma once

#include <mutex>
#include <unordered_set>

template <typename Key>
class SubscriptionSet
{
public:
    void Subscribe(const Key &key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        values_.insert(key);
    }

    void Unsubscribe(const Key &key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        values_.erase(key);
    }

    void UnsubscribeAll()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        values_.clear();
    }

    bool Contains(const Key &key) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return values_.find(key) != values_.end();
    }

    bool Empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return values_.empty();
    }

protected:
    std::unordered_set<Key> Snapshot() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return values_;
    }

private:
    mutable std::mutex mutex_;
    std::unordered_set<Key> values_;
};