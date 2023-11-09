#include <Arduino.h>
#include <PicoMQTT.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixel

#if __has_include("config.h")
#include "config.h"
#endif

// Default WiFi credentials
#ifndef WIFI_SSID
#define WIFI_SSID "American Idiot"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "vini123456"
#endif

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

unsigned long last_pub_time = 0;
int counter = 0;

PicoMQTT::Server mqtt;

void setup() {
    // Setup serial
    Serial.begin(9600);

    // Setup OLED
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, LOW);
    delay(20);
    digitalWrite(OLED_RST, HIGH);

    //Initialize OLED
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

    // Connect to WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) { delay(1000); }

    //Display text
    display.print("MQTT Broker\n");
    display.printf("WiFi %s\n", WIFI_SSID);
    display.printf("IP: %s\n", WiFi.localIP().toString().c_str());
    display.display();
    
    mqtt.begin();
}

void loop() {
    mqtt.loop();

    if(millis() - last_pub_time > 10000){
        last_pub_time = millis();
        mqtt.publish("test/topic", "Hello World! %d", counter++);
    }
}