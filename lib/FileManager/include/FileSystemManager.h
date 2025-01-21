#ifndef FILESYSTEM_MANAGER_H
#define FILESYSTEM_MANAGER_H

#include "FileSystem.h"
#include "LogLevel.h"
#include <functional>
#include <string>
#include <vector>

/**
 * @brief Manages file system operations and logging.
 * 
 * Provides high-level abstractions for interacting with the LittleFS file system.
 * Logging is handled via an injectable logging function for flexibility.
 */
class FileSystemManager {
public:
    /**
     * @brief Constructor accepting a logging method.
     * 
     * @param registerLogMethod A function to register logging methods.
     */
    FileSystemManager(std::function<void(LogLevel, const std::string&)> registerLogMethod);

    /**
     * @brief Initializes the file system.
     * @return True if initialization is successful, otherwise false.
     */
    bool begin();

    /**
     * @brief Flushes a buffer of logs to a specified file.
     * @param buffer The buffer of log messages to write.
     * @param path The file path to write the buffer to.
     */
    void flushBufferToFile(const std::vector<std::string>& buffer, const std::string& path);

    /**
     * @brief Writes data to a file.
     * @param path The file path to write to.
     * @param data The data to write to the file.
     * @return True if the write is successful, otherwise false.
     */
    bool write(const std::string& path, const std::string& data);

    /**
     * @brief Reads data from a file.
     * @param path The file path to read from.
     * @return The file's content as a string.
     */
    std::string read(const std::string& path);

    /**
     * @brief Removes a file.
     * @param path The file path to remove.
     * @return True if the removal is successful, otherwise false.
     */
    bool remove(const std::string& path);

    /**
     * @brief Checks if a file exists.
     * @param path The file path to check.
     * @return True if the file exists, otherwise false.
     */
    bool exists(const std::string& path);

    /**
     * @brief Ensures a file exists, creating it if necessary.
     * @param path The file path to check or create.
     * @return True if the file exists or is created successfully, otherwise false.
     */
    bool createFileIfNotExists(const std::string& path);

private:
    std::function<void(LogLevel, const std::string&)> logMethod; /**< Logging provided by the Logging class on creation */
};

#endif // FILESYSTEM_MANAGER_H
