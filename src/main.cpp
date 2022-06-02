#include <Arduino.h>
#include "WiFi.h"
#include "esp_wifi.h"
#include "HTTPClient.h"  //for sending http requests
#include <Arduino_JSON.h> //for servicing json files
#include <vector>
#define LED_BUILTIN 2
#define GPIO 15
#define SERIAL_SPEED 115200
#define AP_SSID "HADO_ESP32"
#define AP_PASS "HADOboi21"
WiFiClient Wifi1;
const char* serverIP = "192.168.4.2:5000/validate/";
String dataBase_MAC = "d4:25:8b:71:17:db";
unsigned long lastTime = 0;
unsigned long delayTime = 1000; // 1s to check data in database
String id="";
bool pass;
void PrintStations();
String send_request();
bool authenticate(String reqMacAdd);

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
  id=send_request();
  pass=authenticate(id);
  delay(3000);
  if(pass)
  {
    digitalWrite(LED_BUILTIN,HIGH);
    digitalWrite(GPIO,LOW);
    delay(3000);
  }
  digitalWrite(LED_BUILTIN,LOW);
  digitalWrite(GPIO,HIGH);
  pass=false;
  delay(3000);
}

String send_request()
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
      reqmAdd = macAdd;
    macAdd = "";
  }

  return reqmAdd;
}

String httpGETRequest(const char* serverName,String MacAdd) {
  WiFiClient client;
  HTTPClient http;
  String serverPath = serverIP + MacAdd
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverPath);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

bool authenticate(String reqMacAdd)
{
  bool valid =false;
  String response="";
  // HTTP request code:
  if ((millis() - lastTime) > delayTime) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      response = httpGETRequest(serverIP,reqMacAdd);
      Serial.println(response);
    
      JSONVar obj = JSON.parse(response);
  
      
      if(JSON.typeof(obj == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(obj);
    
      // myObject.keys() can be used to get an array of all the keys in the object
      JSONVar keys = myObject.keys();
    
      for (int i = 0; i < keys.length(); i++) {
        JSONVar value = myObject[keys[i]];
        Serial.print(keys[i]);
        Serial.print(" = ");
        Serial.println(value);
      }
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

