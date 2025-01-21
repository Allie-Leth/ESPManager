#include "BatteryZenerSensor.h"

// Constructor
BatteryZenerSensor::BatteryZenerSensor(float battVoltHigh, float battVoltLow, int batteryPin, int controlPin, int numOfReadings)
    : BaseSensor(batteryPin, "Battery Zener Sensor", SensorType::BatteryZener),
      battVoltHigh(battVoltHigh), battVoltLow(battVoltLow),
      batteryPin(batteryPin), controlPin(controlPin), lastError(""), numOfReadings(numOfReadings) {}

// Initialize the sensor
bool BatteryZenerSensor::begin() {
    // Set up the control pin if available
    if (controlPin >= 0) {
        pinMode(controlPin, OUTPUT);
        digitalWrite(controlPin, HIGH); // Enable the sensor
    }

    // Perform a test read to ensure initialization
    float voltage = readPin();
    if (voltage < 0) {
        lastError = "Failed to initialize battery sensor!";
        return false;
    }

    lastError = "";
    return true;
}

/**
 * @brief takes 10,000 samples of the battery voltage and returns the average 
 * @return float of battery voltage converted to a percent of capacity
 */
 
float BatteryZenerSensor::getReading(const bool* readyToReport) const {
    static float batteryValues = 0.0;          // Accumulator for readings
    static int numSamples = 0;                 // Count of samples taken
    static unsigned long lastSampleTime = 0;   // Timestamp of last sample
    const unsigned long sampleInterval = 5;    // Interval between samples (ms)

    // Check if sensors are not ready
    if (!(*readyToReport)) {
        // Time to take a new sample
        if (millis() - lastSampleTime >= sampleInterval) {
            lastSampleTime = millis();
            batteryValues += readPin(); // Accumulate readings
            numSamples++;
        }
        // Return -1 to indicate ongoing sampling
        return -1;
    }

    // Sensors are ready, finalize the calculation
    if (numSamples > 0) {
        float averagedVoltage = batteryValues / numSamples;

        // Reset static variables for the next cycle
        batteryValues = 0.0;
        numSamples = 0;

        // Normalize and convert to percentage
        float normalizedVoltage = convertToNormalLevel(averagedVoltage);
        return convertToPercentage(normalizedVoltage);
    }

    // No samples taken - this is an error state
    lastError = "[ERROR] Battery getReading ran with readyToReport flag set to true";
    return -1;
}

float BatteryZenerSensor::getReading() const {
    float batteryValues = 0.0; // Accumulator for readings

    // Take numOfReadings samples
    for (int i = 0; i < numOfReadings; i++) {
        batteryValues += readPin();
    }

    // Calculate the average voltage
    float averagedVoltage = batteryValues / numOfReadings;

    // Normalize the voltage and convert to percentage
    float normalizedVoltage = convertToNormalLevel(averagedVoltage);
    return convertToPercentage(normalizedVoltage);
}

// Override to get the last error message
const char* BatteryZenerSensor::getErrorMessage() const {
    return lastError.c_str();
}

// Read the raw voltage from the battery pin
float BatteryZenerSensor::readPin() const {
    // Assuming a 12-bit ADC (0-4095), convert the ADC reading to a voltage
    int adcValue = analogRead(batteryPin);
    if (adcValue < 0) { // ADC read error
        return -1.0;
    }
    const float voltage = (adcValue / 4095.0) * 3.3; // Adjust based on your ADC reference voltage
    return voltage;
}

// Convert the raw voltage from the voltage divider to the actual battery voltage
float BatteryZenerSensor::convertToNormalLevel(float rawVoltage, const float R1, const float R2 ) const {
    // Divider ratio: R2 / (R1 + R2)
    const float dividerRatio = R2 / (R1 + R2); // Using 36kΩ and 10kΩ resistors will put us in a voltage range of ~ 1.91V-3.27V
    // Calculate the actual battery voltage
    float batteryVoltage = rawVoltage / dividerRatio;
    return batteryVoltage;
}


// Convert the battery voltage to a percentage
float BatteryZenerSensor::convertToPercentage(float batteryVoltage) const {
    // Clamp the battery voltage to ensure it's within the valid range
    if (batteryVoltage > battVoltHigh) {
        batteryVoltage = battVoltHigh;
    } else if (batteryVoltage < battVoltLow) {
        batteryVoltage = battVoltLow;
    }

    // Map the battery voltage to a percentage
    float percentage = ((batteryVoltage - battVoltLow) / (battVoltHigh - battVoltLow)) * 100;

    // Ensure percentage stays within 0-100 range
    if (percentage > 100) percentage = 100;
    if (percentage < 0) percentage = 0;

    return percentage;

}
