#include <Arduino.h>
#include <unity.h>
#include "BatteryZenerSensor.h"

// Mock analogRead for testing
int mockAnalogReadValue = 0;

int analogRead(int pin) {
    return mockAnalogReadValue; // Return the mock value
}

BatteryZenerSensor ZenerSensor(4.2, 3.0, A0, 2);

void test_sensor_initialization_success() {
    Serial.println("Running test_sensor_initialization_success");
    mockAnalogReadValue = 2048; // Simulate valid ADC value
    TEST_ASSERT_TRUE(ZenerSensor.begin());
    TEST_ASSERT_EQUAL_STRING("", ZenerSensor.getErrorMessage());
    Serial.println("test_sensor_initialization_success complete");
}

void test_sensor_initialization_failure() {
    Serial.println("Running test_sensor_initialization_failure");
    mockAnalogReadValue = -1; // Simulate ADC failure
    TEST_ASSERT_FALSE(ZenerSensor.begin());
    TEST_ASSERT_EQUAL_STRING("Failed to initialize battery sensor!", ZenerSensor.getErrorMessage());
    Serial.println("test_sensor_initialization_failure complete");
}

void test_get_reading_valid() {
    Serial.println("Running test_get_reading_valid");
    mockAnalogReadValue = 2048; // Simulate ADC value
    ZenerSensor.begin();
    float expectedVoltage = (2048 / 4095.0) * 3.3;
    float expectedPercentage = ((expectedVoltage - 3.0) / (4.2 - 3.0)) * 100;

    TEST_ASSERT_FLOAT_WITHIN(0.1, expectedPercentage, ZenerSensor.getReading());
    Serial.println("test_get_reading_valid complete");
}

void setup() {
    Serial.begin(115200); // Start Serial for debug
    delay(2000); // Give time to open serial monitor

    UNITY_BEGIN();

    RUN_TEST(test_sensor_initialization_success);
    RUN_TEST(test_sensor_initialization_failure);
    RUN_TEST(test_get_reading_valid);

    UNITY_END();

    delay(1000); // Allow time for Unity to finish
    ESP.restart(); // Restart the ESP32
}

void loop() {
    // Unity runs tests in setup()
}
