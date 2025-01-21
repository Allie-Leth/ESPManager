#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <LittleFS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


// Variables that need to be set

const char* ssid = "your_ssid";
const char* password = "yours_password";
const char* mqtt_broker = "your_mqtt_broker";
const int mqtt_port = 1883; // Default MQTT port for unencrypted and unauthenticated connections -- adjust if needed
const char* mqtt_username = "your_mqtt_username";
const char* mqtt_password = "your_mqtt_password";

// MQTT Topics, adjust if desired
const char* mqtt_topic_temperature = "temperature/greenhouse/reading";
const char* mqtt_topic_error = "temperature/greenhouse/error";
const char* mqtt_topic_battery = "temperature/greenhouse/battery";
// TODO: change these to their individual components for use in the string builder function utilized by the MQTT publish method.

const char* device_identifier = "esp32-temperature"; 



// TODO: Set up a unique identifier for the device, this should either be user set or generated if there is no user input
// TODO: Add a string builder for the MQTT topics to easier customize them



#define VOLTAGE_PIN 36 // ADC pin for voltage monitoring
#define CONTROL_PIN 19 // GPIO pin for voltage control via transistor

// TODO: Change print statements to memory efficient logging statements. 
// TODO: Move hard coded variables to a config file and set up to push a filesystem on format of the device
// It may be a good idea to split that into its own branch as arduino IDE removed the ability to push files to the ESP32
// and I'd like this to be easy to use so I can offer it to others
// TODO: Implement transistor control to avoid voltage drain as a result of the voltage monitor
// TODO: Add QoS levels or error handling failing to publish to MQTT - if it's unable to publish it should store in the file system and try again on next cycle
// TODO: Move MQTT publishing to its own function - I only expected to publish once when I started this project - clearly that changed
// It should create a string with values, timestamp, and identifier then publish that string to the MQTT topic
// TODO: Move methods to their own classes - this is a mess

WiFiClient espClient;
PubSubClient client(espClient);

#define DHTPIN 16
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// NTP Client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -7 * 3600, 60000); // Denver time (UTC-7)

// File path for storing data
const char* dataFilePath = "/sensor_data.txt";

void connectToWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to Wi-Fi: " + String(ssid) + "\n");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi");
  Serial.println("IP Address: " + WiFi.localIP().toString());
}

// Builds a string from the topic type, location, and attribute
String buildMqttTopic(const char* topicType, const char* location, const char* attribute) {
  String topic = String(topicType) + "/" + String(location) + "/" + String(attribute);
  return topic;
}

void connectToMQTT() {
  if (client.connected()) return;

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.println("Failed to connect to MQTT. State: " + String(client.state()));
      delay(2000);
    }
  }
}

bool mqttPublish(const String& topic, const char* message, const char* mqtt_broker, int mqtt_port, const char* mqtt_username, const char* mqtt_password) {

  if (client.publish(topic.c_str(), message)) {
    Serial.println("Message published successfully");
    client.disconnect();
    return true;
  } else {
    Serial.println("Failed to publish message");
    client.disconnect();
    return false;
  }
}

// Methods for the file system

// Format LittleFS
void formatLittleFS() {
  Serial.println("Formatting LittleFS...");
  if (LittleFS.format()) {
    Serial.println("LittleFS formatted successfully");
  } else {
    Serial.println("Failed to format LittleFS");
  }
}

// Check if LittleFS is mounted, if not, format and mount it
void checkAndMountLittleFS() {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed. Attempting to format...");
    formatLittleFS();
    if (!LittleFS.begin()) {
      Serial.println("LittleFS mount failed after formatting. Critical error");
      return;
    }
  }
  Serial.println("LittleFS mounted successfully");
  Serial.println("Checking to see if log file exists...");
  // After mounting LittleFS, check if the data file exists

  // TODO: Utilize MQTT publish method to publish an error message if the file exists as something has gone wrong
  if (!LittleFS.exists(dataFilePath)) {
    Serial.println("Data file does not exist. Creating a new file...");
    File file = LittleFS.open(dataFilePath, FILE_WRITE);
    if (!file) {
      Serial.println("Failed to create file");
      return;
    }
    file.close();
    Serial.println("File created successfully");
  } else {
    Serial.println("Data file already exists");
  }
}


