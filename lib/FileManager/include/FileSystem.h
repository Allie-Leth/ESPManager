#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <LittleFS.h>
#include <string>

class FileSystem {
public:
    // Constructor
    FileSystem();

    // Writes data to a file 
    bool write(bool OverwriteFile, const std::string& path, const std::string& data);
    bool write(const std::string& path, const std::string& data); // Does not overwrite file by default

    // Read data from a file
    std::string read(const std::string& path);

    // Delete a file 
    bool remove(const std::string& path);

    // Check if a file exists
    bool exists(const std::string& path);

private:
    void logError(const std::string& operation, const std::string& path); // Log errors
    void createInitialFiles(); // Create initial files on first boot
    void parseFirstBootFile(const std::string& content); // Parse the first boot file
    bool isFirstBoot() const; // Check if a file is the first boot file

    int bootFailCount; // Number of failed boot operations
    bool firstBoot;    // Flag to indicate first boot

};

#endif // FILESYSTEM_H
