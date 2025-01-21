#include "Logger.h"
#include <algorithm>
#include <vector>

// Initialize static members
LogLevel Logger::globalLogLevel = LogLevel::INFO;
std::vector<Logger::HandlerWrapper> Logger::handlers;

void Logger::addHandler(LogHandler handler, int priority) {
    handlers.push_back({handler, "", priority});
    sortHandlers();
}

void Logger::removeHandler(const std::string& handlerId) {
    handlers.erase(
        std::remove_if(handlers.begin(), handlers.end(),
                       [&handlerId](const HandlerWrapper& wrapper) {
                           return wrapper.id == handlerId;
                       }),
        handlers.end());
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < globalLogLevel) {
        return; // Skip logs below the global log level
    }

    for (const auto& wrapper : handlers) {
        wrapper.handler(level, message);
    }
}

void Logger::setGlobalLogLevel(LogLevel level) {
    globalLogLevel = level;
}

void Logger::flush() {
    for (const auto& wrapper : handlers) {
        // Placeholder for flush logic if handlers require it
    }
}

void Logger::sortHandlers() {
    std::sort(handlers.begin(), handlers.end(),
              [](const HandlerWrapper& a, const HandlerWrapper& b) {
                  return a.priority > b.priority; // Higher priority handlers first
              });
}
