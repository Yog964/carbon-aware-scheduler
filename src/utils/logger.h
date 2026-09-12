#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>

namespace carbongrid {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

/**
 * @brief Simple logging singleton for the CarbonGrid system.
 * Single-threaded simulation — no mutex needed.
 */
class Logger {
private:
    LogLevel min_level_ = LogLevel::INFO;

    Logger() = default;

    std::string level_to_string(LogLevel level) const {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

public:
    static Logger& get_instance() {
        static Logger instance;
        return instance;
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void set_min_level(LogLevel level) {
        min_level_ = level;
    }

    void log(LogLevel level, const std::string& message) {
        if (level < min_level_) return;

        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);

        std::cout << "[" << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S") << "] "
                  << "[" << level_to_string(level) << "] "
                  << message << std::endl;
    }
};

} // namespace carbongrid
