#include <Arduino.h>
#include <PicoMQTT.h>

PicoMQTT::Server mqtt;

void setup() {
    // Usual setup
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin("MyWiFi", "password");

    // Subscribe to a topic pattern and attach a callback
    mqtt.subscribe("#", [](const char * topic, const char * payload) {
        Serial.printf("Received message in topic '%s': %s\n", topic, payload);
    });

    // Start the broker
    mqtt.begin();
}

void loop() {
    // This will automatically handle client connections.  By default, all clients are accepted.
    mqtt.loop();

    if (random(1000) == 0)
        mqtt.publish("picomqtt/welcome", "Hello from PicoMQTT!");
}
