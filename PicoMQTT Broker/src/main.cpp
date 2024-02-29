#include <Arduino.h>
#include <PicoMQTT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WiFi.h>

void OledClear();
void ShowMsg();

//OLED Settings
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// WiFi Settings
const char* ssid = "MQTT-Broker";
const char* password = "senhaforte123";

// Loop variables
unsigned int update_time = 0;
uint8_t counter = 0;

// MQTT Settings
class MQTT: public PicoMQTT::Server{
    protected:
        PicoMQTT::ConnectReturnCode auth(const char * client_id, const char * username, const char * password) override {
            // only accept client IDs which are 3 chars or longer
            if (String(client_id).length() < 3) {    // client_id is never NULL
                return PicoMQTT::CRC_IDENTIFIER_REJECTED;
            }

            // only accept connections if username and password are provided
            if (!username || !password) {  // username and password can be NULL
                // no username or password supplied
                return PicoMQTT::CRC_NOT_AUTHORIZED;
            }

            // accept two user/password combinations
            if (
                ((String(username) == "alice") && (String(password) == "secret"))
                || ((String(username) == "bob") && (String(password) == "password"))) {
                return PicoMQTT::CRC_ACCEPTED;
            }

            // reject all other credentials
            return PicoMQTT::CRC_BAD_USERNAME_OR_PASSWORD;
        }
}mqtt;

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
  WiFi.softAP(ssid, password);

  // MQTT Setup
  mqtt.begin();
}

void loop() {
  mqtt.loop();

  // Update OLED every 5 seconds
  if (millis() - update_time > 5000) {
    update_time = millis();
    ShowMsg();
  }
}

void OledClear() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
}

void ShowMsg() {
  OledClear();
  int num = WiFi.softAPgetStationNum();
  display.print("MQTT-Broker\n");
  display.printf("IP: %s\n", WiFi.softAPIP().toString().c_str());
  display.printf("PORT: 1883\n");
  display.printf("Clients: %d\n", num);
  display.display();
}