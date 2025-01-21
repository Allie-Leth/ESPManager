#ifndef BASECONNECTION_H
#define BASECONNECTION_H

#include <string>
#include <ConnType.h>


class BaseConnection {
public:


    // Constructor
    BaseConnection(const std::string& name, ConnType type = ConnType::Undefined,
                   const std::string& networkName = "", const std::string& password = "")
        : connectionName(name), connectionType(type), networkName(networkName), password(password), lastError("") {}

    virtual ~BaseConnection() = default;

    // Connection management
    virtual bool begin() = 0;             // Initialize the connection
    virtual void start() = 0;             // Activate the connection
    virtual void stop() = 0;              // Deactivate the connection
    virtual bool isConnected() const = 0; // Check connection status
    virtual void reset() {
        stop();
        begin();
    }

    // Utility methods
    std::string getName() const { return connectionName; }
    ConnType getType() const { return connectionType; }
    std::string getNetworkName() const { return networkName; }

    void setNetworkName(const std::string& name) { networkName = name; }
    void setPassword(const std::string& pwd) { password = pwd; }

    const char* getErrorMessage() const { return lastError.c_str(); }
    void setErrorMessage(const std::string& error) { lastError = error; }

protected:
    std::string connectionName;     // Human-readable name
    ConnType connectionType;  // Type of connection
    std::string networkName;        // Network SSID (or equivalent)
    std::string password;           // Network password
    std::string lastError;          // Stores the last error message
};

#endif // BASECONNECTION_H
