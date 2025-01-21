#include "DHTSensor.h"

// Initialize the sensor
bool DHTSensor::begin() {
    dht.begin();

    // Perform a test read to ensure initialization
    float temp = dht.readTemperature();
    if (isnan(temp)) {
        lastError = "Failed to initialize DHT sensor!";
        return false;
    }

    lastError = "";
    return true;
}

// Override to get temperature as the default reading
float DHTSensor::getReading() const {
    float temp = dht.readTemperature();
    if (isnan(temp)) {
        lastError = "Failed to read temperature!";
        return NAN;
    }
    lastError = "";
    return temp;
}

// Override to get the last error message
const char* DHTSensor::getErrorMessage() const {
    return lastError.c_str();
}

// Method to read both temperature and humidity
bool DHTSensor::readTempAndHumidity(float& temperature, float& humidity) {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        lastError = "Failed to read temperature and humidity!";
        return false;
    }

    lastError = "";
    return true;
}
