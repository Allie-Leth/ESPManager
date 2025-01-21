#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <vector>
#include "BaseSensor.h"
#include "DHTSensor.h"
#include "BatteryZenerSensor.h"

struct SensorData {
    float temperature;
    float humidity;
    bool isValid;
    unsigned long lastReadTime; // Timestamp of the last successful read
};

class SensorManager {
public:
    // Constructor
    explicit SensorManager();

    // Register a sensor by type and pin, dynamically creating the sensor
    bool registerSensor(int pin, BaseSensor::SensorType type, const String& sensorName);

    bool registerSensor(int pin, float highVoltage = 4.2, float lowVoltage = 2.5);
    // Scan for sensors on default pins
    void scanForSensors();

    // Start concurrent reading tasks
    void startConcurrentReading();

    // Get the latest sensor data
    bool getSensorData(int index, float& temperature, float& humidity);

private:
    void waitForSensor(BaseSensor& sensor); // Waits for the sensor to refresh

    static void sensorTask(void* parameters); // Task function for FreeRTOS

    std::vector<BaseSensor*> sensors;  // Vector of sensor pointers
    std::vector<SensorData> sensorResults; // Vector to store sensor data
    const std::vector<int> defaultPins = {26, 27}; // Default pins for DHT22 and other sensors
    const unsigned long refreshInterval = 2000; // 2 seconds between sensor reads
};

#endif // SENSORMANAGER_H
