#include "SensorManager.h"
#include <Arduino.h>
#include <Sensor.h>
#include <stdexcept>

// Constructor
SensorManager::SensorManager() {}

// Register a sensor by type and pin, dynamically creating the sensor
// Register a generic sensor by type and pin
bool SensorManager::registerSensor(int pin, BaseSensor::SensorType type, const String& sensorName) {
    BaseSensor* sensor = nullptr;

    switch (type) {
        case BaseSensor::SensorType::DHT:
            sensor = new DHTSensor(pin);
            if (!sensor->begin()) {
                Serial.print("Failed to initialize DHT Sensor on pin ");
                Serial.println(pin);
                delete sensor;
                return false;
            }
            break;

        default:
            Serial.println("Unsupported sensor type for this method.");
            return false;
    }

    sensors.push_back(sensor);
    sensorResults.push_back({NAN, NAN, false, 0}); // Initialize results
    Serial.print("Registered sensor: ");
    Serial.println(sensor->getName().c_str());
    return true;
}

// Overloaded: Register a battery sensor with custom high and low voltage
bool SensorManager::registerSensor(int pin, float highVoltage, float lowVoltage) {
    auto* batterySensor = new BatteryZenerSensor(highVoltage, lowVoltage, pin);
    if (batterySensor->getReading() < 0) {
        Serial.print("Failed to initialize Battery Sensor on pin ");
        Serial.println(pin);
        delete batterySensor;
        return false;
    }

    sensors.push_back(batterySensor);
    sensorResults.push_back({NAN, NAN, false, 0}); // Initialize results
    Serial.print("Battery sensor registered on pin ");
    Serial.println(pin);
    return true;
}

// Scan for sensors on default pins
void SensorManager::scanForSensors() {
    for (int pin : defaultPins) {
        Serial.print("Scanning pin ");
        Serial.println(pin);

        // Try to detect DHT sensors
        auto* dhtSensor = new DHTSensor(pin);
        if (dhtSensor->begin()) {
            Serial.print("DHT Sensor detected and initialized on pin ");
            Serial.println(pin);
            sensors.push_back(dhtSensor);
            sensorResults.push_back({NAN, NAN, false, 0}); // Initialize results
            continue;
        }
        delete dhtSensor;

        // Try to detect Battery sensors
        auto* batterySensor = new BatteryZenerSensor(4.2, 2.5, pin);
        if (batterySensor->getReading() >= 0) {
            Serial.print("Battery Sensor detected and initialized on pin ");
            Serial.println(pin);
            sensors.push_back(batterySensor);
            sensorResults.push_back({NAN, NAN, false, 0}); // Initialize results
            continue;
        }
        delete batterySensor;

        Serial.print("No sensor detected on pin ");
        Serial.println(pin);
    }
}

// Sensor reading task
void SensorManager::sensorTask(void* parameters) {
    SensorManager* manager = static_cast<SensorManager*>(parameters);

    while (true) {
        for (size_t i = 0; i < manager->sensors.size(); ++i) {
            BaseSensor* sensor = manager->sensors[i];
            SensorData& data = manager->sensorResults[i];

            if (sensor->getType() == BaseSensor::SensorType::DHT) {
                float temperature = NAN;
                float humidity = NAN;

                if (millis() - data.lastReadTime >= manager->refreshInterval) {
                    auto* dhtSensor = static_cast<DHTSensor*>(sensor);
                    dhtSensor->readTempAndHumidity(temperature, humidity);

                    if (!isnan(temperature) && !isnan(humidity)) {
                        data.temperature = temperature;
                        data.humidity = humidity;
                        data.isValid = true;
                        data.lastReadTime = millis();
                    } else {
                        data.isValid = false;
                    }
                }
            } else if (sensor->getType() == BaseSensor::SensorType::BatteryZener) {
                float batteryLevel = sensor->getReading();

                if (!isnan(batteryLevel)) {
                    data.temperature = batteryLevel; // Storing battery level in temperature field
                    data.isValid = true;
                    data.lastReadTime = millis();
                } else {
                    data.isValid = false;
                }
            } else if(sensor->getType() == BaseSensor::SensorType::BatteryVoltage) {
                // TODO: Add support for a raw battery voltage sensor
                throw std::logic_error("BatteryVoltage sensor type not yet implemented.");
            }

        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Wait 500ms before next iteration
    }
}

// Start concurrent reading tasks
void SensorManager::startConcurrentReading() {
    for (size_t i = 0; i < sensors.size(); ++i) {
        if (sensors[i]->getType() == BaseSensor::SensorType::DHT) {
            xTaskCreate(
                sensorTask,                  // Task function
                "SensorTask",                // Name of the task
                2048,                        // Stack size (in words)
                this,                        // Parameters to the task
                1,                           // Priority
                NULL                         // Task handle
            );
        }
    }
}

// Waits for the sensor to refresh
void SensorManager::waitForSensor(BaseSensor& sensor) {
    unsigned long startTime = millis();
    while (millis() - startTime < refreshInterval) {
        vTaskDelay(pdMS_TO_TICKS(100)); // Small delay to yield control
    }
}

// Get the latest sensor data
bool SensorManager::getSensorData(int index, float& temperature, float& humidity) {
    if (index < 0 || index >= sensorResults.size()) {
        Serial.println("Invalid sensor index.");
        return false;
    }

    SensorData& data = sensorResults[index];
    if (!data.isValid) {
        Serial.println("Sensor data is not valid.");
        return false;
    }

    temperature = data.temperature;
    humidity = data.humidity;
    return true;
}
