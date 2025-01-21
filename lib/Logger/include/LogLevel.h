#ifndef LOGLEVEL_H
#define LOGLEVEL_H

#include <string>

/**
 * @brief Enum for standard log levels.
 */
enum class LogLevel {
    SETUP,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

/**
 * @brief Converts a LogLevel to its string representation.
 * 
 * @param level The log level to convert.
 * @return The string representation of the log level.
 */
inline std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::SETUP: return "SETUP";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

#endif // LOGLEVEL_H