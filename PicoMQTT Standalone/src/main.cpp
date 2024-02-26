#include <Arduino.h>
#include <PicoMQTT.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

#define  LMIC_DEBUG_LEVEL = 1 
#define CFG_au915

void do_send(osjob_t* j);
void buildPacket(uint8_t *txBuffer);
void ShowMsg();

// Compile Regression Settings
#ifdef COMPILE_REGRESSION_TEST
#define FILLMEIN 0
#else
#warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
#define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

// APPEUI in LSB
static const u1_t PROGMEM APPEUI[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
void os_getArtEui(u1_t *buf) { memcpy_P(buf, APPEUI, 8); }

// DEVEUI in LSB
static const u1_t PROGMEM DEVEUI[8] = {0x8F, 0x55, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
void os_getDevEui(u1_t *buf) { memcpy_P(buf, DEVEUI, 8); }

// APPKEY in MSB
static const u1_t PROGMEM APPKEY[16] = {0xE4, 0xE8, 0xFB, 0x67, 0x8E, 0x36, 0x2D, 0x40, 0x82, 0xAE, 0x67, 0x63, 0x27, 0x26, 0xD3, 0x3D};
void os_getDevKey(u1_t *buf) { memcpy_P(buf, APPKEY, 16); }

char tempString[16];
static osjob_t sendjob;
static uint8_t txBuffer[9];
bool flag = false; // Flag para debounce

// LoRa transmission time
const unsigned TX_INTERVAL = 30;

// Pin map
const lmic_pinmap lmic_pins = {
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 14, 
    .dio = {26, 35, 34},
};

// LoRaWAN event handler
void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
      }
      // Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
    /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_SCAN_FOUND:
      ||    Serial.println(F("EV_SCAN_FOUND"));
      ||    break;
    */
    case EV_TXSTART:
      Serial.println(F("EV_TXSTART"));
      break;
    case EV_TXCANCELED:
      Serial.println(F("EV_TXCANCELED"));
      break;
    case EV_RXSTART:
      /* do not print anything -- it wrecks timing */
      break;
    case EV_JOIN_TXCOMPLETE:
      Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
      break;
    default:
      Serial.print(F("Unknown event: "));
      Serial.println((unsigned) ev);
      break;
  }
}

// LoRa Send Data
void do_send(osjob_t* j) { 

  // Check if there is not a current TX/RX job running 
  if (LMIC.opmode & OP_TXRXPEND)
  {
    Serial.println(F("OP_TXRXPEND, not sending"));
  }
  else
  {
    buildPacket(txBuffer);
    LMIC_setTxData2(1, txBuffer, sizeof(txBuffer), 0);
    Serial.println(F("Packet queued"));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

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
      
      // Send message to LoRa
      strcpy(tempString, payload);
      do_send(&sendjob);
      
  });

  // Setup LoRa
  pinMode(0, INPUT_PULLUP);

  #ifdef VCC_ENABLE
    // For Pinoccio Scout boards
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
  #endif

  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

}

void loop() {
  mqtt.loop();

  // Update OLED every 5 seconds
  if (millis() - update_time > 5000) {
    update_time = millis();
    ShowMsg();
  }

  std::string tempString = "123";
  do_send(&sendjob);

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
  display.print("Standalone-Broker\n");
  display.printf("IP: %s\n", WiFi.softAPIP().toString().c_str());
  display.printf("PORT: 1818\n");
  display.printf("Clients: %d\n", num);
  display.display();
}

void buildPacket(uint8_t txBuffer[9])
{
  Serial.println(F("Building packet"));
  if(sizeof(tempString) > (sizeof(txBuffer)-1))
  {
    Serial.println(F("Payload too long"));
    return;
  }
  else
  {
    txBuffer[0] = 0x01;
    for(int i = 0; i < sizeof(tempString); i++)
    {
      txBuffer[i+1] = tempString[i];
    }
  }
}