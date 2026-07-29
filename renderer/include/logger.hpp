#pragma once
#include <string_view>

class Logger {
   public:
    enum class Level { INFO, DEBUG, WARNING, ERROR, FATAL };
    bool debug;

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;

    void setMinLevel(Level level) { minLevel = level; }
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger& getInstance();

    void log(std::string_view message, Level level);

   private:
    Level minLevel;
    std::string_view getLevelAsString(Level level);
    Logger();
    ~Logger() = default;
};