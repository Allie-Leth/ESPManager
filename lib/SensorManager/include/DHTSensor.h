#ifndef DHTSENSOR_H
#define DHTSENSOR_H

#include <DHT.h>
#include "BaseSensor.h"

class DHTSensor : public BaseSensor {
public:
    // Constructor
    DHTSensor(int pin, int dhtType = DHT22)
        : BaseSensor(pin, "DHT Sensor", BaseSensor::SensorType::DHT), dht(pin, dhtType), lastError("") {}

    // Initialize the sensor
    bool begin() override;

    // Override the virtual method to get the temperature as the default reading
    float getReading() const override;

    // Override the virtual method to get the last error message
    const char* getErrorMessage() const override;

    // Method to read both temperature and humidity
    bool readTempAndHumidity(float& temperature, float& humidity);

private:
    mutable DHT dht;                  // DHT sensor object
    mutable std::string lastError;    // Stores the last error message
};

#endif // DHTSENSOR_H
