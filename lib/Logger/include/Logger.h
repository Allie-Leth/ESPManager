#ifndef LOGGER_H
#define LOGGER_H

#include "LogLevel.h"
#include <vector>
#include <functional>
#include <string>

/**
 * @brief A global logger for managing and routing log messages.
 * 
 * The Logger class provides centralized logging functionality, allowing messages
 * to be categorized by severity levels and dynamically routed to various backends (handlers).
 */
class Logger {
public:
    /**
     * @brief A callable type for log handlers.
     * 
     * Each handler is a function or callable object that processes a log message.
     * The handler takes two parameters:
     * - `LogLevel`: The severity level of the message.
     * - `std::string`: The log message content.
     */
    using LogHandler = std::function<void(LogLevel, const std::string&)>;

    /**
     * @brief Adds a log handler to the logger.
     * 
     * Handlers process log messages and route them to a specific backend (e.g., file system, serial, network).
     * 
     * @param handler A callable object that processes log messages.
     * @param priority The priority of the handler (higher priorities execute first). Default is 0.
     */
    static void addHandler(LogHandler handler, int priority = 0);

    /**
     * @brief Removes a log handler by its unique ID.
     * 
     * This method removes a previously registered handler from the logger,
     * ensuring it no longer processes messages.
     * 
     * @param handlerId The unique ID of the handler to remove.
     */
    static void removeHandler(const std::string& handlerId);

    /**
     * @brief Logs a message to all registered handlers.
     * 
     * Messages are filtered based on the global log level. Only messages with a severity
     * level greater than or equal to the global log level are processed by the handlers.
     * 
     * @param level The severity level of the log message.
     * @param message The content of the log message.
     */
    static void log(LogLevel level, const std::string& message);

    /**
     * @brief Sets the global log level.
     * 
     * The global log level acts as a filter to suppress less severe messages.
     * Only messages with a severity greater than or equal to the global level are processed.
     * 
     * @param level The new global log level to set.
     */
    static void setGlobalLogLevel(LogLevel level);

    /**
     * @brief Flushes all registered handlers.
     * 
     * This method is a placeholder for handlers that support flushing, such as file-based handlers
     * that may need to persist buffered logs to disk.
     */
    static void flush();

private:
    /**
     * @brief A wrapper for log handlers with metadata.
     * 
     * Each handler is stored with a unique ID and a priority to determine its execution order.
     */
    struct HandlerWrapper {
        LogHandler handler; /**< The log handler function. */
        std::string id;     /**< A unique identifier for the handler. */
        int priority;       /**< The priority of the handler (higher values are executed first). */
    };

    static LogLevel globalLogLevel; /**< The global log level filter. */
    static std::vector<HandlerWrapper> handlers; /**< A collection of registered handlers. */

    /**
     * @brief Sorts handlers by their priority.
     * 
     * Ensures that higher-priority handlers are executed before lower-priority ones.
     */
    static void sortHandlers();
};

#endif // LOGGER_H
