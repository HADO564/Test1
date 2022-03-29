#include <Arduino.h>
#include "WiFi.h"
#define LED_BUILTIN 2
#define SERIAL_SPEED 115200
WiFiClient Wifi1;
const char* ssid= "HUAWEI-yMZb";
const char* password= "cvWGVfw4";
void setup() {
  Serial.begin(SERIAL_SPEED);
  // Set Pin Mode for 2 to Output
  pinMode(LED_BUILTIN, OUTPUT);
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  };
  
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void loop() {
  
}