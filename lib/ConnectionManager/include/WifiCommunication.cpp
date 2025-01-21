#ifndef WIFICOMMUNICATION_H
#define WIFICOMMUNICATION_H

#include <WiFi.h>
#include "BaseConnection.h"

class WiFiCommunication : public BaseConnection {
public:
    // Constructor
    WiFiCommunication(const std::string& ssid, const std::string& password)
        : BaseConnection("WiFi Communication", ConnectionType::WiFi, ssid, password) {}

    // Override methods from BaseConnection
    bool begin() override {
        Serial.println("Initializing WiFi...");
        WiFi.begin(networkName.c_str(), password.c_str());

        int retries = 10;
        while (WiFi.status() != WL_CONNECTED && retries-- > 0) {
            delay(1000);
            Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWiFi connected successfully.");
            return true;
        } else {
            setErrorMessage("Failed to connect to WiFi.");
            Serial.println("\nFailed to connect to WiFi.");
            return false;
        }
    }

    void start() override {
        if (isConnected()) {
            Serial.println("WiFi is already connected.");
        } else {
            begin();
        }
    }

    void stop() override {
        if (isConnected()) {
            Serial.println("Disconnecting WiFi...");
            WiFi.disconnect();
        } else {
            Serial.println("WiFi is not connected.");
        }
    }

    bool isConnected() const override {
        return WiFi.status() == WL_CONNECTED;
    }

    // Additional communication-specific methods
    bool sendData(const String& server, uint16_t port, const String& data) {
        if (!isConnected()) {
            setErrorMessage("WiFi not connected.");
            return false;
        }

        WiFiClient client;
        if (!client.connect(server.c_str(), port)) {
            setErrorMessage("Failed to connect to server.");
            return false;
        }

        client.print(data);
        client.stop();
        return true;
    }

    String receiveData(const String& server, uint16_t port) {
        if (!isConnected()) {
            setErrorMessage("WiFi not connected.");
            return "";
        }

        WiFiClient client;
        if (!client.connect(server.c_str(), port)) {
            setErrorMessage("Failed to connect to server.");
            return "";
        }

        String response = "";
        while (client.available()) {
            response += client.readString();
        }

        client.stop();
        return response;
    }
};

#endif // WIFICOMMUNICATION_H
