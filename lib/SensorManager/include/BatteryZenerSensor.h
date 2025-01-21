#ifndef BATTERYZENERSENSOR_H
#define BATTERYZENERSENSOR_H

#include <Arduino.h>
#include "BaseSensor.h"

class BatteryZenerSensor : public BaseSensor {
public:
    BatteryZenerSensor(float battVoltHigh = 4.2, float battVoltLow = 2.7, int batteryPin = 34, int controlPin = 35, int batteryReadsToAverage = 10000);
    bool begin() override;                        // Initialize the sensor
    // Overload `getReading` to allow for synchronization
    float getReading(const bool* readyToReport) const override; // With synchronization
    float getReading() const override; // Without synchronization
    const char* getErrorMessage() const override; // Get the last error message

private:
    float battVoltHigh;      // Maximum battery voltage - ~4.2v for a fully charged 18650 battery
    float battVoltLow;       // Minimum battery voltage - going below 2.7v can damage the battery and result in unreliable readings
    const int batteryPin;          // ADC pin for reading voltage level
    const int controlPin;          // Optional control pin (e.g., to enable/disable the sensor)
    mutable String lastError; // Stores the last error message
    int numOfReadings; // Number of raw values to average - defaults to 10k for a zener sensor. 

    virtual float readPin() const;                                // Read raw ADC voltage
    float convertToNormalLevel(const float rawVoltage, const float R1 = 36.0, const float R2 = 10.0) const;
  // Convert raw voltage to battery voltageusing default values
    float convertToPercentage(float batteryVoltage) const; // Convert voltage to percentage
};

#endif // BATTERYZENERSENSOR_H
