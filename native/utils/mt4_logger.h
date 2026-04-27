#pragma once

#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

enum class MT4LogLevel
{
    Error = 1,
    Info = 2,
    Debug = 3,
};

class MT4Logger
{
public:
    static MT4Logger &Instance()
    {
        static MT4Logger logger;
        return logger;
    }

    void SetLevel(MT4LogLevel level)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = level;
    }

    bool ShouldLog(MT4LogLevel level) const
    {
        return static_cast<int>(level_) >= static_cast<int>(level);
    }

    template <typename T>
    void Error(const T &msg)
    {
        Write(MT4LogLevel::Error, "[mt4][error] ", msg);
    }

    template <typename T>
    void Info(const T &msg)
    {
        Write(MT4LogLevel::Info, "[mt4] ", msg);
    }

    template <typename T>
    void Debug(const T &msg)
    {
        Write(MT4LogLevel::Debug, "[mt4][debug] ", msg);
    }

private:
    MT4Logger() = default;

    template <typename T>
    void Write(MT4LogLevel msgLevel, const char *prefix, const T &msg)
    {
        if (!ShouldLog(msgLevel))
            return;

        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << prefix << msg << std::endl;
    }

    MT4LogLevel level_ = MT4LogLevel::Error;
    mutable std::mutex mutex_;
};