
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
#include "credentials.h"


fauxmoESP fauxmo;


const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

// Example of data captured by IRrecvDumpV2.ino
uint16_t rawData[] = {
  8996, 4432, 600, 524, 600, 532, 600, 
  1640, 604, 520, 604, 528, 604, 520, 
  604, 528, 600, 524, 608, 1632, 604, 
  1640, 604, 528, 604, 1640, 600, 1640, 
  604, 1640, 604, 1640, 596, 1640, 604, 
  528, 604, 528, 596, 524, 604, 1640, 
  604, 528, 604, 520, 604, 528, 604, 
  520, 604, 1640, 604, 1636, 604, 1640, 
  604, 520, 604, 1640, 604, 1640, 604, 
  1636, 604, 1640, 604, 30008
};

void setup() {
  irsend.begin();
  Serial.begin(19200);
  Serial.print("here");
  WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    Serial.print(WiFi.status());
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println();

    // Connected!

    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    delay(5000);
  fauxmo.createServer(true); // not needed, this is the default value
  fauxmo.setPort(80); // This is required for gen3 devices
  fauxmo.enable(true);
  fauxmo.addDevice("TV");
  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        
        
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
        // Checking for device_id is simpler if you are certain about the order they are loaded and it does not change.
        // Otherwise comparing the device_name is safer.
        if (strcmp(device_name, "TV")==0) {
          irsend.sendRaw(rawData, 68, 38);  // Send a raw data capture at 38kHz.
        }

    });


}

void loop() {
   fauxmo.handle();
}
