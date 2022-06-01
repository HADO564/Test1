#include <Arduino.h>
#include "WiFi.h"
#include "esp_wifi.h"
#include "HTTPClient.h"  //for sending http requests
#include <ArduinoJson.h> //for servicing json files
#include <vector>
#define LED_BUILTIN 2
#define GPIO 15
#define SERIAL_SPEED 115200
#define AP_SSID "HADO_ESP32"
#define AP_PASS "HADOboi21"
WiFiClient Wifi1;
String serverIP = "192.168.4.2:5000";
String dataBase_MAC = "d4:25:8b:71:17:db";
unsigned long lastTime = 0;
unsigned long delayTime = 1000; // 1s to check data in database
void PrintStations();
void send_request();
void authenticate(String reqMacAdd);

void setup()
{
  Serial.begin(SERIAL_SPEED);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.softAP(AP_SSID, AP_PASS);
  pinMode(GPIO, OUTPUT);
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
}

String mac2String(byte ar[])
{
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%02X", ar[i]); // J-M-L: slight modification, added the 0 in the format for padding
    s += buf;
    if (i < 5)
      s += ':';
  }
  return s;
}

void loop()
{
  // PrintStations();
  send_request();
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(GPIO, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(GPIO, HIGH);
  delay(2000);
  delay(3000);
}

void send_request()
{
  // extracting list of stations connected to esp32
  wifi_sta_list_t stationList;

  esp_wifi_ap_get_sta_list(&stationList);

  Serial.print("Number of connected stations: ");
  Serial.println(stationList.num);
  String macAdd;
  String reqmAdd;
  for (int i = 0; i < stationList.num; i++)
  {

    wifi_sta_info_t station = stationList.sta[i];

    for (int j = 0; j < 6; j++)
    {
      char str[3];
      sprintf(str, "%02x", (int)station.mac[j]);
      macAdd += str;

      if (j < 5)
      {
        macAdd += ':';
      }
    }
    if (macAdd == dataBase_MAC)
    {
      continue;
    }
    else
      reqmAdd=macAdd;
    macAdd = "";
  }

  Serial.println("-----------------");

}

void authenticate (String reqMacAdd)
{


  //HTTP request code:
   if ((millis() - lastTime) > delayTime) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = serverIP + "/validate/" +reqMacAdd;
      
      // Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        if(payload=="valid")
        {
          
        }
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


































// void PrintStations(){
//   wifi_sta_list_t stationList;

//   esp_wifi_ap_get_sta_list(&stationList);

//   Serial.print("Number of connected stations: ");
//   Serial.println(stationList.num);

//   for(int i = 0; i < stationList.num; i++) {

//     wifi_sta_info_t station = stationList.sta[i];

//     for(int j = 0; j< 6; j++){
//       char str[3];

//       sprintf(str, "%02x", (int)station.mac[j]);
//       Serial.print(str);

//       if(j<5){
//         Serial.print(":");
//       }
//     }
//     Serial.println();
//   }

//   Serial.println("-----------------");
// }