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
const char* WIFI_SSID = "ESP32";
const char* WIFI_PASSWORD = "12345678";

// Publish Settings
unsigned long last_publish_time = 0;
int greeting_number = 1;

// MQTT Settings
PicoMQTT :: Server mqtt(1818);

void setup() {
  // Setup serial
  Serial.begin(9600);
  Serial.println("Starting...");

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

  // Setup WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, NULL);

  //Display text
  display.print("MQTT Broker\n");
  display.printf("WiFi %s\n", WIFI_SSID);
  display.printf("IP: %s\n", WiFi.softAPIP().toString().c_str());
  display.display();

  // Setup MQTT Broker
  mqtt.begin();

}

void loop() {
  mqtt.loop();

  if(millis() - last_publish_time > 10000){

    // Reading temperature or humidity
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) ) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    // Compute heat index in Celsius
    float hic = dht.computeHeatIndex(t, h, false);

    // Publish to MQTT
    last_publish_time = millis();
    String topic = "esp32/temperature";
    String payload = String(t);
    mqtt.publish(topic, payload);
    

    Serial.print(F("Temperature: "));
    Serial.println(t);


  }
}