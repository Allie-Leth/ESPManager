#include "FileSystemManager.h"
#include "FileSystem.h"
#include <LogLevel.h>
#include <Arduino.h>
#include <LittleFS.h>
#include <string>
#include "FS.h"

#define FORMAT_LITTLEFS_IF_FAILED true

FileSystemManager::FileSystemManager(std::function<void(LogLevel, const std::string&)> registerLogMethod) {
    // Register the logging method with the provided function
    this->logMethod = registerLogMethod;
    if (logMethod) {
        logMethod(LogLevel::SETUP, "FileSystemManager initialized.");
    }
}

bool FileSystemManager::begin() {
    if (logMethod) {
        logMethod(LogLevel::SETUP, "FileSystemManager begin called.");
    }
    return LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED);
}

void FileSystemManager::flushBufferToFile(const std::vector<std::string>& buffer, const std::string& path) {
    File file = LittleFS.open(path.c_str(), "a");
    if (!file) {
        if (logMethod) {
            logMethod(LogLevel::ERROR, "Failed to open log file for writing: " + path);
        }
        return;
    }

    for (const auto& log : buffer) {
        file.println(log.c_str());
    }

    file.close();
    if (logMethod) {
        logMethod(LogLevel::INFO, "Buffer flushed to file: " + path);
    }
}

bool FileSystemManager::write(const std::string& path, const std::string& data) {
    File file = LittleFS.open(path.c_str(), "w");
    if (!file) {
        if (logMethod) {
            logMethod(LogLevel::CRITICAL, "Failed to open file for writing: " + path);
        }
        return false;
    }
    if (file.print(data.c_str()) == 0) {
        if (logMethod) {
            logMethod(LogLevel::CRITICAL, "Failed to write data to file: " + path);
        }
        file.close();
        return false;
    }
    file.close();
    if (logMethod) {
        logMethod(LogLevel::INFO, "File written successfully: " + path);
    }
    return true;
}

std::string FileSystemManager::read(const std::string& path) {
    File file = LittleFS.open(path.c_str(), "r");
    if (!file) {
        if (logMethod) {
            logMethod(LogLevel::ERROR, "Failed to open file for reading: " + path);
        }
        return "";
    }

    std::string content;
    while (file.available()) {
        content += static_cast<char>(file.read());
    }
    file.close();

    if (logMethod) {
        logMethod(LogLevel::INFO, "File read successfully: " + path);
    }
    return content;
}

bool FileSystemManager::remove(const std::string& path) {
    if (!LittleFS.remove(path.c_str())) {
        if (logMethod) {
            logMethod(LogLevel::ERROR, "Failed to remove file: " + path);
        }
        return false;
    }
    if (logMethod) {
        logMethod(LogLevel::INFO, "File removed successfully: " + path);
    }
    return true;
}

bool FileSystemManager::exists(const std::string& path) {
    bool result = LittleFS.exists(path.c_str());
    if (logMethod) {
        logMethod(LogLevel::INFO, "File exists check for path: " + path + ", result: " + (result ? "true" : "false"));
    }
    return result;
}

bool FileSystemManager::createFileIfNotExists(const std::string& path) {
    if (!LittleFS.exists(path.c_str())) {
        File file = LittleFS.open(path.c_str(), "w");
        if (!file) {
            if (logMethod) {
                logMethod(LogLevel::CRITICAL, "Failed to create file: " + path);
            }
            return false;
        }
        file.close();
        if (logMethod) {
            logMethod(LogLevel::INFO, "File created: " + path);
        }
    }
    return true;
}
