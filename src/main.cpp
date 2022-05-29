#include <Arduino.h>
#include "WiFi.h"
#include "esp_wifi.h"
#include "HTTPClient.h"//for sending http requests
#include <ArduinoJson.h>//for servicing json files
#define LED_BUILTIN 2
#define GPIO 15
#define SERIAL_SPEED 115200
#define AP_SSID "HADO_ESP32"
#define AP_PASS "HADOboi21"
WiFiClient Wifi1;
String serverIP="192.168.4.2:5000";
unsigned long lastTime = 0;
unsigned long delayTime=1000;//1s to check data in database

void PrintStations();
void setup() {
  Serial.begin(SERIAL_SPEED);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.softAP(AP_SSID,AP_PASS);
  pinMode(GPIO,OUTPUT);
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  // PrintStations();
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(GPIO,LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN,LOW);
  digitalWrite(GPIO,HIGH);
  delay(2000);
  delay(3000);
}

void send_request()
{





  //HTTP request code:
   if ((millis() - lastTime) > delayTime) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = serverIP + "?temperature=36.00";
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}


// void Authenticate(wifi_sta_info_t station)
// {

// }

void PrintStations(){
  wifi_sta_list_t stationList;
 
  esp_wifi_ap_get_sta_list(&stationList);  
 
  Serial.print("Number of connected stations: ");
  Serial.println(stationList.num);
 
  for(int i = 0; i < stationList.num; i++) {
 
    wifi_sta_info_t station = stationList.sta[i];
 
    for(int j = 0; j< 6; j++){
      char str[3];
 
      sprintf(str, "%02x", (int)station.mac[j]);
      Serial.print(str);
 
      if(j<5){
        Serial.print(":");
      }
    }
    Serial.println();
  }
 
  Serial.println("-----------------");
}