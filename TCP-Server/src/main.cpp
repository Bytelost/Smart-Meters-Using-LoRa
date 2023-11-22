#include <Arduino.h>
#include <WiFi.h>

// WiFi credentials
const char* ssid = "TCP-ESP32";
const char* password = "12345678";

// TCP Server Settings
#define TCP_PORT 4080
WiFiServer TCPServer(TCP_PORT);

void setup() {
  Serial.begin(9600);

  // Create WiFi Hotspot
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println("WiFi Hotspot Created");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Start TCP Server
  TCPServer.begin();
}

void loop() {
  // Check if a client has connected
  WiFiClient client = TCPServer.available();

  // Print Client Message
  if(client){
    String msg = client.readStringUntil('\n');
    Serial.print("Client Message: ");
    Serial.println(msg);
  }
}