#pragma once

#include <functional>

template <typename T>
class ScopedResource {
 public:
  ScopedResource(T* value, std::function<void(T*)> deleter)
      : value_(value), deleter_(std::move(deleter)) {}

  ~ScopedResource() {
    if (value_ != nullptr) {
      deleter_(value_);
    }
  }

  ScopedResource(const ScopedResource&) = delete;
  ScopedResource& operator=(const ScopedResource&) = delete;

  ScopedResource(ScopedResource&& other) noexcept
      : value_(other.value_), deleter_(std::move(other.deleter_)) {
    other.value_ = nullptr;
  }

  ScopedResource& operator=(ScopedResource&& other) noexcept {
    if (this != &other) {
      if (value_ != nullptr) {
        deleter_(value_);
      }
      value_ = other.value_;
      deleter_ = std::move(other.deleter_);
      other.value_ = nullptr;
    }
    return *this;
  }

  T* get() const { return value_; }
  T* release() {
    T* tmp = value_;
    value_ = nullptr;
    return tmp;
  }

 private:
  T* value_;
  std::function<void(T*)> deleter_;
};
