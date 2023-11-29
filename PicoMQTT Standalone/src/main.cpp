#include <Arduino.h>
#include <PicoMQTT.h>
#include <WiFi.h>
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

// WiFi Settings
const char* WIFI_SSID = "Standalone-Broker";
const char* WIFI_PASSWORD = "1234";

// Loop variables
unsigned int update_time = 0;

// MQTT Settings
PicoMQTT :: Server mqtt(1818);

void OledClear() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
}

void showHostNum(){
  int num = WiFi.softAPgetStationNum();
  OledClear();
  display.println("Standalone-Broker");
  display.print("IP:");
  display.println(WiFi.softAPIP());
  display.print("Hosts:");
  display.println(num);
}

void setup() {
  // Setup serial
  Serial.begin(9600);
  Serial.println("Starting...");
  
  // Setup WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);

  // Initialize the OLED display using Wire library
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
  }

  // Subscribe to MQTT topics
  mqtt.subscribe("topic/message", [](const char * topic, const char * payload) {
      Serial.printf("Received message in topic '%s': %s\n", topic, payload);
      
  });

}

void loop() {
  mqtt.loop();
  
  // Update OLED every 10 seconds
  if(millis() - update_time < 10000){
    update_time = millis();
    showHostNum();
  }

}