// Generate string from temp, hum, timestamp, and unique ID, and save it to LittleFS
// If the file does not exist, create it - but throw an error as that is unexpected
void saveToLittleFS(float temp, float hum, const String& timestamp) {
  // Check if the file exists, create it if not
  if (!LittleFS.exists(dataFilePath)) {

    // While it's possible for this to have happened for other reasons, this should have already happened during setup. 
    // It's important to verify ram and flash memory if this error occurs
    // It might be a sign of failing flash memory 
    // ESP32's flash memory is rated for 100,000 write cycles - this device may be reaching the end of its life
    Serial.println("Error, file does not exist");
    Serial.println("This file should have been created during checkAndMountLittleFS()");
    Serial.println("Recreating the file, but logging this as an error - data will be lost, verify ram and flash memory");
    // Create an error message and publish it to MQTT
    char errorMessage[256];
    snprintf(errorMessage, sizeof(errorMessage), "Filesystem Error on device %s at %s", device_identifier, timestamp);
    client.publish(mqtt_topic_error, errorMessage);

    File file = LittleFS.open(dataFilePath, FILE_WRITE);
    if (!file) {
      Serial.println("Failed to create file");
      return;
    }
    file.close();
    Serial.println("File created successfully");
  }

  // Open the file for appending
  File file = LittleFS.open(dataFilePath, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }

  // Write data to the file
  file.printf("[%s] Temp: %.2fC, Humidity: %.2f%%\n", timestamp.c_str(), temp, hum);
  file.close();
  Serial.println("Data saved to LittleFS");
}

// Read data from LittleFS and print it to the serial monitor
// TODO: Debugging method: Remove this method when code is finished
void readFromLittleFS() {
  File file = LittleFS.open(dataFilePath, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Reading data from LittleFS:");
  while (file.available()) {
    char line[128];
    int index = 0;

    while (file.available()) {
      char c = file.read();
      if (c == '\n' || index >= 127) {
        line[index] = '\0';
        Serial.println(line);
        index = 0;
        if (c == '\n') break;
      } else {
        line[index++] = c;
      }
    }
  }

  file.close();
}

// Battery interface methods 

// Read the battery voltage from the ADC pin - this is rough estimate done by using a zener diode
// This could also be done using a voltage sensor, but this is a quick and dirty way to get a rough estimate
// If the transistor is not put in place, this will drain the battery
float readBatteryVoltage() {
  int raw = analogRead(VOLTAGE_PIN);
  float voltage = (raw / 4095.0) * 3.3 * 2; // TODO: Calibrate this value
  Serial.println("Battery Voltage: " + String(voltage) + "V");
  return voltage;
}

// Turn the transistor on or off to control the voltage drain
void controlVoltage(bool state) {
  digitalWrite(CONTROL_PIN, state ? HIGH : LOW);
  Serial.println(String("Voltage control set to: ") + (state ? "ON" : "OFF"));
}

// Pushes battery voltage to MQTT topic temperature/greenhouse/battery
// TODO: when MQTT is moved to its own function, this should implement that functionality
void pushBatteryVoltage(float voltage) {
  String message = String(voltage) + "V";
  if (client.publish(mqtt_topic_battery, message.c_str())) {
    Serial.println("Battery voltage published to MQTT");
  } else {
    Serial.println("Failed to publish battery voltage to MQTT");
  }
}

void setup() {
  Serial.begin(115200);

  connectToWiFi();

  client.setServer(mqtt_broker, mqtt_port);

  dht.begin();

  checkAndMountLittleFS();
  // Get the current time from NTP server for timestamping the data
  timeClient.begin();
  timeClient.update();

  readFromLittleFS();

  // 
  // Initialize ADC
  analogReadResolution(12); // Set ADC resolution to 12 bits

}

// Connects to wifi, connects to MQTT, pulls a timestamp, reads the temperature and humidity, saves it to LittleFS, and publishes it to MQTT
void loop() {
  connectToWiFi();
  connectToMQTT();

  // TODO: Move to a self contained time read function that handles all time activity
  timeClient.update();
  String timestamp = timeClient.getFormattedTime();


  // TODO: Move to a self contained sensor read function that handles all DHT sensor activity
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from DHT sensor");
  } else {
    saveToLittleFS(temp, hum, timestamp);
    String message = "[" + timestamp + "] Temp: " + String(temp) + "C, Humidity: " + String(hum) + "%";
    if (client.publish(mqtt_topic_temperature, message.c_str())) {
      Serial.println("Message published to MQTT");
    } else {
      Serial.println("Failed to publish message to MQTT");
    }
  }


  // TODO: Move to a self contained battery read function that handles all battery activity
  float voltage = readBatteryVoltage();
  pushBatteryVoltage(voltage);

  // Sleep for 5 minutes - good night, sweet prince.
  Serial.println("Going to deep sleep for 5 minutes...");
  esp_sleep_enable_timer_wakeup(300000000);
  esp_deep_sleep_start();
}
