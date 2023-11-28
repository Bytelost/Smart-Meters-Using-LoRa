#include <Arduino.h>
#include <PicoMQTT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WiFi.h>

//OLED Settings
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// WiFi Settings
const char* ssid = "MQTT-Broker";
const char* password = "1234";

// MQTT Settings
PicoMQTT :: Server server(1818);

void setup() {
  Serial.begin(9600);

  // Setup OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  // Initiate OLED
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

  // WiFi Setup
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, NULL);

  //Display text
  display.print("MQTT Broker\n");
  display.printf("WiFi %s\n", ssid);
  display.printf("IP: %s\n", WiFi.softAPIP().toString().c_str());
  display.display();

  // Subscribe to a topic pattern and attach a callback
  server.subscribe("#", [](const char * topic, const char * payload) {
      Serial.printf("Received message in topic '%s': %s\n", topic, payload);
  });

  // MQTT Setup
  server.begin();
}

void loop() {
  server.loop();
}
