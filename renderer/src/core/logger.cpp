#include "logger.hpp"

#include <chrono>
#include <format>
#include <iostream>
#include <stdexcept>

Logger::Logger() : minLevel(Logger::Level::INFO), debug(false) {}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(std::string_view message, Logger::Level level) {
    if (level == Level::DEBUG && debug == false) return;
    if (level < minLevel) return;
    std::clog << std ::format("[{:%Y-%m-%d %H:%M:%S}] [RENDER] [{}] {}",
                              std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()),
                              getStringFromLevel(level), message)
              << std::endl;
}

std::string Logger::getStringFromLevel(Logger::Level level) {
    switch (level) {
        case Logger::Level::INFO:
            return "INFO";
        case Logger::Level::WARNING:
            return "WARN";
        case Logger::Level::ERROR:
            return "ERROR";
        case Logger::Level::FATAL:
            return "FATAL";
        case Logger::Level::DEBUG:
            return "DEBUG";
    }
    throw std::invalid_argument("Invalid log level");
}

Logger::Level Logger::getLevelFromString(const std::string& level) {
    if (level == "INFO") return Logger::Level::INFO;
    if (level == "WARNING" || level == "WARN") return Logger::Level::WARNING;
    if (level == "ERROR") return Logger::Level::ERROR;
    if (level == "DEBUG") return Logger::Level::DEBUG;
    if (level == "FATAL") return Logger::Level::FATAL;
    throw std::invalid_argument("Invalid log level");
}