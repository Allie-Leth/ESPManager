#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include <string>
#include <functional>
#include "Logger.h"

/**
 * @brief Template-based logging handler for dynamic backends.
 * 
 * This class processes log messages based on a user-defined function for 
 * output (e.g., writing to a file, serial output, or network transmission).
 * 
 * @tparam WriteFunc A callable type (e.g., lambda, function pointer) for writing logs.
 */
template <typename WriteFunc>
class LogHandler {
public:
    /**
     * @brief Constructs a LogHandler instance.
     * 
     * @param writeFunc The function or callable to handle writing logs.
     * @param minLevel The minimum log level to process. Defaults to Logger::LogLevel::INFO.
     */
    explicit LogHandler(WriteFunc writeFunc, Logger::LogLevel minLevel = Logger::LogLevel::INFO)
        : writeFunction(writeFunc), minLogLevel(minLevel) {}

    /**
     * @brief Destructor for the LogHandler.
     */
    ~LogHandler() = default;

    /**
     * @brief Processes a log message if the log level meets or exceeds the minimum level.
     * 
     * @param level The log level of the message.
     * @param message The log message to process.
     */
    void handleLog(Logger::LogLevel level, const std::string& message) {
        if (shouldLog(level)) {
            std::string formattedMessage = "[" + Logger::logLevelToString(level) + "] " + message;
            writeFunction(formattedMessage);
        }
    }

    /**
     * @brief Updates the minimum log level dynamically.
     * 
     * @param level The new minimum log level to set.
     */
    void setMinLogLevel(Logger::LogLevel level) {
        minLogLevel = level;
    }

    /**
     * @brief Retrieves the current minimum log level.
     * 
     * @return The current minimum log level.
     */
    Logger::LogLevel getMinLogLevel() const {
        return minLogLevel;
    }

private:
    WriteFunc writeFunction;  /**< The callable function to handle log output. */
    Logger::LogLevel minLogLevel;  /**< The minimum log level to process messages. */

    /**
     * @brief Determines if the log level is allowed to be processed.
     * 
     * @param level The log level to check.
     * @return True if the log level is greater than or equal to the minimum log level.
     */
    bool shouldLog(Logger::LogLevel level) const {
        return level >= minLogLevel;
    }
};

#endif // LOGHANDLER_H
