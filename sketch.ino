#include "DHTesp.h"
#include "PubSubClient.h"
#include "WiFi.h"

#define DHT_PIN 18
#define DHT_TYPE DHTesp::DHT22
#define LED_GREEN 13
#define LED_YELLOW 14
#define LED_RED 12
#define RELAY_PIN 17
#define BUZZER_PIN 19

// WiFi and MQTT configuration
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "mqtt.eclipse.org";

// Initialize DHT sensor and MQTT client
DHTesp dhtSensor;
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Setup DHT sensor
  dhtSensor.setup(DHT_PIN, DHT_TYPE);

  // Initialize GPIO pins
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Connect to WiFi
  connectWiFi();

  // Set up MQTT server
  mqtt.setServer(mqtt_server, 1883);
}

void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  // Wait for WiFi to connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("Connected to WiFi!");
}

void reconnectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect to MQTT broker
    if (mqtt.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  // Reconnect to MQTT if disconnected
  if (!mqtt.connected()) {
    reconnectMQTT();
  }

  mqtt.loop();

  // Read temperature and humidity from DHT22
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  float temperature = data.temperature;
  float humidity = data.humidity;
  
  bool pumpStatus = false;
  
  // Reset output pins
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);

  // Control LEDs, buzzer, and relay based on temperature
  if (temperature > 35) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(RELAY_PIN, HIGH);
    pumpStatus = true;
  } else if (temperature >= 30 && temperature <= 35) {
    digitalWrite(LED_YELLOW, HIGH);
  } else if (temperature < 30) {
    digitalWrite(LED_GREEN, HIGH);
  }

  // Ensure valid data before sending
  if (!isnan(temperature) && !isnan(humidity)) {
    // Log data to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.print(" %, Pump: ");
    Serial.println(pumpStatus ? "ON" : "OFF");

    // Prepare and send data to MQTT broker
    char payload[50];
    snprintf(payload, sizeof(payload),
             "{\"temperature\":%.2f,\"humidity\":%.2f,\"pump\":\"%s\"}", 
             temperature, humidity, pumpStatus ? "ON" : "OFF");
    mqtt.publish("uts_iot_boss", payload);
    
    // Log the sent payload
    Serial.print("Data sent to MQTT: ");
    Serial.println(payload);
  }

  // Delay to prevent flooding the MQTT broker
  delay(2000);
}
