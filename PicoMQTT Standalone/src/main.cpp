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
const char* WIFI_PASSWORD = "123456789";

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

void ShowMsg() {
  OledClear();
  int num = WiFi.softAPgetStationNum();
  display.print("Standalone-Broker\n");
  display.printf("IP: %s\n", WiFi.softAPIP().toString().c_str());
  display.printf("PORT: 1818\n");
  display.printf("Clients: %d\n", num);
  display.display();
}

void setup() {
  // Setup serial
  Serial.begin(9600);
  Serial.println("Starting...");
  
  // Setup WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, NULL);

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

  //Display text
  ShowMsg();  

  // Subscribe to MQTT topics
  mqtt.subscribe("topic/message", [](const char * topic, const char * payload) {
      Serial.printf("Received message in topic '%s': %s\n", topic, payload);
      
  });

}

void loop() {
  mqtt.loop();

  // Update OLED every 5 seconds
  if (millis() - update_time > 5000) {
    update_time = millis();
    ShowMsg();
  }

}