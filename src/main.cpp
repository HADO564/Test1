#include <Arduino.h>
#include "WiFi.h"
#include "esp_wpa2.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#define LED_BUILTIN 2
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SERIAL_SPEED 115200
#define EAP_ID "hwazir.bscs20seecs"
#define EAP_USERNAME "hwazir.bscs20seecs"
#define EAP_PASSWORD "HADOzindabad21"
WiFiClient Wifi1;
const char* ssid= "eduroam";



void setup() {
  Serial.begin(SERIAL_SPEED);
  // Set Pin Mode for 2 to Output
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_MODE_APSTA);
  // WPA2 enterprise magic starts here
    WiFi.disconnect(true);      
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ID, strlen(EAP_ID));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_USERNAME, strlen(EAP_USERNAME));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
    esp_wifi_sta_wpa2_ent_enable(&config);
  // WPA2 enterprise magic ends here


    WiFi.begin(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(WiFi.localIP());
}

void loop() {
  
}