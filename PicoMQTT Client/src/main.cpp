#include <Arduino.h>
#include <PicoMQTT.h>
#include <WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED Settings
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// DHT Setings
#define DHTPIN 17
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// WiFi Settings
const char* WIFI_SSID = "MQTT-Broker";
const char* WIFI_PASSWORD = "1234";

// Publish Settings
unsigned long last_publish_time = 0;

// MQTT Settings
PicoMQTT :: Client client("192.168.4.1", 1818);

void OledDisplay(String topic, String payload){
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(topic);
  display.println(payload);
  display.display();
}

void setup() {
  // Setup serial
  Serial.begin(9600);
  Serial.println("Starting...");

  // Setup WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, NULL);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Setup OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  
  // Initialize the OLED display using Wire library
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
  }

  //Clear the buffer
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);

  // Setup DHT
  dht.begin();

  // Subscribe to a topic pattern and attach a callback
  client.subscribe("topic/temperature", [](const char * topic, const char * payload) {
      Serial.printf("Received message in topic '%s': %s\n", topic, payload);
      OledDisplay(topic, payload);
  });

  // Setup MQTT Broker
  client.begin();
}

void loop(){
  client.loop();

  // Publish a message every 5 seconds
  if (millis() - last_publish_time > 10000) {
    last_publish_time = millis();
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    client.publish("topic/temperature", String(temperature).c_str());
    client.publish("topic/humidity", String(humidity).c_str());
  }
}