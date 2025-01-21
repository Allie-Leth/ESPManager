#ifndef BASESENSOR_H
#define BASESENSOR_H

#include <string>

class BaseSensor {
public:
    enum class SensorType {
        DHT,
        BatteryZener,
        BatteryVoltage,
        Undefined, // Fallback for sensors that are not yet implemented
    };

    BaseSensor(int sensorPin, const std::string& sensorName, SensorType type = SensorType::Undefined)
        : name(sensorName), sensorPin(sensorPin), sensorType(type) {}

    SensorType getType() const { return sensorType; }

    virtual bool begin() = 0;
    virtual ~BaseSensor() = default;

    // Pure virtual method to get the sensor reading as a float (mandatory for all sensors)
    virtual float getReading() const = 0;

    // Optional method to get the sensor reading with a synchronization flag
    virtual float getReading(const bool* readyToReport) const {
        // Default implementation: Alert user that async is not supported
        logUnsupportedAsync();
        return -1.0; // Magic number error value indicating async not supported
    }

    // Virtual method to get the last error message
    virtual const char* getErrorMessage() const { return "Async not implemented for this sensor."; }

    // Method to get the sensor's name
    const std::string& getName() const { return name; }

protected:
    std::string name;  // Sensor name or type
    int sensorPin;
    SensorType sensorType;

    // Helper method to log an error or debug message
    virtual void logUnsupportedAsync() const {
        // For Arduino, you could use Serial logging or other debugging tools
        Serial.print("Error: Async getReading not implemented for sensor ");
        Serial.println(name.c_str());
    }
};

#endif // BASESENSOR_H
