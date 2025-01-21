#ifndef CONNTYPE_H
#define CONNTYPE_H

#include <string>

/**
 * @brief Enum for connection types
 */
enum class ConnType {
    BLE,
    MeshBLE,
    MeshWiFi,
    WiFi,
    LoRa,
    Undefined
};

/**
 * @brief Converts a LogLevel to its string representation.
 * 
 * @param level The log level to convert.
 * @return The string representation of the log level.
 */
inline std::string logLevelToString(ConnType ConnTypes) {
    switch (ConnTypes) {
        case ConnType::BLE: return "BLE";
        case ConnType::MeshBLE: return "MeshBLE";
        case ConnType::MeshWiFi: return "MeshWiFi";
        case ConnType::WiFi: return "WiFi";
        case ConnType::LoRa: return "LoRa";
        case ConnType::Undefined: return "Undefined";
        default: return "UNKNOWN";
    }
}

#endif // CONNTYPES_H