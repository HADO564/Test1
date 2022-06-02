#include <Arduino.h>
#include "WiFi.h"
#include "esp_wifi.h"
#include "HTTPClient.h"  //for sending http requests
#include <Arduino_JSON.h> //for servicing json files
//Macros
#define LED_BUILTIN 2
#define GPIO 15
#define SERIAL_SPEED 115200
#define AP_SSID "HADO_ESP32"
#define AP_PASS "HADOboi21"
//globals
WiFiClient Wifi1;
const char* serverIP = "http://192.168.4.2:5000/validate/";
String dataBase_MAC = "d4:25:8b:71:17:db";
unsigned long lastTime = 0;
unsigned long delayTime = 1000; // 1s to check data in database
String id="";
//functions
bool pass=false;
void PrintStations();
String send_request();
bool authenticate(String reqMacAdd);
String mac2string(byte ar[]);

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

void loop()
{
  time_t now = time(0); 
  delay(5000);//wait 5 seconds after boot to allow connection to server
  id=send_request();//find the MAC_address of the device that is used to access
  if(id!="")
  pass=authenticate(id);
  delay(500);
  if(pass)
  {
    Serial.println("Access granted to " );
    Serial.println(id);
    digitalWrite(LED_BUILTIN,HIGH);
    digitalWrite(GPIO,LOW);
    delay(5000);
  }
  digitalWrite(LED_BUILTIN,LOW);
  digitalWrite(GPIO,HIGH);
  pass=false;
  delay(5000);
}

String send_request()
{
  // extracting list of stations connected to esp32
  wifi_sta_list_t stationList;

  esp_wifi_ap_get_sta_list(&stationList);

  Serial.print("Number of connected stations: ");
  Serial.println(stationList.num);
  String macAdd="";
  String reqmAdd="";
  if (stationList.num>0)
  {
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
    if (macAdd == dataBase_MAC)//removes db mac address from list
    {
      macAdd="";
      continue;
    }
    else
      reqmAdd = macAdd;
    macAdd = "";
  }
  }
  return reqmAdd;
}

String httpGETRequest(const char* serverName,String MacAdd) {
  WiFiClient client;
  HTTPClient http;
  String serverPath = serverIP + MacAdd;
  Serial.println(serverPath);
  // IP address plus route
  http.begin(serverPath);
  
  // Send HTTP get request
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
      response = httpGETRequest(serverIP,reqMacAdd);
    
  
      if(response=="valid")
      {
        return true;
      }
      return false;
    lastTime = millis();
  }
}
//converts mac_address from esp32 station list to string. the mac_address is originally in uint8_t format
String mac2String(byte ar[])
{
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%02X", ar[i]); // J-M-L: slight modification, added the 0 in the format for padding
    s += buf;//add the two digits from the buffer to the string
    if (i < 5)
      s += ':';//add a colon to make it appear like a MAC address
  }
  return s;
}

//code for handling json objects
    //   JSONVar obj = JSON.parse(response);
    //   // if(JSON.typeof(obj == "undefined")) {
    //   //   Serial.println("Parsing input failed!");
    //   //   return;
    //   // }
    
    //   Serial.print("JSON object = ");
    //   Serial.println(obj);
    
    //   // myObject.keys() can be used to get an array of all the keys in the object
    //   JSONVar keys = obj.keys();
    
    //   for (int i = 0; i < keys.length(); i++) {
    //     JSONVar value = obj[keys[i]];
    //     Serial.print(keys[i]);
    //     Serial.print(" = ");
    //     Serial.println(value);
    // }
    // else {
    //   Serial.println("WiFi Disconnected");
    // }
