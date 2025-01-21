#include "FileSystem.h"
#include <Arduino.h> // For Serial output
#include <LittleFS.h>
#include <string>

// Constructor: Initializes the file system
FileSystem::FileSystem() : firstBoot(true), bootFailCount(0) {
    Serial.println("Mounting File System...");


    if (!LittleFS.begin()) {
        Serial.println("LittleFS failed to initialize. Formatting...");
        if (!LittleFS.format()) {
            Serial.println("LittleFS failed to format. Aborting initialization.");
            return;
        }
        else {
            Serial.println("LittleFS formatted successfully.");
        }

        // Retry mounting after formatting
        if (!LittleFS.begin()) {
            Serial.println("LittleFS failed to initialize after formatting. Aborting initialization.");
            return;
        }
        else {
            Serial.println("LittleFS initialized successfully after formatting.");
        }
    } else {
        Serial.println("LittleFS Mounted successfully.");
        Serial.println("Checking if first boot...");
        parseFirstBootFile(read("/logs/firstBoot.txt"));
    }

    // Validate if first boot
    // Check if /logs/firstBoot.txt exists
    Serial.println("LittleFS initialized successfully.");
    Serial.println("Verifying file system integrity...");

    // Check for the other three files
    if (!exists("/logs/data.txt")) {
        Serial.println("Data file not found. Creating...");
        write("/logs/data.txt", "");
        if (!exists("/logs/data.txt")) {
            Serial.println("Failed to create data file.");
        }
    } else {
        Serial.println("data.txt already exists.");
    }

    if (!exists("/logs/error.txt")) {
        Serial.println("Error file not found. Creating...");
        write("/logs/error.txt", "");
        if (!exists("/logs/error.txt")) {
            Serial.println("Failed to create error file.");
        }
    } else {
        Serial.println("error.txt already exists.");
    }

    if (!exists("/logs/info.txt")) {
        Serial.println("Info file not found. Creating...");
        write("/logs/info.txt", "");
        // Check if the info file was created successfully
        if (!exists("/logs/info.txt")) {
            Serial.println("Failed to create info file.");
        }
    } else {
        Serial.println("info.txt already exists.");
    }

    Serial.println("File system initialization complete.");
}


/** 
 * @brief This writes to a file with the option to overwrite the file.
 * @param overwriteFile If true, the file will be overwritten.
 * @param path The path to the file
 * @param data The data to write to the file
 */
bool FileSystem::write(bool overwriteFile, const std::string& path, const std::string& data) {
    const char* mode = overwriteFile ? "w" : "a"; // Use "a" for append, "w" for overwrite
    File file = LittleFS.open(path.c_str(), mode);
    if (!file) {
        return false;
    }
    file.print(data.c_str());
    file.close();
    return true;
}


/**
 * @brief Writes data to a file. This does not overwrite the file by default.
 * @param path The path to the file
 * @param data The data to write to the file
 */
bool FileSystem::write(const std::string& path, const std::string& data) {
    File file = LittleFS.open(path.c_str(), "a");
    if (!file) {
        return false;
    }
    file.print(data.c_str());
    file.close();
    return true;
}



// Reads data from a file and returns it as a string
std::string FileSystem::read(const std::string& path) {
    File file = LittleFS.open(path.c_str(), "r");
    if (!file) {
        return "";
    }
    std::string content;
    while (file.available()) {
        content += static_cast<char>(file.read());
    }
    file.close();
    return content;
}

// Removes a file
bool FileSystem::remove(const std::string& path) {
    return LittleFS.remove(path.c_str());
}

// Checks if a file exists
bool FileSystem::exists(const std::string& path) {
    return LittleFS.exists(path.c_str());
}

// Create initial files on first boot
void FileSystem::createInitialFiles() {
    // Ensure the /logs/ directory exists
    if (!LittleFS.exists("/logs")) {
        Serial.println("Creating /logs directory...");
        if (!LittleFS.mkdir("/logs")) {
            Serial.println("Failed to create /logs directory. Cannot proceed with file creation.");
            return;
        }
    } else {
        Serial.println("/logs directory already exists.");
    }

    // Create data file
    if (!write("/logs/data.txt", "")) {
        Serial.println("Failed to create data file.");
    }

    // Create error file
    if (!write("/logs/error.txt", "")) {
        Serial.println("Failed to create error file.");
    }

    // Create info file
    if (!write("/logs/info.txt", "")) {
        Serial.println("Failed to create info file.");
    }

    // Create first boot marker file
    if (!write("/logs/firstBoot.txt", "First Boot: true")) {
        Serial.println("Failed to create first boot marker file.");
    }

}

// Parses the firstBoot.txt file to extract tracking information
void FileSystem::parseFirstBootFile(const std::string& content) {
    if (content.find("First Boot: true") != std::string::npos) {
        firstBoot = false;
        // overwrite the first boot file
        write(true, "/logs/firstBoot.txt", "First Boot: false");
    } else if (content.find("First Boot: false") != std::string::npos) {
        Serial.println("First boot file found, device has booted before.");
        firstBoot = false;
    } else {
        Serial.println("First boot file found, but could not determine boot status.");
        Serial.println("Content of first boot file: ");
        String fileContents = read("/logs/firstBoot.txt").c_str();
        Serial.println(fileContents);
        firstBoot = false;
    }

    size_t failIndex = content.find("Failed to create files:");
    if (failIndex != std::string::npos) {
        std::string failCountStr = content.substr(failIndex + 23); // 23 = length of "Failed to create files: "
        bootFailCount = std::stoi(failCountStr);
    } else {
        bootFailCount = 0;
    }
}

// Getter for the first boot status
bool FileSystem::isFirstBoot() const {
    return firstBoot;
}
